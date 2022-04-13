#ifndef MEMORIA_FLASH_EXTERNA_H_
#define MEMORIA_FLASH_EXTERNA_H_

#include <stdint.h>
#include <stdio.h>

class FlashExterna {

public:
	static const uintptr_t Start = 0x08000000;
	static const uintptr_t End   = 0x0FFFFFFF;

	FlashExterna();

	/*
	 * @brief escreve na memoria flash externa
	 *
	 * @param[in] addr		endereço na memoria
	 * @param[in] data		dados a serem escritos
	 * @param[in] data_sz	tamanho dos dados
	 */
	bool escreve(uintptr_t addr, uint8_t const *data, size_t const data_sz);

	/*
	 * @brief le um endereco da memoria flash externa
	 *
	 * @param[in]  addr		endereco a ser lido
	 * @param[out] data		buffer para inserir os dados lidos
	 * @param[out] data_sz	tamanho do buffer alocado
	 */
	int leitura(const uintptr_t addr, uint8_t *data, size_t const data_sz);
private:
	int retornoEscrita;
	int retornoInicializacao;

};


#endif /* MEMORIA_FLASH_EXTERNA_H_ */
