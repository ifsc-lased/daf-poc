#ifndef MEMORIA_DE_TRABALHO_H_
#define MEMORIA_DE_TRABALHO_H_

#include "memoria_segura.h"
#include "flash_externa.h"
#include <vector>

class MemoriaDeTrabalho {
public:
	MemoriaDeTrabalho() :
			flashExterna() {
	}
	virtual ~MemoriaDeTrabalho();

	typedef struct {

		uint8_t buf[2932];
		size_t buf_sz;
	} fragmento_DFE_t;

	typedef struct {
		char base64[44];
	} id_autorizacao_t;


	typedef struct {
		uint8_t hash[32];
	} resumo_XML_DFE_t;

	typedef struct {
		MemoriaSegura::uuid_t idDAF;
		uint32_t versaoSB;
		uint32_t modoDeOperacao;
		char idPDV[10];
		uint32_t contador;
		id_autorizacao_t idAut;
		resumo_XML_DFE_t resumoDFE;
	} documento_t;

	typedef struct {
		documento_t doc;
		fragmento_DFE_t frag;
	} autorizacao_t;

	/**
	 * @brief adiciona autorizacao na MT.
	 *
	 * @param[in] aut	struct com as informacoes da autorizacao
	 */
	bool add_autorizacao_DFE(MemoriaDeTrabalho::autorizacao_t &aut);


	/*
	 * @brief remove uma autorizacao da MT
	 *
	 * @param[in] idAut		identificador unico da autorizacao
	 */
	bool remove_autorizacao_DFE(id_autorizacao_t idAut);

	/*
	 * @brief remove uma autorizacao da MT
	 *
	 * @param[in] resumoDFE		resumo do documento fiscal
	 */
	bool remove_autorizacao_DFE(resumo_XML_DFE_t resumoDFE);

	/*
	 * @brief indica se existe a autorizacao na MT
	 *
	 * @param[in] idAut		identificador unico da autorizacao
	 */
	bool existe_autorizacao_DFE(id_autorizacao_t &idAut);

	/*
	 * @brief indica se existe a autorizacao na MT
	 *
	 * @param[in] resumoDFE		resumo do documento fiscal
	 */
	bool existe_autorizacao_DFE(resumo_XML_DFE_t &resumoDFE);

	/*
	 * @brief obtem uma autorizacao retida na MT
	 *
	 * @param[in] idAut		identificador unico da autorizao
	 * @param[out] aut		struct para armazenar a autorizacao
	 */
	bool get_autorizacao_DFE(id_autorizacao_t idAut, autorizacao_t *aut);

	/*
	 * @brief obtem uma autorizacao retida na MT
	 *
	 * @param[in] resumoDFE		resumo do documento fiscal
	 * @param[out] aut			struct para armazenar a autorizacao
	 */
	bool get_autorizacao_DFE(resumo_XML_DFE_t resumoDFE, autorizacao_t *aut);

	/*
	 * @brief obtem uma cadeia de autorizacoes
	 *
	 * @param[in] vecIDs	vetor para alocar as autorizacoes
	 * @param[in] max		numero de autorizacoes
	 */
	template<typename T>
	int get_autorizacoes_DFE(
			std::vector<MemoriaDeTrabalho::id_autorizacao_t, T> &vecIDs,
			uint32_t max) {

		if (max >= vecIDs.max_size())
			max = vecIDs.max_size();

		if (max >= this->get_numero_de_autorizacoes_DFE())
			max = this->get_numero_de_autorizacoes_DFE();


		autorizacao_t *tmp = get_tmp_aut();
		for (uint32_t i = 0; i < 4096; i++) {
			if (!sector_has_data(i))
				continue;

			if (vecIDs.size() >= max)
				break;

			sector_get_data(i, tmp);
			vecIDs.push_back(tmp->doc.idAut);
		}

		return vecIDs.size();
	}

	/*
	 * @brief obtem o total de autorizacoes retidas na MT
	 */
	uint32_t get_numero_de_autorizacoes_DFE(void);

	/*
	 * @brief reseta a memoria de trabalho
	 */
	void reseta(void);

private:
	FlashExterna flashExterna;

	static volatile uint8_t *const controleDeSetor;
	static volatile uint32_t *const iSectorWrite;
	static autorizacao_t * get_tmp_aut(void);
	static const uintptr_t AddrControleDeSetor = MemoriaSegura::AddrNVSRAM
				+ MemoriaSegura::TamanhoNVSRAM - 512;
	static const uintptr_t AddrISetorWrite = AddrControleDeSetor - 4;


	bool sector_has_data(uint32_t iSector);

	int sector_set_data(uint32_t iSector, autorizacao_t &aut);

	void sector_clean_data(uint32_t iSector);
	void sector_get_data(uint32_t iSector, autorizacao_t *aut);

};

#endif /* MEMORIA_DE_TRABALHO_H_ */
