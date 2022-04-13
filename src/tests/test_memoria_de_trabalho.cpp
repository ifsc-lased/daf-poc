#include <stdio.h>
#include <MAX32552.h>
#include <mml_spix.h>
#include <errors.h>
#include "../unity/unity.h"
#include "../debug.h"
#include <string.h>
#include <vector>

#define private public
#include "../memoria/memoria_de_trabalho.h"
#include "../memoria/alocador_de_memoria.h"

void setUp(void) {
}
;
void tearDown(void) {
}
;

void test_controle_de_setor_MS(void) {
	size_t bits_modified = 0;
	for (size_t i = 0; i < 512; i++) {
		uint8_t byte = MemoriaDeTrabalho::controleDeSetor[i];

		for (size_t b = 0; b < 8; b++) {
			if (byte & (1 << b))
				continue;
			else
				bits_modified++;
		}
	}

	DEBUG_PRINTF("bits modificados: %i\n", bits_modified);

	MemoriaDeTrabalho mt;

	uint32_t auts = mt.get_numero_de_autorizacoes_DFE();

	TEST_ASSERT_EQUAL_INT(bits_modified, auts);
}

void print_autorizacoes_na_MT(void) {
	MemoriaDeTrabalho mt;

	std::vector<MemoriaDeTrabalho::id_autorizacao_t,
			AlocadorRing<MemoriaDeTrabalho::id_autorizacao_t>> vecIDs;

	mt.get_autorizacoes_DFE(vecIDs, 10);

	DEBUG_PRINTF("\nAutorizacoes na MT\n");
	for (auto i = vecIDs.begin(); i != vecIDs.end(); i++) {
		MemoriaDeTrabalho::id_autorizacao_t id = *i;
		DEBUG_PRINTF("id: %.*s\n", sizeof(MemoriaDeTrabalho::id_autorizacao_t),
				id.base64);
//		DEBUG_PRINT_ARRAY_HEX(id.base64, sizeof(id.base64));
//		DEBUG_PRINTF("\n");
	}
	DEBUG_PRINTF("\n");

}

void test_pub_MT(void) {

	static MemoriaDeTrabalho::autorizacao_t aut, aut2;
	MemoriaDeTrabalho mt;

	mt.reseta();

	TEST_ASSERT_EQUAL_INT(0, mt.get_numero_de_autorizacoes_DFE());

	// preenche autorizacao com dummy
	memset(&aut, 0xAA, sizeof(aut));
	aut.frag.buf_sz = 10;

	TEST_ASSERT(true == mt.add_autorizacao_DFE(aut));
	TEST_ASSERT_EQUAL_INT(1, mt.get_numero_de_autorizacoes_DFE());

	TEST_ASSERT(true == mt.existe_autorizacao_DFE(aut.doc.idAut));
	TEST_ASSERT(true == mt.existe_autorizacao_DFE(aut.doc.resumoDFE));
	aut.doc.idAut.base64[2]++;
	aut.doc.resumoDFE.hash[2]++;
	TEST_ASSERT(false == mt.existe_autorizacao_DFE(aut.doc.idAut));
	TEST_ASSERT(false == mt.existe_autorizacao_DFE(aut.doc.resumoDFE));
	TEST_ASSERT(false == mt.get_autorizacao_DFE(aut.doc.idAut, &aut2));
	TEST_ASSERT(false == mt.get_autorizacao_DFE(aut.doc.resumoDFE, &aut2));
	TEST_ASSERT(false == mt.remove_autorizacao_DFE(aut.doc.idAut));
	TEST_ASSERT(false == mt.remove_autorizacao_DFE(aut.doc.resumoDFE));
	aut.doc.idAut.base64[2]--; // undo
	aut.doc.resumoDFE.hash[2]--; // undo

	// verifica conteudo da autorizacao salva
	TEST_ASSERT(true == mt.get_autorizacao_DFE(aut.doc.idAut, &aut2));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&aut, &aut2, sizeof(aut)));
	TEST_ASSERT(true == mt.get_autorizacao_DFE(aut.doc.resumoDFE, &aut2));
	TEST_ASSERT_EQUAL_INT(0, memcmp(&aut, &aut2, sizeof(aut)));

	// remove por idAut
	TEST_ASSERT(true == mt.remove_autorizacao_DFE(aut.doc.idAut));
	TEST_ASSERT_EQUAL_INT(0, mt.get_numero_de_autorizacoes_DFE());
	TEST_ASSERT(false == mt.existe_autorizacao_DFE(aut.doc.idAut));
	TEST_ASSERT(false == mt.existe_autorizacao_DFE(aut.doc.resumoDFE));
	TEST_ASSERT(false == mt.get_autorizacao_DFE(aut.doc.idAut, &aut2));
	TEST_ASSERT(false == mt.get_autorizacao_DFE(aut.doc.resumoDFE, &aut2));

	// add de novo
	TEST_ASSERT(true == mt.add_autorizacao_DFE(aut));

	// remove por resumoDFE
	TEST_ASSERT(true == mt.remove_autorizacao_DFE(aut.doc.resumoDFE));
	TEST_ASSERT_EQUAL_INT(0, mt.get_numero_de_autorizacoes_DFE());
	TEST_ASSERT(false == mt.existe_autorizacao_DFE(aut.doc.idAut));
	TEST_ASSERT(false == mt.existe_autorizacao_DFE(aut.doc.resumoDFE));
	TEST_ASSERT(false == mt.get_autorizacao_DFE(aut.doc.idAut, &aut2));
	TEST_ASSERT(false == mt.get_autorizacao_DFE(aut.doc.resumoDFE, &aut2));

}

void test_priv_MT(void) {
	static MemoriaDeTrabalho::autorizacao_t aut, aut2;
	MemoriaDeTrabalho mt;

	mt.reseta();

	memset(&aut, 0xAA, sizeof(aut));

	mt.sector_set_data(5, aut);
	TEST_ASSERT(MemoriaDeTrabalho::controleDeSetor[0] == 0xDF);
	mt.sector_get_data(5, &aut2);
	TEST_ASSERT_EQUAL_INT(0, memcmp(&aut, &aut2, sizeof(aut)));

	memset(&aut, ~0xAA, sizeof(aut));
	mt.sector_set_data(5, aut);
	TEST_ASSERT(MemoriaDeTrabalho::controleDeSetor[0] == 0xDF);
	mt.sector_get_data(5, &aut2);
	TEST_ASSERT_EQUAL_INT(0, memcmp(&aut, &aut2, sizeof(aut)));

	mt.sector_clean_data(5);
	TEST_ASSERT(MemoriaDeTrabalho::controleDeSetor[0] == 0xFF);

	memset(&aut, 0xBB, sizeof(aut));
	mt.sector_set_data(1111, aut);
	TEST_ASSERT(MemoriaDeTrabalho::controleDeSetor[138] == 0x7F);
	mt.sector_clean_data(1111);
	TEST_ASSERT(MemoriaDeTrabalho::controleDeSetor[138] == 0xFF);

	memset(&aut, 0xCC, sizeof(aut));
	mt.sector_set_data(4095, aut);
	TEST_ASSERT(MemoriaDeTrabalho::controleDeSetor[511] == 0x7F);
	mt.sector_set_data(4094, aut);
	TEST_ASSERT(MemoriaDeTrabalho::controleDeSetor[511] == 0x3F);

	mt.sector_clean_data(4094);
	TEST_ASSERT(MemoriaDeTrabalho::controleDeSetor[511] == 0x7F);

}

void test_get_autorizacoes(void) {
	MemoriaDeTrabalho mt;
	static MemoriaDeTrabalho::autorizacao_t aut;

	// preenche autorizacao com dummy
	memset(&aut, 0xAA, sizeof(aut));
	aut.frag.buf_sz = 10;
	mt.add_autorizacao_DFE(aut);

	memset(&aut, 0xBB, sizeof(aut));
	aut.frag.buf_sz = 20;
	mt.add_autorizacao_DFE(aut);

	memset(&aut, 0xCC, sizeof(aut));
	aut.frag.buf_sz = 20;
	mt.add_autorizacao_DFE(aut);

	TEST_ASSERT(mt.get_numero_de_autorizacoes_DFE() == 3);

	std::vector<MemoriaDeTrabalho::id_autorizacao_t,
			AlocadorRing<MemoriaDeTrabalho::id_autorizacao_t>> vecIDs;

	TEST_ASSERT(mt.get_autorizacoes_DFE(vecIDs, 10) == 3);

	MemoriaDeTrabalho::id_autorizacao_t id = vecIDs.back();
	memset(&aut.doc.idAut, 0xCC, sizeof(MemoriaDeTrabalho::id_autorizacao_t));
	TEST_ASSERT(
			memcmp(&id, &aut.doc.idAut,
					sizeof(MemoriaDeTrabalho::id_autorizacao_t)) == 0);
	vecIDs.pop_back();

	id = vecIDs.back();
	memset(&aut.doc.idAut, 0xBB, sizeof(MemoriaDeTrabalho::id_autorizacao_t));
	TEST_ASSERT(
			memcmp(&id, &aut.doc.idAut,
					sizeof(MemoriaDeTrabalho::id_autorizacao_t)) == 0);
	vecIDs.pop_back();

	id = vecIDs.back();
	memset(&aut.doc.idAut, 0xAA, sizeof(MemoriaDeTrabalho::id_autorizacao_t));
	TEST_ASSERT(
			memcmp(&id, &aut.doc.idAut,
					sizeof(MemoriaDeTrabalho::id_autorizacao_t)) == 0);
	vecIDs.pop_back();

}

void test_two_mt_objs(void) {
	static MemoriaDeTrabalho::autorizacao_t aut, aut2;
	MemoriaDeTrabalho mt;

	mt.reseta();

	TEST_ASSERT_EQUAL_INT(0, mt.get_numero_de_autorizacoes_DFE());

	// preenche autorizacao com dummy
	memset(&aut, 'A', sizeof(aut));
	aut.frag.buf_sz = 10;

	TEST_ASSERT(true == mt.add_autorizacao_DFE(aut));
	TEST_ASSERT_EQUAL_INT(1, mt.get_numero_de_autorizacoes_DFE());
	TEST_ASSERT(true == mt.existe_autorizacao_DFE(aut.doc.idAut));
	TEST_ASSERT(true == mt.existe_autorizacao_DFE(aut.doc.resumoDFE));


	MemoriaDeTrabalho mt2;

	TEST_ASSERT_EQUAL_INT(1, mt2.get_numero_de_autorizacoes_DFE());

	

	TEST_ASSERT(true == mt2.existe_autorizacao_DFE(aut.doc.idAut));
	TEST_ASSERT(true == mt2.existe_autorizacao_DFE(aut.doc.resumoDFE));


	// uma autenticacao a ser printada qnd o teste iniciar novamente
	memset(&aut2, 'B', sizeof(aut2));
	aut2.frag.buf_sz = 30;
	TEST_ASSERT(true == mt2.add_autorizacao_DFE(aut2));
	TEST_ASSERT_EQUAL_INT(2, mt2.get_numero_de_autorizacoes_DFE());
}

int main(void) {
	DEBUG_PRINTF_INIT();

	UNITY_BEGIN();

	print_autorizacoes_na_MT();

	DEBUG_PRINTF("Testando test_controle_de_setor_MS()\n");
	RUN_TEST(test_controle_de_setor_MS);

	DEBUG_PRINTF("Testando funcoes publicas da Memoria de Trabalho\n\n");
	RUN_TEST(test_pub_MT);

	DEBUG_PRINTF("Testando funcoes privadas da Memoria de Trabalho\n\n");
	RUN_TEST(test_priv_MT);

	DEBUG_PRINTF("Testando get_autorizacoes()\n");
	RUN_TEST(test_get_autorizacoes);

	DEBUG_PRINTF("Testando test_two_mt_objs()\n");
	RUN_TEST(test_two_mt_objs);

	DEBUG_PRINTF("Testando test_controle_de_setor_MS()\n");
	RUN_TEST(test_controle_de_setor_MS);

	return UNITY_END();
}
