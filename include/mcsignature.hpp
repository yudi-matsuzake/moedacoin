#ifndef MCSIGNATURE_HPP
#define MCSIGNATURE_HPP

#include <vector>

#include <QJsonObject>

#include "mccrypto.hpp"
#include "mcwallet.hpp"

/**
 * @brief Wraps a MCWallet signature
 */
class MCSignature
{
public:
	MCSignature();
	MCSignature(
			const unsigned char* msg,
			const unsigned int msgsize,
			const unsigned char* signature,
			const unsigned int sigsize);

	bool read(const QJsonObject& json);
	bool write(QJsonObject& json);

	/**
	 * @brief verify `msg` authorship
	 * @param wallet
	 * @return true if msg is signed by `wallet`
	 */
	bool verifySignature(MCWallet& wallet);

private:
	std::vector<unsigned char> msg;
	std::vector<unsigned char> signature;
	static const QString JSON_TYPE;
};

#endif // MCSIGNATURE_HPP
