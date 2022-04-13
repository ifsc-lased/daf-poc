#include <MAX325xx.h>
#include <stdio.h>
#include <stddef.h>
#include <ucl/ucl_rsa.h>
#include <ucl/ucl_pkcs1_ssa_pkcs1v15_sha256.h>
#include <ucl/ucl_pkcs1_es_oaep_sha256.h>
#include <ucl/ucl_hmac_sha256.h>
#include <ucl/ucl_sha256.h>
#include <ucl/ucl_retdefs.h>

#include "../cripto/cripto_daf.h"
#include "../unity/unity.h"
#include "../pdafcdc.h"
#include "../usbcdc.h"
#include "../daf.h"
#include "../debug.h"
#include "../pdafcdc_utils.h"

#define TAMANHO_CHAVE_RSA_BITS 2048
#define TAMANHO_CHAVE_RSA_BYTES (TAMANHO_CHAVE_RSA_BITS >> 3)

const static char *g_mensagem =
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit, \
sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Id diam  \
maecenas ultricies mi eget mauris pharetra et ultrices. Placerat orci nulla \
pellentesque dignissim enim sit. Amet nisl suscipit adipiscing bibendum est \
ultricies. Eget lorem dolor sed viverra ipsum nunc aliquet. Ut tristique et \
egestas quis ipsum suspendisse ultrices gravida dictum. Aenean euismod \
elementum nisi quis eleifend quam adipiscing vitae. Sodales ut etiam sit \
amet nisl. Quisque non tellus orci ac auctor augue mauris augue neque. Arcu \
dui vivamus arcu felis bibendum ut tristique et egestas. Vestibulum rhoncus \
est pellentesque elit ullamcorper dignissim cras. Nec ultrices dui sapien eget \
mi proin sed libero. \
Lorem ipsum dolor sit amet, consectetur adipiscing elit, \
sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Id diam  \
maecenas ultricies mi eget mauris pharetra et ultrices. Placerat orci nulla \
pellentesque dignissim enim sit. Amet nisl suscipit adipiscing bibendum est \
ultricies. Eget lorem dolor sed viverra ipsum nunc aliquet. Ut tristique et \
egestas quis ipsum suspendisse ultrices gravida dictum. Aenean euismod \
elementum nisi quis eleifend quam adipiscing vitae. Sodales ut etiam sit \
amet nisl. Quisque non tellus orci ac auctor augue mauris augue neque. Arcu \
dui vivamus arcu felis bibendum ut tristique et egestas. Vestibulum rhoncus \
est pellentesque elit ullamcorper dignissim cras. Nec ultrices dui sapien eget \
mi proin sed libero.";

void setUp(void) {
	CriptoDAF::configura();
}
void tearDown(void) {
}

static chave_publica_RSA_t g_pub_rsa;
static chave_privada_RSA_t g_priv_rsa;

static ucl_rsa_public_key_t g_pub_rsa_ucl;
static ucl_rsa_private_key_t g_priv_rsa_ucl;

void test_gera_chave_RSA(void) {
	chave_publica_RSA_t *pub = &g_pub_rsa;
	chave_privada_RSA_t *priv = &g_priv_rsa;
	ucl_rsa_public_key_t *pub_ucl = &g_pub_rsa_ucl;
	ucl_rsa_private_key_t *priv_ucl = &g_priv_rsa_ucl;

	u8 assinatura[TAMANHO_CHAVE_RSA_BYTES];
	int retval;
	u32 ms;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_chave_RSA(pub, priv, TAMANHO_CHAVE_RSA_BITS);
	DEBUG_PRINTF("gera_chave_RSA(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	priv_ucl->modulus = priv->mod;
	priv_ucl->modulus_length = priv->mod_sz;
	priv_ucl->private_exponent = priv->priv_expoente;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = ucl_pkcs1_ssa_pkcs1v15_sha256_sign(assinatura, (u8*) g_mensagem,
			strlen(g_mensagem), priv_ucl);
	DEBUG_PRINTF("ucl_pkcs1_ssa_pkcs1v15_sha256_sign(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	memcpy(pub_ucl->modulus, pub->mod, pub->mod_sz);
	memcpy(pub_ucl->public_exponent, pub->pub_expoente, pub->pub_expoente_sz);
	pub_ucl->modulus_length = pub->mod_sz;
	pub_ucl->public_exponent_length = pub->pub_expoente_sz;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = ucl_pkcs1_ssa_pkcs1v15_sha256_verify(assinatura, (u8*) g_mensagem,
			strlen(g_mensagem), pub_ucl);
	DEBUG_PRINTF("ucl_pkcs1_ssa_pkcs1v15_sha256_verify(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

void test_gera_assinatura_RSA(void) {
	chave_publica_RSA_t *pub = &g_pub_rsa;
	chave_privada_RSA_t *priv = &g_priv_rsa;
	ucl_rsa_public_key_t *pub_ucl = &g_pub_rsa_ucl;
	u8 assinatura[TAMANHO_CHAVE_RSA_BYTES];
	int retval;

	retval = CriptoDAF::gera_chave_RSA(pub, priv, TAMANHO_CHAVE_RSA_BITS);
	TEST_ASSERT_EQUAL_INT(0, retval);

	retval = CriptoDAF::gera_assinatura_RSA(assinatura, (u8*) g_mensagem,
			strlen(g_mensagem), priv);
	TEST_ASSERT_EQUAL_INT(0, retval);

	memcpy(pub_ucl->modulus, pub->mod, pub->mod_sz);
	memcpy(pub_ucl->public_exponent, pub->pub_expoente, pub->pub_expoente_sz);
	pub_ucl->modulus_length = pub->mod_sz;
	pub_ucl->public_exponent_length = pub->pub_expoente_sz;

	retval = ucl_pkcs1_ssa_pkcs1v15_sha256_verify(assinatura, (u8*) g_mensagem,
			strlen(g_mensagem), pub_ucl);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

void test_verifica_assinatura_RSA(void) {
	chave_publica_RSA_t *pub = &g_pub_rsa;
	chave_privada_RSA_t *priv = &g_priv_rsa;
	u8 assinatura[TAMANHO_CHAVE_RSA_BYTES];
	int retval;

	retval = CriptoDAF::gera_chave_RSA(pub, priv, TAMANHO_CHAVE_RSA_BITS);
	TEST_ASSERT_EQUAL_INT(0, retval);

	retval = CriptoDAF::gera_assinatura_RSA(assinatura, (u8*) g_mensagem,
			strlen(g_mensagem), priv);
	TEST_ASSERT_EQUAL_INT(0, retval);

	retval = CriptoDAF::verifica_assinatura_RSA(assinatura, (u8*) g_mensagem,
			strlen(g_mensagem), pub);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

void test_cifra_RSAAES_OAEP(void) {
	chave_publica_RSA_t *pub = &g_pub_rsa;
	chave_privada_RSA_t *priv = &g_priv_rsa;
	ucl_rsa_private_key_t *priv_ucl = &g_priv_rsa_ucl;

	u8 msg_cifrada[TAMANHO_CHAVE_RSA_BYTES];
	char msg[] = "Hello World!";
	char msg_decifrada[50] = { 0 };
	u32 msg_decifrada_tamanho;
	int retval;

	retval = CriptoDAF::gera_chave_RSA(pub, priv, TAMANHO_CHAVE_RSA_BITS);
	TEST_ASSERT_EQUAL_INT(0, retval);

	retval = CriptoDAF::cifra_RSAAES_OAEP(msg_cifrada, (u8*) msg, strlen(msg),
			pub);
	TEST_ASSERT_EQUAL_INT(0, retval);

	priv_ucl->modulus = priv->mod;
	priv_ucl->modulus_length = priv->mod_sz;
	priv_ucl->private_exponent = priv->priv_expoente;

	retval = ucl_pkcs1_es_oaep_sha256_decrypt((u8*) msg_decifrada,
			&msg_decifrada_tamanho, msg_cifrada, priv_ucl, NULL, 0);
	TEST_ASSERT_EQUAL_INT(0, retval);

	TEST_ASSERT_EQUAL_STRING(msg, msg_decifrada);
}

void test_decifra_RSAAES_OAEP(void) {
	chave_publica_RSA_t *pub = &g_pub_rsa;
	chave_privada_RSA_t *priv = &g_priv_rsa;
	u8 mensagem_cifrada[TAMANHO_CHAVE_RSA_BYTES];
	char mensagem[] = "Hello World!";
	char mensagem_decifrada[50] = { 0 };
	u32 mensagem_decifrada_tamanho;
	int retval;

	retval = CriptoDAF::gera_chave_RSA(pub, priv, TAMANHO_CHAVE_RSA_BITS);
	TEST_ASSERT_EQUAL_INT(0, retval);

	retval = CriptoDAF::cifra_RSAAES_OAEP(mensagem_cifrada, (u8*) mensagem,
			strlen(mensagem), pub);
	TEST_ASSERT_EQUAL_INT(0, retval);

	retval = CriptoDAF::decifra_RSAAES_OAEP((u8*) mensagem_decifrada,
			&mensagem_decifrada_tamanho, mensagem_cifrada, priv);
	TEST_ASSERT_EQUAL_INT(0, retval);

	TEST_ASSERT_EQUAL_STRING(mensagem, mensagem_decifrada);
}

void test_gera_HMAC_SHA256(void) {
	char chave[] = "chave";
	char mensagem[] = "Hello World!";
	u8 resumo[32] = { 0 };
	/*
	 * Para gerar resumo hmac no bash, com saÃ­da hexa para linguagem C: 
	 * echo -n "Hello World!" | openssl dgst -sha256 -hmac "chave" -binary | xxd -i
	 */
	u8 resumo_esperado[] = { 0x44, 0x58, 0x37, 0x2e, 0x55, 0x61, 0x68, 0x17,
			0x87, 0x24, 0xe7, 0x83, 0x59, 0x8a, 0x73, 0x60, 0x4d, 0x56, 0x3f,
			0x6d, 0xe6, 0x37, 0x6f, 0x6f, 0x66, 0x5b, 0x8f, 0xce, 0x81, 0x99,
			0x50, 0x66 };
	int retval;

	retval = CriptoDAF::gera_HMAC_SHA256(resumo, (u8*) mensagem,
			strlen(mensagem), (u8*) chave, strlen(chave));
	TEST_ASSERT_EQUAL_INT(0, retval);

	TEST_ASSERT_EQUAL_HEX8_ARRAY(resumo_esperado, resumo, 32);
}

void test_verifica_HMAC_SHA256(void) {
	char chave[] = "chave";
	char mensagem[] = "Hello World!";
	u8 resumo[32] = { 0 };
	int retval;

	retval = CriptoDAF::gera_HMAC_SHA256(resumo, (u8*) mensagem,
			strlen(mensagem), (u8*) chave, strlen(chave));
	TEST_ASSERT_EQUAL_INT(0, retval);

	retval = CriptoDAF::verifica_HMAC_SHA256(resumo, (u8*) mensagem,
			strlen(mensagem), (u8*) chave, strlen(chave));
	TEST_ASSERT_EQUAL_INT(0, retval);

	resumo[5]++;
	retval = CriptoDAF::verifica_HMAC_SHA256(resumo, (u8*) mensagem,
			strlen(mensagem), (u8*) chave, strlen(chave));
	TEST_ASSERT_EQUAL_INT(1, retval);

	resumo[5]--; // desfaz o erro
	mensagem[5]++;
	retval = CriptoDAF::verifica_HMAC_SHA256(resumo, (u8*) mensagem,
			strlen(mensagem), (u8*) chave, strlen(chave));
	TEST_ASSERT_EQUAL_INT(1, retval);
}

void test_assinatura_valida_ECC256(void) {
	chave_privada_ECC_P256_t priv;
	chave_publica_ECC_P256_t pub;
	assinatura_ECC_P256_t ass;
	int retval;
	u32 ms;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_chave_ECC(&pub, &priv);
	DEBUG_PRINTF("gera_chave_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &priv);
	DEBUG_PRINTF("gera_assinatura_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::verifica_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &pub);
	DEBUG_PRINTF("verifica_assinatura_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

void test_assinatura_valida_ECC384(void) {
	chave_privada_ECC_P384_t priv;
	chave_publica_ECC_P384_t pub;
	assinatura_ECC_P384_t ass;
	int retval;
	u32 ms;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_chave_ECC(&pub, &priv);
	DEBUG_PRINTF("gera_chave_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &priv);
	DEBUG_PRINTF("gera_assinatura_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::verifica_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &pub);
	DEBUG_PRINTF("verifica_assinatura_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

void test_assinatura_msg_invalida_ECC256(void) {
	chave_privada_ECC_P256_t priv;
	chave_publica_ECC_P256_t pub;
	assinatura_ECC_P256_t ass;
	int retval;
	u32 ms;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_chave_ECC(&pub, &priv);
	DEBUG_PRINTF("gera_chave_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &priv);
	DEBUG_PRINTF("gera_assinatura_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::verifica_assinatura_ECC(&ass, (u8*) "123456", 6, &pub);
	DEBUG_PRINTF("verifica_assinatura_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(UCL_ERROR, retval);
}

void test_assinatura_msg_invalida_ECC384(void) {
	chave_privada_ECC_P384_t priv;
	chave_publica_ECC_P384_t pub;
	assinatura_ECC_P384_t ass;
	int retval;
	u32 ms;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_chave_ECC(&pub, &priv);
	DEBUG_PRINTF("gera_chave_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &priv);
	DEBUG_PRINTF("gera_assinatura_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::verifica_assinatura_ECC(&ass, (u8*) "123456", 6, &pub);
	DEBUG_PRINTF("verifica_assinatura_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(UCL_ERROR, retval);
}

void test_assinatura_chave_pub_invalida_ECC256(void) {
	chave_privada_ECC_P256_t priv;
	chave_publica_ECC_P256_t pub;
	assinatura_ECC_P256_t ass;
	int retval;
	u32 ms;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_chave_ECC(&pub, &priv);
	DEBUG_PRINTF("gera_chave_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &priv);
	DEBUG_PRINTF("gera_assinatura_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	pub.x[15] = pub.x[15] + 1;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::verifica_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &pub);
	DEBUG_PRINTF("verifica_assinatura_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(UCL_ERROR, retval);
}

void test_assinatura_chave_pub_invalida_ECC384(void) {
	chave_privada_ECC_P384_t priv;
	chave_publica_ECC_P384_t pub;
	assinatura_ECC_P384_t ass;
	int retval;
	u32 ms;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_chave_ECC(&pub, &priv);
	DEBUG_PRINTF("gera_chave_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &priv);
	DEBUG_PRINTF("gera_assinatura_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	pub.x[15] = pub.x[15] + 1;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::verifica_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &pub);
	DEBUG_PRINTF("verifica_assinatura_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(UCL_ERROR, retval);
}

void test_assinatura_chave_priv_invalida_ECC256(void) {
	chave_privada_ECC_P256_t priv;
	chave_publica_ECC_P256_t pub;
	assinatura_ECC_P256_t ass;
	int retval;
	u32 ms;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_chave_ECC(&pub, &priv);
	DEBUG_PRINTF("gera_chave_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	priv.d[12] = priv.d[12] + 1;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &priv);
	DEBUG_PRINTF("gera_assinatura_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::verifica_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &pub);
	DEBUG_PRINTF("verifica_assinatura_ECC_P256(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(UCL_ERROR, retval);
}

void test_assinatura_chave_priv_invalida_ECC384(void) {
	chave_privada_ECC_P384_t priv;
	chave_publica_ECC_P384_t pub;
	assinatura_ECC_P384_t ass;
	int retval;
	u32 ms;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_chave_ECC(&pub, &priv);
	DEBUG_PRINTF("gera_chave_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	priv.d[12] = priv.d[12] + 1;

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::gera_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &priv);
	DEBUG_PRINTF("gera_assinatura_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(0, retval);

	ms = PDAFCDC_Utils::get_time_as_ms();
	retval = CriptoDAF::verifica_assinatura_ECC(&ass, (u8*) g_mensagem,
			strlen(g_mensagem), &pub);
	DEBUG_PRINTF("verifica_assinatura_ECC_P384(): tempo: %i ms\n",
			PDAFCDC_Utils::get_time_as_ms() - ms);
	TEST_ASSERT_EQUAL_INT(UCL_ERROR, retval);
}

void test_cifragem_ECC256(void) {
	chave_privada_ECC_P256_t priv;
	chave_publica_ECC_P256_t pub;
	cifragem_ECC_P256_t cifragem;
	char msg[] = "Hello World!";
	char msg_padded[64];
	uint8_t msg_cifrada[64];
	char msg_decifrada[64];
	size_t msg_padded_sz, msg_decifrada_sz;
	int retval;

	cifragem.msg_cifrada = msg_cifrada;

	retval = CriptoDAF::gera_chave_ECC(&pub, &priv);
	TEST_ASSERT_EQUAL_INT(0, retval);

	/**
	 * o tamanho da mensagem a ser cifrada deve ser múltipla de 16 bytes
	 * (tamanho do bloco AES).
	 */
	memcpy(msg_padded, msg, strlen(msg));
	msg_padded_sz = strlen(msg) + 16 - (strlen(msg) % 16);

	retval = CriptoDAF::cifra_ECC((uint8_t*) msg_padded, msg_padded_sz, &pub,
			&cifragem);
	TEST_ASSERT_EQUAL_INT(0, retval);

	retval = CriptoDAF::decifra_ECC(&cifragem, &priv, (uint8_t*) msg_decifrada,
			&msg_decifrada_sz);
	TEST_ASSERT_EQUAL_INT(0, retval);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(msg_padded, msg_decifrada, msg_padded_sz);

}

void test_pub_priv_ecc_python(void) {
	static uint8_t d_p256[] = { 0xfb, 0xbe, 0xbd, 0xf9, 0x34, 0x80, 0x69, 0xb3,
			0x27, 0xff, 0xa7, 0x14, 0xdf, 0x81, 0x85, 0x3d, 0xe8, 0xd7, 0x57, 0x42,
			0xb1, 0xa1, 0xbb, 0x86, 0xb0, 0x76, 0x3d, 0x4a, 0xff, 0x9b, 0x04, 0x35 };

	static uint8_t x_p256[] = { 0x5d, 0x72, 0xa7, 0xc7, 0x39, 0x5a, 0xd3, 0x12,
			0xfa, 0xca, 0xa8, 0x92, 0x03, 0x6a, 0x21, 0x0a, 0x27, 0x1f, 0xca, 0xce,
			0xf8, 0x9a, 0x70, 0xfa, 0x73, 0xa5, 0x58, 0x1b, 0xb6, 0x5a, 0xcc, 0xd1 };

	static uint8_t y_p256[] = { 0xdc, 0x1e, 0x2c, 0x89, 0xf0, 0xdc, 0xd0, 0xc7,
			0xb1, 0x89, 0xc0, 0xe8, 0xe2, 0x27, 0x9d, 0xd4, 0xf3, 0xd0, 0x2b, 0xf9,
			0x55, 0x86, 0xf2, 0xb3, 0xa8, 0xa0, 0xb6, 0x73, 0xd8, 0xca, 0xa7, 0x4e };

	static char token[] =
			"eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzI1NiIsImp3ayI6eyJjcnYiOiJQLTI1NiIsIngiOiJYWEtueHpsYTB4TDZ5cWlTQTJvaENpY2Z5czc0bW5ENmM2VllHN1phek5FIiwieSI6IjNCNHNpZkRjME1leGljRG80aWVkMVBQUUtfbFZodkt6cUtDMmM5aktwMDQiLCJrdHkiOiJFQyJ9fQ.eyJ1c2VybmFtZSI6IkpvaG4gRG9lIn0";

	chave_privada_ECC_P256_t priv;
	chave_publica_ECC_P256_t pub;

	memcpy(priv.d, d_p256, sizeof(d_p256));
	memcpy(pub.x, x_p256, sizeof(x_p256));
	memcpy(pub.y, y_p256, sizeof(y_p256));

	assinatura_ECC_P256_t assinatura;

	int retval = CriptoDAF::gera_assinatura_ECC(&assinatura, (uint8_t*)token, strlen(token),
			&priv);
	TEST_ASSERT_EQUAL_INT(0, retval);

	retval = CriptoDAF::verifica_assinatura_ECC(&assinatura, (uint8_t*)token, strlen(token),
			&pub);
	TEST_ASSERT_EQUAL_INT(0, retval);

	uint8_t *buffer = (uint8_t*)assinatura.r;
	DEBUG_PRINT_ARRAY_HEX(buffer, sizeof(assinatura));
}

int main(void) {
	DEBUG_PRINTF_INIT();

	DEBUG_PRINTF("\n\nIniciando testes cripto DAF\n\n");

	DEBUG_PRINTF("\nTamanho da g_mensagem: %i\n", strlen(g_mensagem));

	UNITY_BEGIN();

	DEBUG_PRINTF("\n\n ------------ RSA --------------- \n\n");

	RUN_TEST(test_gera_chave_RSA);
	RUN_TEST(test_gera_assinatura_RSA);
	RUN_TEST(test_verifica_assinatura_RSA);
	RUN_TEST(test_cifra_RSAAES_OAEP);
	RUN_TEST(test_decifra_RSAAES_OAEP);

	DEBUG_PRINTF("\n\n ------------ ECC256 --------------- \n\n");

	RUN_TEST(test_assinatura_valida_ECC256);
	RUN_TEST(test_assinatura_msg_invalida_ECC256);
	RUN_TEST(test_assinatura_chave_pub_invalida_ECC256);
	RUN_TEST(test_assinatura_chave_priv_invalida_ECC256);
	RUN_TEST(test_cifragem_ECC256);

	DEBUG_PRINTF("\n\n ------------ ECC384 --------------- \n\n");

	RUN_TEST(test_assinatura_valida_ECC384);
	RUN_TEST(test_assinatura_msg_invalida_ECC384);
	RUN_TEST(test_assinatura_chave_pub_invalida_ECC384);
	RUN_TEST(test_assinatura_chave_priv_invalida_ECC384);

	DEBUG_PRINTF("\n\n ------------ OUTROS --------------- \n\n");

	RUN_TEST(test_gera_HMAC_SHA256);
	RUN_TEST(test_verifica_HMAC_SHA256);
	RUN_TEST(test_pub_priv_ecc_python);

	return UNITY_END();
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__