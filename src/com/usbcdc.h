#ifndef USBCDC_H_
#define USBCDC_H_

#include <stdio.h>
#include <cdc_acm.h>

#include "pdafcdc_utils.h"
#include "../debug.h"

class USBCDC {
public:
	/**
	 * @brief configura o usbcdc
	 */
	USBCDC();
    /**
     * @brief obtem o numero de bytes para leitura na interface usb
     *
     * @param timeout_ms    periodo para aguardar em milisegundos
     * @return size_t       Maior que zero: quantidade de bytes disponiveis para leitura. Igual a zero: nenhum byte disponivel para leitura.
     */
    size_t bytes_para_ler(uint32_t timeout_ms) {
        size_t bytes;

        bytes = (size_t)acm_canread();

        if (bytes > 0)
            return bytes;

        if (timeout_ms > 0) {
        	PDAFCDC_Utils::delay_ms(timeout_ms);
            bytes = (size_t)acm_canread();
            return bytes;
        }
        else {
            return 0;
        }
    }

    /**
     * @brief le a interface usb de forma bloqueante
     *
     * @param buffer        buffer que recebera os bytes da usb
     * @param tamanho       tamanho do buffer
     * @return size_t       Maior que 0 (sucesso): numero de bytes lidos.
     */
    inline size_t le_bytes_bloqueante(uint8_t *buffer, size_t tamanho) {
    	size_t lidos;

    	lidos = acm_read(buffer, tamanho);

    	if (lidos <= 0)
    		return 0;
    	else
    		return lidos;
    }

    /**
     * @brief escreve na interface usb de forma bloqueante
     *
     * @param buffer        buffer contendo os bytes a serem escritos
     * @param tamanho       tamanho do buffer
     * @return size_t       numero de caracteres escritos com sucesso.
     */
    inline size_t escreve_bytes_bloqueante(uint8_t *buffer, size_t tamanho) {
    	return (size_t)acm_write(buffer, tamanho);
    }
    /**
     * @brief descarta os bytes pendentes de leitura do usbcdc
     * 
     */
    void esvazia_leitura(void);
	static void check_event_flag(void);
};

#endif /* USBCDC_H_ */
