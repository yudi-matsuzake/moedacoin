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
	/**
	 * @brief User constructor without iniatializing anything
	 */
	explicit User();
	/**
	 * @brief User constructor overload
	 * @param name Name of the user
	 * @param pKey Public key of the user
	 */
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
	/**
	 * @brief Transaction constructor without initialazing anything
	 */
	explicit Transaction();
	/**
	 * @brief Transaction constructor overload
	 * @param id Id of the transaction
	 * @param fKey Sender Publick key
	 * @param tkey Destinatary Public key
	 * @param minKey Miner Public key
	 * @param v Value of the transaction
	 */
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
	/**
	 * @brief MCDB Constructor
	 * @param path Path to the SQLite database
	 */
	explicit MCDB(const QString& path);

	/**
	 * @brief Function to initialize the SQlDatabase attribute. If the database doesn't exist, creates a new one.
	 * @param path Path to the SQLite database
	 * @return Error, if so
	 */
	QSqlError initDB(const QString& path);
	/**
	 * @brief Function to create a new moedaCoin database
	 * @return Error, if so
	 */
	QSqlError createNewDatabase();
	/**
	 * @brief Function to insert a new user in the database
	 * @param pub_key Public key of the new user
	 * @param name Name of the neu user
	 * @return Error, if so
	 */
	QSqlError addNewUser(QString pub_key, QString name);
	/**
	 * @brief Function to insert a new Transaction in the database
	 * @param from Sender Public key
	 * @param to Destinatary pulic key
	 * @param miner Miner's Public key
	 * @param value Value of the transaction
	 * @return Error, if so
	 */
	QSqlError addNewTransaction(QString from, QString to, QString miner, float value);
	/**
	 * @brief Function that return all transactions
	 * @return QList which cointains all the transactions
	 */
	QList<Transaction> getAllTransactions();
	/**
	 * @brief Function that return all the transactions given an User's Public key
	 * @param pKey Public key of the User
	 * @return QList which contains the transactions
	 */

	QList<Transaction> getTransactionsByKey(QString pKey);
	/**
	 * @brief Fuction that return the List of all the users
	 * @return QList which contains every user
	 */
	QList<User> getAllUsers();

	~MCDB();

private:
	QSqlDatabase mc_db;
};

#endif // TPDB_H
