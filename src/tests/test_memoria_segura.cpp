#define private public

// the following lines produces hardfault when comparing UINT32 by Unity
//#define UNITY_SUPPORT_64

#include "../unity/unity.h"
#include "../memoria_segura.h"
#include "../debug.h"
#include <string.h>

void setUp(void) {
}
void tearDown(void) {
}

/**
 * Para a NVSRAM manter suas informacoes apos desligar a placa,
 * eh necessario certificar-se que o jumper JP10 (VBAT SEL)
 * esteja conectado no BATT, e nao no 3V3.
 */

void write_pattern(void) {
	volatile uint8_t *p = (uint8_t*)MemoriaSegura::AddrNVSRAM;
	DEBUG_PRINTF("escrevendo pattern na MS:\n");
	for (size_t i = 0; i < MemoriaSegura::TamanhoNVSRAM; i++)
		p[i] = 0xAB;
}

void read_pattern(void) {
	volatile uint8_t *p = (uint8_t*)MemoriaSegura::AddrNVSRAM;
	DEBUG_PRINTF("conteudo da MS:\n");
	for (size_t i = 0; i < MemoriaSegura::TamanhoNVSRAM; i++) {
		DEBUG_PRINTF("%02x", p[i]);
		if (i > 0 && i % 80 == 0)
			DEBUG_PRINTF("\n");
	}
}

void dump_addrs(void) {
	DEBUG_PRINTF("\n\nMemoria Segura Endere�os:\n\n");

	DEBUG_PRINTF("guardas: %p\n", &MemoriaSegura::valores->guardas);
	DEBUG_PRINTF("parametros_atualizacao: %p\n",
			&MemoriaSegura::valores->parametros_atualizacao);
	DEBUG_PRINTF("certificado: %p\n", &MemoriaSegura::valores->certificado);
	DEBUG_PRINTF("chave_ateste_priv: %p\n",
			&MemoriaSegura::valores->chave_ateste_priv);
	DEBUG_PRINTF("chave_ateste_pub: %p\n",
			&MemoriaSegura::valores->chave_ateste_pub);
	DEBUG_PRINTF("chave_priv: %p\n", &MemoriaSegura::valores->chave_priv);
	DEBUG_PRINTF("chave_pub: %p\n", &MemoriaSegura::valores->chave_pub);
	DEBUG_PRINTF("chave_SEF: %p\n", &MemoriaSegura::valores->chave_SEF);
	DEBUG_PRINTF("chave_PAF: %p\n", &MemoriaSegura::valores->chave_PAF);
	DEBUG_PRINTF("contador: %p\n", &MemoriaSegura::valores->contador);
	DEBUG_PRINTF("idDAF: %p\n", &MemoriaSegura::valores->idDAF);
	DEBUG_PRINTF("Modo: %p\n", &MemoriaSegura::valores->modo_de_operacao);
	DEBUG_PRINTF("Falhas: %p\n", &MemoriaSegura::valores->falhas_de_atualizacao);
	DEBUG_PRINTF("CNPJ: %p\n", &MemoriaSegura::valores->CNPJ);
	DEBUG_PRINTF("Modelo DAF: %p\n", &MemoriaSegura::valores->modelo_DAF);
}

void dump_memoria_segura(void) {
	static MemoriaSegura::valores_u valor;

	DEBUG_PRINTF("\n\nMemoria Segura Dump:\n\n");

	valor.contador = MemoriaSegura::leitura_contador();
	DEBUG_PRINTF("Contador: %lu\n", valor.contador);

	valor.guardas[DAF_Utils::GUARDA_t::ESTADO] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);
	valor.guardas[DAF_Utils::GUARDA_t::MAX_DFE] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::MAX_DFE);
	valor.guardas[DAF_Utils::GUARDA_t::MAX_DFE_MODEL] =
			MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::MAX_DFE_MODEL);
	valor.guardas[DAF_Utils::GUARDA_t::NUM_DFE] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::NUM_DFE);
	valor.guardas[DAF_Utils::GUARDA_t::REGOK] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::REGOK);
	valor.guardas[DAF_Utils::GUARDA_t::VIOLADO] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::VIOLADO);
	DEBUG_PRINTF("Guardas: %lu, %lu, %lu, %lu, %lu, %lu\n",
			valor.guardas[DAF_Utils::GUARDA_t::ESTADO],
			valor.guardas[DAF_Utils::GUARDA_t::MAX_DFE],
			valor.guardas[DAF_Utils::GUARDA_t::MAX_DFE_MODEL],
			valor.guardas[DAF_Utils::GUARDA_t::NUM_DFE],
			valor.guardas[DAF_Utils::GUARDA_t::REGOK],
			valor.guardas[DAF_Utils::GUARDA_t::VIOLADO]);

	valor.certificado = MemoriaSegura::leitura_certificado();
	DEBUG_PRINTF("\nCertificado: ");
	DEBUG_PRINT_ARRAY_HEX(valor.certificado.certificado,
			sizeof(valor.certificado.certificado));

	valor.chave_ateste_priv = MemoriaSegura::leitura_chave_ateste_priv();
	DEBUG_PRINTF("\nChave ateste priv: ");
	DEBUG_PRINT_ARRAY_HEX(valor.chave_ateste_priv.d,
			sizeof(valor.chave_ateste_priv.d));

	valor.chave_ateste_pub = MemoriaSegura::leitura_chave_ateste_pub();
	DEBUG_PRINTF("\nChave ateste pub: ");
	DEBUG_PRINT_ARRAY_HEX(valor.chave_ateste_pub.x,
			sizeof(valor.chave_ateste_pub.x));
	DEBUG_PRINT_ARRAY_HEX(valor.chave_ateste_pub.y,
			sizeof(valor.chave_ateste_pub.y));

	valor.chave_paf = MemoriaSegura::leitura_chave_PAF();
	DEBUG_PRINTF("\nChave PAF: ");
	DEBUG_PRINT_ARRAY_HEX(valor.chave_paf.chave, sizeof(valor.chave_paf.chave));

	// ...
}

void testa_escrita_leitura(void) {
	static MemoriaSegura::valores_u valor;

	MemoriaSegura::escrita_contador(123456);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::ESTADO, 1);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::MAX_DFE, 2);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::MAX_DFE_MODEL, 3);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::NUM_DFE, 4);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::REGOK, 5);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::VIOLADO, 6);

	MemoriaSegura::escrita_falhas_de_atualizacao(45);
	MemoriaSegura::escrita_modo_de_operacao(1);

	memset(valor.certificado.certificado, 0xFF,
			sizeof(valor.certificado.certificado));
	MemoriaSegura::escrita_certificado(valor.certificado);

	memset(&valor.chave_ateste_priv, 0xEE, sizeof(valor.chave_ateste_priv));
	MemoriaSegura::escrita_chave_ateste_priv(valor.chave_ateste_priv);

	memset(&valor.chave_ateste_pub, 0xDD, sizeof(valor.chave_ateste_pub));
	MemoriaSegura::escrita_chave_ateste_pub(valor.chave_ateste_pub);

	memset(valor.chave_paf.chave, 0xCC, sizeof(valor.chave_paf.chave));
	MemoriaSegura::escrita_chave_PAF(valor.chave_paf);

	char modelo[] = "Modelo 1.2.3";
	memcpy(valor.modelo_DAF.modelo, modelo, sizeof(modelo));
	MemoriaSegura::escrita_modelo_DAF(valor.modelo_DAF);

	MemoriaSegura::modelo_DAF_t mod = MemoriaSegura::leitura_modelo_DAF();

	TEST_ASSERT_EQUAL_INT(0,
			memcmp(valor.modelo_DAF.modelo, mod.modelo,
					strlen(valor.modelo_DAF.modelo)));


	valor.contador = MemoriaSegura::leitura_contador();
	uint32_t contadorCmp = 123456;
	TEST_ASSERT_EQUAL_UINT32(contadorCmp, valor.contador);

	valor.guardas[DAF_Utils::GUARDA_t::ESTADO] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);
	valor.guardas[DAF_Utils::GUARDA_t::MAX_DFE] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::MAX_DFE);
	valor.guardas[DAF_Utils::GUARDA_t::MAX_DFE_MODEL] =
			MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::MAX_DFE_MODEL);
	valor.guardas[DAF_Utils::GUARDA_t::NUM_DFE] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::NUM_DFE);
	valor.guardas[DAF_Utils::GUARDA_t::REGOK] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::REGOK);
	valor.guardas[DAF_Utils::GUARDA_t::VIOLADO] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::VIOLADO);

	TEST_ASSERT_EQUAL_UINT32(1, valor.guardas[DAF_Utils::GUARDA_t::ESTADO]);
	TEST_ASSERT_EQUAL_UINT32(2, valor.guardas[DAF_Utils::GUARDA_t::MAX_DFE]);
	TEST_ASSERT_EQUAL_UINT32(3,
			valor.guardas[DAF_Utils::GUARDA_t::MAX_DFE_MODEL]);
	TEST_ASSERT_EQUAL_UINT32(4, valor.guardas[DAF_Utils::GUARDA_t::NUM_DFE]);
	TEST_ASSERT_EQUAL_UINT32(5, valor.guardas[DAF_Utils::GUARDA_t::REGOK]);
	TEST_ASSERT_EQUAL_UINT32(6, valor.guardas[DAF_Utils::GUARDA_t::VIOLADO]);

	TEST_ASSERT_EQUAL_UINT32(45,
			MemoriaSegura::leitura_falhas_de_atualizacao());
	TEST_ASSERT_EQUAL_UINT32(1, MemoriaSegura::leitura_modo_de_operacao());

}

void test_contador(void) {
	uint32_t contador = 5;
	MemoriaSegura::escrita_contador(contador);

	TEST_ASSERT_EQUAL(contador, MemoriaSegura::leitura_contador());
}

int main(void) {

	DEBUG_PRINTF_INIT();

	read_pattern();

	UNITY_BEGIN();

//	dump_addrs();
	dump_memoria_segura();

	RUN_TEST(testa_escrita_leitura);
	RUN_TEST(test_contador);

	write_pattern();

	return UNITY_END();
}

