#include "enquadramento.h"
#include "string.h"
#include "arq.h"

#ifdef MAX32552_A1
	#include "../debug.h"

	#include "../timer/timer_maxim.h"
	static bool g_timeout = false;

	int Enquadramento::timer_id = -1;
#endif

uint8_t Enquadramento::buffer_dados[4000];
uint8_t Enquadramento::buffer_enviado[4000];

Enquadramento::Enquadramento() {
	this->state = (int) ESTADO_t::IDLE;
	this->dados_sz = 0;
	this->atualTamanhoDoCampoTamanho = 0;
	this->campoTamanho = 0;
	this->campoComando = PDAFCDC_Utils::COMANDO_t::INVALIDO;
	this->tamanhoDoCampoTamanho = PDAFCDC_Utils::TAMANHO_t::TAM_2;
	this->ping = false;
	this->campoControle = 0x00;
	this->arq = nullptr;
	this->quadroCompletoRecebido = false;
	this->len_buffer_enviado = 0;
}


int Enquadramento::possui_dados() {
#ifdef MAX32552_A1
	return this->usb.bytes_para_ler(0);
#endif
}


bool Enquadramento::le_usb(uint8_t *dados, size_t len) {
	#ifdef MAX32552_A1
		this->usb.le_bytes_bloqueante(dados, len);
	#endif

	for (size_t i = 0; i < len; i++) {
		this->handle_fsm(dados[i]);
	}

	return this->quadroCompletoRecebido;

}

bool Enquadramento::hasQuadroCompletoRecebido() {
	return this->quadroCompletoRecebido;
}

void Enquadramento::escreve_usb(uint8_t *dados, size_t len) {

	memcpy((void*) this->buffer_enviado, (const void*) dados, len);
	this->len_buffer_enviado = len;
	#ifdef MAX32552_A1
		this->usb.escreve_bytes_bloqueante(dados, len);
	#endif
}

void Enquadramento::getBufferEnviado(uint8_t *dados) {
	memcpy((void*) dados, (const void*) this->buffer_enviado,
			this->len_buffer_enviado);
}


void Enquadramento::addArq(ARQ *arq) {
	this->arq = arq;
}


void Enquadramento::sendToArq(uint8_t &comando, uint8_t *dados, size_t len) {
	comando = this->campoComando;
	memcpy((void*) dados, (const void*) this->buffer_dados, len);
	this->quadroCompletoRecebido = false;
	this->zeraVariaveis();
}


void Enquadramento::receiveFromArq(uint8_t *dados, size_t dados_sz) {

	static uint8_t buffer[3500] = { 0 };
	buffer[0] = 0x01;
	buffer[1] = (dados_sz & 0xff00) >> 8;
	buffer[2] = dados_sz & 0xff;

	memcpy(&buffer[3], dados, dados_sz);

	this->escreve_usb(buffer, dados_sz + 3);
}

void Enquadramento::zeraVariaveis() {

	this->state = (int) ESTADO_t::IDLE;
	memset(this->buffer_dados, '\0', 4000);
	memset(this->buffer_tamanho, '\0', 4);
	this->dados_sz = 0;
	this->atualTamanhoDoCampoTamanho = 0;
	this->campoTamanho = 0;
	this->campoComando = PDAFCDC_Utils::COMANDO_t::INVALIDO;
	this->tamanhoDoCampoTamanho = PDAFCDC_Utils::TAMANHO_t::TAM_2;
	this->ping = false;
	this->campoControle = 0x00;
	this->quadroCompletoRecebido = false;
#ifdef MAX32552_A1
	this->timer_id = -1;
#endif
}

/**
 * nao e' possivel passar um metodo como callback para funcao que esteja
 * esperando uma funcao. Metodo tem o parametro 'this' implicito. Por isso
 * o handler aqui deve ser estatico.
 */

void Enquadramento::handle_timeout() {

	/**
	 * usar flag aqui e fazer o handler fora da interrupcao
	 */
#ifdef MAX32552_A1
	g_timeout = true;
	TimerMaxim::disable_callback(Enquadramento::timer_id);
#endif
}

void Enquadramento::timeout_handler() {

#ifdef MAX32552_A1
	if (g_timeout) {
		DEBUG_PRINTF("ENQ timed out. Bytes rxed (dados): %i:\n",
				this->dados_sz);

		for (size_t i = 0; i < this->dados_sz; i++)
			DEBUG_PRINTF("%02x ", this->buffer_dados[i]);
		DEBUG_PRINTF("\n");

		g_timeout = false;
		this->zeraVariaveis();

		TimerMaxim::disable_callback(Enquadramento::timer_id);
	}
#endif
}


void Enquadramento::handle_fsm(uint8_t byte) {

	switch (this->state) {
	case ESTADO_t::IDLE:
		this->handle_fsm_idle(byte);
		break;
	case ESTADO_t::TAMANHO:
		this->handle_fsm_tamanho(byte);
		break;
	case ESTADO_t::RX:
		this->handle_fsm_rx(byte);
		break;
	}
}


void Enquadramento::handle_fsm_idle(uint8_t byte) {

	switch (byte) {
	case PDAFCDC_Utils::COMANDO_t::ENVIAR_MENSAGEM:
#ifdef MAX32552_A1
		DEBUG_PRINTF("Comando ENVIAR_MENSAGEM recebido.\n");
#endif
		this->campoComando = byte;
		this->tamanhoDoCampoTamanho = PDAFCDC_Utils::TAMANHO_t::TAM_2;
		this->state = (int) ESTADO_t::TAMANHO;


#ifdef MAX32552_A1
		this->timer_id = TimerMaxim::enable_callback(Enquadramento::TIMEOUT_MS,
				Enquadramento::handle_timeout);
#endif

		break;

	case PDAFCDC_Utils::COMANDO_t::ENVIAR_BINARIO:
#ifdef MAX32552_A1
		DEBUG_PRINTF("Comando ENVIAR_BINARIO recebido.\n");
#endif
		this->campoComando = byte;
		this->tamanhoDoCampoTamanho = PDAFCDC_Utils::TAMANHO_t::TAM_4;
		this->state = (int) ESTADO_t::TAMANHO;


#ifdef MAX32552_A1
		this->timer_id = TimerMaxim::enable_callback(Enquadramento::TIMEOUT_MS,
				Enquadramento::handle_timeout);
#endif

		break;


	default:
		break;

	}

}


void Enquadramento::handle_fsm_tamanho(uint8_t byte) {

	if (this->atualTamanhoDoCampoTamanho < (this->tamanhoDoCampoTamanho - 1)) {
		this->buffer_tamanho[this->atualTamanhoDoCampoTamanho] = byte;
		this->atualTamanhoDoCampoTamanho++;
	} else if (this->atualTamanhoDoCampoTamanho
			== (this->tamanhoDoCampoTamanho - 1)) {
		this->buffer_tamanho[this->atualTamanhoDoCampoTamanho] = byte;
		this->state = (int) ESTADO_t::RX;

		if (this->tamanhoDoCampoTamanho == PDAFCDC_Utils::TAMANHO_t::TAM_2) {
			this->campoTamanho = (this->buffer_tamanho[0] << 8)
					+ this->buffer_tamanho[1];
#ifdef MAX32552_A1
			DEBUG_PRINTF("Tamanho dos dados: %u\n", this->campoTamanho);
#endif

		} else if (this->tamanhoDoCampoTamanho
				== PDAFCDC_Utils::TAMANHO_t::TAM_4) {
			this->campoTamanho = (this->buffer_tamanho[0] << 24)
					+ (this->buffer_tamanho[1] << 16)
					+ (this->buffer_tamanho[2] << 8) + this->buffer_tamanho[3];
#ifdef MAX32552_A1
			DEBUG_PRINTF("Tamanho dos dados: %u\n", this->campoTamanho);
#endif

		} else {
#ifdef MAX32552_A1
			DEBUG_PRINTF("Tamanho invalido!\n");
#endif
			while (1)
				;

		}
	}


#ifdef MAX32552_A1
	TimerMaxim::disable_callback(Enquadramento::timer_id);
	this->timer_id = TimerMaxim::enable_callback(Enquadramento::TIMEOUT_MS,
			Enquadramento::handle_timeout);
#endif

}


void Enquadramento::handle_fsm_rx(uint8_t byte) {

	if (this->dados_sz < (this->campoTamanho - 1)) {
		this->buffer_dados[this->dados_sz] = byte;
		this->dados_sz++;
#ifdef MAX32552_A1
		TimerMaxim::disable_callback(Enquadramento::timer_id);
		this->timer_id = TimerMaxim::enable_callback(Enquadramento::TIMEOUT_MS,
				Enquadramento::handle_timeout);
#endif

	} else if (this->dados_sz == (this->campoTamanho - 1)) {
		this->buffer_dados[this->dados_sz] = byte;
#ifdef MAX32552_A1
		TimerMaxim::disable_callback(Enquadramento::timer_id);
#endif

		this->state = (int) ESTADO_t::IDLE;
		this->quadroCompletoRecebido = true;

	}

}


size_t Enquadramento::getLenDados() {
	return this->campoTamanho;
}
