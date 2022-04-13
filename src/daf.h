#ifndef DAF_H
#define DAF_H

#include <stdio.h>
#include <iostream>
#include "utils/json/include/m_struct.h"
#include "utils/json/jsondaf.h"

#define JWT_PAYLOAD_MAX_SZ 3500

typedef enum Respostas {
	SUCESSO,
	ESTADO_INCORRETO,
	PEDIDO_MAL_FORMADO,
	ASSINATURA_INVALIDA,
	PAF_DESCONHECIDO,
	HMAC_NAO_CORRESPONDENTE,
	AUTORIZACAO_NAO_ENCONTRADA,
	AUTORIZACAO_RETIDA,
	VERSAO_SB_INVALIDA,
	MODELO_INVALIDO
} Resposta_t;


class DAF {
public:


	enum MAX_JWT_PAYLOAD_SZ {
		ALTERAR_MODO = 34,
		CONFIRMAR_ALTERAR_MODO = 11
	};

	typedef enum {
		INATIVO = 1, PRONTO = 2, BLOQUEADO = 3, INUTILIZADO = 4, BOOTLOADER = 5
	} ESTADO_t;

	typedef enum {
		FALHA = 0, SUCESSO = 1
	} STATUS_PEDIDO_t;

	DAF();

	/*
	 * @brief processa as mensagens recebidas pelo DAF
	 *
	 * @param[in]  msg_raw		objeto json com a mensagem recebida pelo DAF
	 * @param[out] resposta		documento json com a resposta do DAF
	 */
	STATUS_PEDIDO_t processaPedido(uint8_t *msg_raw, json_t &resposta);

private:

	STATUS_PEDIDO_t processaRegistrar(S_JWT &token, json_t &resposta);
	STATUS_PEDIDO_t processaConfirmarRegistro(S_JWT &token, json_t &resposta);
	STATUS_PEDIDO_t processaSolicitarAutenticacao(json_t &resposta);
	STATUS_PEDIDO_t processaAutorizarDFE(S_AUTORIZARDFE &msg, json_t &resposta);
	STATUS_PEDIDO_t processaApagarAutorizacaoRetida(S_APAGARRETIDA &msg,
			json_t &resposta);
	STATUS_PEDIDO_t processaRemoverRegistro(S_JWT &token, json_t &resposta);
	STATUS_PEDIDO_t processaConfirmarRemocaoRegistro(S_JWT &token,
			json_t &resposta);
	STATUS_PEDIDO_t processaConsultarInformacoes(json_t &resposta);
	STATUS_PEDIDO_t processaAtualizarSB(json_t &resposta);
	STATUS_PEDIDO_t processaAtualizarCertificado(S_ATUALIZARCERT &msg,
			json_t &resposta);
	STATUS_PEDIDO_t processaDescarregarRetidos(S_DESCARREGARETIDOS &msg,
			json_t &resposta);
	STATUS_PEDIDO_t processaAlterarModoOperacao(S_JWT &token, json_t &resposta);
	STATUS_PEDIDO_t processaConfirmarAlterarModoOperacao(S_JWT &token,
			json_t &resposta);
	STATUS_PEDIDO_t processaCancelarProcesso(json_t &resposta);

	/*
	 * @brief este metodo serve apenas para reinicar o DAF para um estado inicial
	 * afim de facilitar o desenvolvimento
	 */
	STATUS_PEDIDO_t processaPadraoDeFabrica(json_t &resposta);

	int last_msg;

	uint8_t nonce[16];

};

#endif
