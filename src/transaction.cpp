#include "transaction.hpp"

User::User(){

}

User::User(QString name, QString pKey){
	this->nome = name;
	this->publicKey = pKey;
}

Transaction::Transaction(int id, QString fKey, QString tKey, QString mKey, float v){
	this->value = v;
	this->fromKey = fKey;
	this->toKey = tKey;
	this->minKey = mKey;
	this->id = id;
}


Transaction::Transaction(){

}


Transaction::~Transaction(){

}

QString Transaction::getFromKey() const
{
	return fromKey;
}

void Transaction::setFromKey(const QString &value)
{
	fromKey = value;
}

QString Transaction::getToKey() const
{
	return toKey;
}

void Transaction::setToKey(const QString &value)
{
	toKey = value;
}

QString Transaction::getMinKey() const
{
	return minKey;
}

void Transaction::setMinKey(const QString &value)
{
	minKey = value;
}

float Transaction::getValue() const
{
	return value;
}

void Transaction::setValue(float value)
{
	value = value;
}

int Transaction::getId() const
{
	return id;
}

void Transaction::setId(int value)
{
	id = value;
}
