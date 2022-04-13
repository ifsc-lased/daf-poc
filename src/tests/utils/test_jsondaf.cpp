#include "../../utils/json/include/ArduinoJson-v6.17.2.h"
#include "../../utils/json/include/m_struct.h"
#include "../../utils/json/jsondaf.h"
#include "../../unity/unity.h"
#include <iostream>
void setUp(void){};
void tearDown(void){};

void testa_JSON_invalido(void)
{

    MensagemJSON msg;
    char json_invalido[] = "jsoninvalidonaofazsentido";

    enum JSONDAF::Status status = JSONDAF::getMensagemJSON(json_invalido, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::ERRO_JSON_INVALIDO, status);
}

void testa_campo_invalido(void)
{

    MensagemJSON msg;
    char json_str[] = "{\"a\": \"hello\"}";

    enum JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::ERRO_CAMPO_INVALIDO, status);
}

void testa_msg_invalida(void)
{

    MensagemJSON msg;
    char json_str[] = "{\"msg\":15,\"outrocampo\":45.56}";

    enum JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::ERRO_MENSAGEM_INVALIDA, status);
}

void testa_msg_valida_campo_invalido(void)
{

    MensagemJSON msg;
    // a msg é válida mas o campo é inválido
    char json_str[] = "{\"msg\": 2, \"outrocampo\": 45.33}";

    enum JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::ERRO_CAMPO_INVALIDO, status);
}

void testa_msg_registrar(void)
{

    MensagemJSON msg;
    char json_str[] = "{\"msg\":1,\"jwt\":\"abcdefghi\"}";

    enum JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);
    TEST_ASSERT_EQUAL(JSONDAF::SUCESSO, status);
    TEST_ASSERT_EQUAL_INT(MensagensDAF::REGISTRAR, msg.codigo);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abcdefghi", msg.u_conteudo.token.jwt, 9);
}

void testa_msg_confirmar_registro(void)
{

    MensagemJSON msg;
    char json_str[] = "{\"msg\":2,\"jwt\":\"abcdefghi\"}";

    enum JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status);

    TEST_ASSERT_EQUAL_CHAR_ARRAY("abcdefghi", msg.u_conteudo.token.jwt, 9);
    TEST_ASSERT_EQUAL_INT(MensagensDAF::CONFIRMAR_REGISTRO, msg.codigo);
}

void testa_msg_autorizar_dfe(void)
{

    MensagemJSON msg;

    char json_str[] = "{\"msg\":4,\"fdf\":\"frag\",\"hdf\":\"hash\",\"red\":\"resp\"}";
    JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status);

    TEST_ASSERT_EQUAL_CHAR_ARRAY("frag", msg.u_conteudo.autorizarDFE.fdf, 4);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("hash", msg.u_conteudo.autorizarDFE.hdf, 4);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("resp", msg.u_conteudo.autorizarDFE.red, 4);
    TEST_ASSERT_EQUAL_INT(MensagensDAF::AUTORIZAR_DFE, msg.codigo);
}

void testa_msg_apagar_autorizacao_retida(void)
{

    MensagemJSON msg;

    char json_str[] = "{\"msg\":5,\"aut\":\"id\",\"apg\":\"autorizacao\"}";
    JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status);

    TEST_ASSERT_EQUAL_CHAR_ARRAY("id", msg.u_conteudo.apagarAutorizacaoRetida.aut, 2);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("autorizacao", msg.u_conteudo.apagarAutorizacaoRetida.apg, 11);

    TEST_ASSERT_EQUAL_INT(MensagensDAF::APAGAR_AUTORIZACAO_RETIDA, msg.codigo);
}

void testa_msg_remover_registro(void)
{

    MensagemJSON msg;

    char json_str[] = "{\"msg\":6,\"jwt\":\"abcdefghi\"}";
    JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abcdefghi", msg.u_conteudo.token.jwt, 9);
    TEST_ASSERT_EQUAL_INT(MensagensDAF::REMOVER_REGISTRO, msg.codigo);
}

void testa_msg_confirmar_remocao_registro(void)
{

    MensagemJSON msg;

    char json_str[] = "{\"msg\":7,\"jwt\":\"abcdefghi\"}";
    JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abcdefghi", msg.u_conteudo.token.jwt, 9);
    TEST_ASSERT_EQUAL_INT(MensagensDAF::CONFIRMAR_REMOCAO_REGISTRO, msg.codigo);
}

void testa_msg_atualizar_certificado(void)
{

    MensagemJSON msg;

    char json_str[] = "{\"msg\":10,\"cert\":\"abcdefghi\"}";
    JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abcdefghi", msg.u_conteudo.atualizarCertificado.cert, 9);
    TEST_ASSERT_EQUAL_INT(MensagensDAF::ATUALIZAR_CERTIFICADO, msg.codigo);
}

void testa_msg_descarregar_retidos(void)
{

    MensagemJSON msg;

    char json_str[] = "{\"msg\":11,\"idAut\":\"abcdefghi\"}";
    JSONDAF::Status status = JSONDAF::getMensagemJSON(json_str, &msg);

    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abcdefghi", msg.u_conteudo.descarregarRetidos.aut, 9);
    TEST_ASSERT_EQUAL_INT(MensagensDAF::DESCARREGAR_RETIDOS, msg.codigo);
}

void testa_msg_sem_parametro(void)
{

    MensagemJSON solicita_autenticacao;
    MensagemJSON consultar_informacao;
    MensagemJSON atualizar_sb;
    MensagemJSON cancelar_processo;

    char msg1[] = "{\"msg\":3}";
    char msg2[] = "{\"msg\":8}";
    char msg3[] = "{\"msg\":9}";
    char msg4[] = "{\"msg\":12}";

    JSONDAF::Status status1 = JSONDAF::getMensagemJSON(msg1, &solicita_autenticacao);
    JSONDAF::Status status2 = JSONDAF::getMensagemJSON(msg2, &consultar_informacao);
    JSONDAF::Status status3 = JSONDAF::getMensagemJSON(msg3, &atualizar_sb);
    JSONDAF::Status status4 = JSONDAF::getMensagemJSON(msg4, &cancelar_processo);

    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status1);
    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status2);
    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status3);
    TEST_ASSERT_EQUAL(JSONDAF::Status::SUCESSO, status4);
}


int main()
{
    UNITY_BEGIN();

    RUN_TEST(testa_JSON_invalido);
    RUN_TEST(testa_campo_invalido);
    RUN_TEST(testa_msg_invalida);
    RUN_TEST(testa_msg_valida_campo_invalido);
    RUN_TEST(testa_msg_confirmar_registro);
    RUN_TEST(testa_msg_registrar);
    RUN_TEST(testa_msg_autorizar_dfe);
    RUN_TEST(testa_msg_apagar_autorizacao_retida);
    RUN_TEST(testa_msg_remover_registro);
    RUN_TEST(testa_msg_remover_registro);
    RUN_TEST(testa_msg_atualizar_certificado);
    RUN_TEST(testa_msg_descarregar_retidos);
    RUN_TEST(testa_msg_sem_parametro);

    return UNITY_END();
}
