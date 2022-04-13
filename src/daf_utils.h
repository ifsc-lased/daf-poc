#ifndef DAF_UTILS
#define DAF_UTILS

class DAF_Utils {
public:
	typedef enum artefato {
		CERTIFICADO,
		CHAVE_ATESTE_PRIV,
		CHAVE_ATESTE_PUB,
		CHAVE_PRIVADA,
		CHAVE_PUBLICA,
		CHAVE_SEF,
		CHAVE_PAF,
		CONTADOR,
		ID_DAF,
	} ARTEFATO_t;

	typedef enum guarda {
		MAX_DFE,
		REGOK,
		NUM_DFE,
		MAX_DFE_MODEL,
		VIOLADO,
		ESTADO,
		EM_PROCESSO,
		GUARDAS_SZ
	} GUARDA_t;

	typedef enum atualizacao {
		RESUMO_SB,
		VERSAO_SB,
		FALHAS_ATUALIZACAO,
		MODELO,
		CNPJ
	} ATUALIZACAO_t;
};

#endif
