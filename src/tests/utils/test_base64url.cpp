#include "../../utils/base64url/base64url.h"
#include "../../unity/unity.h"
#include <iostream>
#include <string.h>

#ifdef MAX32552_A1
#include "../../debug.h"
#endif

void setUp(void) {
}
;
void tearDown(void) {
}
;

void testa_encode(void) {

	const char *f = "f";
	const char *fo = "fo";
	const char *foo = "foo";
	const char *foob = "foob";
	const char *fooba = "fooba";
	const char *foobar = "foobar";
	const char *num = "{\"msg\":1,\"nonce\":\"noncjgjjjgje\"}";

	static char encoded[500] = { 0 };

	int retval;

	retval = Base64URL::b64url_encode((uint8_t*) f, strlen(f), encoded,
			sizeof(encoded));

	TEST_ASSERT_GREATER_THAN_INT(0, retval);
	TEST_ASSERT_EQUAL_STRING("Zg", encoded);

	Base64URL::b64url_encode((uint8_t*) fo, strlen(fo), encoded,
			sizeof(encoded));
	TEST_ASSERT_EQUAL_STRING("Zm8", encoded);

	Base64URL::b64url_encode((uint8_t*) foo, strlen(foo), encoded,
			sizeof(encoded));
	TEST_ASSERT_EQUAL_STRING("Zm9v", encoded);

	Base64URL::b64url_encode((uint8_t*) foob, strlen(foob), encoded,
			sizeof(encoded));
	TEST_ASSERT_EQUAL_STRING("Zm9vYg", encoded);

	Base64URL::b64url_encode((uint8_t*) fooba, strlen(fooba), encoded,
			sizeof(encoded));
	TEST_ASSERT_EQUAL_STRING("Zm9vYmE", encoded);

	Base64URL::b64url_encode((uint8_t*) foobar, strlen(foobar), encoded,
			sizeof(encoded));
	TEST_ASSERT_EQUAL_STRING("Zm9vYmFy", encoded);

	Base64URL::b64url_encode((uint8_t*) num, strlen(num), encoded,
			sizeof(encoded));
	TEST_ASSERT_EQUAL_STRING("eyJtc2ciOjEsIm5vbmNlIjoibm9uY2pnampqZ2plIn0",
			encoded);
}

void testa_decode(void) {

	const char f[] = "Zg";
	const char fo[] = "Zm8";
	const char foo[] = "Zm9v";
	const char foob[] = "Zm9vYg";
	const char fooba[] = "Zm9vYmE";
	const char foobar[] = "Zm9vYmFy";
	const char num[] = "eyJtc2ciOjEsIm5vbmNlIjoibm9uY2pnampqZ2plIn0";

	static uint8_t decoded[500];
	int retval;

	retval = Base64URL::b64url_decode(f, strlen(f), decoded, sizeof(decoded));
	TEST_ASSERT_EQUAL_INT(1, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY("f", decoded, 1);

	retval = Base64URL::b64url_decode(fo, strlen(fo), decoded, sizeof(decoded));
	TEST_ASSERT_EQUAL_INT(2, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY("fo", decoded, 2);
//
	retval = Base64URL::b64url_decode(foo, strlen(foo), decoded,
			sizeof(decoded));
	TEST_ASSERT_EQUAL_INT(3, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY("foo", decoded, 3);

	retval = Base64URL::b64url_decode(foob, strlen(foob), decoded,
			sizeof(decoded));
	TEST_ASSERT_EQUAL_INT(4, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY("foob", decoded, 4);

	retval = Base64URL::b64url_decode(fooba, strlen(fooba), decoded,
			sizeof(decoded));
	TEST_ASSERT_EQUAL_INT(5, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY("fooba", decoded, 5);

	retval = Base64URL::b64url_decode(foobar, strlen(foobar), decoded,
			sizeof(decoded));
	TEST_ASSERT_EQUAL_INT(6, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY("foobar", decoded, 6);

	retval = Base64URL::b64url_decode(num, strlen(num), decoded,
			sizeof(decoded));
	TEST_ASSERT_EQUAL_INT(32, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY("{\"msg\":1,\"nonce\":\"noncjgjjjgje\"}",
			decoded, 32);
}

void test_decode_assinatura(void) {
	const char assinatura_b64[] = "ev9OXWiSj9I11tQ3_Nffy_O3ValujFdjs8sDxCPvhvo";
	uint8_t assinatura_raw[] = { 0x7a, 0xff, 0x4e, 0x5d, 0x68, 0x92, 0x8f, 0xd2,
			0x35, 0xd6, 0xd4, 0x37, 0xfc, 0xd7, 0xdf, 0xcb, 0xf3, 0xb7, 0x55,
			0xa9, 0x6e, 0x8c, 0x57, 0x63, 0xb3, 0xcb, 0x03, 0xc4, 0x23, 0xef,
			0x86, 0xfa };

	char assinatura_b64_2[sizeof(assinatura_b64)];
	uint8_t assinatura[32];

	int retval = Base64URL::b64url_decode(assinatura_b64,
			strlen(assinatura_b64), assinatura, sizeof(assinatura));
	TEST_ASSERT_EQUAL_INT(32, retval);
	TEST_ASSERT(memcmp(assinatura_raw, assinatura, 32) == 0);

	retval = Base64URL::b64url_encode(assinatura, sizeof(assinatura),
			assinatura_b64_2, strlen(assinatura_b64));
	TEST_ASSERT_EQUAL_INT(strlen(assinatura_b64), retval);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(assinatura_b64, assinatura_b64_2,
			strlen(assinatura_b64));
}

void test_encode_assinatura(void) {
	uint8_t assinatura_raw[] = { 0xd7, 0x02, 0xda, 0xfb, 0x0e, 0xe8, 0x92, 0x25,
			0x97, 0x39, 0x55, 0xbe, 0x4d, 0x3e, 0x8f, 0x47, 0x9e, 0xcb, 0x4a,
			0xdc, 0xdd, 0xa8, 0x12, 0x3d, 0x95, 0xe9, 0x35, 0x84, 0x14, 0x15,
			0x9b, 0xcc, 0x0c, 0x3a, 0x7b, 0x19, 0x07, 0xf0, 0x32, 0x26, 0x76,
			0x52, 0xb0, 0xd2 };
	char assinatura_b64[100];

	Base64URL::b64url_encode(assinatura_raw, sizeof(assinatura_raw),
			assinatura_b64, sizeof(assinatura_b64));

	printf("assinatura_b64: %s\n", assinatura_b64);
}

int main() {
	UNITY_BEGIN();

#ifdef MAX32552_A1
	DEBUG_PRINTF_INIT();
#endif

	RUN_TEST(testa_encode);
	RUN_TEST(testa_decode);
	RUN_TEST(test_decode_assinatura);
	RUN_TEST(test_encode_assinatura);

	return UNITY_END();
}
