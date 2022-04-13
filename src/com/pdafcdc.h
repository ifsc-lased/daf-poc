#ifndef USB_H
#define USB_H

#include <iostream>
#include "arq.h"
#include "enquadramento.h"

class PDAFCDC {

public:
	PDAFCDC();

	/*
	 * @brief configura as camadas do protocolo
	 */
	void configuraCamadas();

	/*
	 * @brief indica se existe objeto json para ser enviado para API-DAF
	 */
	size_t possuiJson();

	/*
	 * @brief obtem o objeto json da mensagem
	 *
	 * @param[in] mensagem  buffer alocado para inserir o objeto json
	 * @param[in] len		tamanho do buffer alocado
	 */
	void recebeMensagem(uint8_t *mensagem, size_t len);

	/*
	 * @brief obtem o comando da mensagem recebida
	 */
	uint8_t recebeComando();

	/*
	 * @brief envia mensagem para camada ARQ
	 *
	 * @param[in] mensagem 	objeto json com resposta do DAF
	 * @param[in] len		tamanho da mensagem
	 */
	void enviaMensagem(uint8_t *mensagem, size_t len);

private:
	Enquadramento enq;
	ARQ arq;

	static uint8_t buffer[4000];

};
#endif
