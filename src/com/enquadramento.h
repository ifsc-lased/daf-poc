#ifndef COM_ENQUADRAMENTO_H_
#define COM_ENQUADRAMENTO_H_
#include <iostream>
#ifdef MAX32552_A1
#include "usbcdc.h"
#endif
#include "pdafcdc_utils.h"

/*
 * sobre referencia cruzada: https://stackoverflow.com/questions/3735321/solving-cross-referencing
 */

class ARQ;

class Enquadramento {

public:

	enum ESTADO_t {
		IDLE = 0, TAMANHO = 1, RX = 2

	};

	Enquadramento();

	/*
	 * @brief adiciona a camada ARQ ao objeto Enquadramento
	 *
	 * @param[in] arq 		objeto arq
 	 */
	void addArq(ARQ *arq);

	/*
	 * @brief envia mensagem para camada superior ARQ
	 *
	 * @param[in] comando	byte de comando da mensagem
	 * @param[in] dados		buffer com os dados da mensagem
	 * @param[in] dados_sz	tamanho do buffer com os dados da mensagem
	 *
	 */
	void sendToArq(uint8_t &comando, uint8_t *dados, size_t len);

	/*
	 * @brief recebe dados da camada superior ARQ
	 *
	 * @param[in] dados		buffer com os dados da mensagem
	 * @param[in] dados_sz	tamanho do buffer com os dados da mensagem
	 *
	 */

	void receiveFromArq(uint8_t *dados, size_t dados_sz);


	/*
	 * @brief metodo para representar a maquina de estados da camada
	 *
	 * @param[in] byte		byte recebido pela interface usb
	 *
	 */
	void handle_fsm(uint8_t byte);

	/*
	 * @brief implementa o comportamento do estado idle da maquina de estados
	 *
	 * @param[in] byte		byte recebido pela interface usb
	 *
	 */
	void handle_fsm_idle(uint8_t byte);

	/*
	 * @brief implementa o comportamento do estado tamanho da maquina de estados
	 *
	 * @param[in] byte		byte recebido pela interface usb
	 *
	 */
	void handle_fsm_tamanho(uint8_t byte);


	/*
	 * @brief implementa o comportamento do estado rx da maquina de estados
	 *
	 * @param[in] byte		byte recebido pela interface usb
	 *
	 */
	void handle_fsm_rx(uint8_t byte);

	/*
	 * @brief reinicia os atributos da classe
	 */
	void zeraVariaveis();

	/*
	 * @brief callback do timeout
	 */
	static void handle_timeout();

	/*
	 * @brief handler do timeout da maquina de estados
	 */
	void timeout_handler();

	/*
	 * @brief envia dados para a interface usb
	 *
	 * @param[in] dados		informacoes a ser transmitida
	 * @param[in] len		tamanho das informacoes
	 *
	 */
	void escreve_usb(uint8_t *dados, size_t len);

	/*
	 * @brief obtem dados da interface usb
	 *
	 * @param[in] dados		buffer para alocar os dados da camada usb
	 * @param[in] len		tamanho do buffer alocado
	 *
	 */
	bool le_usb(uint8_t *dados, size_t len);

	/*
	 * @brief verifica se a camada usb possui bytes para serem lidos
	 */
	int possui_dados();

	/*
	 * @brief obtem o tamanho do campo dados da mensagem recebida
	 */
	size_t getLenDados();

	/*
	 * @brief indica se a maquina de estados recebeu um quadro completo
	 */
	bool hasQuadroCompletoRecebido(void);

	/*
	 * @brief metodo utilizado apenas nos testes unitarios
	 *
	 * @param[in] dados 	buffer para alocar os dados enviados
	 */
	void getBufferEnviado(uint8_t *dados);

private:
	ARQ *arq;

	static uint8_t buffer_dados[4000];
	static uint8_t buffer_enviado[4000];
	static const size_t TIMEOUT_MS = 500;

	bool quadroCompletoRecebido;
	bool ping;


	int state;
	int atualTamanhoDoCampoTamanho;

	uint8_t campoComando;
	uint8_t buffer_tamanho[4];
	uint8_t campoControle;

	uint32_t campoTamanho;

	size_t dados_sz;
	size_t len_buffer_enviado;

	PDAFCDC_Utils::TAMANHO_t tamanhoDoCampoTamanho;

#ifdef MAX32552_A1
	static int timer_id;
	USBCDC usb;
#endif


};

#endif /* COM_ENQUADRAMENTO_H_ */
