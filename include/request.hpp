#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <QObject>
#include <QJsonObject>
#include <QHostAddress>

class MCResponseDB;

/**
 * @brief Represents a peer connected to the host.
 */
class MCAddress{
public:
	MCAddress();
	MCAddress(QHostAddress address,
		int port);

	~MCAddress();

	QHostAddress getAddress();
	int getPort();

	void setAddress(QHostAddress& addr);
	void setPort(int port);

	bool read(const QJsonObject &json);
	bool write(QJsonObject &json);

private:
	QHostAddress address;
	int port;
};


/*
 * REQUESTS -----------------------------------
 */

/**
 * @brief Base class of a request
 */
class MCRequest : public QObject {
	Q_OBJECT
public:
	MCRequest();
	virtual ~MCRequest();

	virtual bool read(const QJsonObject& json);
	virtual bool write(QJsonObject& json);

	void setPeer(QHostAddress& addr, int port);

protected:
	const static QString JSON_TYPE;
	MCAddress peer;
};

class MCRequestDB : public MCRequest{
	Q_OBJECT
signals:
	void receiveResponse();

public:
	MCRequestDB();
	~MCRequestDB();

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

	void setListeningAddress(QHostAddress& ip);
	void setListeningPort(int port);

private:
	const static	QString method;
	MCAddress	responseAddress;
};

/**
 * @brief Class that represents a request of miner to a transaction
 */
class MCRequestMiner : public MCRequest{
public:
	MCRequestMiner();
	~MCRequestMiner();

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

private:

	const static QString method;
	QString sign;
	// transaction
	// challenge?
};

/**
 * @brief Class that represents a request to update the database
 */
class MCRequestUpdate : public MCRequest{
public:
	MCRequestUpdate();
	~MCRequestUpdate();

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

private:
	const static QString method;
	//transaction
	QString minerSign;
};

/*
 * RESPONSES ---------------------------------------------------
 */

/**
 * @brief Base class of a response;
 */
class MCResponse : public QObject {
	Q_OBJECT
public:
	MCResponse();
	virtual ~MCResponse();

	virtual bool read(const QJsonObject& json);
	virtual bool write(QJsonObject& json);

protected:
	const static QString JSON_TYPE;
	const static QString method;
	MCAddress peer;
};

/**
 * @brief Class that represents a database response
 */
class MCResponseDB : public MCResponse{
public:
	MCResponseDB();
	~MCResponseDB();

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

private:
	const static QString method;
	QString dbData;
};

/**
 * @brief Class that represents a miner's validation of a trasaction
 */
class MCResponseMiner : public MCResponse{
public:
	MCResponseMiner();
	~MCResponseMiner();

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

private:
	const static QString method;
	bool accepted;
	QString sign;

	//transaction
	//challenge?
};

#endif // REQUEST_HPP
