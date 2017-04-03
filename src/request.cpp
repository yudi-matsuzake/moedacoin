#include "request.hpp"

/*
 * request
 */
MoedaNetwork::Request::Request()
{}

MoedaNetwork::Request::~Request()
{}

/*
 * request_db
 */
const QString MoedaNetwork::RequestDB::method = "request_db";

MoedaNetwork::RequestDB::RequestDB()
{}

MoedaNetwork::RequestDB::~RequestDB()
{}

bool MoedaNetwork::RequestDB::read(const QJsonObject &json)
{
	if(this->method != json["method"].toString())
		return false;

	return true;
}

bool MoedaNetwork::RequestDB::write(QJsonObject &json)
{
	json["method"] = method;

	return true;
}

/*
 * response
 */
MoedaNetwork::Response::Response()
{}

MoedaNetwork::Response::~Response()
{}

/*
 * response_db
 */
const QString MoedaNetwork::ResponseDB::method = "response_db";

MoedaNetwork::ResponseDB::ResponseDB()
{}

MoedaNetwork::ResponseDB::~ResponseDB()
{}

bool MoedaNetwork::ResponseDB::read(const QJsonObject &json)
{
	if(this->method != json["method"].toString())
		return false;

	return true;
}

bool MoedaNetwork::ResponseDB::write(QJsonObject &json)
{
	json["method"] = method;

	return true;
}

/* class RequestMiner; */
/* class ResponseMiner; */
/* class RequestUpdate; */
