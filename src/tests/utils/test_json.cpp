// comando p/ compilar e executar o teste, estando no mesmo diretório que teste_json.cpp:
// g++ ../../unity/unity.c teste_json.cpp json.cpp && ./a.out

#include "../../utils/json/include/ArduinoJson-v6.17.2.h"
#include "../../utils/json/include/m_struct.h"
#include "../../utils/json/json.h"
#include "../../unity/unity.h"
#include <iostream>
void setUp(void){};
void tearDown(void){};

void testa_serialize(void)
{
    static StaticJsonDocument<1000> doc;
    uint8_t d[] = {0x64, 0x6f, 0x63, 0x75, 0x6d, 0x65, 0x6e, 0x74, 0x6f, 0x4a, 0x57, 0x54};
    doc["msg"] = 1.0;
    doc["jwt"] = d;

    
    static char serialized[1000];
    size_t ret = JSON::serialize(doc, serialized, sizeof(serialized));
    uint8_t expected[] = "{\"msg\":1,\"jwt\":\"documentoJWT\"}";
    
    TEST_ASSERT_EQUAL(30, ret);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, serialized, 28);

    StaticJsonDocument<1000> doc2;
    
    static char doc2ser[1000];
    doc2["segundo"] = "documento";
    ret = JSON::serialize(doc2, doc2ser, sizeof(doc2ser));
    uint8_t expected2[] = "{\"segundo\":\"documento\"}";
    TEST_ASSERT_EQUAL(23, ret);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected2, doc2ser, 21);
}

void testa_serialize_invalido(void)
{
    StaticJsonDocument<1000> doc;
    doc["msg"] = 1.0;
    doc["jwt"] = "documentoJWT";

    // tamanho insuficiente para conter toda a string serializada
    static char serialized[5];
    size_t ret = JSON::serialize(doc, serialized, sizeof(serialized));
    uint8_t expected[] = "{\"msg\":1,\"jwt\":\"documentoJWT\"}";
    
    TEST_ASSERT_EQUAL(JSON::ERRO_SERIALIZACAO, ret);
}

void testa_deserialize(void)
{
    StaticJsonDocument<1000> doc;

    char serialized[] = "{\"msg\":1,\"jwt\":\"documentoJWT\"}";
    JSON::ERRO ret = JSON::deserialize(serialized, doc);

    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1, doc["msg"]);
    const char *jwt = doc["jwt"];
    TEST_ASSERT_EQUAL_CHAR_ARRAY("documentoJWT", jwt, 12);

    StaticJsonDocument<1000> doc2;
    
    char json[] = "{\"hello\":\"world\"}";
    ret = JSON::deserialize(json, doc2);

    TEST_ASSERT_EQUAL(0, ret);
    const char *world = doc2["hello"];
    TEST_ASSERT_EQUAL_CHAR_ARRAY("world", world, 5);
}

void testa_deserialize_invalido(void)
{
    // tamanho unsuficiente para conter a deserialização
    StaticJsonDocument<10> doc;

    char serialized[] = "{\"msg\":1,\"jwt\":\"documentoJWT\"}";
    JSON::ERRO ret = JSON::deserialize(serialized, doc);

    TEST_ASSERT_EQUAL(JSON::ERRO_DESERIALIZACAO, ret);
}

int main()
{
    UNITY_BEGIN();

    /* RUN_TEST(testa_JSON_invalido);
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
    RUN_TEST(testa_conteudo_registrar);
    RUN_TEST(testa_conteudo_remover_registro);
    RUN_TEST(testa_conteudo_confirmar_registro);
    RUN_TEST(testa_conteudo_confirmar_remocao_registro); */
    RUN_TEST(testa_serialize);
    RUN_TEST(testa_deserialize);
    RUN_TEST(testa_serialize_invalido);
    RUN_TEST(testa_deserialize_invalido);
    return UNITY_END();
}
