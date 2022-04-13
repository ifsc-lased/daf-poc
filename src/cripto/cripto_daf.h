#ifndef CRIPTO_H
#define CRIPTO_H

#include <stdint.h>
#include <stdio.h>

#define BUF_UCL_TAMANHO 8192
#define BUF_AUXILIAR_TAMANHO 8192

typedef struct {
	uint8_t x[32];
	uint8_t y[32];
} chave_publica_ECC_P256_t;

typedef struct {
	uint8_t d[32];
} chave_privada_ECC_P256_t;

typedef struct {
	uint8_t x[48];
	uint8_t y[48];
} chave_publica_ECC_P384_t;

typedef struct {
	uint8_t d[48];
} chave_privada_ECC_P384_t;

typedef struct {
	uint8_t r[32];
	uint8_t s[32];
} assinatura_ECC_P256_t;

typedef struct {
	uint8_t r[48];
	uint8_t s[48];
} assinatura_ECC_P384_t;

typedef struct {
	uint8_t pub_expoente[4];
	uint8_t mod[512];
	size_t mod_sz;
	uint32_t pub_expoente_sz;
} chave_publica_RSA_t;

typedef struct {
	uint8_t *priv_expoente;
	uint8_t *mod;
	size_t mod_sz;
} chave_privada_RSA_t;

typedef struct {
	chave_publica_ECC_P256_t pub;
	uint8_t *msg_cifrada;
	size_t msg_cifrada_sz;
	uint8_t hmac[32];
} cifragem_ECC_P256_t;

class CriptoDAF {
private:
	static uint32_t buf_ucl[BUF_UCL_TAMANHO];
	static uint8_t buf_auxiliar[BUF_AUXILIAR_TAMANHO];

public:

	/**
	 * @brief Configura a biblioteca de criptografia para ser usada. Essa funcao deve ser chamada somente uma vez.
	 *
	 * @return int   0: OK, < 0: codigo do erro.
	 */
	static int configura(void);

	/**
	 * @brief Gera par de chaves EC P-256
	 *
	 * @param[out] pub           chave publica alocada previamente
	 * @param[out] priv          chave privada alocada previamente
	 * @return int          0: sucesso, !=0: erro
	 */
	static int gera_chave_ECC(chave_publica_ECC_P256_t *pub,
			chave_privada_ECC_P256_t *priv);

	/**
	 * @brief Gera par de chaves EC P-384
	 *
	 * @param[out] pub           chave publica alocada previamente
	 * @param[out] priv          chave privada alocada previamente
	 * @return int          0: sucesso, !=0: erro
	 */
	static int gera_chave_ECC(chave_publica_ECC_P384_t *pub,
			chave_privada_ECC_P384_t *priv);

	/**
	 * @brief Gera assinatura usando chave EC P-256
	 *
	 * @param[out] ass           assinatura alocada previamente
	 * @param[in] msg            mensagem a ser assinada
	 * @param[in] msg_tamanho    tamanho da mensagem
	 * @param[in] priv           chave privada usada para assinar
	 * @return int
	 */
	static int gera_assinatura_ECC(assinatura_ECC_P256_t *ass,
			const uint8_t *msg, const size_t msg_tamanho,
			const chave_privada_ECC_P256_t *priv);

	/**
	 * @brief Gera assinatura usando chave EC P-384
	 *
	 * @param[out] ass           assinatura alocada previamente
	 * @param[in] msg            mensagem a ser assinada
	 * @param[in] msg_tamanho    tamanho da mensagem
	 * @param[in] priv           chave privada usada para assinar
	 * @return int
	 */
	static int gera_assinatura_ECC(assinatura_ECC_P384_t *ass,
			const uint8_t *msg, const size_t msg_tamanho,
			const chave_privada_ECC_P384_t *priv);

	/**
	 * @brief Verifica assinatura usando chave EC P-256
	 *
	 * @param[in] ass            assinatura
	 * @param[in] msg            mensagem a ser verificada
	 * @param[in] msg_tamanho    tamanho da mensagem
	 * @param[in] pub            chave publica usada para verificar
	 * @return int
	 */
	static int verifica_assinatura_ECC(const assinatura_ECC_P256_t *ass,
			const uint8_t *msg, const uint32_t msg_tamanho,
			const chave_publica_ECC_P256_t *pub);

	/**
	 * @brief Verifica assinatura usando chave EC P-384
	 *
	 * @param[in] ass            assinatura
	 * @param[in] msg            mensagem a ser verificada
	 * @param[in] msg_tamanho    tamanho da mensagem
	 * @param[in] pub            chave pública usada para verificar
	 * @return int
	 */
	static int verifica_assinatura_ECC(const assinatura_ECC_P384_t *ass,
			const uint8_t *msg, const uint32_t msg_tamanho,
			const chave_publica_ECC_P384_t *pub);

	/**
	 * @brief Gera chave RSA
	 *
	 * @param[out] pub                   chave publica
	 * @param[out] priv                  chave privada
	 * @param[in] tamanho_RSA_bits       tamanho em bits da chave RSA
	 * @return int                       0: OK, < 0: erro ao gerar chaves.
	 */
	static int gera_chave_RSA(chave_publica_RSA_t *pub,
			chave_privada_RSA_t *priv, uint32_t tamanho_RSA_bits);

	/**
	 * @brief Assina a mensagem usando chave RSA
	 *
	 * @param[out] assinatura               buffera que vai receber a assinatura
	 * @param[in] mensagem                  mensagem a ser assinada
	 * @param[in] mensagem_tamanho          tamanho em bytes da mensagem a ser assinada
	 * @param[in] priv                      chave privada que assinara a mensagem
	 * @return int                          0: OK, < 0: erro ao assinar mensagem.
	 */
	static int gera_assinatura_RSA(uint8_t *assinatura, uint8_t *mensagem,
			uint32_t mensagem_tamanho, chave_privada_RSA_t *priv);

	/**
	 * @brief verifica a assinatura RSA
	 *
	 * @param[in] assinatura            buffer com a assinatura
	 * @param[in] mensagem              mensagem a ser verificada
	 * @param[in] mensagem_tamanho      tamanho em bytes da mensagem
	 * @param[in] pub                   chave publica
	 * @return int                      0: assinatura OK.
	 */
	static int verifica_assinatura_RSA(uint8_t *assinatura, uint8_t *mensagem,
			uint32_t mensagem_tamanho, chave_publica_RSA_t *pub);

	/**
	 * @brief cifragem de uma mensagem no esquema de cifragem RSAAES_OAEP
	 *
	 * @param[out] mensagem_cifrada          buffer que recebera mensagem cifrada
	 * @param[in] mensagem                   mensagem a ser cifrada
	 * @param[in] mensagem_tamanho           tamanho em bytes da mensagem a ser cifrada
	 * @param[in] pub                        chave publica
	 * @return int                           0: OK, < 0: erro ao cifrar mensagem.
	 */
	static int cifra_RSAAES_OAEP(uint8_t *mensagem_cifrada, uint8_t *mensagem,
			uint32_t mensagem_tamanho, chave_publica_RSA_t *pub);

	/**
	 * @brief decifragem de uma mensagem cifrada no esquema de cifragem RSAAES_OAEP
	 *
	 * @param[out] mensagem_decifrada                 buffer que recebera mensagem decifrada
	 * @param[out] mensagem_decifrada_tamanho         tamanho do buffer que recebera mensagem decifrada
	 * @param[in] mensagem_cifrada                    mensagem a ser decifrada
	 * @param[in] priv                                chave privada
	 * @return int                                    0: OK, < 0: erro ao decifrar.
	 */
	static int decifra_RSAAES_OAEP(uint8_t *mensagem_decifrada,
			size_t *mensagem_decifrada_tamanho, uint8_t *mensagem_cifrada,
			chave_privada_RSA_t *priv);

	/**
	 * @brief cifragem da mensagem usando ECIES AES128 (CBC) e HMAC SHA-256 sob
	 * a curva P-256
	 *
	 * @param[in] mensagem 		mensagem a ser cifrada com padding de 16 bytes ja
	 * incluidos. O tamanho final da mensagem pode ser calculado como:
	 * tamanho com padding = tamanho + 16 - (tamanho % 16)
	 * @param[in] mensagem_sz 	tamanho da mensagem com padding
	 * @param[in] pub 			chave publica usada para cifragem
	 * @param[out] 				cifragem struct que representa o resultado da cifragem,
	 * contendo a chave publica efemera, hmac e a mensagem cifrada em si.
	 * @return 					se 0 a cifragem foi realizada com sucesso, senao falhou.
	 */
	static int cifra_ECC(uint8_t *mensagem, size_t mensagem_sz,
			chave_publica_ECC_P256_t *pub, cifragem_ECC_P256_t *cifragem);

	/**
	 * @brief decifragem usando ECIES AES128 (CBC) e HMAC SHA-256 sob a curva
	 * P-256
	 *
	 * @param[in] cifragem 		struct que representa o resultado da cifragem,
	 * contendo a chave publica efemera, hmac e a mensagem cifrada em si.
	 * @param[in] priv 			chave privada
	 * @param[out] mensagem 	mensagem decifrada
	 * @param[out] mensagem_sz 	tamanho da mensagem decifrada
	 */
	static int decifra_ECC(cifragem_ECC_P256_t *cifragem,
			chave_privada_ECC_P256_t *priv, uint8_t *mensagem,
			size_t *mensagem_sz);

	/**
	 * @brief geracao de resumos HMAC-SHA256
	 *
	 * @param[out] resumo                   buffer para receber o hmac gerado
	 * @param[in] mensagem                  mensagem a ser resumida
	 * @param[in] mensagem_tamanho          tamanho em bytes da mensagem
	 * @param[in] chave                     chave para a geracao do resumo
	 * @param[in] chave_tamanho             tamanho em bytes da chave
	 * @return int                          0: OK, 0 <: erro.
	 */
	static int gera_HMAC_SHA256(uint8_t *resumo, const uint8_t *mensagem,
			const uint32_t mensagem_tamanho, const uint8_t *chave,
			const size_t chave_tamanho);

	/**
	 * @brief verifica resumos HMAC-SHA256
	 *
	 * @param[in] resumo_gerado           resumo
	 * @param[in] mensagem                mensagem a ser resumida
	 * @param[in] mensagem_tamanho        tamanho em bytes da mensagem a ser resumida
	 * @param[in] chave                   chave para a geracao do resumo
	 * @param[in] chave_tamanho           tamanho em bytes da chave
	 * @return int                        0: Ok, resumos sao iguais. 1: resumos sao diferentes. <0: erro ao gerar resumo.
	 */
	static int verifica_HMAC_SHA256(const uint8_t *resumo_gerado,
			const uint8_t *mensagem, const uint32_t mensagem_tamanho,
			const uint8_t *chave, const uint32_t chave_tamanho);

	/**
	 * @brief gera resumo criptografico SHA-256
	 *
	 * @param[out] resumo                 buffer para receber o hmac gerado
	 * @param[in] mensagem                mensagem a ser resumida
	 * @param[in] mensagem_tamanho        tamanho em bytes da mensagem
	 * @return int
	 */
	static int gera_resumo_SHA256(uint8_t *resumo, uint8_t *mensagem,
			uint32_t mensagem_tamanho);

	/**
	 * @brief verifica resumos SHA256
	 *
	 * @param[in] resumo_gerado             resumo
	 * @param[in] mensagem                  mensagem a ser resumida
	 * @param[in] mensagem_tamanho          tamanho em bytes da mensagem
	 * @return int                          0: Ok, resumos sao iguais. 1: resumos sao diferentes. <0: erro ao gerar resumo.
	 */
	static int verifica_resumo_SHA256(uint8_t *resumo_gerado, uint8_t *mensagem,
			uint32_t mensagem_tamanho);

	/**
	 * @brief gera numero aleatorio
	 *
	 * @param numero        buffer alocado para receber os numeros aleatorios
	 * @param tamanho       quantidade de bytes
	 * @return int          <0: erro ao gerar numeros. >= 1: quantidade de bytes gerados.
	 */
	static int gera_numero_aleatorio(uint8_t *numero, uint32_t tamanho);

};

#endif // CRIPTO_H
