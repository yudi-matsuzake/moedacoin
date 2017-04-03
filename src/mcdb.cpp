#include "mcdb.hpp"
#include <iostream>

bool fileExists(QString path){
	QFileInfo info(path);
	return info.exists() && info.isFile();
}


float walletTotalCoins(QList<Transaction> tList, QString pKey){
	float total = 10;
	foreach (Transaction t, tList) {
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
	QSqlQuery q(this->mc_db);
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
	QSqlQuery q(this->mc_db);
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
	QSqlQuery q(this->mc_db);
	q.prepare("INSERT INTO transactions(fromKey, toKey, minKey, value) VALUES (:fk, :tk, :mk, :v)");
	q.bindValue(":fk", from);
	q.bindValue(":tk", to);
	q.bindValue(":mk", miner);
	q.bindValue(":v", QString::number(value));
	if (!q.exec())
		return q.lastError();

	return QSqlError();
}


QList<Transaction> MCDB::getAllTransactions(){
	QSqlQuery q("SELECT * FROM transactions", this->mc_db);
	QList<Transaction> result;
	while (q.next()){
			result.append(Transaction(q.value(0).toInt(), q.value(1).toString(),
						      q.value(2).toString(), q.value(4).toString(),
						      q.value(3).toFloat()));
	}

	return result;
}


QList<Transaction> MCDB::getTransactionsByKey(QString pKey){
	QSqlQuery q(this->mc_db);
	q.prepare("SELECT * FROM transactions WHERE toKey like :k OR fromKey like :k OR minKey like :k");
	q.bindValue(":k", pKey);
	QList<Transaction> result;
	q.exec();
	while (q.next()){
			result.append(Transaction(q.value(0).toInt(), q.value(1).toString(),
						      q.value(2).toString(), q.value(4).toString(),
						      q.value(3).toFloat()));
	}

	return result;
}


MCDB::MCDB(const QString& path){
	this->initDB(path);
}

QSqlError MCDB::initDB(const QString& path)
{
	bool fileExistance = fileExists(path);
	mc_db = QSqlDatabase::addDatabase("QSQLITE");
	mc_db.setDatabaseName(path);

	if (!mc_db.open()){
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


QList<User> MCDB::getAllUsers(){
	QSqlQuery q("SELECT * FROM info", this->mc_db);
	QList<User> result;

	while (q.next()){
			result.append(User(q.value(1).toString(), q.value(0).toString()));
	}

	return result;
}


MCDB::~MCDB()
{
	this->mc_db.close();
}
