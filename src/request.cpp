#include "request.hpp"

/*
 * request -------------------------------------------------
 */
const QString MCRequest::JSON_TYPE = "request";

MCRequest::MCRequest()
{}

MCRequest::~MCRequest()
{}

/*
 * request_db ---------------------------------------------
 */
const QString MCRequestDB::method = "request_db";

MCRequestDB::MCRequestDB()
{}

MCRequestDB::~MCRequestDB()
{}

bool MCRequestDB::read(const QJsonObject &json)
{
	if(this->method != json["method"].toString())
		return false;

	return true;
}

bool MCRequestDB::write(QJsonObject &json)
{
	json["type"]	= JSON_TYPE;
	json["method"]	= method;

	json["ip"]		= ip.toString();
	json["port"]	= port;

	return true;
}

void MCRequestDB::setIp(QHostAddress& ip)
{
	this->ip = ip;
}

void MCRequestDB::setPort(qint16 port)
{
	this->port = port;
}

/*
 * response -----------------------------------------------
 */
MCResponse::MCResponse()
{}

MCResponse::~MCResponse()
{}

/*
 * peer ---------------------------------------------------
 */
MCPeer::MCPeer()
{}

MCPeer::MCPeer(
		QHostAddress address,
		QString name,
		qint16 port)
{
	this->address	= address;
	this->port		= port;
	this->name		= name;
}

MCPeer::~MCPeer()
{}

QHostAddress MCPeer::getPeerAddress()
{
	return this->address;
}

qint16 MCPeer::getPeerPort()
{
	return this->port;
}

void MCPeer::read(const QJsonObject &json)
{
	this->address	= QHostAddress(json["peerAddress"].toString());
	this->port		= json["peerPort"].toInt();
	this->name		= json["peerAddress"].toString();
}

void MCPeer::write(QJsonObject &json)
{
	json["peerAddress"]	= this->address.toString();
	json["peerPort"]	= this->port;
	json["peerName"]	= this->name;
}
