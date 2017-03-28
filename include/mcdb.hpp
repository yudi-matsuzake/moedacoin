#ifndef TPDB_H
#define TPDB_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QFileInfo>
#include <QSqlQuery>
#include <QDebug>

/**
 * @brief Function to check if a certain file exists given a path
 * @param path Path to the file
 * @return Boolean True for exists and 0 otherwhise
 */

bool fileExists(QString path);

/**
 * @brief The User class
 */

class User
{

public:
	explicit User();
	explicit User(QString name, QString pKey);
	QString publicKey;
	QString nome;
private:

};

/**
 * @brief The Transaction class
 */

class Transaction
{
public:
	explicit Transaction();
	explicit Transaction(int id, QString fKey, QString tkey, QString minKey, float v);
	QString fromKey;
	QString toKey;
	QString minKey;
	float value;
	int id;
	~Transaction();
private:
};


/**
 * @brief Fucntion to calculate, based on transactions, the total amount of an user MoedaCoins
 * @param List containing all transactions
 * @param pKey Public key of the wallet's owner
 * @return Total amount of MoedaCoins
 */

float walletTotalCoins(QList<Transaction> tList, QString pKey);

/**
 * @brief Class to abstract all database functions
 */

class MCDB : QObject
{
    Q_OBJECT

public:
	explicit MCDB(const QString& path);
	QSqlError initDB(const QString& path);
	QSqlError createNewDatabase();
	QSqlError addNewUser(QString pub_key, QString name);
	QSqlError addNewTransaction(QString from, QString to, QString miner, float value);
	QList<Transaction> getAllTransactions();
	QList<Transaction> getTransactionsByKey(QString pKey);
	QList<User> getAllUsers();
	~MCDB();

private:
	QSqlDatabase tp_db;
};

#endif // TPDB_H
