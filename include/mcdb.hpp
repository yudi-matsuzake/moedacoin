#ifndef MCDB_H
#define MCDB_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QFileInfo>
#include <QSqlQuery>
#include <QDebug>
#include "transaction.hpp"
#include "mccrypto.hpp"

/**
 * @brief Function to check if a certain file exists given a path
 * @param path Path to the file
 * @return Boolean True for exists and 0 otherwhise
 */

bool fileExists(QString path);


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

	/**
	 * @brief Function to convert the database into base64
	 * @return QString of the encoding
	 */
	QString toBase64();

	/**
	 * @brief Function to load a full database based on a base64 file
	 * @param base64 buffer containing all the database
	 */
	void setFromBase64(QString baseBuffer);
	~MCDB();

private:
	std::unique_ptr<QSqlDatabase> mc_db;
	QString dbPath;
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
};

#endif // MCDB_H
