#ifndef BASE64URL_H_
#define BASE64URL_H_

#include <iostream>

class Base64URL {

public:
	/**
	 * @brief Codifica um bloco de dados em Base64URL
	 *
	 * @param[in] in      dados a serem codificados
	 * @param[in] in_sz   tamanho do buffer
	 * @param[out] out    buffer que vai receber a string codificada
	 * @param [in] out_sz tamanho do buffer alocado
	 * @return int        0: falha na codificao, > 0: tamanho do buffer codificado
	 */
	static int b64url_encode(const uint8_t *in, size_t in_sz, char *out,
			size_t out_sz);

	/**
	 * @brief Decodifica um bloco de dados em Base64URL
	 *
	 * @param[in] in        string a ser decodificada
	 * @param[in] in_sz     tamanho da string a ser decodificada
	 * @param[out] out      buffer que vai receber os dados decodificado
	 * @param [in] out_sz   tamanho do buffer out alocado
	 * @return int          0: falha na decodificacao, > 0: tamanho do buffer decodificado
	 */
	static int b64url_decode(const char *in, size_t in_sz, uint8_t *out,
			size_t out_sz);

private:

	Base64URL() {
	}

	/**
	 * @brief verifica o tamanho correto do buffer de decodificacao
	 *
	 * @param[in] inlen     tamanho do buffer a ser codificado
	 * @return size_t       tamanho do buffer de saida
	 */
	static size_t b64url_decoded_size(size_t inlen);

	/**
	 * @brief verifica o tamanho correto do buffer de codificacao
	 *
	 * @param[in] inlen     tamanho do buffer a ser decodificado
	 * @return size_t       tamanho do buffer de saida
	 */

	static size_t b64url_encoded_size(size_t inlen);

	/**
	 * @brief verifica se um  caracter e' um caracter valido da tabela de caracteres da codificacao Base64URL
	 *
	 * @param[in] c         caracter a ser verificado
	 * @return int          1: caracter valido, 0: caracter invalido
	 */
	static int b64_isvalidchar(char c);

};

#endif /* BASE64URL_H_ */
