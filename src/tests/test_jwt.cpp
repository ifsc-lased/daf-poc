#define private public // para testar o método privado estático da classe
#include "../cripto/cripto_daf.h"
#include "../utils/jwt/jwt.h"
#include "../unity/unity.h"
#include "../debug.h"
#include <iostream>

void setUp(void) {
}
;
void tearDown(void) {
}
;

static char token1[] =
		"eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImp3ayI6eyJrdHkiOiJSU0EiLCJuIjoicWhaX1BlM2ZTNVpSSE1TcTJJLWdFWVpfS2RybGNySHRIdE94Sl82X2owc0t0UG9HbjgtSVM3UGxYXzJ4NWVBTzRZWWNaYU9BRTFsV2pXZGJqVXZVUjI2ekJ6dW5zOF80U0RzbVdpTXBJcDZQWkFoYk9HTFZGOENmd3hkRjY1bEp2T3RIeE5jSmoxQzZkMS1hUDZNRDBmMGxNVFhJdmk2Qm9veEpvbEx3bHhzdzdmYzVFa1BVajc4NWdaYWFlb2pNZGwwdU1BTUw1S0RsbVA5anBLdXVDdk9hVnJoVXRENF9mSDZFOXAxbGstaDY3QVU3VnphTnM4MEVsVEVvX1pvWXg0Y0JCeUFWWDNld1N6WTI5YUg2OVBrellTb1B6WWk5a2Q4em5OTXBfUlBoTHRraEtnNEpWeU5ndnJyMDIxNmJ5U2pZZkw0WnBuTkdSbEFZcDk1WEZRIiwiZSI6IkFRQUIifX0.eyJpZERBRiI6InVnaHlyY0RZQlcwemFJR0pHM1o2aXciLCJjb250IjowLCJub25jZSI6Im43TlNlVDNCLWxJZklUbWFPaDdSY3cifQ.GTTITLZnGCGi16EuqVfIeRArnDf8en7VOqQAwVeCfrGpLclZn3JmZ6rWMW7SRg5HeGIdGd2XImM5phmfJRK2kXi1OfsBlMyZvGVV5TTwbJSaqsEsFtnzkIYDpnDRwFoctcOqGIo8An4MSSOedXmIge_DIqTNnRDg_TsvHnF7Ijf06gaSo-Fn2aTTp9GPlg5-vd1Hcj20X8rFVOHDFq7I3ETiAUMncRHRL9oEXhH1vuFZwkzgzSaXuunULRp-A-6ZFYQaALB4Cwo6f1BEJt6DaRL5ch5I-E7Cb4cOZ0hZLT-k1fXKNd-enyqSpcBriOhmi2dUEUqh-V9jiig_chmxqg";

static char token2[] =
		"eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImp3ayI6eyJrdHkiOiJSU0EiLCJuIjoiN3VQZFNzYXNCc2NsYmtIcmNpS1h1ZTRmZnRBSnZrQ2k2NnBNcE1xZWJTZnY4TzhpbjJaV3RnSUNGQm40dFRMcDZVWENpbjItUmlncVNfU004STQ1NVpRREVWZ3d1S3N2b2pVVVVURWxsamN5b3FINHllcXZJZVJuS2txbE4xMWJINEM2UzlBR3BQSFNpdE5ldXNBWXR2YUFqQ1NZOEh2QjN4VG0yTDN1ZnppdzVVM1dueGZvQXNZVUtodkRsVDFfSHk0N3ZoV2xmRHh2ZExOY09xRDVDZGItRlJuVUpKOHBWNXlOcHlOcGpMb183OFhUakZ1b1NoY3QzODJNakNDa1BBN2M3SmgwTUhUVnNPZHlmZTE5YzNKYVRxYWxzZ0FpRk5CMkNnX1p0dkNROUtRS3ZRM1hCejFiMlNoRTVmWS0tWTl2R2VRYlRkS0R4SzBQcW9FeTBqU1RFRWdISnFIWVlwOE80aXVBZS03aWFfZ0M3QnhCUVZmVUhXYVhJWm1QUFFNczgzSFNhUlBRRjNMX3IyX2ExMzR1MExJZ25TVEwwb1R2dUk0aWdZUFFMeTdYbWUtYWZGNXJveGN0NmpkVzhCaGdId1l2WjFGSkROS0ozaFg4WVlidjVxYzNpMFJBRmlpaUs1QklsOVNOMGZEU2Q3dFhDSUJpZUIzelpqRzAtWjZaSTNLekVpS0xyRzRXUHAzV1RCeFgzWWJpZWJQN1YtU1cwUnFVSm1neGNxeHcwMXF6dlpRdWNHOW9TTVE3RDBXMnlQVHJaYTR0MmZKTkNMOWdRNHh1UThhSGxrTFJ0c0s4VXUwTk12Rzc4dWdha05sMEFack83NHV6SlVLV0J3ek0zUEtqejlLNmZfUUVpM1M4N1NiVFowcW1DTm1rY2VaazZUbmxkLTgiLCJlIjoiQVFBQiJ9fQ.eyJqd3QiOiJleUowZVhBaU9pSktWMVFpTENKaGJHY2lPaUpTVXpJMU5pSXNJbXAzYXlJNmV5SnJkSGtpT2lKU1UwRWlMQ0p1SWpvaWNXaGFYMUJsTTJaVE5WcFNTRTFUY1RKSkxXZEZXVnBmUzJSeWJHTnlTSFJJZEU5NFNsODJYMm93YzB0MFVHOUhiamd0U1ZNM1VHeFlYeko0TldWQlR6UlpXV05hWVU5QlJURnNWMnBYWkdKcVZYWlZVakkyZWtKNmRXNXpPRjgwVTBSemJWZHBUWEJKY0RaUVdrRm9ZazlIVEZaR09FTm1kM2hrUmpZMWJFcDJUM1JJZUU1alNtb3hRelprTVMxaFVEWk5SREJtTUd4TlZGaEpkbWsyUW05dmVFcHZiRXgzYkhoemR6ZG1ZelZGYTFCVmFqYzROV2RhWVdGbGIycE5aR3d3ZFUxQlRVdzFTMFJzYlZBNWFuQkxkWFZEZGs5aFZuSm9WWFJFTkY5bVNEWkZPWEF4YkdzdGFEWTNRVlUzVm5waFRuTTRNRVZzVkVWdlgxcHZXWGcwWTBKQ2VVRldXRE5sZDFONldUSTVZVWcyT1ZCcmVsbFRiMUI2V1drNWEyUTRlbTVPVFhCZlVsQm9USFJyYUV0bk5FcFdlVTVuZG5KeU1ESXhObUo1VTJwWlprdzBXbkJ1VGtkU2JFRlpjRGsxV0VaUklpd2laU0k2SWtGUlFVSWlmWDAuZXlKcFpFUkJSaUk2SW5WbmFIbHlZMFJaUWxjd2VtRkpSMHBITTFvMmFYY2lMQ0pqYjI1MElqb3dMQ0p1YjI1alpTSTZJbTQzVGxObFZETkNMV3hKWmtsVWJXRlBhRGRTWTNjaWZRLkdUVElUTFpuR0NHaTE2RXVxVmZJZVJBcm5EZjhlbjdWT3FRQXdWZUNmckdwTGNsWm4zSm1aNnJXTVc3U1JnNUhlR0lkR2QyWEltTTVwaG1mSlJLMmtYaTFPZnNCbE15WnZHVlY1VFR3YkpTYXFzRXNGdG56a0lZRHBuRFJ3Rm9jdGNPcUdJbzhBbjRNU1NPZWRYbUlnZV9ESXFUTm5SRGdfVHN2SG5GN0lqZjA2Z2FTby1GbjJhVFRwOUdQbGc1LXZkMUhjajIwWDhyRlZPSERGcTdJM0VUaUFVTW5jUkhSTDlvRVhoSDF2dUZad2t6Z3pTYVh1dW5VTFJwLUEtNlpGWVFhQUxCNEN3bzZmMUJFSnQ2RGFSTDVjaDVJLUU3Q2I0Y09aMGhaTFQtazFmWEtOZC1lbnlxU3BjQnJpT2htaTJkVUVVcWgtVjlqaWlnX2NobXhxZyJ9.J2FgpnC7THqu9Yybqfo19gsPOhJ9QMwMrbtfi8Zh8wMssjfbt-torN0fN6fFmyXUstGdIMO48x8ZBMddmHQ8aX536Abo8xIqvL6WEwECOsT3YqIH0epXF_I7g8JA982Ev0lsiqyLbWipqeeJWfZcYW0vdTRw8EP9amwRmqkrqxgcltY0A6FgK4b_r9KrtvnFFp9RXnB29IvMK3YTAuoKxZQ4zwux85x0CLu_nJdNQFBuo5BvPUiJdRi7p041hlQyCKQCogwdsiAKsNM1xDmTVY9iiFWPMzIup3oQ6O7fQtxPkmY8oecsYBUpVnw-4TJ1S-ujxcyrimMIwB6z-DVIstqNPH0cZb1gZAgCZwM2363epumS8sz3BiZq63jIURqzPhvb8-7RI4tz_FCBWRViRLOPOwLWWMujeBr_VbgA_KA_rT4WO83HlAd61TALAZByo9JIJQW9FtzSEac6GAPVffow72bygGdnMujJSqrZMGI3Njo02HBvLP_xOPKHcsV4AVkgWCrYabFXhntDrSVAI-eIueUCiZH9-_S1WNsW80-lSb8bv4Womepy6WBo2MVIYuO5BS_zGXbbVL-FT6ZyLj9IsxKJ-24oKYhcWP7x4cRPnoS1jVGxBmvqlSU1hugdKnOhNkWwPLxZ0ZwtmmLGLOVxC6F5ktoLFZ50O8hk5kE";

static char token3[] =
		"eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJub25jZSI6ImFoZ2R2eGMxMjNmZ3h2Y2dkZXQxZjQifQ.horLPJEueSLBdahjFekd4QoTL4TafHMHCd_bymNPEm0";

static uint8_t d_p256[] = { 0xfb, 0xbe, 0xbd, 0xf9, 0x34, 0x80, 0x69, 0xb3,
		0x27, 0xff, 0xa7, 0x14, 0xdf, 0x81, 0x85, 0x3d, 0xe8, 0xd7, 0x57, 0x42,
		0xb1, 0xa1, 0xbb, 0x86, 0xb0, 0x76, 0x3d, 0x4a, 0xff, 0x9b, 0x04, 0x35 };

static uint8_t x_p256[] = { 0x5d, 0x72, 0xa7, 0xc7, 0x39, 0x5a, 0xd3, 0x12,
		0xfa, 0xca, 0xa8, 0x92, 0x03, 0x6a, 0x21, 0x0a, 0x27, 0x1f, 0xca, 0xce,
		0xf8, 0x9a, 0x70, 0xfa, 0x73, 0xa5, 0x58, 0x1b, 0xb6, 0x5a, 0xcc, 0xd1 };

static uint8_t y_p256[] = { 0xdc, 0x1e, 0x2c, 0x89, 0xf0, 0xdc, 0xd0, 0xc7,
		0xb1, 0x89, 0xc0, 0xe8, 0xe2, 0x27, 0x9d, 0xd4, 0xf3, 0xd0, 0x2b, 0xf9,
		0x55, 0x86, 0xf2, 0xb3, 0xa8, 0xa0, 0xb6, 0x73, 0xd8, 0xca, 0xa7, 0x4e };


static char token_ecc_p256[] =
		"eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzI1NiIsImp3ayI6eyJjcnYiOiJQLTI1NiIsIngiOiJYWEtueHpsYTB4TDZ5cWlTQTJvaENpY2Z5czc0bW5ENmM2VllHN1phek5FIiwieSI6IjNCNHNpZkRjME1leGljRG80aWVkMVBQUUtfbFZodkt6cUtDMmM5aktwMDQiLCJrdHkiOiJFQyJ9fQ.eyJ1c2VybmFtZSI6IkpvaG4gRG9lIn0.KNrsPeASOjmWMjQDw_hsNoIkF2jZuQujeEWEmv0EI_nbsvp9fXwODFQ3EMDnQfT0lRJYcvsCkaYs_wlw_4uENQ";

static uint8_t d_p384[] = { 0x5e, 0xde, 0x69, 0xee, 0xfa, 0x64, 0xcf, 0xea,
		0x28, 0x06, 0xed, 0xac, 0x72, 0x4f, 0x95, 0x9f, 0xb3, 0xb9, 0x3f, 0x35,
		0xa5, 0x9a, 0x9b, 0xe6, 0xc8, 0x38, 0xcf, 0x48, 0xba, 0x05, 0x39, 0xf4,
		0x56, 0xaa, 0xc9, 0xd5, 0xdd, 0x2f, 0x86, 0x76, 0x74, 0xf3, 0x7c, 0xdd,
		0x37, 0x85, 0x08, 0xd3

};

static uint8_t x_p384[] = { 0x9c, 0xbe, 0xe0, 0x8e, 0x2a, 0x66, 0xbf, 0xcf,
		0xcf, 0xc5, 0xe4, 0x0a, 0x3b, 0xee, 0x93, 0xcd, 0x30, 0xc8, 0x5b, 0xb7,
		0xf5, 0xff, 0xcd, 0x68, 0xe8, 0x7c, 0x85, 0xb1, 0x78, 0x24, 0x44, 0xf0,
		0x6a, 0xb8, 0xe5, 0xb4, 0xf8, 0x18, 0x8f, 0x41, 0xbd, 0xf1, 0x31, 0x8d,
		0x26, 0x62, 0x26, 0xa3 };

static uint8_t y_p384[] = { 0xda, 0xac, 0x4b, 0x0b, 0x15, 0xe4, 0x1e, 0x2f,
		0x03, 0x66, 0xc1, 0x9a, 0xf9, 0x42, 0x3e, 0xb8, 0x2b, 0x28, 0x70, 0x68,
		0x85, 0xbc, 0xbb, 0x54, 0x7c, 0x4a, 0x5f, 0x0a, 0xab, 0xf9, 0x1f, 0x8d,
		0xe4, 0x01, 0xe9, 0x4a, 0xeb, 0x1b, 0x6f, 0xe1, 0xd1, 0xc5, 0xf4, 0xc4,
		0x4b, 0x70, 0xd7, 0x35

};

static char token_ecc_p384[] =
		"eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzM4NCIsImp3ayI6eyJjcnYiOiJQLTM4NCIsIngiOiJuTDdnamlwbXY4X1B4ZVFLTy02VHpURElXN2YxXzgxbzZIeUZzWGdrUlBCcXVPVzAtQmlQUWIzeE1ZMG1ZaWFqIiwieSI6IjJxeExDeFhrSGk4RFpzR2EtVUktdUNzb2NHaUZ2THRVZkVwZkNxdjVINDNrQWVsSzZ4dHY0ZEhGOU1STGNOYzEiLCJrdHkiOiJFQyJ9fQ.eyJ1c2VybmFtZSI6IkpvaG4gRG9lIn0.r2JKqO-RTHdZEQ2oXriDlDVc7T3InWacW1yHgY59Ri-2I_4UGTcZ-baj6t4tqPeEXuZMzHAzQvnachjBCcNfJnsodkXrD661qfunFYB3RLCWhqYpuPLqZJUHTK2ekdFA";

static char tokenjwe_bytes_random[] =
		"eyJhbGciOiJFQ0RILUVTIiwiZW5jIjoiQTEyOENCQy1IUzI1NiIsImVwayI6eyJjcnYiOiJQLTI1NiIsImt0eSI6IkVDIiwieCI6IjU3TmlXck5aeWRDV09BcEdQMTk5LUF1QlhFNjh6Q3hhTHJ5Vmhza09EdnciLCJ5IjoiaDl1SXU3ZHJuRnlnWHdkd2NsZkRmUDJsbEV4RHVwUHY0T0VrM2lLbmFPSSJ9fQ..VScndIosIcheUF91zOVxGw.IZrG1sFdUQNPsS2A8iOQN-kwwd47X3lFKVNXgiujLupIGXnIz8fi7HzizPm-p-jprCmqdUorNBOAgQI0stOvcg.IWFCkID6fFMc9vWxUSHvUw";

uint8_t chavesef_arr[] = { 0x21, 0x9a, 0xc6, 0xd6, 0xc1, 0x5d, 0x51, 0x03, 0x4f,
		0xb1, 0x2d, 0x80, 0xf2, 0x23, 0x90, 0x37, 0xe9, 0x30, 0xc1, 0xde, 0x3b,
		0x5f, 0x79, 0x45, 0x29, 0x53, 0x57, 0x82, 0x2b, 0xa3, 0x2e, 0xea, 0x48,
		0x19, 0x79, 0xc8, 0xcf, 0xc7, 0xe2, 0xec, 0x7c, 0xe2, 0xcc, 0xf9, 0xbe,
		0xa7, 0xe8, 0xe9, 0xac, 0x29, 0xaa, 0x75, 0x4a, 0x2b, 0x34, 0x13, 0x80,
		0x81, 0x02, 0x34, 0xb2, 0xd3, 0xaf, 0x72 };

static char tokenjwe_sequencia_numerica[] =
		"eyJhbGciOiJFQ0RILUVTIiwiZW5jIjoiQTEyOENCQy1IUzI1NiIsImVwayI6eyJjcnYiOiJQLTI1NiIsImt0eSI6IkVDIiwieCI6IjU3TmlXck5aeWRDV09BcEdQMTk5LUF1QlhFNjh6Q3hhTHJ5Vmhza09EdnciLCJ5IjoiaDl1SXU3ZHJuRnlnWHdkd2NsZkRmUDJsbEV4RHVwUHY0T0VrM2lLbmFPSSJ9fQ..VScndIosIcheUF91zOVxGw.MTIzNDU2Nzg5MTIzNDU2Nzg5MTIzNDU2Nzg5MTIzNDU2Nzg5MTIzNDU2Nzg5MTIzNDU2Nzg5MTIzNDU2Nzg5MQ.IWFCkID6fFMc9vWxUSHvUw";

uint8_t chavesef_numerica[] =
		"1234567891234567891234567891234567891234567891234567891234567891";

void test_verificaTokenJWE(void) {
	static uint8_t payload[64];
	static uint8_t payload2[64];
	JWT::u_chave_privada_t priv;

	JWT::VALIDACAO res = JWT::verificaTokenJWE(tokenjwe_bytes_random,
			strlen(tokenjwe_bytes_random), &priv, payload, 64);

	TEST_ASSERT_EQUAL_INT(JWT::VALIDACAO::ASSINATURA_VALIDA, res);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(chavesef_arr, payload, 64);

	res = JWT::verificaTokenJWE(tokenjwe_sequencia_numerica,
			strlen(tokenjwe_sequencia_numerica), &priv, payload2, 64);

	TEST_ASSERT_EQUAL_INT(JWT::VALIDACAO::ASSINATURA_VALIDA, res);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(chavesef_numerica, payload2, 64);
}

void test_getParametrosToken(void) {
	JWT::fields_jws_t campos;
	enum JWT::ERRO retval;

	retval = JWT::getParametrosToken(token1, strlen(token1), &campos);
	TEST_ASSERT_EQUAL_INT(JWT::ERRO::SUCESSO, retval);

	TEST_ASSERT_EQUAL_INT(543, campos.header.sz);
	TEST_ASSERT_EQUAL_INT(102, campos.payload.sz);
	TEST_ASSERT_EQUAL_INT(342, campos.assinatura.sz);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(
			"eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImp3ayI6eyJrdHkiOiJSU0EiLCJuIjoicWhaX1BlM2ZTNVpSSE1TcTJJLWdFWVpfS2RybGNySHRIdE94Sl82X2owc0t0UG9HbjgtSVM3UGxYXzJ4NWVBTzRZWWNaYU9BRTFsV2pXZGJqVXZVUjI2ekJ6dW5zOF80U0RzbVdpTXBJcDZQWkFoYk9HTFZGOENmd3hkRjY1bEp2T3RIeE5jSmoxQzZkMS1hUDZNRDBmMGxNVFhJdmk2Qm9veEpvbEx3bHhzdzdmYzVFa1BVajc4NWdaYWFlb2pNZGwwdU1BTUw1S0RsbVA5anBLdXVDdk9hVnJoVXRENF9mSDZFOXAxbGstaDY3QVU3VnphTnM4MEVsVEVvX1pvWXg0Y0JCeUFWWDNld1N6WTI5YUg2OVBrellTb1B6WWk5a2Q4em5OTXBfUlBoTHRraEtnNEpWeU5ndnJyMDIxNmJ5U2pZZkw0WnBuTkdSbEFZcDk1WEZRIiwiZSI6IkFRQUIifX0",
			campos.header.b64, campos.header.sz);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(
			"eyJpZERBRiI6InVnaHlyY0RZQlcwemFJR0pHM1o2aXciLCJjb250IjowLCJub25jZSI6Im43TlNlVDNCLWxJZklUbWFPaDdSY3cifQ",
			campos.payload.b64, campos.payload.sz);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(
			"GTTITLZnGCGi16EuqVfIeRArnDf8en7VOqQAwVeCfrGpLclZn3JmZ6rWMW7SRg5HeGIdGd2XImM5phmfJRK2kXi1OfsBlMyZvGVV5TTwbJSaqsEsFtnzkIYDpnDRwFoctcOqGIo8An4MSSOedXmIge_DIqTNnRDg_TsvHnF7Ijf06gaSo-Fn2aTTp9GPlg5-vd1Hcj20X8rFVOHDFq7I3ETiAUMncRHRL9oEXhH1vuFZwkzgzSaXuunULRp-A-6ZFYQaALB4Cwo6f1BEJt6DaRL5ch5I-E7Cb4cOZ0hZLT-k1fXKNd-enyqSpcBriOhmi2dUEUqh-V9jiig_chmxqg",
			campos.assinatura.b64, campos.assinatura.sz);

	retval = JWT::getParametrosToken(token2, strlen(token2), &campos);
	TEST_ASSERT_EQUAL_INT(JWT::ERRO::SUCESSO, retval);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(
			"eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImp3ayI6eyJrdHkiOiJSU0EiLCJuIjoiN3VQZFNzYXNCc2NsYmtIcmNpS1h1ZTRmZnRBSnZrQ2k2NnBNcE1xZWJTZnY4TzhpbjJaV3RnSUNGQm40dFRMcDZVWENpbjItUmlncVNfU004STQ1NVpRREVWZ3d1S3N2b2pVVVVURWxsamN5b3FINHllcXZJZVJuS2txbE4xMWJINEM2UzlBR3BQSFNpdE5ldXNBWXR2YUFqQ1NZOEh2QjN4VG0yTDN1ZnppdzVVM1dueGZvQXNZVUtodkRsVDFfSHk0N3ZoV2xmRHh2ZExOY09xRDVDZGItRlJuVUpKOHBWNXlOcHlOcGpMb183OFhUakZ1b1NoY3QzODJNakNDa1BBN2M3SmgwTUhUVnNPZHlmZTE5YzNKYVRxYWxzZ0FpRk5CMkNnX1p0dkNROUtRS3ZRM1hCejFiMlNoRTVmWS0tWTl2R2VRYlRkS0R4SzBQcW9FeTBqU1RFRWdISnFIWVlwOE80aXVBZS03aWFfZ0M3QnhCUVZmVUhXYVhJWm1QUFFNczgzSFNhUlBRRjNMX3IyX2ExMzR1MExJZ25TVEwwb1R2dUk0aWdZUFFMeTdYbWUtYWZGNXJveGN0NmpkVzhCaGdId1l2WjFGSkROS0ozaFg4WVlidjVxYzNpMFJBRmlpaUs1QklsOVNOMGZEU2Q3dFhDSUJpZUIzelpqRzAtWjZaSTNLekVpS0xyRzRXUHAzV1RCeFgzWWJpZWJQN1YtU1cwUnFVSm1neGNxeHcwMXF6dlpRdWNHOW9TTVE3RDBXMnlQVHJaYTR0MmZKTkNMOWdRNHh1UThhSGxrTFJ0c0s4VXUwTk12Rzc4dWdha05sMEFack83NHV6SlVLV0J3ek0zUEtqejlLNmZfUUVpM1M4N1NiVFowcW1DTm1rY2VaazZUbmxkLTgiLCJlIjoiQVFBQiJ9fQ",
			campos.header.b64, 998);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(
			"eyJqd3QiOiJleUowZVhBaU9pSktWMVFpTENKaGJHY2lPaUpTVXpJMU5pSXNJbXAzYXlJNmV5SnJkSGtpT2lKU1UwRWlMQ0p1SWpvaWNXaGFYMUJsTTJaVE5WcFNTRTFUY1RKSkxXZEZXVnBmUzJSeWJHTnlTSFJJZEU5NFNsODJYMm93YzB0MFVHOUhiamd0U1ZNM1VHeFlYeko0TldWQlR6UlpXV05hWVU5QlJURnNWMnBYWkdKcVZYWlZVakkyZWtKNmRXNXpPRjgwVTBSemJWZHBUWEJKY0RaUVdrRm9ZazlIVEZaR09FTm1kM2hrUmpZMWJFcDJUM1JJZUU1alNtb3hRelprTVMxaFVEWk5SREJtTUd4TlZGaEpkbWsyUW05dmVFcHZiRXgzYkhoemR6ZG1ZelZGYTFCVmFqYzROV2RhWVdGbGIycE5aR3d3ZFUxQlRVdzFTMFJzYlZBNWFuQkxkWFZEZGs5aFZuSm9WWFJFTkY5bVNEWkZPWEF4YkdzdGFEWTNRVlUzVm5waFRuTTRNRVZzVkVWdlgxcHZXWGcwWTBKQ2VVRldXRE5sZDFONldUSTVZVWcyT1ZCcmVsbFRiMUI2V1drNWEyUTRlbTVPVFhCZlVsQm9USFJyYUV0bk5FcFdlVTVuZG5KeU1ESXhObUo1VTJwWlprdzBXbkJ1VGtkU2JFRlpjRGsxV0VaUklpd2laU0k2SWtGUlFVSWlmWDAuZXlKcFpFUkJSaUk2SW5WbmFIbHlZMFJaUWxjd2VtRkpSMHBITTFvMmFYY2lMQ0pqYjI1MElqb3dMQ0p1YjI1alpTSTZJbTQzVGxObFZETkNMV3hKWmtsVWJXRlBhRGRTWTNjaWZRLkdUVElUTFpuR0NHaTE2RXVxVmZJZVJBcm5EZjhlbjdWT3FRQXdWZUNmckdwTGNsWm4zSm1aNnJXTVc3U1JnNUhlR0lkR2QyWEltTTVwaG1mSlJLMmtYaTFPZnNCbE15WnZHVlY1VFR3YkpTYXFzRXNGdG56a0lZRHBuRFJ3Rm9jdGNPcUdJbzhBbjRNU1NPZWRYbUlnZV9ESXFUTm5SRGdfVHN2SG5GN0lqZjA2Z2FTby1GbjJhVFRwOUdQbGc1LXZkMUhjajIwWDhyRlZPSERGcTdJM0VUaUFVTW5jUkhSTDlvRVhoSDF2dUZad2t6Z3pTYVh1dW5VTFJwLUEtNlpGWVFhQUxCNEN3bzZmMUJFSnQ2RGFSTDVjaDVJLUU3Q2I0Y09aMGhaTFQtazFmWEtOZC1lbnlxU3BjQnJpT2htaTJkVUVVcWgtVjlqaWlnX2NobXhxZyJ9",
			campos.payload.b64, 1332);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(
			"J2FgpnC7THqu9Yybqfo19gsPOhJ9QMwMrbtfi8Zh8wMssjfbt-torN0fN6fFmyXUstGdIMO48x8ZBMddmHQ8aX536Abo8xIqvL6WEwECOsT3YqIH0epXF_I7g8JA982Ev0lsiqyLbWipqeeJWfZcYW0vdTRw8EP9amwRmqkrqxgcltY0A6FgK4b_r9KrtvnFFp9RXnB29IvMK3YTAuoKxZQ4zwux85x0CLu_nJdNQFBuo5BvPUiJdRi7p041hlQyCKQCogwdsiAKsNM1xDmTVY9iiFWPMzIup3oQ6O7fQtxPkmY8oecsYBUpVnw-4TJ1S-ujxcyrimMIwB6z-DVIstqNPH0cZb1gZAgCZwM2363epumS8sz3BiZq63jIURqzPhvb8-7RI4tz_FCBWRViRLOPOwLWWMujeBr_VbgA_KA_rT4WO83HlAd61TALAZByo9JIJQW9FtzSEac6GAPVffow72bygGdnMujJSqrZMGI3Njo02HBvLP_xOPKHcsV4AVkgWCrYabFXhntDrSVAI-eIueUCiZH9-_S1WNsW80-lSb8bv4Womepy6WBo2MVIYuO5BS_zGXbbVL-FT6ZyLj9IsxKJ-24oKYhcWP7x4cRPnoS1jVGxBmvqlSU1hugdKnOhNkWwPLxZ0ZwtmmLGLOVxC6F5ktoLFZ50O8hk5kE",
			campos.assinatura.b64, 683);

	retval = JWT::getParametrosToken(token3, strlen(token3), &campos);
	TEST_ASSERT_EQUAL_INT(JWT::ERRO::SUCESSO, retval);

	TEST_ASSERT_EQUAL_CHAR_ARRAY("eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9",
			campos.header.b64, 36);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(
			"eyJub25jZSI6ImFoZ2R2eGMxMjNmZ3h2Y2dkZXQxZjQifQ",
			campos.payload.b64, 46);

	TEST_ASSERT_EQUAL_CHAR_ARRAY("horLPJEueSLBdahjFekd4QoTL4TafHMHCd_bymNPEm0",
			campos.assinatura.b64, 43);
}




void test_tokenHS256(void) {
	const char payload[] = "{\"username\":\"Rafael\"}";
	const char chave[] = "chave";

	/**
	 * testa geracao do token
	 */

	char token[300];
	int retval = JWT::geraTokenHS256((uint8_t*) payload, strlen(payload),
			(uint8_t*) chave, strlen(chave), token, sizeof(token));
	TEST_ASSERT_GREATER_THAN(0, retval);
	// ref.: https://jwt.io/
	const char token_esperado[] =
			"eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ1c2VybmFtZSI6IlJhZmFlbCJ9.ev9OXWiSj9I11tQ3_Nffy_O3ValujFdjs8sDxCPvhvo";
	TEST_ASSERT_EQUAL_CHAR_ARRAY(token_esperado, token, strlen(token));

	/**
	 * testa verificacao do token
	 */

	static uint8_t payload_verificado[300];
	retval = JWT::verificaTokenHS256(token, strlen(token), (uint8_t*) chave,
			strlen(chave), payload_verificado, sizeof(payload_verificado));
	TEST_ASSERT_GREATER_THAN(0, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(payload, payload_verificado, retval);
}

void test_verificaTokenES256(void) {
	chave_publica_ECC_P256_t chave_publica;
	memcpy((void*) chave_publica.x, (const void*) x_p256, 32);
	memcpy((void*) chave_publica.y, (const void*) y_p256, 32);
	uint8_t payload[50];
	StaticJsonDocument<100> payload_json;

	int retval = JWT::verificaTokenES256(token_ecc_p256, strlen(token_ecc_p256),
			&chave_publica, payload, sizeof(payload));

	TEST_ASSERT_GREATER_THAN(0, retval);

	TEST_ASSERT(
			JSON::SUCESSO == JSON::deserialize((char* )payload, payload_json));
	char *username = payload_json["username"];
	TEST_ASSERT(memcmp(username, "John Doe", strlen("John Doe")) == 0);

	StaticJsonDocument<300> header_json;
	char header[162];
	TEST_ASSERT_GREATER_THAN(0,
			Base64URL::b64url_decode(token_ecc_p256, 214, (uint8_t* )header,
					sizeof(header)));
	TEST_ASSERT(JSON::SUCESSO == JSON::deserialize(header, header_json));

	char *typ = header_json["typ"];
	char *alg = header_json["alg"];
	char *crv = header_json["jwk"]["crv"];
	char *x = header_json["jwk"]["x"];
	char *y = header_json["jwk"]["y"];
	char *kty = header_json["jwk"]["kty"];

	TEST_ASSERT(0 == memcmp(typ, "JWT", 3));
	TEST_ASSERT(0 == memcmp(alg, "ES256", 5));
	TEST_ASSERT(0 == memcmp(crv, "P-256", 5));
	TEST_ASSERT(
			0 == memcmp(x, "XXKnxzla0xL6yqiSA2ohCicfys74mnD6c6VYG7ZazNE", 43));
	TEST_ASSERT(
			0 == memcmp(y, "3B4sifDc0MexicDo4ied1PPQK_lVhvKzqKC2c9jKp04", 43));
	TEST_ASSERT(0 == memcmp(kty, "EC", 2));
}

void test_geraTokenES256(void) {

	const char payload[] = "{\"username\":\"John Doe\"}";
	chave_privada_ECC_P256_t chave_privada;
	chave_publica_ECC_P256_t chave_publica;

	memcpy((void*) chave_privada.d, (const void*) d_p256, 32);
	memcpy((void*) chave_publica.x, (const void*) x_p256, 32);
	memcpy((void*) chave_publica.y, (const void*) y_p256, 32);

	static char token[400];

	/**
	 * geracao do token
	 */

	int retval = JWT::geraTokenES256((uint8_t*) payload, strlen(payload),
			&chave_privada, token, sizeof(token), true, &chave_publica);

	TEST_ASSERT_GREATER_THAN(0, retval);

	// verifica o header

	StaticJsonDocument<300> header_json;
	char header[162];
	TEST_ASSERT_GREATER_THAN(0,
			Base64URL::b64url_decode(token_ecc_p256, 214, (uint8_t* )header,
					sizeof(header)));
	TEST_ASSERT(JSON::SUCESSO == JSON::deserialize(header, header_json));

	char *typ = header_json["typ"];
	char *alg = header_json["alg"];
	char *crv = header_json["jwk"]["crv"];
	char *x = header_json["jwk"]["x"];
	char *y = header_json["jwk"]["y"];
	char *kty = header_json["jwk"]["kty"];

	TEST_ASSERT(0 == memcmp(typ, "JWT", 3));
	TEST_ASSERT(0 == memcmp(alg, "ES256", 5));
	TEST_ASSERT(0 == memcmp(crv, "P-256", 5));
	TEST_ASSERT(
			0 == memcmp(x, "XXKnxzla0xL6yqiSA2ohCicfys74mnD6c6VYG7ZazNE", 43));
	TEST_ASSERT(
			0 == memcmp(y, "3B4sifDc0MexicDo4ied1PPQK_lVhvKzqKC2c9jKp04", 43));
	TEST_ASSERT(0 == memcmp(kty, "EC", 2));

	/**
	 * verificacao do token
	 * a assinatura eh diferente para cada assinatura gerada, por isso nao deve
	 * ser testada
	 */

	retval = JWT::verificaTokenES256(token_ecc_p256, strlen(token_ecc_p256),
			&chave_publica, (uint8_t*) payload, sizeof(payload));

	TEST_ASSERT_GREATER_THAN(0, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(token_ecc_p256, token, 246);
}

void test_verificaTokenES384(void) {
	chave_publica_ECC_P384_t chave_publica;
	memcpy((void*) chave_publica.x, (const void*) x_p384, 48);
	memcpy((void*) chave_publica.y, (const void*) y_p384, 48);

	uint8_t payload[50];
	StaticJsonDocument<100> payload_json;

	int retval = JWT::verificaTokenES384(token_ecc_p384, strlen(token_ecc_p384),
			&chave_publica, payload, sizeof(payload));

	TEST_ASSERT_GREATER_THAN(0, retval);

	TEST_ASSERT(
			JSON::SUCESSO == JSON::deserialize((char* )payload, payload_json));
	char *username = payload_json["username"];
	TEST_ASSERT(memcmp(username, "John Doe", strlen("John Doe")) == 0);

	StaticJsonDocument<350> header_json;
	char header[205];
	TEST_ASSERT_GREATER_THAN(0,
			Base64URL::b64url_decode(token_ecc_p384, 270, (uint8_t* )header,
					sizeof(header)));
	TEST_ASSERT(JSON::SUCESSO == JSON::deserialize(header, header_json));

	char *typ = header_json["typ"];
	char *alg = header_json["alg"];
	char *crv = header_json["jwk"]["crv"];
	char *x = header_json["jwk"]["x"];
	char *y = header_json["jwk"]["y"];
	char *kty = header_json["jwk"]["kty"];

	TEST_ASSERT(0 == memcmp(typ, "JWT", 3));
	TEST_ASSERT(0 == memcmp(alg, "ES384", 5));
	TEST_ASSERT(0 == memcmp(crv, "P-384", 5));
	TEST_ASSERT(
			0 == memcmp(x, "nL7gjipmv8_PxeQKO-6TzTDIW7f1_81o6HyFsXgkRPBquOW0-BiPQb3xMY0mYiaj", 64));
	TEST_ASSERT(
			0 == memcmp(y, "2qxLCxXkHi8DZsGa-UI-uCsocGiFvLtUfEpfCqv5H43kAelK6xtv4dHF9MRLcNc1", 64));
	TEST_ASSERT(0 == memcmp(kty, "EC", 2));
}

void test_geraTokenES384(void) {

	const char payload[] = "{\"username\":\"John Doe\"}";
	chave_privada_ECC_P384_t chave_privada;
	chave_publica_ECC_P384_t chave_publica;

	memcpy((void*) chave_privada.d, (const void*) d_p384, 48);
	memcpy((void*) chave_publica.x, (const void*) x_p384, 48);
	memcpy((void*) chave_publica.y, (const void*) y_p384, 48);

	static char token[600];

	/**
	 * geracao do token
	 */

	int retval = JWT::geraTokenES384((uint8_t*) payload, strlen(payload),
			&chave_privada, token, sizeof(token), true, &chave_publica);

	TEST_ASSERT_GREATER_THAN(0, retval);

	// verifica o header

	StaticJsonDocument<350> header_json;
	char header[205];
	TEST_ASSERT_GREATER_THAN(0,
			Base64URL::b64url_decode(token_ecc_p384, 270, (uint8_t* )header,
					sizeof(header)));
	TEST_ASSERT(JSON::SUCESSO == JSON::deserialize(header, header_json));

	char *typ = header_json["typ"];
	char *alg = header_json["alg"];
	char *crv = header_json["jwk"]["crv"];
	char *x = header_json["jwk"]["x"];
	char *y = header_json["jwk"]["y"];
	char *kty = header_json["jwk"]["kty"];

	TEST_ASSERT(0 == memcmp(typ, "JWT", 3));
	TEST_ASSERT(0 == memcmp(alg, "ES384", 5));
	TEST_ASSERT(0 == memcmp(crv, "P-384", 5));
	TEST_ASSERT(
			0 == memcmp(x, "nL7gjipmv8_PxeQKO-6TzTDIW7f1_81o6HyFsXgkRPBquOW0-BiPQb3xMY0mYiaj", 64));
	TEST_ASSERT(
			0 == memcmp(y, "2qxLCxXkHi8DZsGa-UI-uCsocGiFvLtUfEpfCqv5H43kAelK6xtv4dHF9MRLcNc1", 64));
	TEST_ASSERT(0 == memcmp(kty, "EC", 2));

	/**
	 * verificacao do token
	 * a assinatura eh diferente para cada assinatura gerada, por isso nao deve
	 * ser testada
	 */

	retval = JWT::verificaTokenES384(token_ecc_p384, strlen(token_ecc_p384),
			&chave_publica, (uint8_t*) payload, sizeof(payload));

	TEST_ASSERT_GREATER_THAN(0, retval);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(token_ecc_p384, token, 302);
}

int main() {
	UNITY_BEGIN();

	DEBUG_PRINTF_INIT();

	CriptoDAF::configura();

	RUN_TEST(test_getParametrosToken);
	RUN_TEST(test_tokenHS256);
	RUN_TEST(test_verificaTokenES256);
	RUN_TEST(test_geraTokenES256);
	RUN_TEST(test_verificaTokenES384);
	RUN_TEST(test_geraTokenES384);

	return UNITY_END();
}
