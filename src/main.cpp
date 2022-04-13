#include <stdio.h>
#include <stddef.h>
#include <MAX325xx.h>
#include <string.h>

#include "com/pdafcdc.h"
#include "com/arq.h"
#include "com/enquadramento.h"
#include "utils/json/json.h"
#include "com/usbcdc.h"
#include "daf.h"
#include "debug.h"
#include "string.h"
#include "memoria/memoria_segura.h"
#include "LED/MaximLED.h"

void uart_handler(void) {
}
;

void handleLed(MaximLED &led, DAF::ESTADO_t estado) {

	static uint32_t lastTs = 0;

	switch (estado) {
	case DAF::ESTADO_t::PRONTO:
		led.liga();
		break;
	case DAF::ESTADO_t::INATIVO:
	default:
		if ((PDAFCDC_Utils::get_time_as_ms() - lastTs) >= 500) {
			led.toggle();
			lastTs = PDAFCDC_Utils::get_time_as_ms();
		}

		break;
	}
}

int main(void) {


	DEBUG_PRINTF_INIT();

	DEBUG_PRINTF("Iniciando DAF\n");

	PDAFCDC pdafcdc;
	DAF daf;
	MaximLED led;


	while (1) {

		USBCDC::check_event_flag();

		handleLed(led,
				(DAF::ESTADO_t) MemoriaSegura::leitura_guarda(
						DAF_Utils::GUARDA_t::ESTADO));

		size_t mensagem_len = pdafcdc.possuiJson();
		if (mensagem_len <= 0)
			continue;

		static uint8_t buffer_mensagem[3500];
		pdafcdc.recebeMensagem(buffer_mensagem, mensagem_len);

		static json_t resposta;
		resposta.clear();

		if (daf.processaPedido(buffer_mensagem, resposta)
				== DAF::STATUS_PEDIDO_t::SUCESSO) {
			DEBUG_PRINTF("Msg processada com sucesso\n");
			size_t serialize_sz = JSON::serialize(resposta,
					(char*) buffer_mensagem, sizeof(buffer_mensagem));

			if (serialize_sz > 0)
				pdafcdc.enviaMensagem(buffer_mensagem, serialize_sz);
			else
				DEBUG_PRINTF("Serialized falhou\n");
		} else {
			DEBUG_PRINTF("Msg processada com erro(s)\n");
		}

	}
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__
