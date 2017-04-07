#ifndef MCCRYPTO_HPP
#define MCCRYPTO_HPP

#include "mcsignature.hpp"
#include "mcwallet.hpp"

class MCSignature;
namespace MCCrypto{

	/**
	 * @brief	Encode and decode in base64
	 * @param	msg to encode/decode
	 * @return	the decoded/encode msg
	 */
	QString base64Encode(std::vector<unsigned char>& v);
	QString base64Encode(const QString& v);

	std::vector<unsigned char> base64Decode(QString& encode);
	QString base64Decode(const QString& v);

	/**
	 * @brief	simple wrapper for sign method of `wallet`
	 * @param	wallet
	 * @param	msg
	 * @param	len
	 * @return	Class `MCSignature` that wraps a MCwallet sign
	 */
	MCSignature signature(
		MCWallet &wallet,
		const unsigned char* msg,
		size_t len);

}

#endif // MCCRYPTO_HPP
