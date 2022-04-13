#include "pdafcdc.h"
#include "string.h"
#include "../debug.h"

uint8_t PDAFCDC::buffer[4000];

PDAFCDC::PDAFCDC() {

	this->enq.addArq(&this->arq);
	this->arq.addEnq(&this->enq);
}


size_t PDAFCDC::possuiJson() {


	this->arq.timeout_handler();
	this->enq.timeout_handler();


	size_t len_raw = this->enq.possui_dados();
	if (len_raw <= 0) {
		return 0;
	}

	static uint8_t dados_raw[128];
	if (len_raw > sizeof(dados_raw))
		len_raw = sizeof(dados_raw);

	if ((this->enq.le_usb(dados_raw, len_raw)) == false) {
		return 0;
	}

	DEBUG_PRINTF("Quadro completo recebido...\n");

	static uint8_t dados_arq[3500];
	uint8_t comando;
	size_t dados_arq_sz = this->enq.getLenDados();

	this->enq.sendToArq(comando, dados_arq, dados_arq_sz);

	this->arq.receiveFromEnq(comando, dados_arq[0], &dados_arq[1], dados_arq_sz - 1);

	if (this->arq.possui_dados() == false) {
		return 0;
	}

	this->arq.sendToAPIDAF(this->buffer, sizeof(this->buffer));

	return (dados_arq_sz - 1);

}

uint8_t PDAFCDC::recebeComando(){
	return this->arq.getComando();

}

void PDAFCDC::recebeMensagem(uint8_t *mensagem, size_t len) {
	memcpy((void*) mensagem, (const void*) this->buffer, len);
}

void PDAFCDC::enviaMensagem(uint8_t *mensagem, size_t len) {
	this->arq.receiveFromAPIDAF(mensagem, len);
}

