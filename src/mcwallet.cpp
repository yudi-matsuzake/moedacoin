#include "mcwallet.hpp"

const int MCWallet::padding = RSA_PKCS1_PADDING;

MCWallet::MCWallet () :
	rsa(RSA_new())
{
	RSA_blinding_on(rsa, NULL);
}

MCWallet::MCWallet (const char* wallet_file):
	MCWallet()
{
	read(wallet_file);
}

MCWallet::MCWallet (const std::string& wallet_file):
	MCWallet(wallet_file.c_str())
{
}

MCWallet::~MCWallet()
{
	RSA_free(rsa);
}

void MCWallet::generateKeys(int bits)
{
	std::unique_ptr<BIGNUM, decltype(&BN_free)> bignum(BN_new(), BN_free);

	assert(BN_set_word(bignum.get(), RSA_F4) == 1);
	assert(RSA_generate_key_ex(rsa, bits, bignum.get(), NULL) == 1);
	assert(RSA_check_key(rsa) == 1);
}

void MCWallet::writePubKey (const char* filename)
{
	std::unique_ptr<BIO, decltype(&BIO_free)>
		pem_file(BIO_new_file(filename, "w"), BIO_free);
	assert(pem_file.get() != NULL);

	writePubKey(pem_file);
}

void MCWallet::writePubKey (std::unique_ptr<BIO, decltype(&BIO_free)>& pem_bio)
{
	assert(PEM_write_bio_RSAPublicKey(pem_bio.get(), rsa) == 1);
}

void MCWallet::writePubKey (const std::string& filename)
{
	writePubKey(filename.c_str());
}

void MCWallet::writePubKeyToMemBuf (std::vector<char>& membuf)
{
	membuf.clear();
	membuf.resize(0);

	std::unique_ptr<BIO, decltype(&BIO_free)>
		membio(BIO_new(BIO_s_mem()), BIO_free);

	writePubKey(membio);

	char buffer[BUFSIZ];
	long n_data;

	while((n_data = BIO_read(membio.get(), buffer, BUFSIZ)) > 0)
		membuf.insert(std::end(membuf), buffer, buffer+n_data);
}

std::string MCWallet::writePubKeyToMemBuf()
{
	std::string str;

	std::unique_ptr<BIO, decltype(&BIO_free)>
		membio(BIO_new(BIO_s_mem()), BIO_free);

	writePubKey(membio);

	char buffer[BUFSIZ];
	long n_data;

	while((n_data = BIO_read(membio.get(), buffer, BUFSIZ)) > 0)
		str.append(buffer, n_data);

	return str;
}

void MCWallet::writeToMemBuf (std::vector<char>& membuf)
{
	membuf.clear();
	membuf.resize(0);

	std::unique_ptr<BIO, decltype(&BIO_free)>
		membio(BIO_new(BIO_s_mem()), BIO_free);

	write(membio);

	char buffer[BUFSIZ];
	long n_data;

	while((n_data = BIO_read(membio.get(), buffer, BUFSIZ)) > 0)
		membuf.insert(std::end(membuf), buffer, buffer+n_data);
}

void MCWallet::write(std::unique_ptr<BIO, decltype(&BIO_free)>& pem_bio)
{
	assert(pem_bio.get() != NULL);

	assert(PEM_write_bio_RSAPrivateKey(
				pem_bio.get(),
				rsa,
				NULL,
				NULL,
				0,
				NULL,
				NULL) == 1);
}

void MCWallet::write (const char* filename)
{
	std::unique_ptr<BIO, decltype(&BIO_free)>
		pem_file(BIO_new_file(filename, "w"), BIO_free);

	write(pem_file);
}

void MCWallet::write (const std::string& filename)
{
	write(filename.c_str());
}

void MCWallet::readPubKey (const char* filename)
{
	assert(filename != NULL);

	std::unique_ptr<BIO, decltype(&BIO_free)>
		pem_file(BIO_new_file(filename, "r"), BIO_free);

	assert(pem_file.get() != NULL);
	
	readPubKey(pem_file);
}

void MCWallet::readPubKey(const std::string& filename)
{
	readPubKey(filename.c_str());
}

void MCWallet::readPubKey(
		std::unique_ptr<BIO, decltype(&BIO_free)>& pem_bio)
{
	RSA_free(rsa);
	rsa = PEM_read_bio_RSAPublicKey(
			pem_bio.get(),
			NULL,
			NULL,
			NULL);
	assert(rsa != NULL);
}

void MCWallet::readPubKeyFromMem(const char* membuf)
{
	assert(membuf != NULL);
	std::unique_ptr<BIO, decltype(&BIO_free)>
		pem_file(BIO_new_mem_buf(membuf, -1), BIO_free);

	readPubKey(pem_file);
}

void MCWallet::readPubKeyFromMem(const std::string& membuf)
{
	readFromMem(membuf.c_str());
}

void MCWallet::read(const std::string& filename)
{
	read (filename.c_str());
}

void MCWallet::read(const char* filename)
{
	assert(filename != NULL);

	std::unique_ptr<BIO, decltype(&BIO_free)>
		pem_file(BIO_new_file(filename, "r"), BIO_free);

	read(pem_file);
}

void MCWallet::read (std::unique_ptr<BIO, decltype(&BIO_free)>& pem_bio)
{
	assert(pem_bio.get() != NULL);
	PEM_read_bio_RSAPrivateKey(
			pem_bio.get(),
			&(rsa),
			NULL,
			NULL);
	assert(RSA_check_key(rsa) == 1);
}

void MCWallet::readFromMem (const char* membuf)
{
	assert(membuf != NULL);

	std::unique_ptr<BIO, decltype(&BIO_free)>
		pem_file(BIO_new_mem_buf(membuf, -1), BIO_free);

	read(pem_file);
}

void MCWallet::readFromMem (const std::string& membuf)
{
	readFromMem(membuf.c_str());
}

int MCWallet::pubEncrypt (
		const unsigned char* data,
		int size,
		unsigned char* encrypted)
{
	assert(data != NULL);
	assert(size != 0);
	assert(encrypted != NULL);

	int n_data_encrypted = RSA_public_encrypt (
						size,
						data,
						encrypted,
						rsa,
						MCWallet::padding);
	assert(n_data_encrypted != -1);
	return n_data_encrypted;
}

int MCWallet::pubDecrypt (
		const unsigned char* encrypted,
		int size,
		unsigned char* decrypted)
{
	assert(encrypted != NULL);
	assert(size != 0);
	assert(decrypted != NULL);

	int n_data_decrypted = RSA_public_decrypt(
						size,
						encrypted,
						decrypted,
						rsa,
						MCWallet::padding);
	assert(n_data_decrypted != -1);
	return n_data_decrypted;
}

int MCWallet::privEncrypt (
		const unsigned char* data,
		int size,
		unsigned char* encrypted)
{
	assert(data != NULL);
	assert(size != 0);
	assert(encrypted != NULL);

	int n_data_encrypted = RSA_private_encrypt (
						size,
						data,
						encrypted,
						rsa,
						MCWallet::padding);

	assert(n_data_encrypted != -1);
	return n_data_encrypted;
}

int MCWallet::privDecrypt (
		const unsigned char* encrypted,
		int size,
		unsigned char* decrypted)
{
	assert(encrypted != NULL);
	assert(size != 0);
	assert(decrypted != NULL);

	int n_data_decrypted = RSA_private_decrypt(
						size,
						encrypted,
						decrypted,
						rsa,
						MCWallet::padding);
	assert(n_data_decrypted != -1);
	return n_data_decrypted;
}

int MCWallet::rsaSize (void)
{
	return RSA_size(rsa);
}

unsigned int MCWallet::sign (
		const unsigned char* msg,
		unsigned int msg_size,
		unsigned char* signature)
{
	assert(msg != NULL);
	assert(msg_size != 0);
	assert(signature != NULL);

	unsigned int size = 0;
	unsigned char hash[SHA512_DIGEST_LENGTH];

	assert(SHA512(msg, msg_size, hash) == hash);

	int signature_success =
		RSA_sign(NID_sha512, hash, SHA512_DIGEST_LENGTH, signature, &size, rsa);

	assert(signature_success == 1);

	return size;
}

bool MCWallet::signVerify (
		const unsigned char* msg,
		unsigned int msg_size,
		unsigned char* signature,
		unsigned int signature_size)
{
	assert(msg != NULL);
	assert(msg_size != 0);
	assert(signature != NULL);
	assert(signature_size != 0);

	unsigned char hash[SHA512_DIGEST_LENGTH];
	assert(SHA512(msg, msg_size, hash) == hash);

	return RSA_verify(
			NID_sha512,
			hash,
			SHA512_DIGEST_LENGTH,
			signature,
			signature_size,
			rsa);
}

std::string MCWallet::pubKeyToString (void)
{
	std::unique_ptr<BIO, decltype(&BIO_free)>
		membio(BIO_new(BIO_s_mem()), BIO_free);

	assert(BN_print(membio.get(), rsa->n) == 1);

	BUF_MEM* mem = NULL;
	BIO_get_mem_ptr(membio.get(), &mem);
	assert(mem != NULL);

	assert(mem && mem->data && mem->length);
	std::string s(mem->data, mem->length);

	return s;
}
