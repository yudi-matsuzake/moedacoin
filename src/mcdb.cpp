#include "mcdb.hpp"
#include <iostream>

QString MCDB::toBase64(){
	QFile f(dbPath);
	f.open(QIODevice::ReadOnly);
	QByteArray array = f.readAll();
	std::vector<unsigned char> buffer(array.begin(), array.end());
	return MCCrypto::base64Encode(buffer);
}

void MCDB::setFromBase64(QString baseBuffer){
	(*mc_db).close();
	std::vector<unsigned char> result = MCCrypto::base64Decode(baseBuffer);
	QFile f(dbPath);
	f.open(QIODevice::ReadWrite | QIODevice::Truncate);
	f.write((char*)result.data(), result.size());
	f.close();
	this->initDB(dbPath);
}


bool fileExists(QString path){
	QFileInfo info(path);
	return info.exists() && info.isFile();
}


float walletTotalCoins(QList<MCTransaction> tList, QString pKey){
	float total = 10;
	foreach (MCTransaction t, tList) {
			if (!(QString::compare(pKey, t.getToKey()))){
				total += t.getValue();
			}

			else if (!(QString::compare(pKey, t.getFromKey())))
				total -= t.getValue();
			else
				total += 100/(float)t.getId();
	}

	return total;
}


QSqlError MCDB::createNewDatabase(){
	QSqlQuery q(*mc_db);
	if (!q.exec(QString("create table users(pub_key varchar(256) primary key)")))
		return q.lastError();

	if (!q.exec(QString("create table transactions (id integer primary key autoincrement, "
				  "fromKey varchar(256), toKey varchar(256), value float, minKey varchar(256),"
				  "foreign key (fromKey) references users(pub_key), "
				  "foreign key (toKey) references users(pub_key),"
				  "foreign key (minKey) references users(pub_key))")))
		return q.lastError();


	if (!q.exec(QString("create table info (pub_key varchar(256) primary key, name varchar(256), "
			    "foreign key (pub_key) references users(pub_key))")))
		return q.lastError();

	return QSqlError();
}


QSqlError MCDB::addNewUser(QString pub_key, QString name){
	QSqlQuery q(*mc_db);
	q.prepare("INSERT INTO users(pub_key) VALUES (:p)");
	q.bindValue(":p", pub_key);
	if (!q.exec())
		return q.lastError();
	q.prepare("INSERT INTO info(pub_key, name) VALUES (:p, :n)");
	q.bindValue(":p", pub_key);
	q.bindValue(":n", name);
	if (!q.exec())
		return q.lastError();

	return QSqlError();
}


QSqlError MCDB::addNewTransaction(QString from, QString to, QString miner, float value){
	QSqlQuery q(*mc_db);
	q.prepare("INSERT INTO transactions(fromKey, toKey, minKey, value) VALUES (:fk, :tk, :mk, :v)");
	q.bindValue(":fk", from);
	q.bindValue(":tk", to);
	q.bindValue(":mk", miner);
	q.bindValue(":v", QString::number(value));

	if (!verifyUserExistance(from))
		addNewUser(from, from.left(8));

	if (!verifyUserExistance(to))
		addNewUser(to, to.left(8));

	if (!verifyUserExistance(miner))
		addNewUser(miner, miner.left(8));

	if (!q.exec())
		return q.lastError();

	return QSqlError();
}


QList<MCTransaction> MCDB::getAllTransactions(){
	QSqlQuery q("SELECT * FROM transactions", *mc_db);
	QList<MCTransaction> result;
	while (q.next()){
			result.append(MCTransaction(q.value(0).toInt(), q.value(1).toString(),
						      q.value(2).toString(), q.value(4).toString(),
						      q.value(3).toFloat()));
	}

	return result;
}


QList<MCTransaction> MCDB::getTransactionsByKey(QString pKey){
	QSqlQuery q(*mc_db);
	q.prepare("SELECT * FROM transactions WHERE toKey like :k OR fromKey like :k OR minKey like :k");
	q.bindValue(":k", pKey);
	QList<MCTransaction> result;
	q.exec();
	while (q.next()){
			result.append(MCTransaction(q.value(0).toInt(), q.value(1).toString(),
						      q.value(2).toString(), q.value(4).toString(),
						      q.value(3).toFloat()));
	}

	return result;
}


MCDB::MCDB(const QString& path){

	std::unique_ptr<QSqlDatabase> new_mcdb(
				new QSqlDatabase());

	mc_db = std::move(new_mcdb);
	this->initDB(path);
	this->dbPath = path;
}

QSqlError MCDB::initDB(const QString& path)
{
	bool fileExistance = fileExists(path);
	*mc_db = QSqlDatabase::addDatabase("QSQLITE");
	(*mc_db).setDatabaseName(path);

	if (!(*mc_db).open()){
		qDebug() << "Conexão com Database falhou.";
		return QSqlError();
	}

	else{
		qDebug() << "Conectado com o Database" << path;
		if (!(fileExistance)){
			qDebug() << "Criando um novo database para:" << path;
			qDebug() << this->createNewDatabase().databaseText();
		}
	}

	return QSqlError();
}

bool MCDB::verifyUserExistance(QString pubKey){
	QSqlQuery q(*mc_db);
	q.prepare("SELECT * FROM users WHERE pub_key like :k");
	q.bindValue(":k", pubKey);
	q.exec();
	if (q.next())
		return true;

	return false;
}


QList<MCUser> MCDB::getAllUsers(){
	QSqlQuery q("SELECT * FROM info", *mc_db);
	QList<MCUser> result;

	while (q.next()){
			result.append(MCUser(q.value(1).toString(), q.value(0).toString()));
	}

	return result;
}


MCDB::~MCDB()
{
	(*mc_db).close();
}
