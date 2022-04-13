#include "jsondaf.h"
#include <math.h>

JSONDAF::Status JSONDAF::getMensagemJSON(const char *raw_json,
		MensagemJSON *mensagemJSON) {

	static StaticJsonDocument<3500> documento;

	DeserializationError error = deserializeJson(documento, raw_json);

	if (error) {
		return Status::ERRO_JSON_INVALIDO;
	}

	if (documento["msg"] == nullptr)
		return Status::ERRO_CAMPO_INVALIDO;

	mensagemJSON->codigo = documento["msg"];
	switch (mensagemJSON->codigo) {
	case MensagensDAF::REGISTRAR:
	case MensagensDAF::CONFIRMAR_REGISTRO:
	case MensagensDAF::REMOVER_REGISTRO:
	case MensagensDAF::CONFIRMAR_REMOCAO_REGISTRO:
	case MensagensDAF::ALTERAR_MODO_OPERACAO:
	case MensagensDAF::CONFIRMAR_ALTERAR_MODO_OPERACAO:
		if (documento["jwt"] == nullptr)
			return Status::ERRO_CAMPO_INVALIDO;

		mensagemJSON->u_conteudo.token.jwt = documento["jwt"];
		mensagemJSON->u_conteudo.token.sz = strlen(documento["jwt"]);

		break;

	case MensagensDAF::AUTORIZAR_DFE:

		if (documento["fdf"] == nullptr || documento["hdf"] == nullptr
				|| documento["pdv"] == nullptr || documento["red"] == nullptr)
			return Status::ERRO_CAMPO_INVALIDO;

		if (strlen(documento["fdf"]) > (size_t) ceil(2860 * 4 / 3)
				|| strlen(documento["hdf"]) != 43
				|| strlen(documento["pdv"]) != 10
				|| strlen(documento["red"]) != 43)
			return Status::ERRO_CAMPO_INVALIDO;


		mensagemJSON->u_conteudo.autorizarDFE.fdf = documento["fdf"];
		mensagemJSON->u_conteudo.autorizarDFE.hdf = documento["hdf"];
		mensagemJSON->u_conteudo.autorizarDFE.pdv = documento["pdv"];
		mensagemJSON->u_conteudo.autorizarDFE.red = documento["red"];
		break;


	case MensagensDAF::APAGAR_AUTORIZACAO_RETIDA:

		if (documento["aut"] == nullptr || documento["apg"] == nullptr)
			return Status::ERRO_CAMPO_INVALIDO;
		if (strlen(documento["aut"]) != 43 || strlen(documento["apg"]) != 43)
			return Status::ERRO_CAMPO_INVALIDO;
		mensagemJSON->u_conteudo.apagarAutorizacaoRetida.aut =
				documento["aut"];
		mensagemJSON->u_conteudo.apagarAutorizacaoRetida.apg =
				documento["apg"];
		break;

	case MensagensDAF::ATUALIZAR_CERTIFICADO:

		if (documento["crt"] == nullptr)
			return Status::ERRO_CAMPO_INVALIDO;

		mensagemJSON->u_conteudo.atualizarCertificado.cert = documento["crt"];
		break;

	case DESCARREGAR_RETIDOS:
		if (documento["aut"] == nullptr)
			return Status::ERRO_CAMPO_INVALIDO;

		if (strlen(documento["aut"]) != 43)
			return Status::ERRO_CAMPO_INVALIDO;
		mensagemJSON->u_conteudo.descarregarRetidos.aut = documento["aut"];
		break;

		if (documento["idAut"] == nullptr)
			return Status::ERRO_CAMPO_INVALIDO;

		if (strlen(documento["aut"]) != 43)
			return Status::ERRO_CAMPO_INVALIDO;
		mensagemJSON->u_conteudo.descarregarRetidos.aut = documento["aut"];
		break;

	case MensagensDAF::SOLICITAR_AUTENTICACAO:
	case MensagensDAF::CONSULTAR_INFORMACOES:
	case MensagensDAF::ATUALIZAR_SB:
	case MensagensDAF::CANCELAR_PROCESSO:
	case MensagensDAF::PADRAO_DE_FABRICA:
		break;

	default:
		return Status::ERRO_MENSAGEM_INVALIDA;
	}

	return Status::SUCESSO;
}
