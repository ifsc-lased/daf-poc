#include "json.h"
#include <iostream>
#include <typeinfo>
#include "string.h"

JSON::ERRO JSON::deserialize(char *input, JsonDocument &doc)
{
    DeserializationError erro = deserializeJson(doc, input);
    if (erro)
    {
        return ERRO_DESERIALIZACAO;
    }

    return SUCESSO;
}

size_t JSON::serialize(const JsonDocument& doc, char *output, size_t out_sz)
{
    size_t expected_sz = measureJson(doc);

    if (expected_sz > out_sz)
        return ERRO_SERIALIZACAO;

    size_t res = serializeJson(doc, output, out_sz);
    
    if (res <= 0)
        return ERRO_SERIALIZACAO;

    return res;
}
