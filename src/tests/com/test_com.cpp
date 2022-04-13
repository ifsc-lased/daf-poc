/*
 * teste_com.cpp
 *
 *  Created on: 11 de mai de 2021
 *      Author: sellp
 */

#include "../../unity/unity.h"
#include <iostream>
#include <string.h>
#include "../../com/enquadramento.h"
#include "../../com/arq.h"

void setUp(void) {
}
;
void tearDown(void) {
}
;

static uint8_t msg_consultar_informacoes[] = { 0x01, 0x00, 0x0a, 0x00, 0x7b,
		0x22, 0x6d, 0x73, 0x67, 0x22, 0x3a, 0x38, 0x7d };

static uint8_t msg_consultar_informacoes_2[] = { 0x01, 0x00, 0x0a, 0x08, 0x7b,
		0x22, 0x6d, 0x73, 0x67, 0x22, 0x3a, 0x38, 0x7d };

void test_enquadramento_recepcao(void) {
	Enquadramento enq;

	uint8_t buffer[10];
	uint8_t buffer2[10];
	uint8_t comando1;
	uint8_t comando2;
	size_t len_arq;
	int sucesso = 1;
	for (int i = 0; i < 13; i++) {
		enq.handle_fsm(msg_consultar_informacoes[i]);
	}

	if (enq.hasQuadroCompletoRecebido() == true) {
		len_arq = enq.getLenDados();

	
		enq.sendToArq(comando1,buffer, len_arq);

	}
	TEST_ASSERT_EQUAL_UINT8(0x01, comando1);
	TEST_ASSERT_EQUAL_size_t(10, len_arq);

	TEST_ASSERT_EQUAL_UINT8_ARRAY(msg_consultar_informacoes + 3, buffer, 10);

	// testando duas recepcoes
	for (int i = 0; i < 13; i++) {
		enq.handle_fsm(msg_consultar_informacoes_2[i]);
	}

	if (enq.hasQuadroCompletoRecebido() == true) {
		len_arq = enq.getLenDados();

	
		enq.sendToArq(comando2,buffer2, len_arq);

	}
	TEST_ASSERT_EQUAL_UINT8(0x01, comando2);
	TEST_ASSERT_EQUAL_size_t(10, len_arq);

	TEST_ASSERT_EQUAL_UINT8_ARRAY(msg_consultar_informacoes_2 + 3, buffer2, 10);

}

void test_ARQ_recepcao(void) {
	ARQ arq;
	uint8_t buffer[9];

	arq.receiveFromEnq((uint8_t) 0x01,msg_consultar_informacoes[3],
			msg_consultar_informacoes + 4, 9);

	if (arq.possui_dados() == true) {
		arq.sendToAPIDAF(buffer, sizeof(buffer));

	}

	TEST_ASSERT_EQUAL_UINT8_ARRAY(msg_consultar_informacoes + 4, buffer, 9);
	TEST_ASSERT_EQUAL_UINT8(0x01, arq.getComando());

	//testando duas recepcoes
	uint8_t buffer2[9];

	arq.receiveFromEnq((uint8_t) 0x01,msg_consultar_informacoes_2[3],
			msg_consultar_informacoes_2 + 4, 9);

	if (arq.possui_dados() == true) {
		arq.sendToAPIDAF(buffer2,  sizeof(buffer));

	}

	TEST_ASSERT_EQUAL_UINT8_ARRAY(msg_consultar_informacoes_2 + 4, buffer2, 9);


}

void test_envio(void) {
	ARQ arq;
	Enquadramento enq;
	uint8_t buffer[13];
	uint8_t buffer2[13];
	arq.addEnq(&enq);
	enq.addArq(&arq);

	arq.receiveFromAPIDAF(msg_consultar_informacoes + 4, 9);

	enq.getBufferEnviado(buffer);

	TEST_ASSERT_EQUAL_UINT8_ARRAY(msg_consultar_informacoes, buffer, 13);

}

int main() {
	UNITY_BEGIN();

	RUN_TEST(test_enquadramento_recepcao);
	RUN_TEST(test_ARQ_recepcao);
	RUN_TEST(test_envio);

	return UNITY_END();
}

