#include "mcrequest.hpp"

/*
 * address ---------------------------------------------------
 */
MCAddress::MCAddress()
	: address("0.0.0.0"),
	  port(0)
{}

MCAddress::MCAddress(
		QHostAddress address,
		int port)
	: address(address),
	  port(port)
{}

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

	if(!responseAddress.read(json["responseAddress"].toObject()))
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

MCAddress MCRequestDB::getResponseAddress() const
{
	return responseAddress;
}

/*
 * miner_request
 */
const QString MCRequestMiner::method = "miner_request";

MCRequestMiner::MCRequestMiner()
{
}

MCRequestMiner::MCRequestMiner(
		const MCTransaction& t,
		const MCSignature& s)
{
	this->transaction = t;
	this->signature = s;
}

MCRequestMiner::~MCRequestMiner()
{
}

MCSignature MCRequestMiner::getSignature() const
{
	return signature;
}

void MCRequestMiner::setSignature(const MCSignature &value)
{
	signature = value;
}

MCTransaction MCRequestMiner::getTransaction() const
{
	return transaction;
}

void MCRequestMiner::setTransaction(const MCTransaction &value)
{
	transaction = value;
}

MCAddress MCRequestMiner::getResponseAddress() const
{
	return responseAddress;
}

void MCRequestMiner::setResponseAddress(const MCAddress &value)
{
	responseAddress = value;
}

bool MCRequestMiner::read(const QJsonObject& json)
{
	if(!MCRequest::read(json))
		return false;

	if(json["method"] != this->method)
		return false;

	if(!this->signature.read(json["signature"].toObject()))
		return false;

	if(!this->transaction.read(json["transaction"].toObject()))
		return false;

	if(!this->responseAddress.read(json["responseAddress"].toObject()))
		return false;

	return true;
}

bool MCRequestMiner::write(QJsonObject& json)
{
	if(!MCRequest::write(json))
		return false;
	json["method"]		= this->method;

	QJsonObject jSignature;
	if(!this->signature.write(jSignature))
		return false;
	json["signature"] = jSignature;

	QJsonObject jTransaction;
	if(!this->transaction.write(jTransaction))
		return false;
	json["transaction"] = jTransaction;

	QJsonObject jAddress;
	if(!this->responseAddress.write(jAddress))
		return false;
	json["responseAddress"] = jAddress;

	return true;
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

MCResponseDB::MCResponseDB(MCRequestDB* request)
	: request(request)
{}

MCResponseDB::~MCResponseDB()
{
	if(request)
		delete request;
}

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

QString MCResponseDB::getDbData()
{
	return this->dbData;
}

void MCResponseDB::setDbData(QString s)
{
	this->dbData = s;
}

MCRequestDB* MCResponseDB::getRequest()
{
	return this->request;
}

/*
 * update_request
 */

const QString MCRequestUpdate::method = "update_request";

MCRequestUpdate::MCRequestUpdate()
{
}

MCRequestUpdate::MCRequestUpdate(
		const MCTransaction& t)//,
		//const MCSignature& s)
{
	this->transaction = t;
//	this->signature = s;
}

MCRequestUpdate::~MCRequestUpdate()
{
}

bool MCRequestUpdate::read(const QJsonObject& json){
	if(!MCRequest::read(json))
		return false;

	if (json["method"].toString() != this->method)
		return false;

	//if (!this->signature.read(json["signature"].toObject()))
	//	return false;


	if (!this->transaction.read(json["transaction"].toObject()))
		return false;

	return true;
}

bool MCRequestUpdate::write(QJsonObject& json){

	if (!MCRequest::write(json))
		return false;

	json["method"] = this->method;

	QJsonObject jTransaction;
	if (!this->transaction.write(jTransaction))
		return false;

	json["transaction"] = jTransaction;


	//QJsonObject jSignature;
	//if (!this->signature.write(jSignature))
	//	return false;

	//json["signature"] = jSignature;

	return true;
}

MCTransaction MCRequestUpdate::getTransaction() const
{
	return transaction;
}

/*
 * response_miner
 */
const QString MCResponseMiner::method = "response_miner";

MCResponseMiner::MCResponseMiner(MCRequestMiner* request)
	: accepted(false),
	  request(request)
{}

MCResponseMiner::~MCResponseMiner()
{}

bool MCResponseMiner::read(const QJsonObject &json)
{
	if(!MCResponse::read(json))
		return false;

	if(json["method"].toString() != this->method)
		return false;

	this->accepted = json["accepted"].toBool();

	QJsonObject jTransaction = json["transaction"].toObject();
	if(!this->transaction.read(jTransaction))
		return false;

	QJsonObject jSignature = json["signature"].toObject();
	if(!this->signature.read(jSignature))
		return false;

	this->reason = json["reason"].toString();

	return true;
}

bool MCResponseMiner::write(QJsonObject &json)
{
	if(!MCResponse::write(json))
		return false;

	json["method"]		= this->method;
	json["accepted"	]	= this->accepted;

	QJsonObject jTransaction;
	if(!this->transaction.write(jTransaction))
		return false;
	json["transaction"]	= jTransaction;

	QJsonObject jSignature;
	if(!this->signature.write(jSignature))
		return false;
	json["signature"] = jSignature;

	json["reason"] = this->reason;

	return true;
}

bool MCResponseMiner::getAccepted() const
{
	return accepted;
}

void MCResponseMiner::setAccepted(bool value)
{
	accepted = value;
}

MCSignature MCResponseMiner::getSignature() const
{
	return signature;
}

void MCResponseMiner::setSignature(const MCSignature &value)
{
	signature = value;
}

MCRequestMiner *MCResponseMiner::getRequest() const
{
	return request;
}

void MCResponseMiner::setRequest(MCRequestMiner *value)
{
	request = value;
}

QString MCResponseMiner::getReason() const
{
	return reason;
}

void MCResponseMiner::setReason(const QString &value)
{
	reason = value;
}

MCTransaction MCResponseMiner::getTransaction() const
{
	return transaction;
}

void MCResponseMiner::setTransaction(const MCTransaction &value)
{
	transaction = value;
}
