#include "memoria_de_trabalho.h"

#include <MAX32552.h>
#include <stdio.h>

#include <mml_spix.h>
#include "W25Q.h" // módulo da Maxim
#include <string.h>
#include "flash_externa.h"

volatile uint8_t *const MemoriaDeTrabalho::controleDeSetor =
		(uint8_t*) AddrControleDeSetor;
volatile uint32_t *const MemoriaDeTrabalho::iSectorWrite =
		(uint32_t*) AddrISetorWrite;

static MemoriaDeTrabalho::autorizacao_t g_tmp_aut;

MemoriaDeTrabalho::autorizacao_t* MemoriaDeTrabalho::get_tmp_aut(void) {
	return &g_tmp_aut;
}

MemoriaDeTrabalho::~MemoriaDeTrabalho() {

}

bool MemoriaDeTrabalho::sector_has_data(uint32_t iSector) {
	uint32_t quociente, resto;

	quociente = iSector >> 3;
	resto = iSector % 8;

	if (controleDeSetor[quociente] & (1 << resto))
		return false;
	else
		return true;
}

int MemoriaDeTrabalho::sector_set_data(uint32_t iSector, autorizacao_t &aut) {
	uint32_t quociente, resto;
	quociente = iSector >> 3;
	resto = iSector % 8;

	controleDeSetor[quociente] &= ~(1 << resto);

	return this->flashExterna.escreve(SPI_EXTERNAL_FLASH_BASE + iSector * 4096,
			(unsigned char*) &aut, sizeof(aut));
}

void MemoriaDeTrabalho::sector_clean_data(uint32_t iSector) {
	uint32_t quociente, resto;

	quociente = iSector >> 3;
	resto = iSector % 8;

	controleDeSetor[quociente] |= 1 << resto;
}

void MemoriaDeTrabalho::sector_get_data(uint32_t iSector, autorizacao_t *aut) {
	uint32_t offset = iSector * 4096;

	flashExterna.leitura(SPI_EXTERNAL_FLASH_BASE + offset, (uint8_t*) aut,
			sizeof(autorizacao_t));
}

bool MemoriaDeTrabalho::existe_autorizacao_DFE(id_autorizacao_t &idAut) {

	for (uint32_t i = 0; i < 4096; i++) {
		if (!sector_has_data(i))
			continue;

		sector_get_data(i, &g_tmp_aut);

		if (memcmp(&g_tmp_aut.doc.idAut, &idAut, sizeof(id_autorizacao_t)) == 0)
			return true;
	}

	return false;
}

bool MemoriaDeTrabalho::existe_autorizacao_DFE(resumo_XML_DFE_t &resumoDFE) {

	for (uint32_t i = 0; i < 4096; i++) {
		if (!sector_has_data(i))
			continue;

		sector_get_data(i, &g_tmp_aut);

		if (memcmp(&g_tmp_aut.doc.resumoDFE, &resumoDFE,
				sizeof(resumo_XML_DFE_t)) == 0)
			return true;
	}

	return false;
}

bool MemoriaDeTrabalho::remove_autorizacao_DFE(id_autorizacao_t idAut) {
	uint32_t num_dfe = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::NUM_DFE);

	for (uint32_t i = 0; i < 4096; i++) {
		sector_get_data(i, &g_tmp_aut);

		if (memcmp(&g_tmp_aut.doc.idAut, &idAut, sizeof(id_autorizacao_t))
				== 0) {
			sector_clean_data(i);
			num_dfe--;
			MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::NUM_DFE,
					num_dfe);
			return true;
		}
	}

	return false;
}

bool MemoriaDeTrabalho::remove_autorizacao_DFE(resumo_XML_DFE_t resumoDFE) {
	uint32_t num_dfe = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::NUM_DFE);

	for (uint32_t i = 0; i < 4096; i++) {
		sector_get_data(i, &g_tmp_aut);

		if (memcmp(&g_tmp_aut.doc.resumoDFE, &resumoDFE,
				sizeof(resumo_XML_DFE_t)) == 0) {
			sector_clean_data(i);
			num_dfe--;
			MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::NUM_DFE,
					num_dfe);
			return true;
		}
	}

	return false;
}

uint32_t MemoriaDeTrabalho::get_numero_de_autorizacoes_DFE(void) {
	return MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::NUM_DFE);
}

void MemoriaDeTrabalho::reseta(void) {

	memset((uint8_t*) controleDeSetor, 0xFF, 512);
	*iSectorWrite = 0;
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::NUM_DFE, 0);
}

bool MemoriaDeTrabalho::add_autorizacao_DFE(
		MemoriaDeTrabalho::autorizacao_t &aut) {

	uint32_t num_dfe = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::NUM_DFE);
	uint32_t max_dfe = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::MAX_DFE_MODEL);

	if (num_dfe >= max_dfe)
		return false;

	if (sizeof(aut) >= 4096)
		return false;


	for (uint32_t j = 0; j < 4095; j++) {

		if (sector_has_data(*iSectorWrite & 0xFFF)) {
			(*iSectorWrite)++;
			continue;
		}

		sector_set_data(*iSectorWrite & 0xFFF, aut);
		num_dfe++;
		MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::NUM_DFE, num_dfe);
		return true;
	}

	return false;
}

bool MemoriaDeTrabalho::get_autorizacao_DFE(id_autorizacao_t idAut,
		autorizacao_t *aut) {

	for (uint32_t i = 0; i < 4096; i++) {
		if (!sector_has_data(i))
			continue;

		sector_get_data(i, &g_tmp_aut);

		if (0 == memcmp(&g_tmp_aut.doc.idAut, &idAut, sizeof(idAut))) {
			memcpy(aut, &g_tmp_aut, sizeof(*aut));
			return true;
		}
	}

	return false;
}
bool MemoriaDeTrabalho::get_autorizacao_DFE(resumo_XML_DFE_t resumoDFE,
		autorizacao_t *aut) {

	for (uint32_t i = 0; i < 4096; i++) {
		if (!sector_has_data(i))
			continue;

		sector_get_data(i, &g_tmp_aut);

		if (0
				== memcmp(&g_tmp_aut.doc.resumoDFE, &resumoDFE,
						sizeof(resumoDFE))) {
			memcpy(aut, &g_tmp_aut, sizeof(*aut));
			return true;
		}
	}

	return false;
}
