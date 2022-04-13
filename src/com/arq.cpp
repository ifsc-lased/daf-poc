#include "arq.h"
#include <string.h>
#include "enquadramento.h"
#include "../timer/timer_maxim.h"
#include "../debug.h"

int ARQ::timer_id = -1;

uint8_t ARQ::buffer_mensagem_recebida[4000] = { 0 };
uint8_t ARQ::buffer_mensagem_enviada[4000];

static bool g_timeout = false;


ARQ::ARQ() {
	this->seq_rx = false;
	this->seq_tx = false;
	this->tentativasDeRetransmissao = 0;
	this->state = (int) ARQ::ESTADOS_t::IDLE;
	this->mensagem_enviada_len = 0;
	this->dados_len = 0;
	this->dados = false;
	this->enq = nullptr;
	this->timer_id = -1;
	this->comando = 0x00;
}

uint8_t ARQ::getComando() {
	return this->comando;
}


void ARQ::addEnq(Enquadramento *enq) {
	this->enq = enq;
}


void ARQ::sendToAPIDAF(uint8_t *dados, size_t dados_sz) {

	memcpy((void*) dados, (const void*) this->buffer_mensagem_recebida,
			this->dados_len);
	this->dados = false;
}


void ARQ::sendToEnq(uint8_t *dados, size_t dados_sz) {

	memcpy((void*) this->buffer_mensagem_enviada, (const void*) dados,
			dados_sz);
	this->mensagem_enviada_len = dados_sz;
	this->enq->receiveFromArq(dados, dados_sz);

}


void ARQ::receiveFromAPIDAF(uint8_t *dados, size_t dados_sz) {

	if (this->state == ARQ::ESTADOS_t::IDLE) {
		this->idle(ARQ::CONTROLE_t::NONE, dados, dados_sz);
	} else {
#ifdef MAX32552_A1
		DEBUG_PRINTF("receiveFromAPIDAF() fora do estado IDLE!\n");
#endif
	}
}


void ARQ::receiveFromEnq(uint8_t comando, uint8_t controle, uint8_t *dados,
		size_t dados_sz) {
	this->comando = comando;
	this->handle_fsm(controle, dados, dados_sz);

}

/**
 * nao e' possivel passar um metodo como callback para funcao que esteja
 * esperando uma funcao. Metodo tem o parametro 'this' implicito. Por isso
 * o handler aqui deve ser estatico.
 */


void ARQ::handle_timeout() {

	/**
	 * usar flag aqui e fazer o handler fora da interrupcao
	 */
#ifdef MAX32552_A1
	g_timeout = true;
	TimerMaxim::disable_callback(ARQ::timer_id);
#endif
}

void ARQ::mecanismoDeRetransmissao() {

	if (this->tentativasDeRetransmissao < this->TENTATIVAS_MAX) {

		DEBUG_PRINTF("Reenviando msg...\n");
		this->tentativasDeRetransmissao++;
		this->sendToEnq(this->buffer_mensagem_enviada,
				this->mensagem_enviada_len);

#ifdef MAX32552_A1

		TimerMaxim::disable_callback(ARQ::timer_id);
		this->timer_id = TimerMaxim::enable_callback(ARQ::TIMEOUT_MS,
				ARQ::handle_timeout);
#endif

	} else {
		this->state = (int) ARQ::ESTADOS_t::IDLE;
		this->tentativasDeRetransmissao = 0;

#ifdef MAX32552_A1

		TimerMaxim::disable_callback(ARQ::timer_id);
#endif

	}
}

void ARQ::timeout_handler() {
#ifdef MAX32552_A1
	if (g_timeout) {
		DEBUG_PRINTF("ARQ timed out.\n");
		g_timeout = false;
		mecanismoDeRetransmissao();
	}
#endif
}

void ARQ::handle_fsm(uint8_t controle, uint8_t *dados, size_t dados_sz) {

	switch (this->state) {
	case ARQ::ESTADOS_t::IDLE:
		this->idle(controle, dados, dados_sz);
		break;
	case ARQ::ESTADOS_t::WAIT:
		this->wait(controle, dados, dados_sz);
		break;
	}

}


void ARQ::wait(uint8_t controle, uint8_t *dados, size_t dados_sz) {

	if ((this->seq_tx == true && controle == ARQ::CONTROLE_t::ACK_1)
			|| (this->seq_tx == false && controle == ARQ::CONTROLE_t::ACK_0)) {
		this->seq_tx = !this->seq_tx;
		this->state = (int) ARQ::ESTADOS_t::IDLE;
		this->tentativasDeRetransmissao = 0;
#ifdef MAX32552_A1

		TimerMaxim::disable_callback(ARQ::timer_id);
#endif
	} else if ((this->seq_tx == true && controle == ARQ::CONTROLE_t::ACK_0)
			|| (this->seq_tx == false && controle == ARQ::CONTROLE_t::ACK_1)) {

		mecanismoDeRetransmissao();

	} else if ((this->seq_rx == true && controle == ARQ::CONTROLE_t::DATA_1)
			|| (this->seq_rx == false && controle == ARQ::CONTROLE_t::DATA_0)) {
		this->seq_rx = !this->seq_rx;
		this->ack(this->seq_rx);
		this->dados = true;
		this->dados_len = dados_sz;
		memcpy((void*) this->buffer_mensagem_recebida, (const void*) dados,
				dados_sz);

	} else if ((this->seq_rx == false && controle == ARQ::CONTROLE_t::DATA_1)
			|| (this->seq_rx == true && controle == ARQ::CONTROLE_t::DATA_0)) {
		this->ack(!this->seq_rx);
	}
}

void ARQ::idle(uint8_t controle, uint8_t *dados, size_t dados_sz) {

	if (controle == ARQ::CONTROLE_t::NONE) {
		if (this->seq_tx == false) {
			controle = 0x00;
		} else {
			controle = 0x08;
		}

		this->state = (int) ARQ::ESTADOS_t::WAIT;

		static uint8_t buffer[3500] = { 0 };
		buffer[0] = controle;
		if (dados_sz > sizeof(buffer)) {

			dados_sz = sizeof(buffer);
		}
		memcpy(&buffer[1], dados, dados_sz);

		this->sendToEnq(buffer, dados_sz + 1);

#ifdef MAX32552_A1

		this->timer_id = TimerMaxim::enable_callback(ARQ::TIMEOUT_MS,
				ARQ::handle_timeout);
#endif

	} else {

		if ((this->seq_rx == true && controle == ARQ::CONTROLE_t::DATA_1)
				|| (this->seq_rx == false && controle == ARQ::CONTROLE_t::DATA_0)) {

			this->state = (int) ARQ::ESTADOS_t::IDLE;
#ifdef MAX32552_A1
			this->ack(this->seq_rx);
#endif

			this->seq_rx = !this->seq_rx;

			this->dados = true;
			this->dados_len = dados_sz;

			memcpy((void*) this->buffer_mensagem_recebida, (const void*) dados,
					dados_sz);

		} else if ((this->seq_rx == false && controle == ARQ::CONTROLE_t::DATA_1)
				|| (this->seq_rx == true && controle == ARQ::CONTROLE_t::DATA_0)) {
			this->state = ARQ::ESTADOS_t::IDLE;
#ifdef MAX32552_A1
			this->ack(!this->seq_rx);
#endif
		}

	}
}


void ARQ::ack(bool seq) {
	uint8_t ack;
	if (seq) {
		ack = ARQ::CONTROLE_t::ACK_1;
	} else {
		ack = ARQ::CONTROLE_t::ACK_0;
	}
#ifdef MAX32552_A1
	DEBUG_PRINTF("Enviando ACK: %02x\n", ack);
#endif
	this->enq->receiveFromArq(&ack, 1);
}


bool ARQ::possui_dados() {
	return this->dados;
}

