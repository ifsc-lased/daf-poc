#ifndef MEMORIA_SEGURA_H_
#define MEMORIA_SEGURA_H_

#include <stdint.h>
#include <stddef.h>
#include "../daf_utils.h"
#include "../cripto/cripto_daf.h"

#define CERTIFICADO_MAX_SZ 1024

class MemoriaSegura {
public:
	static const uintptr_t AddrNVSRAM = 0x20080000;
	static const size_t TamanhoNVSRAM = 8 * 1024;
	typedef struct {
		uint32_t versao_SB;
		uint8_t resumo_SB[32];
		bool particaoExistente;
	} parametros_atualizacao_t;
	typedef struct {
		size_t sz;
		uint8_t certificado[CERTIFICADO_MAX_SZ];
	} certificado_t;
	typedef struct {
		uint8_t chave[64];
	} chave_SEF_t;
	typedef struct {
		uint8_t chave[64];
	} chave_PAF_t;
	typedef struct {
		uint8_t valor[16];
	} uuid_t;
	typedef struct {
		char modelo[20];
	} modelo_DAF_t;
	typedef struct {
		char numeros[15];
	} CNPJ_t;

	typedef struct {
		uint32_t guardas[DAF_Utils::GUARDAS_SZ];
		parametros_atualizacao_t parametros_atualizacao;
		certificado_t certificado;
		chave_publica_ECC_P384_t chave_do_certificado;
		chave_privada_ECC_P384_t chave_ateste_priv;
		chave_publica_ECC_P384_t chave_ateste_pub;
		chave_privada_ECC_P256_t chave_priv;
		chave_publica_ECC_P256_t chave_pub;
		chave_SEF_t chave_SEF;
		chave_PAF_t chave_PAF;
		uint32_t contador;
		uuid_t idDAF;
		uint32_t modo_de_operacao;
		uint32_t falhas_de_atualizacao;
		CNPJ_t CNPJ;
		modelo_DAF_t modelo_DAF;
	} valores_t;

	typedef union {
		parametros_atualizacao_t parametros;
		certificado_t certificado;
		chave_publica_ECC_P384_t chave_do_certificado;
		chave_SEF_t chave_sef;
		chave_PAF_t chave_paf;
		chave_privada_ECC_P384_t chave_ateste_priv;
		chave_publica_ECC_P384_t chave_ateste_pub;
		chave_privada_ECC_P256_t chave_privada;
		chave_publica_ECC_P256_t chave_publica;
		uint32_t contador;
		uuid_t iddaf;
		uint32_t guardas[DAF_Utils::GUARDAS_SZ];
		uint32_t modo_de_operacao;
		uint32_t falhas_de_atualizacao;
		CNPJ_t CNPJ;
		modelo_DAF_t modelo_DAF;
	} valores_u;


	virtual ~MemoriaSegura();

	static uint32_t leitura_guarda(DAF_Utils::GUARDA_t guarda);

	static void escrita_guarda(DAF_Utils::GUARDA_t guarda, uint32_t valor);

	static MemoriaSegura::parametros_atualizacao_t leitura_parametros_atualizacao(
			void);


	static void escrita_parametros_atualizacao(
			MemoriaSegura::parametros_atualizacao_t parametros);

	static MemoriaSegura::certificado_t leitura_certificado(void);
	static void escrita_certificado(MemoriaSegura::certificado_t certificado);

	static chave_publica_ECC_P384_t leitura_chave_do_certificado(void);
	static void escrita_chave_do_certificado(chave_publica_ECC_P384_t chave);

	static chave_privada_ECC_P384_t leitura_chave_ateste_priv(void);
	static void escrita_chave_ateste_priv(chave_privada_ECC_P384_t chave);

	static chave_publica_ECC_P384_t leitura_chave_ateste_pub(void);
	static void escrita_chave_ateste_pub(chave_publica_ECC_P384_t chave);

	static chave_privada_ECC_P256_t leitura_chave_priv(void);
	static void escrita_chave_priv(chave_privada_ECC_P256_t chave);

	static chave_publica_ECC_P256_t leitura_chave_pub(void);
	static void escrita_chave_pub(chave_publica_ECC_P256_t chave);

	static MemoriaSegura::chave_SEF_t leitura_chave_SEF(void);
	static void escrita_chave_SEF(MemoriaSegura::chave_SEF_t chave);

	static MemoriaSegura::chave_PAF_t leitura_chave_PAF(void);
	static void escrita_chave_PAF(MemoriaSegura::chave_PAF_t chave);

	static uint32_t leitura_contador(void);
	static void escrita_contador(uint32_t contador);

	static MemoriaSegura::uuid_t leitura_idDAF(void);
	static void escrita_idDAF(MemoriaSegura::uuid_t id);

	static uint32_t leitura_modo_de_operacao(void);
	static void escrita_modo_de_operacao(uint32_t modo);

	static uint32_t leitura_falhas_de_atualizacao(void);
	static void escrita_falhas_de_atualizacao(uint32_t falhas);

	static CNPJ_t leitura_CNPJ(void);
	static void escrita_CNPJ(CNPJ_t CNPJ);

	static modelo_DAF_t leitura_modelo_DAF(void);
	static void escrita_modelo_DAF(modelo_DAF_t modelo);

private:

	static valores_t *const valores;
};

#endif /* MEMORIA_SEGURA_H_ */
