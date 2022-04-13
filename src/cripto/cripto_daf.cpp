#include <string.h>
#include <ucl/ucl_rsa.h>
#include <ucl/ucl_pkcs1_ssa_pkcs1v15_sha256.h>
#include <ucl/ucl_pkcs1_es_oaep_sha256.h>
#include <ucl/ucl_hmac_sha256.h>
#include <ucl/ucl_sha256.h>
#include <ucl/ucl_rng.h>
#include <ucl/ucl_sys.h>
#include <ucl/ucl_ecc_keygen.h>
#include <ucl/ecdsa_generic_api.h>
#include <ucl/ucl_ecies.h>
#include <ucl/ucl_aes.h>

#include "cripto_daf.h"

uint32_t CriptoDAF::buf_ucl[BUF_UCL_TAMANHO];
uint8_t CriptoDAF::buf_auxiliar[8192];

int CriptoDAF::gera_chave_ECC(chave_publica_ECC_P256_t *pub,
		chave_privada_ECC_P256_t *priv) {
	ucl_type_ecc_u8_affine_point ucl_pub;
	int retval;

	ucl_pub.x = pub->x;
	ucl_pub.y = pub->y;

	retval = ucl_ecc_keygeneration_p256r1(ucl_pub, priv->d);

	return retval;
}

int CriptoDAF::gera_chave_ECC(chave_publica_ECC_P384_t *pub,
		chave_privada_ECC_P384_t *priv) {
	ucl_type_ecc_u8_affine_point ucl_pub;
	int retval;

	ucl_pub.x = pub->x;
	ucl_pub.y = pub->y;

	retval = ucl_ecc_keygeneration_p384r1(ucl_pub, priv->d);

	return retval;
}

int CriptoDAF::gera_assinatura_ECC(assinatura_ECC_P256_t *ass,
		const uint8_t *msg, const size_t msg_tamanho,
		const chave_privada_ECC_P256_t *priv) {
	ucl_type_ecdsa_signature ucl_ass;
	uint32_t configuration;
	int retval;

	ucl_ass.r = ass->r;
	ucl_ass.s = ass->s;

	configuration = ((secp256r1.curve) << UCL_CURVE_SHIFT)
			^ (UCL_MSG_INPUT << UCL_INPUT_SHIFT)
			^ (UCL_SHA256 << UCL_HASH_SHIFT);

	retval = ucl_ecdsa_signature(ucl_ass, priv->d, &ucl_sha256,
			const_cast<uint8_t*>(msg), msg_tamanho, &secp256r1, configuration);

	return retval;
}

int CriptoDAF::gera_assinatura_ECC(assinatura_ECC_P384_t *ass,
		const uint8_t *msg, const size_t msg_tamanho,
		const chave_privada_ECC_P384_t *priv) {
	ucl_type_ecdsa_signature ucl_ass;
	uint32_t configuration;
	int retval;

	ucl_ass.r = ass->r;
	ucl_ass.s = ass->s;

	configuration = ((secp384r1.curve) << UCL_CURVE_SHIFT)
			^ (UCL_MSG_INPUT << UCL_INPUT_SHIFT)
			^ (UCL_SHA384 << UCL_HASH_SHIFT);

	retval = ucl_ecdsa_signature(ucl_ass, priv->d, &ucl_sha384, msg,
			msg_tamanho, &secp384r1, configuration);

	return retval;
}

int CriptoDAF::verifica_assinatura_ECC(const assinatura_ECC_P256_t *ass,
		const uint8_t *msg, const uint32_t msg_tamanho,
		const chave_publica_ECC_P256_t *pub) {
	ucl_type_ecdsa_signature ucl_ass;
	ucl_type_ecc_u8_affine_point ucl_pub;
	uint32_t configuration;
	int retval;

	ucl_ass.r = ass->r;
	ucl_ass.s = ass->s;

	ucl_pub.x = pub->x;
	ucl_pub.y = pub->y;

	configuration = ((secp256r1.curve) << UCL_CURVE_SHIFT)
			^ (UCL_MSG_INPUT << UCL_INPUT_SHIFT)
			^ (UCL_SHA256 << UCL_HASH_SHIFT);

	retval = ucl_ecdsa_verification(ucl_pub, ucl_ass, &ucl_sha256, msg,
			msg_tamanho, &secp256r1, configuration);

	return retval;
}

int CriptoDAF::verifica_assinatura_ECC(const assinatura_ECC_P384_t *ass,
		const uint8_t *msg, const uint32_t msg_tamanho,
		const chave_publica_ECC_P384_t *pub) {
	ucl_type_ecdsa_signature ucl_ass;
	ucl_type_ecc_u8_affine_point ucl_pub;
	uint32_t configuration;
	int retval;

	ucl_ass.r = ass->r;
	ucl_ass.s = ass->s;

	ucl_pub.x = pub->x;
	ucl_pub.y = pub->y;

	configuration = ((secp384r1.curve) << UCL_CURVE_SHIFT)
			^ (UCL_MSG_INPUT << UCL_INPUT_SHIFT)
			^ (UCL_SHA384 << UCL_HASH_SHIFT);

	retval = ucl_ecdsa_verification(ucl_pub, ucl_ass, &ucl_sha384, msg,
			msg_tamanho, &secp384r1, configuration);

	return retval;
}

int CriptoDAF::configura(void) {
	// inicialização da biblioteca criptográfica ucl
	int retval;

	retval = ucl_init((unsigned int*)CriptoDAF::buf_ucl, (unsigned int) BUF_UCL_TAMANHO);

	return retval;
}

static ucl_rsa_public_key_t g_rsa_pub_key;


int CriptoDAF::gera_chave_RSA(chave_publica_RSA_t *pub,
		chave_privada_RSA_t *priv, uint32_t tamanho_RSA_bits) {
	uint8_t *p, *q, *d;

	size_t mod_tamanho_bytes;
	int retval;

	mod_tamanho_bytes = tamanho_RSA_bits >> 3; // divide por 8

	p = &CriptoDAF::buf_auxiliar[0];
	q = &CriptoDAF::buf_auxiliar[mod_tamanho_bytes / 2];
	d = &CriptoDAF::buf_auxiliar[mod_tamanho_bytes];

	priv->priv_expoente = d;
	priv->mod = pub->mod;
	priv->mod_sz = pub->mod_sz = mod_tamanho_bytes;

	pub->pub_expoente[0] = 0x01;
	pub->pub_expoente[1] = 0x00;
	pub->pub_expoente[2] = 0x01;
	pub->pub_expoente_sz = 3;

	retval = ucl_rsa_param_generation_fips186_4(pub->mod, p, q,
			priv->priv_expoente, pub->pub_expoente, (int) pub->pub_expoente_sz,
			pub->mod_sz);

	return retval;
}

int CriptoDAF::gera_assinatura_RSA(uint8_t *assinatura, uint8_t *mensagem,
		uint32_t mensagem_tamanho, chave_privada_RSA_t *priv) {
	int retval;
	ucl_rsa_private_key_t priv_ucl;

	priv_ucl.modulus = priv->mod;
	priv_ucl.modulus_length = priv->mod_sz;
	priv_ucl.private_exponent = priv->priv_expoente;

	retval = ucl_pkcs1_ssa_pkcs1v15_sha256_sign(assinatura, mensagem,
			mensagem_tamanho, &priv_ucl);

	return retval;
}

int CriptoDAF::verifica_assinatura_RSA(uint8_t *assinatura, uint8_t *mensagem,
		uint32_t mensagem_tamanho, chave_publica_RSA_t *pub) {
	int retval;
	ucl_rsa_public_key_t *pub_ucl = &g_rsa_pub_key;

	memcpy(pub_ucl->modulus, pub->mod, pub->mod_sz);
	memcpy(pub_ucl->public_exponent, pub->pub_expoente, pub->pub_expoente_sz);
	pub_ucl->modulus_length = pub->mod_sz;
	pub_ucl->public_exponent_length = pub->pub_expoente_sz;

	retval = ucl_pkcs1_ssa_pkcs1v15_sha256_verify(assinatura, mensagem,
			mensagem_tamanho, pub_ucl);

	return retval;
}

int CriptoDAF::cifra_RSAAES_OAEP(uint8_t *mensagem_cifrada, uint8_t *mensagem,
		uint32_t mensagem_tamanho, chave_publica_RSA_t *pub) {
	int retval;
	ucl_rsa_public_key_t *pub_ucl = &g_rsa_pub_key;

	memcpy(pub_ucl->modulus, pub->mod, pub->mod_sz);
	memcpy(pub_ucl->public_exponent, pub->pub_expoente, pub->pub_expoente_sz);
	pub_ucl->modulus_length = pub->mod_sz;
	pub_ucl->public_exponent_length = pub->pub_expoente_sz;

	retval = ucl_pkcs1_es_oaep_sha256_encrypt(mensagem_cifrada, mensagem,
			mensagem_tamanho, pub_ucl, NULL, 0);

	return retval;
}

int CriptoDAF::decifra_RSAAES_OAEP(uint8_t *mensagem_decifrada,
		size_t *mensagem_decifrada_tamanho, uint8_t *mensagem_cifrada,
		chave_privada_RSA_t *priv) {
	int retval;
	ucl_rsa_private_key_t priv_ucl;

	priv_ucl.modulus = priv->mod;
	priv_ucl.modulus_length = priv->mod_sz;
	priv_ucl.private_exponent = priv->priv_expoente;

	retval = ucl_pkcs1_es_oaep_sha256_decrypt(mensagem_decifrada,
			mensagem_decifrada_tamanho, mensagem_cifrada, &priv_ucl, NULL, 0);

	return retval;
}

int CriptoDAF::cifra_ECC(uint8_t *mensagem, size_t mensagem_sz,
		chave_publica_ECC_P256_t *pub, cifragem_ECC_P256_t *cifragem) {
	int retval;

	cifragem->msg_cifrada_sz = mensagem_sz;

	retval = ucl_ecies_encrypt_p256r1_aes_hmac_sha256(cifragem->pub.x,
			cifragem->pub.y, cifragem->msg_cifrada, cifragem->hmac,
			SECP256R1_BYTESIZE, (u8*) secp256r1.xg, (u8*) secp256r1.yg, pub->x,
			pub->y, (u8*) secp256r1.a, (u8*) secp256r1.n, (u8*) secp256r1.p,
			mensagem, mensagem_sz);

	return retval;
}

int CriptoDAF::decifra_ECC(cifragem_ECC_P256_t *cifragem,
		chave_privada_ECC_P256_t *priv, uint8_t *mensagem,
		size_t *mensagem_sz) {
	int retval;

	*mensagem_sz = cifragem->msg_cifrada_sz;

	retval = ucl_ecies_decrypt_p256r1_aes_hmac_sha256(mensagem,
	SECP256R1_BYTESIZE, (u8*) secp256r1.xg, (u8*) secp256r1.yg,
			(u8*) secp256r1.a, (u8*) secp256r1.n, (u8*) secp256r1.p, priv->d,
			cifragem->pub.x, cifragem->pub.y, cifragem->msg_cifrada,
			cifragem->msg_cifrada_sz, cifragem->hmac);

	return retval;
}

int CriptoDAF::gera_HMAC_SHA256(uint8_t *resumo, const uint8_t *mensagem,
		const uint32_t mensagem_tamanho, const uint8_t *chave,
		const size_t chave_tamanho) {
	int retval;

	retval = ucl_hmac_sha256(resumo, 32, const_cast<u8*>(mensagem),
			mensagem_tamanho, const_cast<u8*>(chave), chave_tamanho);

	return retval;
}

int CriptoDAF::verifica_HMAC_SHA256(const uint8_t *resumo_gerado,
		const uint8_t *mensagem, const uint32_t mensagem_tamanho,
		const uint8_t *chave, const uint32_t chave_tamanho) {
	uint8_t resumo_cmp[32];
	int retval;

	retval = ucl_hmac_sha256(resumo_cmp, 32, const_cast<uint8_t*>(mensagem),
			mensagem_tamanho, const_cast<uint8_t*>(chave), chave_tamanho);
	if (retval != 0)
		return retval;

	if (memcmp(resumo_gerado, resumo_cmp, 32) == 0)
		return 0;

	return 1;
}

int CriptoDAF::gera_resumo_SHA256(uint8_t *resumo, uint8_t *mensagem,
		uint32_t mensagem_tamanho) {
	int retval;

	retval = ucl_sha256(resumo, mensagem, mensagem_tamanho);

	return retval;
}

int CriptoDAF::verifica_resumo_SHA256(uint8_t *resumo_gerado, uint8_t *mensagem,
		uint32_t mensagem_tamanho) {
	uint8_t resumo_cmp[32];
	int retval;

	retval = ucl_sha256(resumo_cmp, mensagem, mensagem_tamanho);
	if (retval != 0)
		return retval;

	if (memcmp(resumo_gerado, resumo_cmp, 32) == 0)
		return 0;

	return 1;
}

int CriptoDAF::gera_numero_aleatorio(uint8_t *numero, uint32_t tamanho) {
	int retval;

	retval = ucl_rng_read(numero, tamanho, UCL_RAND_DEFAULT);

	return retval;
}
