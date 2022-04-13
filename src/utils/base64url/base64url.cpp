#include "base64url.h"
#include <math.h>
#include <string.h>

static const char b64chars[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static int32_t b64urlinvs[] = { 62, -1, -1, 52, 53, 54, 55, 56, 57, 58, 59, 60,
		61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1,
		63, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
		42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1 };

int Base64URL::b64_isvalidchar(char c) {
	if (c >= '0' && c <= '9')
		return 1;
	if (c >= 'A' && c <= 'Z')
		return 1;
	if (c >= 'a' && c <= 'z')
		return 1;
	if (c == '-' || c == '_')
		return 1;
	return 0;
}

size_t Base64URL::b64url_encoded_size(size_t inlen) {
	float ret;
	ret = inlen;
	ret = (float) inlen * 4 / 3;
	return (int) ceil(ret);
}

size_t Base64URL::b64url_decoded_size(size_t inlen) {
	float ret;
	ret = (float) inlen / 4 * 3;
	return (int) floor(ret);
}

int Base64URL::b64url_encode(const uint8_t *in, size_t in_sz, char *out,
		size_t out_sz) {
	size_t i;
	size_t j;
	size_t v;

	if (Base64URL::b64url_encoded_size(in_sz) > out_sz)
		return 0;

	if (in == nullptr || in_sz == 0)
		return 0;

	for (i = 0, j = 0; i < in_sz; i += 3, j += 4) {

		v = in[i];
		v = i + 1 < in_sz ? v << 8 | in[i + 1] : v << 8;
		v = i + 2 < in_sz ? v << 8 | in[i + 2] : v << 8;

		out[j] = b64chars[(v >> 18) & 0x3F];
		out[j + 1] = b64chars[(v >> 12) & 0x3F];
		if (i + 1 < in_sz) {

			out[j + 2] = b64chars[(v >> 6) & 0x3F];
		}
		if (i + 2 < in_sz) {

			out[j + 3] = b64chars[v & 0x3F];
		}
	}

	out[Base64URL::b64url_encoded_size(in_sz)] = '\0';

	return Base64URL::b64url_encoded_size(in_sz);
}

int Base64URL::b64url_decode(const char *in, size_t in_sz, uint8_t *out,
		size_t out_sz) {
	size_t i;
	size_t j;
	int32_t v;

	if (Base64URL::b64url_decoded_size(in_sz) > out_sz)
		return 0;

	if (in == nullptr || in_sz == 0)
		return 0;

	for (i = 0; i < in_sz; i++) {
		if (!b64_isvalidchar(in[i])) {

			return 0;
		}
	}

	for (i = 0, j = 0; i < in_sz; i += 4, j += 3) {
		// a cada bloco de 4 characters (ou menos)
		v = b64urlinvs[in[i] - 45];
		uint32_t k = 1;
		if ((i + 1) < in_sz) {
			v = (v << 6) | b64urlinvs[in[i + 1] - 45];
			k++;
		}
		if ((i + 2) < in_sz) {
			v = (v << 6) | b64urlinvs[in[i + 2] - 45];
			k++;
		}
		if ((i + 3) < in_sz) {
			v = (v << 6) | b64urlinvs[in[i + 3] - 45];
			k++;
		}

		switch (k) {
		case 1:
			// 6 bits, teoricamente invalido
			out[j] = v;
			break;
		case 2:
			// 12 bits, deve truncar primeiro char
			out[j] = v >> 4;
			break;
		case 3:
			// 18 bits, deve truncar primeiro char
			out[j] = v >> 10;
			out[j + 1] = (v >> 2) & 0xFF;
			break;
		case 4:
			// 24 bits, sem truncamento
			out[j] = v >> 16;
			out[j + 1] = (v >> 8) & 0xFF;
			out[j + 2] = v & 0xFF;
		}
	}

	return Base64URL::b64url_decoded_size(in_sz);
}
