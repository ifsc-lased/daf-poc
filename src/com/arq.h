#ifndef COM_ARQ_H_
#define COM_ARQ_H_
#include <iostream>

/*
 * sobre referencia cruzada: https://stackoverflow.com/questions/3735321/solving-cross-referencing
 */

class Enquadramento;

class ARQ {

public:

	enum ESTADOS_t {
		IDLE = 0, WAIT = 1
	};

	enum CONTROLE_t {
		DATA_0 = 0x00, DATA_1 = 0x08, ACK_0 = 0x80, ACK_1 = 0x88, NONE = 0xff
	};
	ARQ();

	/*
	 * @brief adiciona a camada de enquadramento ao objeto ARQ
	 *
	 * @param[in] enq 		objeto enquadramento
 	 */
	void addEnq(Enquadramento *enq);

	/*
	 * @brief recebe dados da camada Enq
	 *
	 * @param[in] comando	byte de comando da mensagem
	 * @param[in] controle  byte de controle da mensagem
	 * @param[in] dados		buffer com os dados da mensagem
	 * @param[in] dados_sz	tamanho do buffer com os dados da mensagem
	 *
	 */
	void receiveFromEnq(uint8_t comando, uint8_t controle, uint8_t *dados,
			size_t dados_sz);

	/*
	 * @brief recebe dados da camada superior (API-DAF)
	 *
	 * @param[in] dados		buffer com os dados da mensagem
	 * @param[in] dados_sz	tamanho do buffer com os dados da mensagem
	 *
	 */
	void receiveFromAPIDAF(uint8_t *dados, size_t dados_sz);


	/*
	 * @brief envia mensagem para camada inferior Enq
	 *
	 * @param[in] dados		buffer com os dados da mensagem
	 * @param[in] dados_sz	tamanho do buffer com os dados da mensagem
	 *
	 */
	void sendToEnq(uint8_t *dados, size_t dados_sz);

	/*
	 * @brief envia mensagem para camada superior (API-DAF)
	 *
	 * @param[in] dados		buffer com os dados da mensagem
	 * @param[in] dados_sz	tamanho do buffer com os dados da mensagem
	 *
	 */
	void sendToAPIDAF(uint8_t *dados, size_t dados_sz);

	/*
	 * @brief indica se a maquina de estados possui dados para serem transmitidos para a camada superior
	 */
	bool possui_dados();

	/*
	 * @brief obtem o campo comando da mensagem
	 */
	uint8_t getComando();

	/*
	 * @brief metodo para representar a maquina de estados da camada
	 *
	 * @param[in] comando	byte de comando da mensagem
	 * @param[in] dados		buffer com os dados da mensagem
	 * @param[in] dados_sz	tamanho do buffer com os dados da mensagem
	 *
	 */
	void handle_fsm(uint8_t controle, uint8_t *dados, size_t dados_sz);

	/*
	 * @brief implementa o comportamento do estado idle da maquina de estados
	 *
	 * @param[in] controle  byte de controle da mensagem
	 * @param[in] dados		buffer com os dados da mensagem
	 * @param[in] dados_sz	tamanho do buffer com os dados da mensagem
	 *
	 */
	void idle(uint8_t controle, uint8_t *dados, size_t dados_sz);

	/*
	 * @brief implementa o comportamento do estado wait da maquina de estados
	 *
	 * @param[in] controle  byte de controle da mensagem
	 * @param[in] dados		buffer com os dados da mensagem
	 * @param[in] dados_sz	tamanho do buffer com os dados da mensagem
	 *
	 */
	void wait(uint8_t controle, uint8_t *dados, size_t dados_sz);

	/*
	 * @brief callback do timeout desta camada
	 */
	static void handle_timeout();

	/*
	 * @brief implementa do callback desta camada
	 */
	void timeout_handler();


	/*
	 * @brief envia ack para camada inferior
	 *
	 * @param[in] seq 	numero de sequencia do ack
	 */
	void ack(bool seq);

	/*
	 * @brief retransmite mensagem para camada inferior
	 */
	void mecanismoDeRetransmissao(void);

private:
	Enquadramento *enq;

	static const int TENTATIVAS_MAX = 3;
	static uint8_t buffer_mensagem_recebida[4000];
	static uint8_t buffer_mensagem_enviada[4000];
	static const size_t TIMEOUT_MS = 1000;
	static int timer_id;

	bool seq_rx;
	bool seq_tx;
	bool dados;

	int tentativasDeRetransmissao;
	int state;
	int dados_len;

	uint8_t comando;
	size_t mensagem_enviada_len = { 0 };



};

#endif /* COM_ARQ_H_ */
