#ifndef UTILS_JSON_JSON_H_
#define UTILS_JSON_JSON_H_
#include "include/ArduinoJson-v6.17.2.h"
#include "include/m_struct.h"
#include <iostream>
#define MAX_LENGTH_JSON 3500

typedef StaticJsonDocument<MAX_LENGTH_JSON> json_t;

class JSON
{


public:
    enum ERRO
    {   
        SUCESSO = 0,
        ERRO_DESERIALIZACAO = -1,
        ERRO_SERIALIZACAO = -2
    };

    /**
     * @brief Serializa um documento JSON 
     * 
     * @param [in] doc            documento JSON gerado na aplicacao
     * @param [out] output        buffer para alocar o documento json string serializado
     * @param [in] out_sz         tamanho do buffer alocado para receber a string serializada.
     * @return size_t             < 0: falha na serializacao, > 0: numero de bytes escritos
     */
    
    static size_t serialize(const JsonDocument& doc, char *output, size_t out_sz);

    /**
     * @brief Deserializa um documento JSON
     * 
     * @param [in] input        string json a ser deserializada
     * @param [out] doc         referencia ao documento json que sera preenchido com a string json
     * @return int              0: sucesso, < 0: erro
     */

    static ERRO deserialize(char *input, JsonDocument &doc);
  
};

#endif /* UTILS_JSON_JSON_H_ */
