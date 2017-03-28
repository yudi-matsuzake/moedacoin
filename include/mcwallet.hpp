#pragma once

#include <memory>
#include <cassert>

#include <string>
#include <vector>

#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/x509.h>

/**
  * Class that represents a wallet. It's function it's to provide
  * an abstraction to a cryto currency wallet and the most fundamental
  * methods of cryptography.
  */
class MCWallet
{
public:
	/**
	  * Constructs the TCWallet.
	  */
	MCWallet ();
	MCWallet (const char* wallet_file);
	MCWallet (const std::string& wallet_file);
	virtual ~MCWallet ();

	/** Generate public and private keys for `TCWallet`.
	  * Stores in `rsa`.
	  *
	  * @param bits	number in bits of the size of the keys
	  *
	  */
	void generateKeys (int bits = 2048);

	/** Writes the public key in the pkcs pem.
	  *
	  * @param filename	path to the file to the key to be written.
	  */
	void writePubKey (const std::string& filename);
	void writePubKey (const char* filename);

	/** Writes the public key in the pkcs pem format in a memory buffer.
	  * TODO: A more datailed description of writePubKeyToMemBuf.
	  *
	  * @param membuf	std::vector to hold the pem in the memory
	  */
	void writePubKeyToMemBuf (std::vector<char>& membuf);
	std::string writePubKeyToMemBuf();

	/** Writes the wallet in the pkcs pem format.
	  *
	  * @param filename	path to the file to the keys to be written.
	  */
	void write (const std::string& filename);
	void write (const char* filename);

	/** Writes the keus in the pkcs pem format in a memory buffer.
	  * TODO: A more datailed description of writeToMemBuf.
	  *
	  * @param membuf	std::vector to holds the pem in the memory
	  */
	void writeToMemBuf (std::vector<char>& membuf);

	/** Reads the public key in the pkcs pem format.
	  *
	  * @param filename	path to the file to the key to be read
	  */
	void readPubKey (const char* filename);
	void readPubKey (const std::string& filename);

	/** Reads the public key in the pkcs pem format from a memory buffer.
	  *
	  * @param membuf	null-terminated memory buffer
	  */
	void readPubKeyFromMem (const char* membuf);
	void readPubKeyFromMem (const std::string& membuf);

	/** Reads the keys in the pkcs pem format.
	  *
	  * @param filename	path to the file to the key to be read
	  */
	void read (const char* filename);
	void read (const std::string& filename);

	/** Reads the keys in pkcs pem format from a memory buffer.
	  * TODO: A more datailed description of readFromMem.
	  *
	  * @param membuf	null-terminated memory buffer
	  */
	void readFromMem (const char* membuf);
	void readFromMem (const std::string& membuf);

	/** Encrypts the `data` with the public key of the wallet and
	  * stores the encrypted data in `encrypted`.
	  *
	  * @param data		pointer to the data to be encrypted
	  * @param size		size of the data
	  * @param encrypted	pointer to write the encrypted data
	  */
	int pubEncrypt (const unsigned char* data, int size, unsigned char* encrypted);

	/** Decrypts the `encrypted` with the public key of the wallet
	  * and stores the decrypted data in the `decrypted`.
	  *
	  * @param encrypted	pointer to the data to be decrypted
	  * @param size		size of the data
	  * @param decrypted	pointer to write the decrypted data
	  */
	int pubDecrypt (const unsigned char* encrypted, int size, unsigned char* decrypted);

	/** Encrypts the `data` with the private key of the wallet
	  * and stores the decrypted data in the `decrypted`.
	  *
	  * @param data		pointer to the data to be encrypted
	  * @param size		size of the data
	  * @param decrypted	pointer to write the decrypted data
	  */
	int privEncrypt (const unsigned char* data, int size, unsigned char* encrypted);

	/** Decrypts the `encrypted` with the private key of the wallet
	  * and stores the decrypted data in the `decrypted`.
	  *
	  * @param encrypted	pointer to the data to be decrypted
	  * @param size		size of the data
	  * @param decrypted	pointer to write the decrypted data
	  */
	int privDecrypt (const unsigned char* encrypted, int size, unsigned char* decrypted);

	/** The size of the rsa structure.
	  * This is useful for the size of the decrypted data.
	  *
	  * @return	the size of the buffer needed for decryption
	  */
	int rsaSize (void);

	/** Signs the `msg` with size `msg_size`.
	  * TODO: A more datailed description of sign.
	  *
	  * @param msg		`msg` to be sign
	  * @param msg_size	size of `msg`
	  * @param signature	the buffer to the signature
	  *
	  * @return	the size of signature
	  */
	unsigned int sign (
			const unsigned char* msg,
			unsigned int msg_size,
			unsigned char* signature);

	/** Verifies the signature `signature` of `msg`.
	  * TODO: A more datailed description of signVerify.
	  *
	  * @param msg			`msg` to be verified
	  * @param msg_size		size of `msg`
	  * @param signature		signature of `msg`
	  * @param signature_size	size of `signature`
	  *
	  * @return true in case the sign is from this wallet, false otherwise
	  */
	bool signVerify (
			const unsigned char* msg,
			unsigned int msg_size,
			unsigned char* signature,
			unsigned int signature_size);

private:
	RSA* rsa;

	static const int padding;
	void read (std::unique_ptr<BIO, decltype(&BIO_free)>& pem_bio);
	void readPubKey (std::unique_ptr<BIO, decltype(&BIO_free)>& pem_bio);
	void write(std::unique_ptr<BIO, decltype(&BIO_free)>& pem_bio);
	void writePubKey (std::unique_ptr<BIO, decltype(&BIO_free)>& pem_bio);

};
