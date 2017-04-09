#include "mcsignature.hpp"

const QString MCSignature::JSON_TYPE("mcsignature");

MCSignature::MCSignature()
{
}


MCSignature::MCSignature(
			const unsigned char* msg,
			const unsigned int msgsize,
			const unsigned char* signature,
			const unsigned int sigsize)
	: msg(msg, msg+msgsize),
	  signature(signature, signature+sigsize)
{}

bool MCSignature::read(const QJsonObject& json)
{
	if(json["type"] != JSON_TYPE)
		return false;

	QString msg = json["msg"].toString();
	QString sig = json["signature"].toString();

	this->msg =
		MCCrypto::base64Decode(msg);

	this->signature =
		MCCrypto::base64Decode(sig);

	return true;
}

bool MCSignature::verifySignature(MCWallet &wallet)
{
	return wallet.signVerify(
				msg.data(),
				msg.size(),
				signature.data(),
				signature.size());
}

std::vector<unsigned char> MCSignature::getMsg() const
{
	return msg;
}

void MCSignature::setMsg(const std::vector<unsigned char> &value)
{
	msg = value;
}

std::vector<unsigned char> MCSignature::getSignature() const
{
	return signature;
}

void MCSignature::setSignature(const std::vector<unsigned char> &value)
{
	signature = value;
}


bool MCSignature::write(QJsonObject& json)
{
	json["type"]		= JSON_TYPE;
	json["msg"]			= MCCrypto::base64Encode(this->msg);
	json["signature"]	= MCCrypto::base64Encode(this->signature);
	return true;
}
