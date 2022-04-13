#ifndef UTILS_JSON_JSONDAF_H_
#define UTILS_JSON_JSONDAF_H_

#include <iostream>
#include "json.h"
#include "include/m_struct.h"

class JSONDAF{
    public:

     enum Status
    {
        SUCESSO = 0,
        ERRO_MENSAGEM_INVALIDA = 1,
        ERRO_CAMPO_INVALIDO = 2,
        ERRO_JSON_INVALIDO = 3,
        ERRO_OUTRO = 4
    };

 /**
     * @brief extrai os dados do objeto json para uma struct definida
     * 
     * @param [in] raw_json         string contendo a cadeia de caracteres do objeto json
     * @param [out] mensagemJSON    ponteiro para a struct que vai receber os valores extraidoss do objeto json
     * @return Status               codigo de status (sucesso, tipo de erro, etc)
     */
    static Status getMensagemJSON(const char *raw_json, MensagemJSON *mensagemJSON);

};


#endif /* UTILS_JSON_JSONDAF_H_ */
