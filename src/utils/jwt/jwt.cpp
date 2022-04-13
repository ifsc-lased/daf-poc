#include "../jwt/jwt.h"
#include <string.h>
#include "../../cripto/cripto_daf.h"

int JWT::geraTokenHS256(const uint8_t *payload, const size_t payload_sz,
		const uint8_t *chave, const size_t chave_sz, char *token,
		const size_t token_max_sz) {

	const char header[] = "{\"typ\":\"JWT\",\"alg\":\"HS256\"}";

	size_t token_sz = 0;

	/**
	 * header
	 */

	size_t encoded_sz = Base64URL::b64url_encode((uint8_t*) header,
			strlen(header), token, token_max_sz);
	if (encoded_sz == 0)
		return ERR_TAMANHO_TOKEN;
	token_sz += encoded_sz;

	/**
	 * payload
	 */

	token[token_sz++] = '.';

	encoded_sz = Base64URL::b64url_encode(payload, payload_sz, &token[token_sz],
			token_max_sz - token_sz);
	if (encoded_sz == 0)
		return ERR_TAMANHO_TOKEN;
	token_sz += encoded_sz;

	token[token_sz++] = '.';

	/**
	 * assinatura
	 */

	uint8_t hmac[32];

	// mensagem a ser assinada: base64(header) + "." + base64(payload)
	// ref.: RFC 7515

	// '-1' para nao incluir o ultimo '.' do token
	if (CriptoDAF::gera_HMAC_SHA256(hmac, (uint8_t*) token, token_sz - 1, chave,
			chave_sz) != 0)
		return ERR_CRIPTO;

	encoded_sz = Base64URL::b64url_encode(hmac, sizeof(hmac), &token[token_sz],
			token_max_sz - token_sz);
	if (encoded_sz == 0)
		return ERR_TAMANHO_TOKEN;
	token_sz += encoded_sz;

	token[token_sz++] = '\0';

	return token_sz;
}

int JWT::verificaTokenES256(const char *token, const size_t token_sz,
		const chave_publica_ECC_P256_t *chave_publica, uint8_t *payload,
		const size_t payload_max_sz) {

	JWT::fields_jws_t campos;
	if (getParametrosToken(token, token_sz, &campos) != JWT::SUCESSO)
		return ERR_TAMANHO_TOKEN;

	if ((campos.header.sz + campos.payload.sz + campos.assinatura.sz + 2)
			!= token_sz)
		return ERR_TAMANHO_TOKEN;

	uint8_t assinatura[64];
	Base64URL::b64url_decode((const char*) campos.assinatura.b64,
			campos.assinatura.sz, assinatura, 64);

	assinatura_ECC_P256_t ass;

	memcpy(ass.r, assinatura, 32);
	memcpy(ass.s, &assinatura[32], 32);

	const uint8_t *msg_assinada = (uint8_t*) token;
	size_t msg_assinada_sz = campos.header.sz + 1 + campos.payload.sz;

	if (CriptoDAF::verifica_assinatura_ECC(&ass, msg_assinada, msg_assinada_sz,
			chave_publica) != 0)
		return ERR_ASSINATURA;

	// assinatura valida

	return Base64URL::b64url_decode(campos.payload.b64, campos.payload.sz,
			payload, payload_max_sz);

}

int JWT::verificaTokenES384(const char *token, const size_t token_sz,
		const chave_publica_ECC_P384_t *chave_publica, uint8_t *payload,
		const size_t payload_max_sz) {

	JWT::fields_jws_t campos;
	if (getParametrosToken(token, token_sz, &campos) != JWT::SUCESSO)
		return ERR_TAMANHO_TOKEN;

	if ((campos.header.sz + campos.payload.sz + campos.assinatura.sz + 2)
			!= token_sz)
		return ERR_TAMANHO_TOKEN;

	uint8_t assinatura[96];
	Base64URL::b64url_decode((const char*) campos.assinatura.b64,
			campos.assinatura.sz, assinatura, 96);

	assinatura_ECC_P384_t ass;

	memcpy(ass.r, assinatura, 48);
	memcpy(ass.s, &assinatura[48], 48);

	const uint8_t *msg_assinada = (uint8_t*) token;
	size_t msg_assinada_sz = campos.header.sz + 1 + campos.payload.sz;

	if (CriptoDAF::verifica_assinatura_ECC(&ass, msg_assinada, msg_assinada_sz,
			chave_publica) != 0)
		return ERR_ASSINATURA;

	// assinatura valida

	return Base64URL::b64url_decode(campos.payload.b64, campos.payload.sz,
			payload, payload_max_sz);

}

int JWT::verificaTokenHS256(const char *token, const size_t token_sz,
		const uint8_t *chave, const size_t chave_sz, uint8_t *payload,
		const size_t payload_max_sz) {

	JWT::fields_jws_t campos;

	if (getParametrosToken(token, token_sz, &campos) != JWT::SUCESSO)
		return ERR_TAMANHO_TOKEN;

	// sanity checking
	if ((campos.header.sz + campos.payload.sz + campos.assinatura.sz + 2)
			!= token_sz)
		return ERR_TAMANHO_TOKEN;

	uint8_t assinatura[32];
	if (Base64URL::b64url_decode(campos.assinatura.b64, campos.assinatura.sz,
			assinatura, sizeof(assinatura)) != 32)
		return ERR_BASE64;

	/**
	 * mensagem a ser verificada: base64(header) + "." + base64(payload)
	 * ref.: RFC 7515
	 */
	const uint8_t *msg_assinada = (uint8_t*) token;
	size_t msg_assinada_sz = campos.header.sz + 1 + campos.payload.sz;

	if (CriptoDAF::verifica_HMAC_SHA256(assinatura, msg_assinada,
			msg_assinada_sz, chave, chave_sz) != 0)
		return ERR_ASSINATURA;

	// assinatura valida

	return Base64URL::b64url_decode(campos.payload.b64, campos.payload.sz,
			payload, payload_max_sz);
}

int JWT::geraTokenES256(const uint8_t *payload, const size_t payload_sz,
		const chave_privada_ECC_P256_t *chave_priv, char *token,
		const size_t token_max_sz, const bool has_JWK,
		const chave_publica_ECC_P256_t *chave_pub) {

	size_t encoded_sz = 0;
	size_t token_sz = 0;

	/**
	 * header do token JWT
	 */

	const char header[] = "{\"typ\":\"JWT\",\"alg\":\"ES256\"}";

	if (has_JWK) {
		StaticJsonDocument<500> jwk_header;
		if (JSON::deserialize(const_cast<char*>(header), jwk_header)
				!= JSON::SUCESSO)
			return ERR_JSON;

		jwk_header["jwk"]["crv"] = "P-256";
		char x_b64[44];
		char y_b64[44];

		if (Base64URL::b64url_encode((uint8_t*) chave_pub->x,
				sizeof(chave_pub->x), x_b64, sizeof(x_b64)) <= 0)
			return ERR_BASE64;
		if (Base64URL::b64url_encode((uint8_t*) chave_pub->y,
				sizeof(chave_pub->y), y_b64, sizeof(y_b64)) <= 0)
			return ERR_BASE64;

		jwk_header["jwk"]["x"] = x_b64;
		jwk_header["jwk"]["y"] = y_b64;
		jwk_header["jwk"]["kty"] = "EC";

		char jwk_header_final[250];
		if (JSON::serialize(jwk_header, jwk_header_final,
				sizeof(jwk_header_final)) <= 0)
			return ERR_JSON;

		encoded_sz = Base64URL::b64url_encode((uint8_t*) jwk_header_final,
				strlen(jwk_header_final), token, token_max_sz);
		if (encoded_sz == 0)
			return ERR_TAMANHO_TOKEN;
		token_sz += encoded_sz;
	} else {
		encoded_sz = Base64URL::b64url_encode((uint8_t*) header, strlen(header),
				token, token_max_sz);
		if (encoded_sz == 0)
			return ERR_TAMANHO_TOKEN;
		token_sz += encoded_sz;
	}

	/**
	 * payload do token JWT
	 */

	token[token_sz++] = '.';

	encoded_sz = Base64URL::b64url_encode(payload, payload_sz, &token[token_sz],
			token_max_sz - token_sz);
	if (encoded_sz == 0)
		return ERR_TAMANHO_TOKEN;
	token_sz += encoded_sz;

	token[token_sz++] = '.';

	/**
	 * assinatura do token JWT
	 *
	 * obs.: a assinatura usando a chave EC, no token JWT, e' a concatenacao dos
	 * parametros r e s da assinatura
	 */

	assinatura_ECC_P256_t assinatura;

	// '-1' para nao incluir o ultimo '.' do token
	if (CriptoDAF::gera_assinatura_ECC(&assinatura, (uint8_t*) token,
			token_sz - 1, chave_priv) != 0)
		return ERR_CRIPTO;

	encoded_sz = Base64URL::b64url_encode((uint8_t*) &assinatura,
			sizeof(assinatura), &token[token_sz], token_max_sz - token_sz);
	if (encoded_sz == 0)
		return ERR_TAMANHO_TOKEN;
	token_sz += encoded_sz;

	token[token_sz++] = '\0';
	return token_sz;
}

int JWT::geraTokenES384(const uint8_t *payload, const size_t payload_sz,
		const chave_privada_ECC_P384_t *chave_priv, char *token,
		const size_t token_max_sz, const bool has_JWK,
		const chave_publica_ECC_P384_t *chave_pub) {
	size_t encoded_sz = 0;
	size_t token_sz = 0;

	/**
	 * header do token JWT
	 */

	const char header[] = "{\"typ\":\"JWT\",\"alg\":\"ES384\"}";

	if (has_JWK) {
		StaticJsonDocument<300> jwk_header;
		if (JSON::deserialize(const_cast<char*>(header), jwk_header)
				!= JSON::SUCESSO)
			return ERR_JSON;

		jwk_header["jwk"]["crv"] = "P-384";
		char x_b64[65];
		char y_b64[65];

		Base64URL::b64url_encode((uint8_t*) chave_pub->x, sizeof(chave_pub->x),
				x_b64, sizeof(x_b64));
		Base64URL::b64url_encode((uint8_t*) chave_pub->y, sizeof(chave_pub->y),
				y_b64, sizeof(y_b64));

		jwk_header["jwk"]["x"] = x_b64;
		jwk_header["jwk"]["y"] = y_b64;
		jwk_header["jwk"]["kty"] = "EC";

		char jwk_header_final[300];
		if (JSON::serialize(jwk_header, jwk_header_final,
				sizeof(jwk_header_final)) <= 0)
			return ERR_JSON;

		encoded_sz = Base64URL::b64url_encode((uint8_t*) jwk_header_final,
				strlen(jwk_header_final), token, token_max_sz);
		if (encoded_sz == 0)
			return ERR_TAMANHO_TOKEN;
		token_sz += encoded_sz;
	} else {
		encoded_sz = Base64URL::b64url_encode((uint8_t*) header, strlen(header),
				token, token_max_sz);
		if (encoded_sz == 0)
			return ERR_TAMANHO_TOKEN;
		token_sz += encoded_sz;
	}

	/**
	 * payload do token JWT
	 */

	token[token_sz++] = '.';

	encoded_sz = Base64URL::b64url_encode(payload, payload_sz, &token[token_sz],
			token_max_sz - token_sz);
	if (encoded_sz == 0)
		return ERR_TAMANHO_TOKEN;
	token_sz += encoded_sz;

	token[token_sz++] = '.';

	/**
	 * assinatura do token JWT
	 *
	 * obs.: a assinatura usando a chave EC, no token JWT, e' a concatenacao dos
	 * parametros r e s da assinatura
	 */

	assinatura_ECC_P384_t assinatura;

	// '-1' para nao incluir o ultimo '.' do token
	if (CriptoDAF::gera_assinatura_ECC(&assinatura, (uint8_t*) token,
			token_sz - 1, chave_priv) != 0)
		return ERR_CRIPTO;

	encoded_sz = Base64URL::b64url_encode((uint8_t*) &assinatura,
			sizeof(assinatura), &token[token_sz], token_max_sz - token_sz);
	if (encoded_sz == 0)
		return ERR_TAMANHO_TOKEN;
	token_sz += encoded_sz;

	token[token_sz++] = '\0';
	return token_sz;
}


JWT::VALIDACAO JWT::verificaTokenJWE(char *token, size_t token_sz,
		JWT::u_chave_privada_t *privkey, uint8_t *out, size_t out_sz) {

	/**
	 *  para esta implementacao, a chave SEF foi transmitida em claro, codificada em base64URL
	 *  em funcao das bibliotecas criptograficas da fabricante do mcu
	 */

	JWT::fields_jwe_t campos;

	if (getParametrosTokenJWE(token, token_sz, &campos) != JWT::ERRO::SUCESSO)
		return JWT::VALIDACAO::TOKEN_INVALIDO;


	if (Base64URL::b64url_decode(campos.ciphertext.b64, campos.ciphertext.sz,
			out, out_sz) <= 0)
		return JWT::VALIDACAO::BASE64_INVALIDO;


	return JWT::VALIDACAO::ASSINATURA_VALIDA;

}


JWT::ERRO JWT::getParametrosTokenJWE(char *token, size_t token_sz,
		JWT::fields_jwe_t *campos) {

	campos->header.sz = 0;
	campos->encrypted_key.sz = 0;
	campos->iv.sz = 0;
	campos->ciphertext.sz = 0;
	campos->authentication_tag.sz = 0;

	campos->header.b64 = token;

	size_t i;

	for (i = 0; i < token_sz; i++) {
		if (token[i] == '.')
			break;
		else
			campos->header.sz++;
	}

	i++;
	campos->encrypted_key.b64 = &token[i];

	for (; i < token_sz; i++) {
		if (token[i] == '.')
			break;
		else
			campos->encrypted_key.sz++;
	}

	i++;
	campos->iv.b64 = &token[i];

	for (; i < token_sz; i++) {
		if (token[i] == '.')
			break;
		else
			campos->iv.sz++;
	}

	i++;
	campos->ciphertext.b64 = &token[i];
	for (; i < token_sz; i++) {
		if (token[i] == '.')
			break;
		else
			campos->ciphertext.sz++;
	}

	i++;

	campos->authentication_tag.b64 = &token[i];
	for (; i < token_sz; i++) {
		campos->authentication_tag.sz++;
	}
	if (campos->header.sz == 0 || campos->authentication_tag.sz == 0
			|| campos->iv.sz == 0)
		return JWT::TOKEN_TAMANHO_INVALIDO;
	else
		return JWT::SUCESSO;
}

JWT::ERRO JWT::getParametrosToken(const char *token, const size_t token_sz,
		JWT::fields_jws_t *campos) {

	campos->header.sz = 0;
	campos->payload.sz = 0;
	campos->assinatura.sz = 0;

	campos->header.b64 = token;

	size_t i;
	for (i = 0; i < token_sz; i++) {
		if (token[i] == '.')
			break;
		else
			campos->header.sz++;
	}

	i++;
	campos->payload.b64 = &token[i];

	for (; i < token_sz; i++) {
		if (token[i] == '.')
			break;
		else
			campos->payload.sz++;
	}

	i++;
	campos->assinatura.b64 = &token[i];

	for (; i < token_sz; i++) {
		campos->assinatura.sz++;
	}

	if (campos->header.sz == 0 || campos->payload.sz == 0
			|| campos->assinatura.sz == 0)
		return JWT::TOKEN_TAMANHO_INVALIDO;
	else
		return JWT::SUCESSO;
}
