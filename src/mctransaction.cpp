#include "mctransaction.hpp"

MCUser::MCUser(){

}

MCUser::MCUser(QString name, QString pKey){
	this->nome = name;
	this->publicKey = pKey;
}

MCTransaction::MCTransaction(int id, QString fKey, QString tKey, QString mKey, float v){
	this->value = v;
	this->fromKey = fKey;
	this->toKey = tKey;
	this->minKey = mKey;
	this->id = id;
}

const QString MCTransaction::JSON_TYPE = "transaction";
const float MCTransaction::VALUE_INTERVAL = 0.0001;

MCTransaction::MCTransaction()
{
	this->value = 0.0;
	this->id = 0.0;
}

MCTransaction::~MCTransaction()
{

}

bool MCTransaction::read(const QJsonObject &json)
{
	if(json["type"].toString() != JSON_TYPE)
		return false;

	this->id	= json["id"].toInt();
	this->value	= json["value"].toDouble();
	this->fromKey	= MCCrypto::base64Decode(json["fromKey"].toString());
	this->toKey	= MCCrypto::base64Decode(json["toKey"].toString());
	this->minKey	= MCCrypto::base64Decode(json["minKey"].toString());

	return true;
}

bool MCTransaction::write(QJsonObject &json)
{
	json["type"]	= JSON_TYPE;

	json["id"]	= this->id;
	json["value"]	= this->value;
	json["fromKey"]	= MCCrypto::base64Encode(this->fromKey);
	json["toKey"]	= MCCrypto::base64Encode(this->toKey);
	json["minKey"]	= MCCrypto::base64Encode(this->minKey);

	return true;
}


bool MCTransaction::operator==(MCTransaction& a)
{
	return	this->id == a.getId()
		&& this->value >= a.getValue()-VALUE_INTERVAL
		&& this->value <= a.getValue()+VALUE_INTERVAL
		&& this->fromKey == a.getFromKey()
		&& this->toKey == a.getToKey()
		&& this->minKey == a.getMinKey();
}

bool MCTransaction::operator!=(MCTransaction& a)
{
	return !(this->operator==(a));
}

QString MCTransaction::getFromKey() const
{
	return fromKey;
}

void MCTransaction::setFromKey(const QString &value)
{
	fromKey = value;
}

QString MCTransaction::getToKey() const
{
	return toKey;
}

void MCTransaction::setToKey(const QString &value)
{
	toKey = value;
}

QString MCTransaction::getMinKey() const
{
	return minKey;
}

void MCTransaction::setMinKey(const QString &value)
{
	minKey = value;
}

float MCTransaction::getValue() const
{
	return value;
}

void MCTransaction::setValue(double value)
{
	this->value = value;
}

int MCTransaction::getId() const
{
	return id;
}

void MCTransaction::setId(int value)
{
	id = value;
}
