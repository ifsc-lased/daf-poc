#ifndef UTILS_JWT_JWT_H_
#define UTILS_JWT_JWT_H_

#include <iostream>
#include "../base64url/base64url.h"
#include "../json/json.h"
#include "../../cripto/cripto_daf.h"
#include "../../memoria/memoria_segura.h"

class JWT {

public:

	enum ERR_CODE {
		ERR_BASE64 = -1,
		ERR_ASSINATURA = -2,
		ERR_TAMANHO_TOKEN = -3,
		ERR_CRIPTO = -4,
		ERR_JSON = -5
	};

	enum ALG {
		HS256 = 0, RS256 = 1, ES256 = 2, ES384 = 3,
	};

	enum ERRO {
		SUCESSO = 1, TOKEN_TAMANHO_INVALIDO = 2, ALG_NAO_SUPORTADO = 0
	};

	enum VALIDACAO {
		ASSINATURA_VALIDA = 0,
		ASSINATURA_INVALIDA = 1,
		TOKEN_INVALIDO = 2,
		BASE64_INVALIDO = 3,
		ALG_INVALIDO = 4,
	};

	typedef struct {
		const char *b64;
		size_t sz;
	} b64_field_t;

	typedef struct {
		b64_field_t header;
		b64_field_t encrypted_key;
		b64_field_t iv;
		b64_field_t ciphertext;
		b64_field_t authentication_tag;
	} fields_jwe_t;

	typedef struct {
		b64_field_t header;
		b64_field_t payload;
		b64_field_t assinatura;
	} fields_jws_t;

	typedef union {
		chave_publica_ECC_P384_t *ECC384;
		chave_publica_ECC_P256_t *ECC256;
		chave_publica_RSA_t *RSA;
		chave_publica_ECC_P256_t *ECC_P256;
		chave_publica_ECC_P384_t *ECC_P384;
		MemoriaSegura::chave_SEF_t *chave_sef;
	} u_chave_publica_t;

	typedef union {
		chave_privada_ECC_P256_t *ECC_P256;
		chave_privada_RSA_t *RSA;
		chave_privada_ECC_P384_t *ECC_P384;
		MemoriaSegura::chave_SEF_t *chave_sef;
	} u_chave_privada_t;

	/**
	 * @brief gera token JWT assinado com algoritmo HS256
	 *
	 * @param[in] payload			cadeia de caracteres com objeto json do payload do token
	 * @param[in] payload_sz		tamanho do payload em bytes
	 * @param[in] chave				cadeia de caracteres com a chave para assinar o token
	 * @param[in] chave_sz			tamanho da chave em bytes
	 * @param[out] token			cadeia de caracteres do token jwt
	 * @param[out] token_max_sz 	tamanho do buffer alocado para receber o token jwt
	 *
	 */
	static int geraTokenHS256(const uint8_t *payload,
			const size_t payload_sz, const uint8_t *chave,
			const size_t chave_sz, char *token, const size_t token_max_sz);


	/**
	 * @brief valida a assinatura de um token JWT assinado com algoritmo HS256
	 *
	 * @param[in] token				cadeia de caracteres do token jwt
	 * @param[in] token_sz			tamanho do token jwt
	 * @param[in] chave				cadeia de caracteres com a chave que assinou o token
	 * @param[in] chave_sz			tamanho da chave em bytes
	 * @param[out] payload			buffer para alocar o payload do token jwt
	 * @param[out] payload_max_sz	tamanho do buffer alocado para o payload do token
	 *
	 */
	static int verificaTokenHS256(const char *token, const size_t token_sz,
			const uint8_t *chave, const size_t chave_sz, uint8_t *payload,
			const size_t payload_max_sz);


	/**
	 * @brief gera token JWT assinado com algoritmo ES256
	 *
	 * @param[in] payload			cadeia de caracteres com objeto json do payload do token
	 * @param[in] payload_sz		tamanho do payload em bytes
	 * @param[in] chave_priv		struct que contem a chave privada utilizada para gerar a assinatura do token
	 * @param[out] token			cadeia de caracteres do token jwt
	 * @param[out] token_max_sz 	tamanho do buffer alocado para receber o token jwt
	 * @param[in] has_JWK			variavel que indica se a chave publica vai ser transmitida no token
	 * @param[in] chave_pub			struct que contem a chave publica par da chave privada que assinou o token
	 *
	 */
	static int geraTokenES256(const uint8_t *payload,
			const size_t payload_sz, const chave_privada_ECC_P256_t *chave_priv,
			char *token, const size_t token_max_sz, const bool has_JWK = false,
			const chave_publica_ECC_P256_t *chave_pub = nullptr);

	/**
	 * @brief valida a assinatura de um token JWT assinado com algoritmo ES256
	 *
	 * @param[in] token				cadeia de caracteres do token jwt
	 * @param[in] token_sz			tamanho do token jwt
	 * @param[in] chave_publica		struct que contem a chave publica par da chave privada que assinou o token
	 * @param[out] payload			buffer para alocar o payload do token jwt
	 * @param[out] payload_max_sz	tamanho do buffer alocado para o payload do token
	 *
	 */
	static int verificaTokenES256(const char* token, const size_t token_sz, const chave_publica_ECC_P256_t *chave_publica,
	 uint8_t *payload, const size_t payload_max_sz);

	/**
	 * @brief gera token JWT assinado com algoritmo ES384
	 *
	 * @param[in] payload			cadeia de caracteres com objeto json do payload do token
	 * @param[in] payload_sz		tamanho do payload em bytes
	 * @param[in] chave_priv		struct que contem a chave privada utilizada para gerar a assinatura do token
	 * @param[out] token			cadeia de caracteres do token jwt
	 * @param[out] token_max_sz 	tamanho do buffer alocado para receber o token jwt
	 * @param[in] has_JWK			variavel que indica se a chave publica vai ser transmitida no token
	 * @param[in] chave_pub			struct que contem a chave publica par da chave privada que assinou o token
	 *
	 */
	static int geraTokenES384(const uint8_t *payload,
			const size_t payload_sz, const chave_privada_ECC_P384_t *chave_priv,
			char *token, const size_t token_max_sz, const bool has_JWK = false,
			const chave_publica_ECC_P384_t *chave_pub = nullptr);


	/**
	 * @brief valida a assinatura de um token JWT assinado com algoritmo ES384
	 *
	 * @param[in] token				cadeia de caracteres do token jwt
	 * @param[in] token_sz			tamanho do token jwt
	 * @param[in] chave_publica		struct que contem a chave publica par da chave privada que assinou o token
	 * @param[out] payload			buffer para alocar o payload do token jwt
	 * @param[out] payload_max_sz	tamanho do buffer alocado para o payload do token
	 *
	 */
	static int verificaTokenES384(const char* token, const size_t token_sz, const chave_publica_ECC_P384_t *chave_publica,
	 uint8_t *payload, const size_t payload_max_sz);

	/**
	 * @brief obtem o payload de um token JWE que fora cifrado
	 *
	 * @param[in] token				cadeia de caracteres do token jwt
	 * @param[in] token_sz			tamanho do token jwt
	 * @param[in] privkey			strcut que contem a chave privada para decifrar o token jwe
	 * @param[out] out				buffer para alocar o payload do token jwe
	 * @param[out] out_sz	tamanho do buffer alocado para o payload do token
	 *
	 */
	static VALIDACAO verificaTokenJWE(char *token, size_t token_sz,
			JWT::u_chave_privada_t *privkey, uint8_t *out, size_t out_sz);

private:

	/**
	 * @brief extrai os campos do token jwt
	 *
	 * @param [in]  token       token jwt
	 * @param [in]  token_sz    tamanho token JWT em bytes
	 * @param [out] campos      struct contendo os campos do token jwt
	 * @return ERRO             resultado do processamento
	 */
	static ERRO getParametrosToken(const char *token, const size_t token_sz,
			JWT::fields_jws_t *campos);

	/**
	 * @brief extrai os campos do token jwe
	 *
	 * @param [in]  token       token jwe
	 * @param [in]  token_sz    tamanho token jwe em bytes
	 * @param [out] campos      struct contendo os campos do token jwt
	 * @return ERRO             resultado do processamento
	 */
	static ERRO getParametrosTokenJWE(char *token, size_t token_sz,
			JWT::fields_jwe_t *campos);
};

#endif /* UTILS_JWT_JWT_H_ */
