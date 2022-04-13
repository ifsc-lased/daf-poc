#include <ucl/ucl_rsa.h>
#include <string.h>

#include "memoria_segura.h"
#include "../daf_utils.h"
#include "../debug.h"

MemoriaSegura::valores_t *const MemoriaSegura::valores =
		(MemoriaSegura::valores_t*) MemoriaSegura::AddrNVSRAM;


static_assert(sizeof(MemoriaSegura::valores_t) < (MemoriaSegura::TamanhoNVSRAM / 2),
		"Nao devemos ocupar mais que metade da NVSRAM com os valores da memoria segura");

uint32_t MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t guarda) {
	return valores->guardas[guarda];
}

void MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t guarda, uint32_t valor) {
	valores->guardas[guarda] = valor;
}

MemoriaSegura::parametros_atualizacao_t MemoriaSegura::leitura_parametros_atualizacao(
		void) {
	return valores->parametros_atualizacao;
}

void MemoriaSegura::escrita_parametros_atualizacao(
		MemoriaSegura::parametros_atualizacao_t parametros) {
	valores->parametros_atualizacao = parametros;
}

MemoriaSegura::certificado_t MemoriaSegura::leitura_certificado(void) {
	return valores->certificado;
}

void MemoriaSegura::escrita_certificado(
		MemoriaSegura::certificado_t certificado) {
	valores->certificado = certificado;
}

chave_publica_ECC_P384_t MemoriaSegura::leitura_chave_do_certificado(void) {
	return valores->chave_do_certificado;
}

void MemoriaSegura::escrita_chave_do_certificado(chave_publica_ECC_P384_t chave) {
	valores->chave_do_certificado = chave;
}

chave_privada_ECC_P384_t MemoriaSegura::leitura_chave_ateste_priv(void) {
	return valores->chave_ateste_priv;
}

void MemoriaSegura::escrita_chave_ateste_priv(chave_privada_ECC_P384_t chave) {
	valores->chave_ateste_priv = chave;
}

chave_publica_ECC_P384_t MemoriaSegura::leitura_chave_ateste_pub(void) {
	return valores->chave_ateste_pub;
}

void MemoriaSegura::escrita_chave_ateste_pub(chave_publica_ECC_P384_t chave) {
	valores->chave_ateste_pub = chave;
}

chave_privada_ECC_P256_t MemoriaSegura::leitura_chave_priv(void) {
	return valores->chave_priv;
}

void MemoriaSegura::escrita_chave_priv(chave_privada_ECC_P256_t chave) {
	valores->chave_priv = chave;
}

chave_publica_ECC_P256_t MemoriaSegura::leitura_chave_pub(void) {
	return valores->chave_pub;
}

void MemoriaSegura::escrita_chave_pub(chave_publica_ECC_P256_t chave) {
	valores->chave_pub = chave;
}

MemoriaSegura::chave_SEF_t MemoriaSegura::leitura_chave_SEF(void) {
	return valores->chave_SEF;
}

void MemoriaSegura::escrita_chave_SEF(MemoriaSegura::chave_SEF_t chave) {
	valores->chave_SEF = chave;
}

MemoriaSegura::chave_PAF_t MemoriaSegura::leitura_chave_PAF(void) {
	return valores->chave_PAF;
}

void MemoriaSegura::escrita_chave_PAF(MemoriaSegura::chave_PAF_t chave) {
	valores->chave_PAF = chave;
}

uint32_t MemoriaSegura::leitura_contador(void) {
	return valores->contador;
}

void MemoriaSegura::escrita_contador(uint32_t contador) {
	valores->contador = contador;
}

MemoriaSegura::uuid_t MemoriaSegura::leitura_idDAF(void) {
	return valores->idDAF;
}

void MemoriaSegura::escrita_idDAF(MemoriaSegura::uuid_t id) {
	valores->idDAF = id;
}

uint32_t MemoriaSegura::leitura_modo_de_operacao(void) {
	return valores->modo_de_operacao;
}
void MemoriaSegura::escrita_modo_de_operacao(uint32_t modo) {
	valores->modo_de_operacao = modo;
}

uint32_t MemoriaSegura::leitura_falhas_de_atualizacao(void) {
	return valores->falhas_de_atualizacao;
}
void MemoriaSegura::escrita_falhas_de_atualizacao(uint32_t falhas) {
	valores->falhas_de_atualizacao = falhas;
}

MemoriaSegura::CNPJ_t MemoriaSegura::leitura_CNPJ(void) {
	return valores->CNPJ;
}
void MemoriaSegura::escrita_CNPJ(MemoriaSegura::CNPJ_t CNPJ) {
	valores->CNPJ = CNPJ;
}

MemoriaSegura::modelo_DAF_t MemoriaSegura::leitura_modelo_DAF(void) {
	return valores->modelo_DAF;
}

void MemoriaSegura::escrita_modelo_DAF(MemoriaSegura::modelo_DAF_t modelo) {
	valores->modelo_DAF = modelo;
}
