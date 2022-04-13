#ifndef PDAFCDC_UTILS_H
#define PDAFCDC_UTILS_H
#ifdef MAX32552_A1
#include <MAX32552.h>
#include <mml_rtc.h>
#endif
class PDAFCDC_Utils {
    public:
    typedef enum erro {
        SEM_ERRO         = 0x00,

        COMANDO_INVALIDO = 0x01,

        TAMANHO_INVALIDO = 0x02,

		TIMEOUT          = 0x03,

		DESCONHECIDO     = 0x04

    } ERRO_t;

    typedef enum comando {
    	INVALIDO        = 0x00,
        ENVIAR_MENSAGEM = 0x01,
        ENVIAR_BINARIO  = 0x02,
        PING            = 0x03,
        ERRO            = 0x04
    } COMANDO_t;

    typedef enum tamanho{
    	TAM_2 = 2,
    	TAM_4 = 4
    } TAMANHO_t;

#ifdef MAX32552_A1
    static inline unsigned int get_time_as_ms(void)
    {
        /* Check RTC Status. */
    	/* Required for delay_ms() to work properly */
    	if ( !(RTC->CN & RTC_CN_RTCE_Msk) ) {
    		/* RTC is not started yet */
    		mml_rtc_start_up();
    	}

    	unsigned int ms;

    	ms = (unsigned int) ((RTC->SEC * 1000) + (float)(3.9063 * RTC->SSEC)); // 1/256 = 0.0039063

    	return ms;
    }

    static inline void delay_ms(unsigned int ms)
    {
    	unsigned int startTime;

    	startTime = get_time_as_ms();
    	while (get_time_as_ms() < (startTime + ms))
    		;
    	return;
    }
    #endif
};

#endif
