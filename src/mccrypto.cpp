#include "mccrypto.hpp"

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <openssl/sha.h>

namespace MCCrypto{

int base64_encode(
		const unsigned char* buffer,
		size_t length,
		char** base64)
{
	BIO *bio, *b64;
	BUF_MEM *buf_ptr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	//Ignore newlines - write everything in one line
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &buf_ptr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	*base64=buf_ptr->data;

	if(buf_ptr->length)
		(*base64)[buf_ptr->length] = '\0';
	return buf_ptr->length;
}

size_t calc_decode_length(const char* b64input)
{
	size_t len = strlen(b64input),
		padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len-1] == '=') //last char is =
		padding = 1;

	return (len*3)/4 - padding;
}

int base64_decode(
		char* base64,
		unsigned char** buffer,
		size_t* length)
{
	BIO *bio, *b64;

	size_t decode_len = calc_decode_length(base64);
	*buffer = (unsigned char*)malloc(decode_len + 1);
	(*buffer)[decode_len] = '\0';

	bio = BIO_new_mem_buf(base64, -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	//Do not use newlines to flush buffer
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	*length = BIO_read(bio, *buffer, strlen(base64));

	//length should equal decode_len, else something went horribly wrong
	assert(*length == decode_len);
	BIO_free_all(bio);

	return (0); //success
}

QString base64Encode(std::vector<unsigned char>& v)
{
	char* buffer64 = NULL;
	base64_encode(v.data(), v.size(), &buffer64);
	QString r(buffer64);
	delete buffer64;
	return r;
}

QString base64Decode(char* buffer64)
{
	unsigned char* buffer = NULL;
	size_t size;
	base64_decode(buffer64, &buffer, &size);
	QString r(buffer64);
	delete buffer;
	return r;
}

std::vector<unsigned char> base64Decode(QString& encode)
{
	unsigned char* buffer = NULL;

	QByteArray m = encode.toLatin1();

	size_t size;
	base64_decode(m.data(), &buffer, &size);

	std::vector<unsigned char> v(buffer, buffer+size);
	delete buffer;

	return v;
}

MCSignature signature(
		MCWallet &wallet,
		const unsigned char* msg,
		size_t len)
{
	std::unique_ptr<unsigned char[]> sign(
				new unsigned char[wallet.rsaSize()]);

	unsigned int sigsize = wallet.sign(msg, len, sign.get());

	return MCSignature(msg, len, sign.get(), sigsize);
}

} //namespace MCCrypto
