#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <QObject>
#include <QJsonObject>
#include <QHostAddress>

class MCResponseDB;

/**
  * @brief type of the socket connection
  */
typedef enum {
	TCP,
	UDP,
	UDP_MULTICAST
}SocketType;

/**
 * @brief Represents a peer connected to the host.
 */
class MCPeer{
public:
	MCPeer();
	MCPeer(QHostAddress address,
		QString name,
		qint16 port);

	~MCPeer();

	QHostAddress getPeerAddress();
	qint16 getPeerPort();

	void read(const QJsonObject &json);
	void write(QJsonObject &json);

private:
	QHostAddress address;
	QString name;
	qint16 port;
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

	virtual bool read(const QJsonObject& json) = 0;
	virtual bool write(QJsonObject& json) = 0;

protected:
	const static QString JSON_TYPE;
	const static QString method;
	static const SocketType sType;
	MCPeer peer;
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

	void setIp(QHostAddress& ip);
	void setPort(qint16 port);

private:
	const static	QString method;
	static const	SocketType sType = UDP_MULTICAST;
	QHostAddress	ip;
	qint16			port;
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
	static const SocketType sType;
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
	static const SocketType sType;
	//transaction
	QString minerSign;
};

/*
 * RESPONSES ---------------------------------------------------
 */

/**
 * @brief Base class of a response;
 */
class MCResponse : public MCRequest{
public:
	MCResponse();
	virtual ~MCResponse();

	virtual bool read(const QJsonObject& json) = 0;
	virtual bool write(QJsonObject& json) = 0;
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
	static const SocketType sType;
	QByteArray dbData;
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
	static const SocketType sType;
	bool accepted;
	QString sign;

	//transaction
	//challenge?
};

#endif // REQUEST_HPP
