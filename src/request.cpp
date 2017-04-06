#include "request.hpp"

/*
 * address ---------------------------------------------------
 */
MCAddress::MCAddress()
{}

MCAddress::MCAddress(
		QHostAddress address,
		int port)
{
	this->address	= address;
	this->port	= port;
}

MCAddress::~MCAddress()
{}

QHostAddress MCAddress::getAddress()
{
	return this->address;
}

int MCAddress::getPort()
{
	return this->port;
}

void MCAddress::setAddress(QHostAddress& addr)
{
	this->address = addr;
}

void MCAddress::setPort(int port)
{
	this->port = port;
}

bool MCAddress::read(const QJsonObject &json)
{
	this->address	= QHostAddress(json["ip"].toString());
	this->port	= json["port"].toInt();
	
	return true;
}

bool MCAddress::write(QJsonObject &json)
{
	json["ip"]	= this->address.toString();
	json["port"]	= this->port;
	
	return true;
}

/*
 * request -------------------------------------------------
 */
const QString MCRequest::JSON_TYPE = "request";

MCRequest::MCRequest()
{}

MCRequest::~MCRequest()
{}

bool MCRequest::read(const QJsonObject& json)
{
	if(json["type"].toString() != JSON_TYPE)
		return false;

	return true;
}

void MCRequest::setPeer(QHostAddress& h, int port)
{
	peer.setAddress(h);
	peer.setPort(port);
}

bool MCRequest::write(QJsonObject& json)
{
	json["type"] = JSON_TYPE;

	QJsonObject peer;
	if(!this->peer.write(peer))
		return false;

	json["peer"] = peer;

	return true;
}

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
	if(!MCRequest::read(json))
		return false;

	if(json["method"].toString() != method)
		return false;

	if(!responseAddress.read(json))
		return false;

	return true;
}

bool MCRequestDB::write(QJsonObject &json)
{
	if(!MCRequest::write(json))
		return false;

	json["method"]	= method;

	QJsonObject responseAddr;
	responseAddress.write(responseAddr);
	json["responseAddress"] = responseAddr;

	return true;
}

void MCRequestDB::setListeningAddress(QHostAddress& ip)
{
	this->responseAddress.setAddress(ip);
}

void MCRequestDB::setListeningPort(int port)
{
	this->responseAddress.setPort(port);
}

/*
 * response -----------------------------------------------
 */
const QString MCResponse::JSON_TYPE = "response";

MCResponse::MCResponse()
{}

MCResponse::~MCResponse()
{}

bool MCResponse::read(const QJsonObject& json)
{
	if(json["type"].toString() != JSON_TYPE)
		return false;
	return true;
}

bool MCResponse::write(QJsonObject& json)
{
	json["type"] = JSON_TYPE;

	return true;
}

/*
 * response_db --------------------------------------------
 */
const QString MCResponseDB::method = "response_db";

MCResponseDB::MCResponseDB()
{}

MCResponseDB::~MCResponseDB()
{}

bool MCResponseDB::read(const QJsonObject &json)
{
	if(!MCResponse::read(json))
		return false;

	if(json["method"].toString() != this->method)
		return false;

	this->dbData = json["dbData"].toString();

	return true;
}

bool MCResponseDB::write(QJsonObject &json)
{
	if(!MCResponse::write(json))
		return false;

	json["method"] = this->method;
	json["dbData"] = this->dbData;

	return true;
}
