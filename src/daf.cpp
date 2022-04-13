#include <stdio.h>

#include "daf.h"
#include "utils/json/jsondaf.h"
#include "debug.h"
#include "utils/jwt/jwt.h"
#include "cripto/cripto_daf.h"
#include "memoria/memoria_segura.h"
#include "memoria/memoria_de_trabalho.h"
#include <string>
#include "memoria/alocador_de_memoria.h"


static uint8_t g_tmp_work_buf[sizeof(MemoriaDeTrabalho::autorizacao_t)];


DAF::DAF() {
	CriptoDAF::configura();
	this->last_msg = 0;
}
;

DAF::STATUS_PEDIDO_t DAF::processaPedido(uint8_t *msg_raw, json_t &resposta) {

	MensagemJSON msg_json;
	JSONDAF::Status mensagem_valida = JSONDAF::getMensagemJSON((char*) msg_raw,
			&msg_json);

	if (mensagem_valida != JSONDAF::Status::SUCESSO) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return STATUS_PEDIDO_t::FALHA;
	}


	STATUS_PEDIDO_t status = STATUS_PEDIDO_t::FALHA;


	if (MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO)
				== true) {

		switch(this->last_msg){
		case 1:
			if (msg_json.codigo == 14){
				status = DAF::processaCancelarProcesso(resposta);
				return status;
			}
			else if(msg_json.codigo != 2){
				resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
				return STATUS_PEDIDO_t::SUCESSO;

			}
			break;
		case 3:
			if (msg_json.codigo == 14){
				status = DAF::processaCancelarProcesso(resposta);
				return status;
			}
			else if(msg_json.codigo != 4){
				resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
				return STATUS_PEDIDO_t::SUCESSO;

			}
			break;
		case 6:
			if (msg_json.codigo == 14){
				status = DAF::processaCancelarProcesso(resposta);
				return status;
			}
			else if(msg_json.codigo != 7){
				resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
				return STATUS_PEDIDO_t::SUCESSO;

			}
			break;
		case 12:
			if (msg_json.codigo == 14){
				status = DAF::processaCancelarProcesso(resposta);
				return status;
			}
			else if(msg_json.codigo != 13){
				resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
				return STATUS_PEDIDO_t::SUCESSO;

			}
			break;

		}

	}

	switch (msg_json.codigo) {
	case REGISTRAR:
		status = DAF::processaRegistrar(msg_json.u_conteudo.token, resposta);
		break;
	case CONFIRMAR_REGISTRO:
		status = DAF::processaConfirmarRegistro(msg_json.u_conteudo.token,
				resposta);
		break;
	case SOLICITAR_AUTENTICACAO:
		status = DAF::processaSolicitarAutenticacao(resposta);
		break;
	case AUTORIZAR_DFE:
		status = DAF::processaAutorizarDFE(msg_json.u_conteudo.autorizarDFE,
				resposta);
		break;
	case APAGAR_AUTORIZACAO_RETIDA:
		status = DAF::processaApagarAutorizacaoRetida(
				msg_json.u_conteudo.apagarAutorizacaoRetida, resposta);
		break;
	case REMOVER_REGISTRO:
		status = DAF::processaRemoverRegistro(msg_json.u_conteudo.token,
				resposta);
		break;
	case CONFIRMAR_REMOCAO_REGISTRO:
		status = DAF::processaConfirmarRemocaoRegistro(
				msg_json.u_conteudo.token, resposta);
		break;
	case CONSULTAR_INFORMACOES:
		status = DAF::processaConsultarInformacoes(resposta);
		break;
	case ATUALIZAR_SB:
		status = DAF::processaAtualizarSB(resposta);
		break;
	case ATUALIZAR_CERTIFICADO:
		status = DAF::processaAtualizarCertificado(
				msg_json.u_conteudo.atualizarCertificado, resposta);
		break;
	case DESCARREGAR_RETIDOS:
		status = DAF::processaDescarregarRetidos(
				msg_json.u_conteudo.descarregarRetidos, resposta);
		break;
	case ALTERAR_MODO_OPERACAO:
		status = DAF::processaAlterarModoOperacao(msg_json.u_conteudo.token,
				resposta);
		break;
	case CONFIRMAR_ALTERAR_MODO_OPERACAO:
		status = DAF::processaConfirmarAlterarModoOperacao(
				msg_json.u_conteudo.token, resposta);
		break;
	case CANCELAR_PROCESSO:
		status = DAF::processaCancelarProcesso(resposta);
		break;
	case PADRAO_DE_FABRICA:
		status = DAF::processaPadraoDeFabrica(resposta);
		break;
	case INVALIDA:
	default:
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return STATUS_PEDIDO_t::FALHA;
		break;
	}

	return status;
}

DAF::STATUS_PEDIDO_t DAF::processaAlterarModoOperacao(S_JWT &token,
		json_t &resposta) {

	// Verifica se esta no estado correto
	ESTADO_t estado = (ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::ESTADO);

	if (estado != ESTADO_t::PRONTO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// Verifica se nao possui DFe retido
	uint32_t numDFE = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::NUM_DFE);

	if (numDFE > 0) {
		resposta["res"] = Resposta_t::AUTORIZACAO_RETIDA;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// Valida assinatura do token jwt
	MemoriaSegura::chave_SEF_t chaveSEF = MemoriaSegura::leitura_chave_SEF();
	AlocadorRing<char> charRing;
	char *payload = charRing.allocate(MAX_JWT_PAYLOAD_SZ::ALTERAR_MODO);

	int retval = JWT::verificaTokenHS256(token.jwt, token.sz, chaveSEF.chave,
			sizeof(chaveSEF.chave), (uint8_t*) payload,
			MAX_JWT_PAYLOAD_SZ::ALTERAR_MODO);

	if (retval == JWT::ERR_ASSINATURA) {
		resposta["res"] = Resposta_t::HMAC_NAO_CORRESPONDENTE;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	} else if (retval <= 0) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// Valida o payload do token jwt
	StaticJsonDocument<150> payloadJSON;
	if (JSON::deserialize(payload, payloadJSON) != JSON::SUCESSO) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	if (payloadJSON["nnc"] == nullptr || strlen(payloadJSON["nnc"]) != 22) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}


	// Gera payload do resposta
	StaticJsonDocument<200> payload_json_jwt;
	char iddaf_base64[23];
	MemoriaSegura::uuid_t iddaf = MemoriaSegura::leitura_idDAF();
	if (Base64URL::b64url_encode(iddaf.valor, sizeof(iddaf.valor), iddaf_base64,
			sizeof(iddaf_base64)) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	uint32_t contador = MemoriaSegura::leitura_contador();

	payload_json_jwt["daf"] = iddaf_base64;
	payload_json_jwt["cnt"] = contador;
	payload_json_jwt["nnc"] = payloadJSON["nnc"];


	char *payload_jwt_serializado = charRing.allocate(300);
	size_t payload_jwt_serializado_sz = JSON::serialize(payload_json_jwt,
			payload_jwt_serializado, 300);
	if (payload_jwt_serializado_sz <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	char *token_res = charRing.allocate(600);

	// Gera token da resposta
	retval = JWT::geraTokenHS256((uint8_t*) payload_jwt_serializado,
			payload_jwt_serializado_sz, chaveSEF.chave, sizeof(chaveSEF.chave),
			token_res, 600);
	if (retval <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO, true);

	// Gera resposta
	resposta["res"] = Resposta_t::SUCESSO;
	resposta["jwt"] = token_res;
	this->last_msg = 12;
	return DAF::STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaConfirmarAlterarModoOperacao(S_JWT &token,
		json_t &resposta) {

	// Verifica se o processo ja havia sido iniciado
	if (MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO)
			== false) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// Valida assinatura do token jwt
	MemoriaSegura::chave_SEF_t chaveSEF = MemoriaSegura::leitura_chave_SEF();

	AlocadorRing<char> charRing;
	char *payload = charRing.allocate(
			MAX_JWT_PAYLOAD_SZ::CONFIRMAR_ALTERAR_MODO);

	int retval = JWT::verificaTokenHS256(token.jwt, token.sz, chaveSEF.chave,
			sizeof(chaveSEF.chave), (uint8_t*) payload,
			MAX_JWT_PAYLOAD_SZ::CONFIRMAR_ALTERAR_MODO);

	if (retval == JWT::ERR_ASSINATURA) {
		resposta["res"] = Resposta_t::HMAC_NAO_CORRESPONDENTE;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	} else if (retval <= 0) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}
	// Valida o payload do token jwt
	StaticJsonDocument<100> payloadJSON;
	if (JSON::deserialize(payload, payloadJSON) != JSON::SUCESSO)
		return DAF::STATUS_PEDIDO_t::FALHA;

	if (payloadJSON["mop"] == nullptr || payloadJSON["mop"] > 1
			|| payloadJSON["mop"] < 0) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// Atualiza valores na MS
	MemoriaSegura::escrita_modo_de_operacao(payloadJSON["mop"]);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO, false);


	// Gera a resposta
	resposta["res"] = Resposta_t::SUCESSO;
	this->last_msg = 13;
	return DAF::STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaRegistrar(S_JWT &token, json_t &resposta) {

	// Verifica se esta no estado correto

	ESTADO_t estado = (ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);

	if (estado != ESTADO_t::INATIVO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}


	// Valida assinatura do token
	chave_publica_ECC_P384_t chave_do_certificado =
			MemoriaSegura::leitura_chave_do_certificado();

	AlocadorRing<char> charRing;

	char *payload_token_jwt = charRing.allocate(JWT_PAYLOAD_MAX_SZ);

	int retval = JWT::verificaTokenES384(token.jwt, token.sz,
			&chave_do_certificado, (uint8_t*) payload_token_jwt,
			JWT_PAYLOAD_MAX_SZ);

	if (retval == JWT::ERR_ASSINATURA) {
		resposta["res"] = Resposta_t::ASSINATURA_INVALIDA;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	} else if (retval <= 0) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}


	// Valida payload do token jwt
	StaticJsonDocument<50> payload_jwt_JSON;

	if (JSON::deserialize(payload_token_jwt, payload_jwt_JSON)
			!= JSON::SUCESSO) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}


	char *nnc = payload_jwt_JSON["nnc"];
	if (nnc == nullptr || strlen(nnc) != 22) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}



	// gera as chaves criptograficas e armazena na memoria segura
	chave_publica_ECC_P256_t chave_pub_daf;
	chave_privada_ECC_P256_t chave_priv_daf;
	CriptoDAF::gera_chave_ECC(&chave_pub_daf, &chave_priv_daf);
	MemoriaSegura::escrita_chave_pub(chave_pub_daf);
	MemoriaSegura::escrita_chave_priv(chave_priv_daf);

	// montando o json de resposta

	StaticJsonDocument<100> resposta_json;

	MemoriaSegura::uuid_t idDAF = MemoriaSegura::leitura_idDAF();
	char idDAF_b64[23];
	Base64URL::b64url_encode(idDAF.valor, sizeof(idDAF.valor), idDAF_b64,
			sizeof(idDAF_b64));
	resposta_json["daf"] = idDAF_b64;
	resposta_json["cnt"] = MemoriaSegura::leitura_contador();
	resposta_json["nnc"] = payload_jwt_JSON["nnc"];

	char resposta_serialized[100];

	JSON::serialize(resposta_json, resposta_serialized,
			sizeof(resposta_serialized));

	// gera os dois tokens jwt
	char *token_res = charRing.allocate(JWT_PAYLOAD_MAX_SZ);

	retval = JWT::geraTokenES256((uint8_t*) resposta_serialized,
			strlen(resposta_serialized), &chave_priv_daf, token_res,
			JWT_PAYLOAD_MAX_SZ, true, &chave_pub_daf);

	if (retval <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;



	char *token_res_json = charRing.allocate(strlen(token_res) + 20);
	sprintf(token_res_json, "{\"jwt\": \"%s\"}", token_res);

	chave_privada_ECC_P384_t ateste_priv =
			MemoriaSegura::leitura_chave_ateste_priv();
	chave_publica_ECC_P384_t ateste_pub =
			MemoriaSegura::leitura_chave_ateste_pub();

	char *token_res_final = charRing.allocate(JWT_PAYLOAD_MAX_SZ);

	retval = JWT::geraTokenES384((uint8_t*) token_res_json,
			strlen(token_res_json), &ateste_priv, token_res_final,
			JWT_PAYLOAD_MAX_SZ, true, &ateste_pub);

	if (retval <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	// gera resposta

	resposta["res"] = Resposta_t::SUCESSO;
	resposta["jwt"] = token_res_final;

	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO, 1);
	this->last_msg = 1;

	return STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaConfirmarRegistro(S_JWT &token,
		json_t &resposta) {

	// verifica se o processo ja foi iniciado

	if (MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO)
				== false) {
			resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
			return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	chave_publica_ECC_P384_t chave_do_certificado =
			MemoriaSegura::leitura_chave_do_certificado();

	AlocadorRing<char> charRing;

	char *payload_jwt = charRing.allocate(JWT_PAYLOAD_MAX_SZ);

	// valida o token jwt
	int retval = JWT::verificaTokenES384(token.jwt, token.sz,
			&chave_do_certificado, (uint8_t*) payload_jwt,
			JWT_PAYLOAD_MAX_SZ);

	if (retval == JWT::ERR_ASSINATURA) {
		resposta["res"] = Resposta_t::ASSINATURA_INVALIDA;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	} else if (retval <= 0) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}


	StaticJsonDocument<500> payload_jwt_JSON;

	if (JSON::deserialize(payload_jwt, payload_jwt_JSON) != JSON::SUCESSO) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::FALHA;
	}

	// verifica conteudo do payload

	if (payload_jwt_JSON["chp"] == nullptr || payload_jwt_JSON["chs"] == nullptr
			|| strlen(payload_jwt_JSON["chp"]) != 86
			|| payload_jwt_JSON["mop"] > 1 || payload_jwt_JSON["mop"] < 0
			|| strlen(payload_jwt_JSON["chs"]) < 86) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}


	// armazena chave SEF
	MemoriaSegura::chave_SEF_t chaveSEF;
	JWT::u_chave_privada_t chave_privada;
	chave_privada_ECC_P256_t chave_privada_ms =
			MemoriaSegura::leitura_chave_priv();
	chave_privada.ECC_P256 = &chave_privada_ms;
	char *chs = payload_jwt_JSON["chs"];
	if (JWT::verificaTokenJWE(chs, strlen(chs), &chave_privada, chaveSEF.chave,
			sizeof(chaveSEF.chave)) != JWT::VALIDACAO::ASSINATURA_VALIDA) {
		resposta["res"] = Resposta_t::ASSINATURA_INVALIDA;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	MemoriaSegura::escrita_chave_SEF(chaveSEF);

	// armazena chave PAF
	char *chp_b64 = payload_jwt_JSON["chp"];
	MemoriaSegura::chave_PAF_t chave_paf;
	if (Base64URL::b64url_decode(chp_b64, strlen(chp_b64), chave_paf.chave,
			sizeof(chave_paf.chave)) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;


	MemoriaSegura::escrita_chave_PAF(chave_paf);

	// atualiza guardas
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::ESTADO,
			(uint32_t) ESTADO_t::PRONTO);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::REGOK, 1);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO, 0);

	MemoriaSegura::escrita_modo_de_operacao(payload_jwt_JSON["mop"]);

	// gera resposta
	resposta["res"] = Resposta_t::SUCESSO;
	this->last_msg = 2;
	return STATUS_PEDIDO_t::SUCESSO;
}
DAF::STATUS_PEDIDO_t DAF::processaSolicitarAutenticacao(json_t &resposta) {

	// verifica se esta no estado correto
	if ((ESTADO_t) MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::ESTADO)
			!= ESTADO_t::PRONTO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// gera nonce
	if (CriptoDAF::gera_numero_aleatorio(this->nonce, sizeof(this->nonce)) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	char nonce_b64[23];
	if (Base64URL::b64url_encode(this->nonce, sizeof(this->nonce), nonce_b64,
			sizeof(nonce_b64)) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	// gera resposta
	resposta["res"] = Resposta_t::SUCESSO;
	resposta["nnc"] = nonce_b64;
	this->last_msg = 3;

	return STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaAutorizarDFE(S_AUTORIZARDFE &msg,
		json_t &resposta) {

	// verifica se esta no estado correto
	if ((ESTADO_t) MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::ESTADO)
			!= ESTADO_t::PRONTO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	MemoriaSegura::parametros_atualizacao_t parametros_at;
	uint8_t msg_para_verificar[sizeof(this->nonce)
			+ sizeof(MemoriaDeTrabalho::resumo_XML_DFE_t)];

	// verifica HMAC

	uint8_t tmp_hash[32];
	if (Base64URL::b64url_decode(msg.red, 43, tmp_hash, 32) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;


	memcpy(&msg_para_verificar[0], this->nonce, sizeof(this->nonce));
	if (Base64URL::b64url_decode(msg.hdf, strlen(msg.hdf),
			&msg_para_verificar[sizeof(this->nonce)],
			sizeof(msg_para_verificar) - sizeof(this->nonce)) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	MemoriaSegura::chave_PAF_t chavePAF = MemoriaSegura::leitura_chave_PAF();

	if (CriptoDAF::verifica_HMAC_SHA256(tmp_hash, msg_para_verificar,
			sizeof(msg_para_verificar), (uint8_t*) &chavePAF, sizeof(chavePAF))
			!= 0) {
		resposta["res"] = Resposta_t::PAF_DESCONHECIDO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// verifica se existe a autorizacao na MT a partir da hash do DFE

	MemoriaDeTrabalho::resumo_XML_DFE_t *resumoDFE =
			(MemoriaDeTrabalho::resumo_XML_DFE_t*) g_tmp_work_buf;

	MemoriaDeTrabalho mt = MemoriaDeTrabalho();
	size_t decoded = Base64URL::b64url_decode(msg.hdf, strlen(msg.hdf),
			resumoDFE->hash, sizeof(resumoDFE->hash));
	if (decoded == 0) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return STATUS_PEDIDO_t::FALHA;
	}

	AlocadorRing<MemoriaDeTrabalho::autorizacao_t> ring;
	MemoriaDeTrabalho::autorizacao_t *autorizacao = ring.allocate(1);

	if (mt.existe_autorizacao_DFE(*resumoDFE)) {

		mt.get_autorizacao_DFE(*resumoDFE, autorizacao);

		resposta["res"] = Resposta_t::SUCESSO;

	} else {

		// gera documento de autorizacao

		uint32_t contador = MemoriaSegura::leitura_contador();
		contador++;
		MemoriaSegura::escrita_contador(contador);

		g_tmp_work_buf[0] = (contador & 0xFF000000) >> 24;
		g_tmp_work_buf[1] = (contador & 0x00FF0000) >> 16;
		g_tmp_work_buf[2] = (contador & 0x0000FF00) >> 8;
		g_tmp_work_buf[3] = contador & 0x000000FF;

		size_t written = sizeof(contador);

		decoded = Base64URL::b64url_decode(msg.fdf, strlen(msg.fdf),
				&g_tmp_work_buf[written],
				sizeof(g_tmp_work_buf) - written - sizeof(tmp_hash));
		written += decoded;

		if (decoded == 0) {
			resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
			return STATUS_PEDIDO_t::FALHA;
		}

		decoded = Base64URL::b64url_decode(msg.hdf, strlen(msg.hdf),
				&g_tmp_work_buf[sizeof(contador) + decoded],
				sizeof(g_tmp_work_buf) - written);
		written += decoded;

		if (decoded == 0) {
			resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
			return STATUS_PEDIDO_t::FALHA;
		}

		MemoriaSegura::chave_SEF_t chaveSEF = MemoriaSegura::leitura_chave_SEF();
		CriptoDAF::gera_HMAC_SHA256(tmp_hash, &g_tmp_work_buf[0], written,
				(uint8_t*) &chaveSEF, sizeof(chaveSEF));


		if (Base64URL::b64url_encode(tmp_hash, sizeof(tmp_hash),
				autorizacao->doc.idAut.base64,
				sizeof(autorizacao->doc.idAut.base64)) <= 0)
			return STATUS_PEDIDO_t::FALHA;


		Base64URL::b64url_decode(msg.hdf,  strlen(msg.hdf), autorizacao->doc.resumoDFE.hash, 32);
		autorizacao->doc.contador = contador;
		autorizacao->doc.idDAF = MemoriaSegura::leitura_idDAF();
		parametros_at = MemoriaSegura::leitura_parametros_atualizacao();
		autorizacao->doc.versaoSB = parametros_at.versao_SB;
		autorizacao->doc.modoDeOperacao =
				MemoriaSegura::leitura_modo_de_operacao();
		memcpy(autorizacao->doc.idPDV, msg.pdv, strlen(msg.pdv) + 1);


		decoded = Base64URL::b64url_decode(msg.fdf, strlen(msg.fdf),
				autorizacao->frag.buf, sizeof(autorizacao->frag.buf));
		autorizacao->frag.buf_sz = decoded;

		if (decoded == 0) {
			resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
			return STATUS_PEDIDO_t::FALHA;
		}

		if (mt.add_autorizacao_DFE(*autorizacao) == false)
			return STATUS_PEDIDO_t::FALHA;

		// muda estado para bloqueado se numero de autorizacoes maior que max_dfe
		if (mt.get_numero_de_autorizacoes_DFE()
				>= MemoriaSegura::leitura_guarda(
						DAF_Utils::GUARDA_t::MAX_DFE)) {
			MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::ESTADO,
					ESTADO_t::BLOQUEADO);
		}

		resposta["res"] = Resposta_t::SUCESSO;
	}

	// gera payload da resposta
	StaticJsonDocument<300> campos_do_jwt;
	AlocadorRing<char> charRing;
	char *payload_do_jwt = charRing.allocate(300);


	MemoriaSegura::uuid_t iddaf = MemoriaSegura::leitura_idDAF();
	char iddaf_base64[23];
	if (Base64URL::b64url_encode(iddaf.valor, sizeof(iddaf.valor), iddaf_base64,
			sizeof(iddaf_base64)) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	campos_do_jwt["daf"] = iddaf_base64;
	campos_do_jwt["vsb"] = autorizacao->doc.versaoSB;
	campos_do_jwt["mop"] = autorizacao->doc.modoDeOperacao;
	campos_do_jwt["pdv"] = autorizacao->doc.idPDV;
	campos_do_jwt["cnt"] = autorizacao->doc.contador;
	campos_do_jwt["aut"] = autorizacao->doc.idAut.base64;

	if (JSON::serialize(campos_do_jwt, payload_do_jwt, 300) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	// gera token jwt

	MemoriaSegura::chave_SEF_t chaveSEF = MemoriaSegura::leitura_chave_SEF();
	char *token_jwt = charRing.allocate(JWT_PAYLOAD_MAX_SZ);
	JWT::geraTokenHS256((uint8_t*) payload_do_jwt, strlen(payload_do_jwt),
			chaveSEF.chave, sizeof(chaveSEF.chave), token_jwt,
			JWT_PAYLOAD_MAX_SZ);

	// gera resposta
	resposta["jwt"] = token_jwt;
	this->last_msg = 4;
	return DAF::STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaApagarAutorizacaoRetida(S_APAGARRETIDA &msg,
		json_t &resposta) {

	// verifica se esta no estado correto
	ESTADO_t estado = (ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);
	if (estado != ESTADO_t::PRONTO && estado != ESTADO_t::BLOQUEADO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}


	// verifica se existe o documento na MT
	MemoriaDeTrabalho mt = MemoriaDeTrabalho();
	MemoriaDeTrabalho::id_autorizacao_t idAut_b64;

	memcpy(idAut_b64.base64, msg.aut, sizeof(idAut_b64));

	if (!mt.existe_autorizacao_DFE(idAut_b64)) {
		resposta["res"] = Resposta_t::AUTORIZACAO_NAO_ENCONTRADA;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	MemoriaSegura::chave_SEF_t chaveSEF;
	chaveSEF = MemoriaSegura::leitura_chave_SEF();

	uint8_t aut_raw[32];
	if (Base64URL::b64url_decode(idAut_b64.base64, 43, aut_raw, sizeof(aut_raw))
			<= 0) {
		return DAF::STATUS_PEDIDO_t::FALHA;
	}

	uint8_t apg_raw[32];
	if (Base64URL::b64url_decode(msg.apg, 43, apg_raw, sizeof(apg_raw)) <= 0) {
		return DAF::STATUS_PEDIDO_t::FALHA;
	}


	// verifica se existe a autorizacao para remover o documento
	if (CriptoDAF::verifica_HMAC_SHA256(apg_raw, aut_raw, sizeof(aut_raw),
			chaveSEF.chave, sizeof(chaveSEF.chave)) != 0) {
		resposta["res"] = Resposta_t::HMAC_NAO_CORRESPONDENTE;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// remove o documento
	if (!mt.remove_autorizacao_DFE(idAut_b64)) {
		return DAF::STATUS_PEDIDO_t::FALHA;
	}


	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::ESTADO,
			ESTADO_t::PRONTO);


	// gera resposta
	resposta["res"] = Resposta_t::SUCESSO;
	this->last_msg = 5;
	return DAF::STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaRemoverRegistro(S_JWT &token,
		json_t &resposta) {

	// verifica se esta no estado correto
	if ((ESTADO_t) MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::ESTADO)
			!= ESTADO_t::PRONTO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// verifica se existem autorizacoes retidas no DAF
	if (MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::NUM_DFE) > 0) {
		resposta["res"] = Resposta_t::AUTORIZACAO_RETIDA;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}


	chave_publica_ECC_P384_t chave_do_certificado =
			MemoriaSegura::leitura_chave_do_certificado();

	AlocadorRing<char> charRing;

	char *payload_jwt = charRing.allocate(JWT_PAYLOAD_MAX_SZ);

	// valida o token jwt
	int retval = JWT::verificaTokenES384(token.jwt, token.sz,
			&chave_do_certificado, (uint8_t*) payload_jwt,
			JWT_PAYLOAD_MAX_SZ);

	if (retval == JWT::ERR_ASSINATURA) {
		resposta["res"] = Resposta_t::ASSINATURA_INVALIDA;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	} else if (retval <= 0) {
		// outros erros
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	StaticJsonDocument<300> payload_jwt_JSON;

	if (JSON::deserialize(payload_jwt, payload_jwt_JSON) != JSON::SUCESSO)
		return DAF::STATUS_PEDIDO_t::FALHA;

	// verifica conteudo do payload
	char *nnc_b64 = payload_jwt_JSON["nnc"];
	if (nnc_b64 == nullptr || strlen(nnc_b64) != 22) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	uint8_t nnc[16];
	if (Base64URL::b64url_decode(nnc_b64, strlen(nnc_b64), nnc, sizeof(nnc))
			<= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	memcpy(this->nonce, nnc, sizeof(nnc));

	// gera payload da resposta

	MemoriaSegura::uuid_t iddaf = MemoriaSegura::leitura_idDAF();
	char iddaf_b64[23];
	if (Base64URL::b64url_encode(iddaf.valor, sizeof(iddaf.valor), iddaf_b64,
			sizeof(iddaf_b64)) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	payload_jwt_JSON.clear();
	payload_jwt_JSON["daf"] = iddaf_b64;
	payload_jwt_JSON["cnt"] = MemoriaSegura::leitura_contador();
	payload_jwt_JSON["nnc"] = nnc_b64;

	if (JSON::serialize(payload_jwt_JSON, payload_jwt, JWT_PAYLOAD_MAX_SZ) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	// gera token com chave privada do DAF
	char *token_res = charRing.allocate(JWT_PAYLOAD_MAX_SZ);
	chave_privada_ECC_P256_t chave_privada =
			MemoriaSegura::leitura_chave_priv();
	if (JWT::geraTokenES256((uint8_t*) payload_jwt, strlen(payload_jwt),
			&chave_privada, token_res, JWT_PAYLOAD_MAX_SZ, false, nullptr) <= 0)
		return DAF::STATUS_PEDIDO_t::SUCESSO;

	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO, true);

	// gera resposta
	resposta["res"] = Resposta_t::SUCESSO;
	resposta["jwt"] = token_res;
	this->last_msg = 6;
	return STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaConfirmarRemocaoRegistro(S_JWT &token,
		json_t &resposta) {

	// verifica se o processo foi iniciado
	if (MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO)
			== false) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	chave_publica_ECC_P384_t chave_do_certificado =
			MemoriaSegura::leitura_chave_do_certificado();

	AlocadorRing<char> charRing;

	char *payload_jwt = charRing.allocate(JWT_PAYLOAD_MAX_SZ);

	// valida o token jwt
	int retval = JWT::verificaTokenES384(token.jwt, token.sz,
			&chave_do_certificado, (uint8_t*) payload_jwt,
			JWT_PAYLOAD_MAX_SZ);

	if (retval == JWT::ERR_ASSINATURA) {
		resposta["res"] = Resposta_t::ASSINATURA_INVALIDA;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	} else if (retval <= 0) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	StaticJsonDocument<300> payload_jwt_JSON;

	if (JSON::deserialize(payload_jwt, payload_jwt_JSON) != JSON::SUCESSO)
		return DAF::STATUS_PEDIDO_t::FALHA;

	// verifica conteudo do payload
	char *evn = payload_jwt_JSON["evn"];
	if (evn == nullptr || strlen(evn) != 7 || memcmp(evn, "REMOVER", 7) != 0) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// remove registro do daf
	MemoriaSegura::chave_PAF_t chavePAF = { };
	MemoriaSegura::chave_SEF_t chaveSEF = { };
	chave_privada_ECC_P256_t chaveDAF = { };
	MemoriaSegura::escrita_chave_PAF(chavePAF);
	MemoriaSegura::escrita_chave_SEF(chaveSEF);
	MemoriaSegura::escrita_chave_priv(chaveDAF);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::ESTADO,
			ESTADO_t::INATIVO);
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::REGOK, false);

	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO, false);


	// gera a resposta
	resposta["res"] = Resposta_t::SUCESSO;
	this->last_msg = 7;
	return DAF::STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaConsultarInformacoes(json_t &resposta) {

	// verifica se esta no estado correto
	ESTADO_t estado = (ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);
	if (estado != ESTADO_t::PRONTO && estado != ESTADO_t::BLOQUEADO
			&& estado != ESTADO_t::INATIVO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// gera resposta
	resposta["res"] = Resposta_t::SUCESSO;

	MemoriaSegura::uuid_t iddaf = MemoriaSegura::leitura_idDAF();
	char iddaf_b64[23];
	Base64URL::b64url_encode(iddaf.valor, sizeof(iddaf.valor), iddaf_b64,
			sizeof(iddaf_b64));
	resposta["daf"] = iddaf_b64;

	resposta["mop"] = MemoriaSegura::leitura_modo_de_operacao();

	MemoriaSegura::parametros_atualizacao_t sb;
	sb = MemoriaSegura::leitura_parametros_atualizacao();

	resposta["vsb"] = sb.versao_SB;

	uint8_t hash_sb_b64[43];
	Base64URL::b64url_encode((const uint8_t*) sb.resumo_SB,
			sizeof(sb.resumo_SB), (char*) hash_sb_b64, sizeof(hash_sb_b64));

	resposta["hsb"] = hash_sb_b64;

	MemoriaSegura::CNPJ_t CNPJ = MemoriaSegura::leitura_CNPJ();
	resposta["fab"] = CNPJ.numeros;
	MemoriaSegura::modelo_DAF_t modelo = MemoriaSegura::leitura_modelo_DAF();

	resposta["mdl"] = modelo.modelo;
	resposta["cnt"] = MemoriaSegura::leitura_contador();

	static MemoriaSegura::certificado_t certificado =
			MemoriaSegura::leitura_certificado();
	resposta["crt"] = certificado.certificado;

	if (estado == ESTADO_t::PRONTO)
		resposta["est"] = "PRONTO";
	else if (estado == ESTADO_t::BLOQUEADO)
		resposta["est"] = "BLOQUEADO";
	else if (estado == ESTADO_t::INATIVO)
		resposta["est"] = "INATIVO";
	else
		return DAF::STATUS_PEDIDO_t::FALHA;

	uint32_t max_dfe_model = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::MAX_DFE_MODEL);
	uint32_t max_dfe = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::MAX_DFE);
	if (max_dfe_model >= max_dfe)
		resposta["mxd"] = max_dfe;
	else
		resposta["mxd"] = max_dfe_model;

	resposta["ndf"] = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::NUM_DFE);


	AlocadorRing<MemoriaDeTrabalho::id_autorizacao_t> ring;
	std::vector<MemoriaDeTrabalho::id_autorizacao_t,
			AlocadorRing<MemoriaDeTrabalho::id_autorizacao_t>> vecIDs;

	MemoriaDeTrabalho mt;
	mt.get_autorizacoes_DFE(vecIDs, 10);

	JsonArray rts = resposta["rts"].to<JsonArray>();
	for (auto i = vecIDs.begin(); i != vecIDs.end(); ++i) {
		MemoriaDeTrabalho::id_autorizacao_t id = *i;
		rts.add(id.base64);
	}
	this->last_msg = 8;
	return DAF::STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaAtualizarSB(json_t &resposta) {

	// verifica se esta no estado correto
	DAF::ESTADO_t estado = (DAF::ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);
	if (estado != DAF::ESTADO_t::INATIVO && estado != DAF::ESTADO_t::PRONTO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return STATUS_PEDIDO_t::SUCESSO;
	}

	// verifica se existem autorizacoes retidas
	if (MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::NUM_DFE) >= 1) {
		resposta["res"] = Resposta_t::AUTORIZACAO_RETIDA;
		return STATUS_PEDIDO_t::SUCESSO;
	}

	// gera resposta
	resposta["res"] = Resposta_t::SUCESSO;
	this->last_msg = 9;
	return STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaAtualizarCertificado(S_ATUALIZARCERT &msg,
		json_t &resposta) {

	// verifica se esta no estado correto
	ESTADO_t estado = (ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);
	if (estado != ESTADO_t::PRONTO && estado != ESTADO_t::INATIVO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return STATUS_PEDIDO_t::SUCESSO;
	}

	// verifica se existem autorizacoes retidas
	if (MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::NUM_DFE) >= 1) {
		resposta["res"] = Resposta_t::AUTORIZACAO_RETIDA;
		return STATUS_PEDIDO_t::SUCESSO;
	}

	// verifica mensagem recebida
	if (msg.sz < sizeof(chave_publica_ECC_P384_t)) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return STATUS_PEDIDO_t::SUCESSO;
	}

	// atualiza certificado
	chave_publica_ECC_P384_t chave_do_certificado;
	if (Base64URL::b64url_decode(msg.cert, strlen(msg.cert),
			(uint8_t*) &chave_do_certificado, sizeof(chave_do_certificado))
			<= 0)
		return STATUS_PEDIDO_t::FALHA;

	MemoriaSegura::escrita_chave_do_certificado(chave_do_certificado);

	// gera resposta
	resposta["res"] = Resposta_t::SUCESSO;
	this->last_msg = 10;
	return STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaDescarregarRetidos(
		struct S_DESCARREGARETIDOS &msg, json_t &resposta) {
	// verifica se esta no estado correto
	ESTADO_t estado = (ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);

	if (estado != ESTADO_t::PRONTO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// verifica payload
	if (strlen(msg.aut) != 43) {
		resposta["res"] = Resposta_t::PEDIDO_MAL_FORMADO;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// verifica se existe a autorizacao na MT
	MemoriaDeTrabalho::id_autorizacao_t idAut_b64;
	memcpy(idAut_b64.base64, msg.aut, sizeof(idAut_b64));

	MemoriaDeTrabalho mt = MemoriaDeTrabalho();
	if (!mt.existe_autorizacao_DFE(idAut_b64)) {
		resposta["res"] = Resposta_t::AUTORIZACAO_NAO_ENCONTRADA;
		return DAF::STATUS_PEDIDO_t::SUCESSO;
	}

	// obtem a autorizacao
	AlocadorRing<MemoriaDeTrabalho::autorizacao_t> ring;
	MemoriaDeTrabalho::autorizacao_t *autorizacao = ring.allocate(1);


	mt.get_autorizacao_DFE(idAut_b64, autorizacao);


	AlocadorRing<char> ringChar;

	size_t xml_essencial_b64_sz = autorizacao->frag.buf_sz * 1.5;
	char *xml_essencial_b64 = ringChar.allocate(xml_essencial_b64_sz);
	if (Base64URL::b64url_encode(autorizacao->frag.buf,
			autorizacao->frag.buf_sz, xml_essencial_b64, xml_essencial_b64_sz)
			<= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	uint8_t resumo[43];
	if (Base64URL::b64url_encode(autorizacao->doc.resumoDFE.hash,
			32, (char*)resumo, 43)
			<= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;


	StaticJsonDocument<300> campos_do_jwt;
	AlocadorRing<char> charRing;
	char *payload_do_jwt = charRing.allocate(300);


	MemoriaSegura::uuid_t iddaf = MemoriaSegura::leitura_idDAF();
	char iddaf_base64[23];
	if (Base64URL::b64url_encode(iddaf.valor, sizeof(iddaf.valor), iddaf_base64,
			sizeof(iddaf_base64)) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	campos_do_jwt["daf"] = iddaf_base64;
	campos_do_jwt["vsb"] = autorizacao->doc.versaoSB;
	campos_do_jwt["mop"] = autorizacao->doc.modoDeOperacao;
	campos_do_jwt["pdv"] = autorizacao->doc.idPDV;
	campos_do_jwt["cnt"] = autorizacao->doc.contador;
	campos_do_jwt["aut"] = autorizacao->doc.idAut.base64;

	if (JSON::serialize(campos_do_jwt, payload_do_jwt, 300) <= 0)
		return DAF::STATUS_PEDIDO_t::FALHA;

	MemoriaSegura::chave_SEF_t chaveSEF = MemoriaSegura::leitura_chave_SEF();
	char *token_jwt = charRing.allocate(JWT_PAYLOAD_MAX_SZ);
	JWT::geraTokenHS256((uint8_t*) payload_do_jwt, strlen(payload_do_jwt),
			chaveSEF.chave, sizeof(chaveSEF.chave), token_jwt,
			JWT_PAYLOAD_MAX_SZ);



	// gera a resposta
	resposta["res"] = Resposta_t::SUCESSO;
	resposta["jwt"] = token_jwt;
	resposta["fdf"] = xml_essencial_b64;
	resposta["hdf"] = resumo;


	this->last_msg = 11;
	return DAF::STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaCancelarProcesso(json_t &resposta) {

	// verifica estado correto
	ESTADO_t estado = (ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);
	if (estado != ESTADO_t::INATIVO && estado != ESTADO_t::PRONTO) {
		resposta["res"] = Resposta_t::ESTADO_INCORRETO;
		return STATUS_PEDIDO_t::SUCESSO;
	}

	// cancela processo
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO, false);

	// gera resposta
	resposta["res"] = Resposta_t::SUCESSO;
	this->last_msg = 14;
	return STATUS_PEDIDO_t::SUCESSO;
}

DAF::STATUS_PEDIDO_t DAF::processaPadraoDeFabrica(json_t &resposta) {


	chave_privada_ECC_P384_t priv_ateste;
	chave_publica_ECC_P384_t pub_ateste;

	static uint8_t chave_ateste_x[] = {
			0x2d, 0x3b, 0xca, 0x67, 0xbe, 0x72, 0xf2, 0xfe, 0x99, 0xf5, 0xbe,
			0xcd, 0xca, 0x73, 0x1c, 0x05, 0x1e, 0xf1, 0x7a, 0x8b, 0x18, 0xe0,
			0xdd, 0xb7, 0x34, 0x13, 0x4e, 0xe8, 0x65, 0x20, 0xe2, 0xa4, 0xbb,
			0xba, 0x6c, 0x96, 0x98, 0x75, 0xff, 0xb7, 0x98, 0x03, 0x88, 0x96,
			0x27, 0x97, 0x4a, 0x9e
	};

	static uint8_t chave_ateste_y[] = {
			0xe9, 0xbd, 0x33, 0x61, 0x78, 0xb1, 0x17, 0x6b, 0x8f, 0x32, 0x3d,
			0xe6, 0xba, 0x8f, 0x7a, 0xd9, 0x03, 0xb7, 0xd2, 0xe5, 0xaf, 0xd6,
			0x56, 0x4c, 0x62, 0xdf, 0x72, 0xa0, 0x37, 0x29, 0xaa, 0x71, 0xca,
			0x3a, 0xb7, 0x7c, 0x50, 0xd7, 0xe3, 0xa5, 0x14, 0x08, 0x6e, 0xae,
			0xac, 0x2b, 0x6e, 0x81
	};

	static uint8_t chave_ateste_d[] = {
			0x56, 0x4d, 0x21, 0xaf, 0xc2, 0xed, 0x2e, 0x84, 0xbb, 0x81, 0x05,
			0xa7, 0x09, 0xcb, 0xb9, 0x01, 0x05, 0x39, 0xf0, 0xb8, 0x55, 0xb4,
			0x07, 0x25, 0x7f, 0xdd, 0xea, 0xb7, 0x3c, 0x51, 0x38, 0xa5, 0xf6,
			0xbd, 0x71, 0x93, 0x5d, 0x56, 0x6a, 0x6e, 0x2e, 0xd5, 0x11, 0x0e,
			0x78, 0xb6, 0x0d, 0x65
	};

	memcpy((void*) pub_ateste.x, (const void*) chave_ateste_x, sizeof(chave_ateste_x));
	memcpy((void*) pub_ateste.y, (const void*) chave_ateste_y, sizeof(chave_ateste_y));
	memcpy((void*) priv_ateste.d, (const void*) chave_ateste_d, sizeof(chave_ateste_d));

	MemoriaSegura::escrita_chave_ateste_priv(priv_ateste);
	MemoriaSegura::escrita_chave_ateste_pub(pub_ateste);


	static uint8_t chave_sef_x[] = { 0x27, 0x63, 0x59, 0xa4, 0x14, 0xfc, 0x44,
			0xd2, 0xb1, 0x56, 0xe2, 0x68, 0x3c, 0x3b, 0x51, 0xc8, 0x76, 0x70,
			0x01, 0x30, 0x10, 0x08, 0xf2, 0xd9, 0x24, 0xba, 0x47, 0x04, 0x47,
			0xc7, 0x63, 0x0d, 0xf8, 0xa2, 0xa9, 0x28, 0xc5, 0xaf, 0x2b, 0x85,
			0x6b, 0xc7, 0x6e, 0x4c, 0xf4, 0xac, 0x40, 0xce };

	static uint8_t chave_sef_y[] = { 0xd9, 0x4f, 0x9a, 0x54, 0x4c, 0x2d, 0x38,
			0x6c, 0x42, 0x85, 0x9a, 0x85, 0xb5, 0x44, 0xd0, 0xd9, 0xaf, 0x76,
			0xb0, 0x3b, 0x8d, 0xb4, 0x4b, 0x1c, 0xbf, 0xc2, 0xa2, 0xab, 0xad,
			0xc0, 0xdf, 0xcb, 0xec, 0x06, 0xb7, 0x64, 0x12, 0x85, 0xc7, 0x42,
			0x60, 0x1e, 0xe4, 0x75, 0x35, 0x3d, 0x5c, 0x1c };

	chave_publica_ECC_P384_t chave_publica_SEF_certificado;
	memcpy(chave_publica_SEF_certificado.x, chave_sef_x, sizeof(chave_sef_x));
	memcpy(chave_publica_SEF_certificado.y, chave_sef_y, sizeof(chave_sef_y));
	MemoriaSegura::escrita_chave_do_certificado(chave_publica_SEF_certificado);

	MemoriaSegura::escrita_contador(0);

	MemoriaSegura::uuid_t idDAF = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 15 };
	MemoriaSegura::escrita_idDAF(idDAF);

	MemoriaSegura::parametros_atualizacao_t params;
	params.versao_SB = 1;
	for (int i = 0;  i< 32; i++){
		params.resumo_SB[i] = (uint8_t) i;
	}

	MemoriaSegura::escrita_parametros_atualizacao(params);
	MemoriaSegura::modelo_DAF_t modelo;
	memcpy(modelo.modelo, "daf-poc", sizeof("daf-poc"));
	MemoriaSegura::escrita_modelo_DAF(modelo);

	MemoriaSegura::CNPJ_t cnpj;
	memcpy(cnpj.numeros, "81025043000123", 15);
	MemoriaSegura::escrita_CNPJ(cnpj);

	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::ESTADO,
			(uint32_t) DAF::ESTADO_t::INATIVO);

	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::MAX_DFE, 5);

	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::MAX_DFE_MODEL, 5);

	MemoriaSegura::escrita_falhas_de_atualizacao(0);

	MemoriaDeTrabalho mt;
	mt.reseta();

	MemoriaSegura::escrita_modo_de_operacao(0);

	static MemoriaSegura::certificado_t certificado;
	char substituto[] = "-----BEGIN CERTIFICATE-----\nMIICJjCCAaugAwIBAgIUaS3sIKXSF6A7DJbWAJefB3sn95gwCgYIKoZIzj0EAwIw\n\ndDEMMAoGA1UECgwDU0VGMQ4wDAYDVQQLDAVHRVNBQzELMAkGA1UEBhMCQlIxFzAV\nBgNVBAgMDlNhbnRhIENhdGFyaW5hMRYwFAYDVQQHDA1GbG9yaWFub3BvbGlzMRYw\nFAYDVQQDDA1zZWYuc2MuZ292LmJyMB4XDTIxMDUyODE0MzUxOFoXDTMxMDUyNzE0\nMzUxOFowdDEMMAoGA1UECgwDU0VGMQ4wDAYDVQQLDAVHRVNBQzELMAkGA1UEBhMC\nQlIxFzAVBgNVBAgMDlNhbnRhIENhdGFyaW5hMRYwFAYDVQQHDA1GbG9yaWFub3Bv\nbGlzMRYwFAYDVQQDDA1zZWYuc2MuZ292LmJyMHYwEAYHKoZIzj0CAQYFK4EEACID\nYgAEJ2NZpBT8RNKxVuJoPDtRyHZwATAQCPLZJLpHBEfHYw34oqkoxa8rhWvHbkz0\nrEDO2U+aVEwtOGxChZqFtUTQ2a92sDuNtEscv8Kiq63A38vsBrdkEoXHQmAe5HU1\nPVwcMAoGCCqGSM49BAMCA2kAMGYCMQDpSFdJD8/VGq/yXYEEUoxr0AB8rjcwdL3S\nTQ3ZtdW4j6sam5CSdFhkUKSuvW17UMkCMQC+O8Srmy4sZVBlb1gCOxxEmmwxVxEc\nLPHYbf1rpnfzuq44vqAI8MnRcAnLgtQNmr8=\n-----END CERTIFICATE-----";

	memcpy(certificado.certificado, substituto, sizeof(substituto));
	certificado.sz = sizeof(substituto);
	MemoriaSegura::escrita_certificado(certificado);

	resposta["res"] = Resposta_t::SUCESSO;
	return STATUS_PEDIDO_t::SUCESSO;
}
