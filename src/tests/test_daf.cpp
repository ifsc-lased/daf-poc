// comando p/ compilar e executar o teste, estando no mesmo diretório que test_daf.cpp:
// g++ ../unity/unity.c test_daf.cpp ../daf.cpp && ./a.out

#define private public

#include "../unity/unity.h"
#include <iostream>
#include "../daf.h"
#include "../pdafcdc_utils.h"
#include "../daf_utils.h"
#include "../memoria_segura.h"
#include "../debug.h"
#include "../utils/jwt/jwt.h"
#include "../memoria/memoria_de_trabalho.h"
#include "../memoria/alocador_de_memoria.h"

using namespace std;
void setUp(void) {
}
;
void tearDown(void) {
}
;

// sef priv d = 0x5ede69eefa64cfea2806edac724f959fb3b93f35a59a9be6c838cf48ba0539f456aac9d5dd2f867674f37cdd378508d3

static uint8_t x_sef[] = { 0x9c, 0xbe, 0xe0, 0x8e, 0x2a, 0x66, 0xbf, 0xcf, 0xcf,
		0xc5, 0xe4, 0x0a, 0x3b, 0xee, 0x93, 0xcd, 0x30, 0xc8, 0x5b, 0xb7, 0xf5,
		0xff, 0xcd, 0x68, 0xe8, 0x7c, 0x85, 0xb1, 0x78, 0x24, 0x44, 0xf0, 0x6a,
		0xb8, 0xe5, 0xb4, 0xf8, 0x18, 0x8f, 0x41, 0xbd, 0xf1, 0x31, 0x8d, 0x26,
		0x62, 0x26, 0xa3

};

static uint8_t y_sef[] = { 0xda, 0xac, 0x4b, 0x0b, 0x15, 0xe4, 0x1e, 0x2f, 0x03,
		0x66, 0xc1, 0x9a, 0xf9, 0x42, 0x3e, 0xb8, 0x2b, 0x28, 0x70, 0x68, 0x85,
		0xbc, 0xbb, 0x54, 0x7c, 0x4a, 0x5f, 0x0a, 0xab, 0xf9, 0x1f, 0x8d, 0xe4,
		0x01, 0xe9, 0x4a, 0xeb, 0x1b, 0x6f, 0xe1, 0xd1, 0xc5, 0xf4, 0xc4, 0x4b,
		0x70, 0xd7, 0x35 };

static const uint8_t token_registrar[] =
		"eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzM4NCIsImp3ayI6eyJjcnYiOiJQLTM4NCIsIngiOiJuTDdnamlwbXY4X1B4ZVFLTy02VHpURElXN2YxXzgxbzZIeUZzWGdrUlBCcXVPVzAtQmlQUWIzeE1ZMG1ZaWFqIiwieSI6IjJxeExDeFhrSGk4RFpzR2EtVUktdUNzb2NHaUZ2THRVZkVwZkNxdjVINDNrQWVsSzZ4dHY0ZEhGOU1STGNOYzEiLCJrdHkiOiJFQyJ9fQ.eyJubmMiOiJNVEl6TkRVMk56ZzVNREV5TXpRMU5nIn0.GYHmbNXKDpMaXRvcWUYxkpeQY-c80Pkx8zMlzu4PNIyWm7Z3B4wEtNUsCe9dhic0QSu03IFHbTCNRB51SEqrpQd7cy-259L1rpGWy-Dljl2qC1Z2YM4x1Z9esscOs3jH";

static const char token_confirmarRegistro[] =
		"eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzM4NCIsImp3ayI6eyJjcnYiOiJQLTM4NCIsIngiOiJuTDdnamlwbXY4X1B4ZVFLTy02VHpURElXN2YxXzgxbzZIeUZzWGdrUlBCcXVPVzAtQmlQUWIzeE1ZMG1ZaWFqIiwieSI6IjJxeExDeFhrSGk4RFpzR2EtVUktdUNzb2NHaUZ2THRVZkVwZkNxdjVINDNrQWVsSzZ4dHY0ZEhGOU1STGNOYzEiLCJrdHkiOiJFQyJ9fQ.eyJjaHMiOiJoZWFkZXIuandlZW5jcnlwdGVka2V5Lml2LlNJN0JSV2pXTjY2R016VWkwZkthM0RzRTZoNzM5bXNwX1piNVlwUW8yd1BBUDJySEk4alMtemJKbXdLLVl4SDlpd0xmQ0pybFpNRFVnLU9kZU5HdFZ3LnRhZyIsImNocCI6Im9RdENTcFNhTkdvY0I0YU8yR3E0WjdOUE5OQ2RSSXRMNy1Iem5kOE1JQTBBSVVjbkJjbzZILTZ2SlVtNTQ5cGlnWXViMFAyVWwzUXdFMTJjcjd2a0hnIiwibW9wIjowfQ.yiS6Le_0krR2m8OeF7nab-6g95W0z9pOWIYF7C0BdIjDXlmfzlKvg5qihjqUynev2VvPtmMLNrvqsVPtZGpEe3uETNOFn_wlXMjyl09Rmydf_jq0gOrGchjXNPsRrMaT";


static const char token_confirmarRemocaoRegistro[] =
		"eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzM4NCIsImp3ayI6eyJjcnYiOiJQLTM4NCIsIngiOiJuTDdnamlwbXY4X1B4ZVFLTy02VHpURElXN2YxXzgxbzZIeUZzWGdrUlBCcXVPVzAtQmlQUWIzeE1ZMG1ZaWFqIiwieSI6IjJxeExDeFhrSGk4RFpzR2EtVUktdUNzb2NHaUZ2THRVZkVwZkNxdjVINDNrQWVsSzZ4dHY0ZEhGOU1STGNOYzEiLCJrdHkiOiJFQyJ9fQ.eyJldm4iOiJSRU1PVkVSIn0.Hq_60QjPdzgt8Bn3Eb-ZKh8wSuk2nMF8xndZr3KnAzB7Wf9GIs14m3d5agWuvzYypzffIIkSSedd3x5tgRzZVcvxNfcmueWA1VEUWLnurOEyhdOPAwM3SwGwFjE5E_s0";

char autorizacao_b64[44];

void test_UCRegistrar(void) {
	/**
	 * primeiro preparar-se-a o cenario.
	 */

	/**
	 *  gera chaves de ateste. Essa chave deve ser inserida em tempo de manufatura no DAF.
	 *  Vamos simular essa insercao em tempo de manufatura ao inserir as chaves agora.
	 */
	chave_privada_ECC_P384_t priv_ateste;
	chave_publica_ECC_P384_t pub_ateste;
	CriptoDAF::gera_chave_ECC(&pub_ateste, &priv_ateste);
	MemoriaSegura::escrita_chave_ateste_priv(priv_ateste);
	MemoriaSegura::escrita_chave_ateste_pub(pub_ateste);

	/** O token JWT eh assinado com a chave privada da SEF
	 * correspondente a chave publica contida no certificado
	 * digital da SEF inserido no DAF
	 */
	chave_publica_ECC_P384_t chave_publica_SEF_certificado;
	memcpy((void*) chave_publica_SEF_certificado.x, (const void*) x_sef, 48);
	memcpy((void*) chave_publica_SEF_certificado.y, (const void*) y_sef, 48);
	MemoriaSegura::escrita_chave_do_certificado(chave_publica_SEF_certificado);

	// msg com token jwt
	static StaticJsonDocument<sizeof(token_registrar) + 50> msg_json_obj;
	msg_json_obj["msg"] = MensagensDAF::REGISTRAR;
	msg_json_obj["jwt"] = token_registrar;

	static char msg[sizeof(token_registrar) + 50];
	int retval = JSON::serialize(msg_json_obj, msg, sizeof(msg));
	TEST_ASSERT_GREATER_THAN(0, retval);

	// zera contador monotonico (verificar na resposta da msg)
	MemoriaSegura::escrita_contador(0);

	// seta identificador unico do DAF (verificar na resposta da msg)
	MemoriaSegura::uuid_t idDAF = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 15 };
	MemoriaSegura::escrita_idDAF(idDAF);

	//  seta versao do SB
	MemoriaSegura::parametros_atualizacao_t params;
	params.versao_SB = 1;
	MemoriaSegura::escrita_parametros_atualizacao(params);

	// seta modelo
	MemoriaSegura::modelo_DAF_t modelo;
	memcpy(modelo.modelo, "modelo123", sizeof("modelo123"));
	MemoriaSegura::escrita_modelo_DAF(modelo);

	// seta CNPJ
	MemoriaSegura::CNPJ_t cnpj;
	memcpy(cnpj.numeros, "12345678901234", 15);
	MemoriaSegura::escrita_CNPJ(cnpj);

	// seta estado
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::ESTADO,
			(uint32_t) DAF::ESTADO_t::INATIVO);

	/**
	 * cenario montado, agora testar-se-a de fato o processo de registro
	 */

	static StaticJsonDocument<MAX_LENGTH_JSON> resposta;
	DAF d;

	S_JWT token;
	token.jwt = (const char*) token_registrar;
	token.sz = strlen((const char*) token_registrar);

	uint32_t t1 = PDAFCDC_Utils::get_time_as_ms();
	DAF::STATUS_PEDIDO_t processo = d.processaRegistrar(token, resposta);
	uint32_t t2 = PDAFCDC_Utils::get_time_as_ms();
	DEBUG_PRINTF("\nProcesso registrar, tempo: %i ms\n", t2 - t1);

	// verificando sucesso do pedido
	TEST_ASSERT_EQUAL(DAF::STATUS_PEDIDO_t::SUCESSO, processo);
	TEST_ASSERT_EQUAL(Resposta_t::SUCESSO, resposta["res"]);
	TEST_ASSERT_NOT_NULL(resposta["jwt"]);

	uint32_t emProcesso = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::EM_PROCESSO);
	TEST_ASSERT_EQUAL_INT(1, emProcesso);

	/**
	 * verifica os dois tokens retornados na resposta
	 */

	chave_publica_ECC_P256_t chave_pub_daf = MemoriaSegura::leitura_chave_pub();
	chave_publica_ECC_P384_t chave_ateste_pub_daf =
			MemoriaSegura::leitura_chave_ateste_pub();

	static uint8_t payload_jwt[JWT_PAYLOAD_MAX_SZ];
	static uint8_t payload_res[100];

	retval = JWT::verificaTokenES384(resposta["jwt"], strlen(resposta["jwt"]),
			&chave_ateste_pub_daf, payload_jwt, sizeof(payload_jwt));
	TEST_ASSERT_GREATER_OR_EQUAL(1, retval);

	retval = JWT::verificaTokenES256((const char*) payload_jwt,
			strlen((const char*) payload_jwt), &chave_pub_daf, payload_res,
			sizeof(payload_res));
	TEST_ASSERT_GREATER_OR_EQUAL(1, retval);

	resposta.clear();
	JSON::deserialize((char*) payload_res, resposta);
	TEST_ASSERT_EQUAL_INT(22, strlen(resposta["daf"]));
	// verifica cnt
	TEST_ASSERT_EQUAL_INT(MemoriaSegura::leitura_contador(), resposta["cnt"]);
	TEST_ASSERT_EQUAL_INT(22, strlen(resposta["nnc"]));

	/**
	 * testar-se-a agora o confirmar registro
	 */
	resposta.clear();

	/*
	 Chave SEF em base64URL: SI7BRWjWN66GMzUi0fKa3DsE6h739msp_Zb5YpQo2wPAP2rHI8jS-zbJmwK-YxH9iwLfCJrlZMDUg-OdeNGtVw
	 Chave PAF em base64URL: oQtCSpSaNGocB4aO2Gq4Z7NPNNCdRItL7-Hznd8MIA0AIUcnBco6H-6vJUm549pigYub0P2Ul3QwE12cr7vkHg
	 */

	/**
	 * por enquanto a chave sef esta sendo enviada em claro no campo 'ciphertext' do token JWE
	 */
	token.jwt = (char*) token_confirmarRegistro;
	token.sz = strlen((const char*) token_confirmarRegistro);
	d.processaConfirmarRegistro(token, resposta);

	// verificando sucesso do pedido
	TEST_ASSERT_EQUAL(Resposta_t::SUCESSO, resposta["res"]);

	// em processo
	TEST_ASSERT_EQUAL_INT(0,
			MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO));
	// guarda
	TEST_ASSERT_EQUAL_INT(DAF::ESTADO_t::PRONTO,
			MemoriaSegura::leitura_guarda(DAF_Utils::ESTADO));
	// regok
	TEST_ASSERT_EQUAL_INT(1,
			MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::REGOK));
}

void test_UCModoDeOperacao(void) {
	/**
	 * preparacao do teste
	 */
	MemoriaSegura::chave_SEF_t chaveSEF = MemoriaSegura::leitura_chave_SEF();

	// gera payload do jwt

	// nnc: 1234567890123456 (raw)
	char jwt_payload[] = "{\"nnc\":\"MTIzNDU2Nzg5MDEyMzQ1Ng\"}";
	char jwt[200];
	JWT::geraTokenHS256((uint8_t*) jwt_payload, strlen(jwt_payload),
			chaveSEF.chave, sizeof(chaveSEF.chave), jwt, sizeof(jwt));

	// estado pronto
	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::ESTADO,
			DAF::ESTADO_t::PRONTO);

	// modo de operacao
	MemoriaSegura::escrita_modo_de_operacao(0);

	// limpa autorizacoes
	MemoriaDeTrabalho mt;
	mt.reseta();

	/**
	 * teste da funcao processaAlterarModoOperacao()
	 */

	static json_t resposta;
	S_JWT token;
	token.jwt = jwt;
	token.sz = strlen(jwt);

	DAF d;
	DAF::STATUS_PEDIDO_t status = d.processaAlterarModoOperacao(token,
			resposta);
	TEST_ASSERT_EQUAL_INT(DAF::STATUS_PEDIDO_t::SUCESSO, status);

	// verifica resposta
	TEST_ASSERT_EQUAL_INT(Resposta_t::SUCESSO, resposta["res"]);

	const char *resposta_jwt = resposta["jwt"];
	char payload_res[200];
	int retval = JWT::verificaTokenHS256(resposta_jwt, strlen(resposta_jwt),
			chaveSEF.chave, sizeof(chaveSEF.chave), (uint8_t*) payload_res,
			sizeof(payload_res));
	TEST_ASSERT_GREATER_OR_EQUAL(1, retval);
	resposta.clear();
	JSON::deserialize(payload_res, resposta);

	const char *nnc = resposta["nnc"];
	const char *daf = resposta["daf"];
	uint32_t cnt = resposta["cnt"];

	TEST_ASSERT_EQUAL_INT(22, strlen(daf));
	TEST_ASSERT_EQUAL_INT(22, strlen(nnc));
	TEST_ASSERT_EQUAL_INT(0, memcmp("MTIzNDU2Nzg5MDEyMzQ1Ng", nnc, 22));
//	TEST_ASSERT_EACH_EQUAL_CHAR("MTIzNDU2Nzg5MDEyMzQ1Ng", nnc, 22);
	TEST_ASSERT_EQUAL_INT(MemoriaSegura::leitura_contador(), cnt);

	/**
	 *  teste da processaConfirmarAlterarModoOperacao()
	 */

	char jwt_payload_2[] = "{\"mop\": 1}";
	JWT::geraTokenHS256((uint8_t*) jwt_payload_2, strlen(jwt_payload_2),
			chaveSEF.chave, sizeof(chaveSEF.chave), jwt, sizeof(jwt));

	resposta.clear();
	token.jwt = jwt;
	token.sz = strlen(jwt);
	status = d.processaConfirmarAlterarModoOperacao(token, resposta);
	TEST_ASSERT_EQUAL_INT(DAF::STATUS_PEDIDO_t::SUCESSO, status);
	TEST_ASSERT_EQUAL_INT(Resposta_t::SUCESSO, resposta["res"]);
	TEST_ASSERT_EQUAL_INT(1, MemoriaSegura::leitura_modo_de_operacao());
}

void test_UCAutorizarDFE(void) {

	DAF d;
	static json_t resposta;
	DAF::STATUS_PEDIDO_t status = d.processaSolicitarAutenticacao(resposta);

	TEST_ASSERT_EQUAL(DAF::STATUS_PEDIDO_t::SUCESSO, status);
	TEST_ASSERT_EQUAL(Resposta_t::SUCESSO, resposta["res"]);
	const char *nnc = resposta["nnc"];
	TEST_ASSERT_EQUAL_INT(22, strlen((const char* ) nnc));

	/** Prepara mensagem 4*/
	resposta.clear();
	MemoriaSegura::chave_PAF_t chavePAF = MemoriaSegura::leitura_chave_PAF();
	uint8_t to_be_digested[100];

	uint8_t nonce[16];
	uint8_t resumo[32];
	Base64URL::b64url_decode(
			(const char*) "rewEdBv2pCsj2bKvtD-kbeRqRgnRdgTV1Na25kbDF2I", 43,
			resumo, 32);
	Base64URL::b64url_decode(nnc, strlen(nnc), nonce, 16);
	memcpy(to_be_digested, nonce, 16);
	memcpy(&to_be_digested[16], resumo, 32);

	uint8_t resposta_desafio[32];
	CriptoDAF::gera_HMAC_SHA256(resposta_desafio, to_be_digested, 48,
			chavePAF.chave, sizeof(chavePAF.chave));

	uint8_t resp_desb64[43];
	Base64URL::b64url_encode(resposta_desafio, 32, (char*) resp_desb64, 43);

	// salva contador monotonico e numDFE antes de autorizar para
	// poder comparar depois da operacao
	uint32_t contadorMonotonico = MemoriaSegura::leitura_contador();
	uint32_t numDFE = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::NUM_DFE);

	S_AUTORIZARDFE msg;

	msg.fdf = "eG1sY29tcGxldG8"; // "xmlcompleto"
	msg.hdf = "rewEdBv2pCsj2bKvtD-kbeRqRgnRdgTV1Na25kbDF2I";
	msg.pdv = "pdv10chars";
	msg.red = (const char*) resp_desb64;

	uint32_t t1 = PDAFCDC_Utils::get_time_as_ms();
	status = d.processaAutorizarDFE(msg, resposta);
	uint32_t t2 = PDAFCDC_Utils::get_time_as_ms();
	DEBUG_PRINTF("\nProcesso autorizarDFE, tempo: %u ms\n", t2 - t1);

	TEST_ASSERT_EQUAL(DAF::STATUS_PEDIDO_t::SUCESSO, status);
	TEST_ASSERT_EQUAL(Resposta_t::SUCESSO, resposta["res"]);

	static uint8_t payload_res[500];
	MemoriaSegura::chave_SEF_t chaveSEF = MemoriaSegura::leitura_chave_SEF();
	int retval = JWT::verificaTokenHS256(resposta["jwt"],
			strlen(resposta["jwt"]), chaveSEF.chave, sizeof(chaveSEF.chave),
			(uint8_t*) payload_res, sizeof(payload_res));
	TEST_ASSERT_GREATER_OR_EQUAL(1, retval);

	resposta.clear();
	JSON::deserialize((char*) payload_res, resposta);
	// verificada len iddaf
	TEST_ASSERT_EQUAL_INT(22, strlen(resposta["daf"]));

	// verifica vsb
	TEST_ASSERT_LESS_OR_EQUAL(255, resposta["vsb"]);
	TEST_ASSERT_GREATER_OR_EQUAL(1, resposta["vsb"]);

	// verifica mop
	TEST_ASSERT_LESS_OR_EQUAL(1, resposta["mop"]);
	TEST_ASSERT_GREATER_OR_EQUAL(0, resposta["mop"]);

	// verifica id pdv
	const char *pdv = resposta["pdv"];
	TEST_ASSERT_EQUAL_CHAR_ARRAY("pdv10chars", pdv, 10);

	// verifica contador monotonico e numDFE
	uint32_t cnt = resposta["cnt"];
	TEST_ASSERT_EQUAL_INT(contadorMonotonico + 1, cnt);
	TEST_ASSERT_EQUAL_INT(numDFE + 1,
			MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::NUM_DFE));

	// verifica len idaut
	TEST_ASSERT_EQUAL_INT(43, strlen(resposta["aut"]));

	const char *aut = resposta["aut"];
	memcpy(autorizacao_b64, aut, sizeof(autorizacao_b64));

	// por ultimo, verifica se foi add na MT mesmo
	MemoriaDeTrabalho mt;
	TEST_ASSERT_EQUAL(1, mt.get_numero_de_autorizacoes_DFE());

	MemoriaDeTrabalho::id_autorizacao_t idAut;
	memcpy(idAut.base64, autorizacao_b64, sizeof(idAut.base64));

	TEST_ASSERT_TRUE(mt.existe_autorizacao_DFE(idAut));
}

void test_UCAutorizarDFE_tamanho_maximo(void) {

	DAF d;
	static json_t resposta;
	DAF::STATUS_PEDIDO_t status = d.processaSolicitarAutenticacao(resposta);

	TEST_ASSERT_EQUAL(DAF::STATUS_PEDIDO_t::SUCESSO, status);
	TEST_ASSERT_EQUAL(Resposta_t::SUCESSO, resposta["res"]);
	const char *nnc = resposta["nnc"];
	TEST_ASSERT_EQUAL_INT(22, strlen((const char* ) nnc));

	/** Prepara mensagem 4*/
	resposta.clear();
	MemoriaSegura::chave_PAF_t chavePAF = MemoriaSegura::leitura_chave_PAF();
	uint8_t to_be_digested[100];

	uint8_t nonce[16];
	uint8_t resumo[32];
	Base64URL::b64url_decode(
			(const char*) "rewEdBv2pCsj2bKvtD-kbeRqRgnRdgTV1Na25kbDF2I", 43,
			resumo, 32);
	Base64URL::b64url_decode(nnc, strlen(nnc), nonce, 16);
	memcpy(to_be_digested, nonce, 16);
	memcpy(&to_be_digested[16], resumo, 32);

	uint8_t resposta_desafio[32];
	CriptoDAF::gera_HMAC_SHA256(resposta_desafio, to_be_digested, 48,
			chavePAF.chave, sizeof(chavePAF.chave));

	uint8_t resp_desb64[43];
	Base64URL::b64url_encode(resposta_desafio, 32, (char*) resp_desb64, 43);

	// salva contador monotonico e numDFE antes de autorizar para
	// poder comparar depois da operacao
	uint32_t contadorMonotonico = MemoriaSegura::leitura_contador();
	uint32_t numDFE = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::NUM_DFE);

	S_AUTORIZARDFE msg;

	static MemoriaDeTrabalho::fragmento_DFE_t frag;
	memset(&frag.buf, 0xAB, sizeof(frag.buf));
	static char frag_b64[5000];
	Base64URL::b64url_encode((uint8_t*) &frag.buf, sizeof(frag.buf), frag_b64,
			sizeof(frag_b64));

	msg.fdf = frag_b64;
	msg.hdf = "rewEdBv2pCsj2bKvtD-kbeRqRgnRdgTV1Na25kbDF2I";
	msg.pdv = "pdv10chars";
	msg.red = (const char*) resp_desb64;


	uint32_t t1 = PDAFCDC_Utils::get_time_as_ms();
	status = d.processaAutorizarDFE(msg, resposta);
	uint32_t t2 = PDAFCDC_Utils::get_time_as_ms();
	DEBUG_PRINTF("\nProcesso autorizarDFE (max), tempo: %i ms\n", t2 - t1);

	TEST_ASSERT_EQUAL(DAF::STATUS_PEDIDO_t::SUCESSO, status);
	TEST_ASSERT_EQUAL(Resposta_t::SUCESSO, resposta["res"]);

	static uint8_t payload_res[500];
	MemoriaSegura::chave_SEF_t chaveSEF = MemoriaSegura::leitura_chave_SEF();
	int retval = JWT::verificaTokenHS256(resposta["jwt"],
			strlen(resposta["jwt"]), chaveSEF.chave, sizeof(chaveSEF.chave),
			(uint8_t*) payload_res, sizeof(payload_res));
	TEST_ASSERT_GREATER_OR_EQUAL(1, retval);

	resposta.clear();
	JSON::deserialize((char*) payload_res, resposta);
	// verificada len iddaf
	TEST_ASSERT_EQUAL_INT(22, strlen(resposta["daf"]));

	// verifica vsb
	TEST_ASSERT_LESS_OR_EQUAL(255, resposta["vsb"]);
	TEST_ASSERT_GREATER_OR_EQUAL(1, resposta["vsb"]);

	// verifica mop
	TEST_ASSERT_LESS_OR_EQUAL(1, resposta["mop"]);
	TEST_ASSERT_GREATER_OR_EQUAL(0, resposta["mop"]);

	// verifica id pdv
	const char *pdv = resposta["pdv"];
	TEST_ASSERT_EQUAL_CHAR_ARRAY("pdv10chars", pdv, 10);

	// verifica contador monotonico e numDFE
	uint32_t cnt = resposta["cnt"];
	TEST_ASSERT_EQUAL_INT(contadorMonotonico + 1, cnt);
	TEST_ASSERT_EQUAL_INT(numDFE + 1,
			MemoriaSegura::leitura_guarda(DAF_Utils::GUARDA_t::NUM_DFE));

	// verifica len idaut
	TEST_ASSERT_EQUAL_INT(43, strlen(resposta["aut"]));
}

void test_UCDescarregarRetidos(void) {
	/* prepara o cenario */

	S_DESCARREGARETIDOS msg = { nullptr, 0 };

	msg.aut = autorizacao_b64;
	msg.sz = 43;
	DAF d;
	static json_t resposta;

	/* inicio do teste */

	DAF::STATUS_PEDIDO_t status = d.processaDescarregarRetidos(msg, resposta);

	TEST_ASSERT_EQUAL(DAF::STATUS_PEDIDO_t::SUCESSO, status);
	TEST_ASSERT_EQUAL(Resposta_t::SUCESSO, resposta["res"]);

	const char *aut = resposta["aut"];
	TEST_ASSERT_NOT_NULL(aut);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(msg.aut, aut, msg.sz);

	const char *fdf = resposta["fdf"];
	TEST_ASSERT_NOT_NULL(fdf);
	TEST_ASSERT_EQUAL_CHAR_ARRAY("eG1sY29tcGxldG8", fdf, 11); // "xmlcompleto"
}

void test_UCApagarAutorizacaoRetida(void) {

	/* preparacao do cenário */

	uint8_t apg_raw[32];
	char apg_b64[44];
	MemoriaSegura::chave_SEF_t chaveSEF = MemoriaSegura::leitura_chave_SEF();
	int retval = CriptoDAF::gera_HMAC_SHA256(apg_raw,
			(uint8_t*) autorizacao_b64, 43, chaveSEF.chave,
			sizeof(chaveSEF.chave));
	TEST_ASSERT_EQUAL_INT(0, retval);

	Base64URL::b64url_encode(apg_raw, sizeof(apg_raw), apg_b64,
			sizeof(apg_b64));

	S_APAGARRETIDA msg;
	msg.aut = autorizacao_b64;
	msg.apg = apg_b64;

	static StaticJsonDocument<MAX_LENGTH_JSON> resposta;
	/* inicio do teste */

	DAF d;
	uint32_t t1 = PDAFCDC_Utils::get_time_as_ms();
	DAF::STATUS_PEDIDO_t status = d.processaApagarAutorizacaoRetida(msg,
			resposta);
	uint32_t t2 = PDAFCDC_Utils::get_time_as_ms();
	DEBUG_PRINTF("\nProcesso apagarAutorizacaoRetida, tempo: %u ms\n", t2 - t1);

	TEST_ASSERT_EQUAL_INT(DAF::STATUS_PEDIDO_t::SUCESSO, status);
	TEST_ASSERT_EQUAL_INT(Resposta_t::SUCESSO, resposta["res"]);
}

void test_UCRemoverRegistro(void) {

	/**
	 * primeiro preparar-se-a o cenario.
	 *
	 * O token JWT e assinado com a chave privada da SEF
	 * correspondente a chave publica contida no certificado
	 * digital da SEF inserido no DAF
	 */

	chave_publica_ECC_P384_t chave_publica_SEF_certificado;

	memcpy((void*) chave_publica_SEF_certificado.x, (const void*) x_sef, 48);
	memcpy((void*) chave_publica_SEF_certificado.y, (const void*) y_sef, 48);
	MemoriaSegura::escrita_chave_do_certificado(chave_publica_SEF_certificado);

	static StaticJsonDocument<MAX_LENGTH_JSON> resposta;
	DAF d;

	S_JWT token;
	token.jwt = (const char*) token_registrar;
	token.sz = strlen((const char*) token_registrar);
	d.processaRemoverRegistro(token, resposta);

	// verificando sucesso do pedido

	TEST_ASSERT_EQUAL(Resposta_t::SUCESSO, resposta["res"]);
	TEST_ASSERT_NOT_NULL(resposta["jwt"]);
	uint32_t emProcesso = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::EM_PROCESSO);
	TEST_ASSERT_EQUAL_INT(1, emProcesso);

	chave_publica_ECC_P256_t chave_pub_daf = MemoriaSegura::leitura_chave_pub();
	uint8_t payload_res[100];

	int retval = JWT::verificaTokenES256((const char*) resposta["jwt"],
			strlen((const char*) resposta["jwt"]), &chave_pub_daf, payload_res,
			sizeof(payload_res));
	TEST_ASSERT_GREATER_OR_EQUAL(1, retval);
	resposta.clear();
	JSON::deserialize((char*) payload_res, resposta);

	TEST_ASSERT_EQUAL_INT(22, strlen(resposta["daf"]));

	// verifica cnt
	TEST_ASSERT_EQUAL_INT(MemoriaSegura::leitura_contador(), resposta["cnt"]);
	// verifica nnc
	const char *nnc = resposta["nnc"];
	TEST_ASSERT_EQUAL_CHAR_ARRAY("MTIzNDU2Nzg5MDEyMzQ1Ng", nnc, 22);

	/* Prepara confirmar remocao registro */

	S_JWT msg;

	msg.jwt = token_confirmarRemocaoRegistro;
	msg.sz = strlen(token_confirmarRemocaoRegistro);

	resposta.clear();

	d.processaConfirmarRemocaoRegistro(msg, resposta);

	TEST_ASSERT_EQUAL_INT(Resposta_t::SUCESSO, resposta["res"]);
	emProcesso = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::EM_PROCESSO);
	TEST_ASSERT_EQUAL_INT(0, emProcesso);

	DAF::ESTADO_t estado = (DAF::ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::ESTADO);
	TEST_ASSERT_EQUAL_INT(DAF::ESTADO_t::INATIVO, estado);
}

void test_UCConsultarInformacoes(void) {

	static json_t resposta;
	DAF d;
	d.processaConsultarInformacoes(resposta);
	TEST_ASSERT_EQUAL(Resposta_t::SUCESSO, resposta["res"]);

	MemoriaSegura::uuid_t iddaf = MemoriaSegura::leitura_idDAF();

	// testa campo daf
	const char *daf = resposta["daf"];
	TEST_ASSERT_EQUAL_INT(22, strlen(daf));
	uint8_t iddaf_b64[22];
	Base64URL::b64url_encode(iddaf.valor, 16, (char*) iddaf_b64, 22);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(iddaf_b64, daf, 22);

	// testa campo mop
	uint32_t mop = MemoriaSegura::leitura_modo_de_operacao();
	TEST_ASSERT_EQUAL_INT(mop, resposta["mop"]);
	TEST_ASSERT_GREATER_OR_EQUAL(0, resposta["mop"]);
	TEST_ASSERT_LESS_OR_EQUAL(1, resposta["mop"]);

	// testa campo vsb
	TEST_ASSERT_LESS_OR_EQUAL(255, resposta["vsb"]);

	// testa campo hsb
	const char *hsb = resposta["hsb"];
	TEST_ASSERT_EQUAL_INT(43, strlen(hsb));

	// testa campo fab
	const char *fab = resposta["fab"];
	TEST_ASSERT_EQUAL_INT(14, strlen(fab));

	// testa campo mdl
	const char *mdl = resposta["mdl"];
	MemoriaSegura::modelo_DAF_t modelo = MemoriaSegura::leitura_modelo_DAF();
	TEST_ASSERT_EQUAL_CHAR_ARRAY(modelo.modelo, mdl, strlen(modelo.modelo));
	TEST_ASSERT_GREATER_OR_EQUAL(1, strlen(mdl));
	TEST_ASSERT_LESS_OR_EQUAL(20, strlen(mdl));

	// testa campo cnt
	TEST_ASSERT_NOT_NULL(resposta["cnt"]);
	uint32_t cnt = MemoriaSegura::leitura_contador();
	TEST_ASSERT_EQUAL_INT(cnt, resposta["cnt"]);

	// testa campo est
	const char *est = resposta["est"];
	DAF::ESTADO_t estado = (DAF::ESTADO_t) MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::ESTADO);

	if (estado == DAF::ESTADO_t::INATIVO)
		TEST_ASSERT_EQUAL_CHAR_ARRAY("INATIVO", est, 7);
	else if (estado == DAF::ESTADO_t::PRONTO)
		TEST_ASSERT_EQUAL_CHAR_ARRAY("PRONTO", est, 6);
	else if (estado == DAF::ESTADO_t::BLOQUEADO)
		TEST_ASSERT_EQUAL_CHAR_ARRAY("BLOQUEADO", est, 9);
	else
		TEST_ASSERT_MESSAGE(0 == 1,
				"Estado invalido: nao eh INATIVO, PRONTO ou BLOQUEADO.\n");


}

void test_UCCancelarProcesso(void) {

	/* preparando o teste */
	DAF d;

	json_t resposta;

	MemoriaSegura::escrita_guarda(DAF_Utils::GUARDA_t::EM_PROCESSO, 1);

	// verificando sucesso do pedido
	uint32_t emProcesso = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::EM_PROCESSO);
	TEST_ASSERT_EQUAL_INT(1, emProcesso);

	d.processaCancelarProcesso(resposta);

	TEST_ASSERT_EQUAL_INT(Resposta_t::SUCESSO, resposta["res"]);
	emProcesso = MemoriaSegura::leitura_guarda(
			DAF_Utils::GUARDA_t::EM_PROCESSO);
	TEST_ASSERT_EQUAL_INT(0, emProcesso);
}

void test_UCAtualizarSB(void) {

	/* preparando o cenario */

	/* assumindo que o daf nao tem autorizacao retida */

	DAF d;
	static json_t resposta;

	d.processaAtualizarSB(resposta);
	TEST_ASSERT_EQUAL_INT(Resposta_t::SUCESSO, resposta["res"]);
}



int main() {
	UNITY_BEGIN();

	DEBUG_PRINTF_INIT();

	CriptoDAF::configura();

	DEBUG_PRINTF("\nIniciando testes DAF Core\n");

	RUN_TEST(test_UCRegistrar);
	RUN_TEST(test_UCModoDeOperacao);
	RUN_TEST(test_UCAutorizarDFE);
//	RUN_TEST(test_UCAutorizarDFE_tamanho_maximo);
	RUN_TEST(test_UCDescarregarRetidos);
	RUN_TEST(test_UCConsultarInformacoes);
	RUN_TEST(test_UCApagarAutorizacaoRetida);
	RUN_TEST(test_UCRemoverRegistro);

	RUN_TEST(test_UCAtualizarSB);
	RUN_TEST(test_UCCancelarProcesso);

	return UNITY_END();
}
