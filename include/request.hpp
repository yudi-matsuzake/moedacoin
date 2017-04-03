#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "moedanetwork.hpp"

#include <QJsonObject>

/**
 * @brief Base class of a request
 */
class MoedaNetwork::Request{
public:
	Request();
	virtual ~Request();

	virtual bool read(const QJsonObject& json) = 0;
	virtual bool write(QJsonObject& json) = 0;

private:
	const static QString method;
	static const SocketType sType;
};

class MoedaNetwork::RequestDB : private MoedaNetwork::Request{
public:
	RequestDB();
	~RequestDB();

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

private:
	const static QString method;
	static const SocketType sType = MoedaNetwork::UDP_MULTICAST;
};

/**
 * @brief Class that represents a request of miner to a transaction
 */
class MoedaNetwork::RequestMiner : public MoedaNetwork::Request{
public:
	RequestMiner();
	~RequestMiner();

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
class MoedaNetwork::RequestUpdate : public MoedaNetwork::Request{
public:
	RequestUpdate();
	~RequestUpdate();

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

private:
	const static QString method;
	static const SocketType sType;
	//transaction
	QString minerSign;
};

/**
 * @brief Base class of a response;
 */
class MoedaNetwork::Response : public MoedaNetwork::Request{
public:
	Response();
	virtual ~Response();

	virtual bool read(const QJsonObject& json) = 0;
	virtual bool write(QJsonObject& json) = 0;
};

/**
 * @brief Class that represents a database response
 */
class MoedaNetwork::ResponseDB : public MoedaNetwork::Response{
public:
	ResponseDB();
	~ResponseDB();

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

private:
	const static QString method;
	static const SocketType sType;
	QByteArray db_data;
};

/**
 * @brief Class that represents a miner's validation of a trasaction
 */
class MoedaNetwork::ResponseMiner : public MoedaNetwork::Response{
public:
	ResponseMiner();
	~ResponseMiner();

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
