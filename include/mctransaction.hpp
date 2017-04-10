#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <QString>
#include <QJsonObject>

#include "mccrypto.hpp"

/**
 * @brief The User class
 */

class MCUser
{
public:
	/**
	 * @brief User constructor without iniatializing anything
	 */
	explicit MCUser();
	/**
	 * @brief User constructor overload
	 * @param name Name of the user
	 * @param pKey Public key of the user
	 */
	explicit MCUser(QString name, QString pKey);
	QString publicKey;
	QString nome;
private:

};

/**
 * @brief The Transaction class
 */

class MCTransaction
{
public:
	static const QString DEFAULT_MINER_KEY;

	/**
	 * @brief Transaction constructor without initialazing anything
	 */
	explicit MCTransaction();
	/**
	 * @brief Transaction constructor overload
	 * @param id Id of the transaction
	 * @param fKey Sender Publick key
	 * @param tkey Destinatary Public key
	 * @param minKey Miner Public key
	 * @param v Value of the transaction
	 */
	explicit MCTransaction(int id, QString fKey, QString tkey, QString minKey, float v);
	~MCTransaction();

	QString getFromKey() const;
	void setFromKey(const QString &value);

	QString getToKey() const;
	void setToKey(const QString &value);

	QString getMinKey() const;
	void setMinKey(const QString &value);

	float getValue() const;
	void setValue(double value);

	int getId() const;
	void setId(int value);

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

	bool operator==(MCTransaction& a);
	bool operator!=(MCTransaction& a);

private:
	static const QString JSON_TYPE;
	static const float VALUE_INTERVAL;
	QString fromKey;
	QString toKey;
	QString minKey;
	double value;
	int id;
};

#endif // TRANSACTION_HPP
