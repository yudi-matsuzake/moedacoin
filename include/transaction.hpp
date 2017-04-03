#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <QString>
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
	~Transaction();

	QString getFromKey() const;
	void setFromKey(const QString &value);

	QString getToKey() const;
	void setToKey(const QString &value);

	QString getMinKey() const;
	void setMinKey(const QString &value);

	float getValue() const;
	void setValue(float value);

	int getId() const;
	void setId(int value);

private:
	QString fromKey;
	QString toKey;
	QString minKey;
	float value;
	int id;
};

#endif // TRANSACTION_HPP
