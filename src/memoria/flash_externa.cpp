#include "flash_externa.h"

#include <MAX32552.h>
#include <mml_spix.h>
#include "W25Q.h" // módulo da Maxim
#include "string.h"

extern mml_qspi_flash_t W25Q_config;

FlashExterna::FlashExterna() :
		retornoEscrita(0), retornoInicializacao(0) {

	static bool already_configured = false;

	if (already_configured) {
		return;
	}

	already_configured = true;

	int result = COMMON_ERR_UNKNOWN;
	mml_qspi_config_t spix_config;
	/* Initialization of QUAD SPI */
	/* SPIx Interface configuration */
	spix_config.mode = SPIX_CFG_MODE_SPIX_Mode_0;
	spix_config.polarity = SPIX_CFG_SSPOL_Slave_Select_Active_Low;
	spix_config.baud = SPIX_DEFAULT_BAUDRATE;
	spix_config.ssact = SPIX_CFG_SSACT_0_system_clock;
	spix_config.ssiact = SPIX_CFG_SSIACT_1_system_clock;
	spix_config.cmd_val = 0x0B;
	spix_config.cmd_width = SPIX_FETCH_CNTL_CMD_WTH_Quad_SDIO;
	spix_config.addr_width = SPIX_FETCH_CNTL_ADR_WTH_Quad_SDIO;
	spix_config.data_width = SPIX_FETCH_CNTL_DAT_WTH_Quad_SDIO;
	spix_config.addr4 = SPIX_FETCH_CNTL_ADDR4_3_Byte_Address_Mode;
	spix_config.md_clk = 0x02;
	spix_config.md_no_command =
			SPIX_MODE_CTRL_NO_CMD_Send_Read_Command_Every_Time;
	spix_config.md_data = 0x00;
	spix_config.md_out_en = 0x00;
	/* For high speed SPI speed like 54MHz, spix_config.fb_md field must be set: 0x01	*/
	spix_config.fb_md = 0x00;
	spix_config.sclk_ds = SPIX_IO_CTRL_SCLK_DS_High_Drive_Strength;
	spix_config.ss_ds = SPIX_IO_CTRL_SS_DS_High_Drive_Strength;
	spix_config.sdio_ds = SPIX_IO_CTRL_SDIO_DS_High_Drive_Strength;
	spix_config.pu_pd_ctrl = SPIX_IO_CTRL_PU_PD_CTRL_Pull_Up;

	// encryption (0 = enable)
	spix_config.dec_en = 0;

	/* Master controller configuration */
	spix_config.ssel = SPIXMC_CONFIG_SSEL_Slave_0;
	spix_config.pgsz = SPIXMC_CONFIG_PGSZ_32_bytes;
	spix_config.bb_mode = SPIXMC_GEN_CTRL_BB_MODE_Disable;
	spix_config.ss_dr = SPIXMC_GEN_CTRL_SSDR_Selected_Slave_Select_Output_is_1;
	spix_config.sck_dr = SPIXMC_GEN_CTRL_SCKDR_SCLK_is_0;
	spix_config.sdio_data_in = 0;
	spix_config.bb_dat = 0;
	spix_config.bb_dat_out_en = 0;
	spix_config.sck_fb = SPIXMC_GEN_CTRL_SCKFB_Enable_SCLK_feedback_mode;
	/* Initialize QUAD SPI */
	result = mml_spix_init(spix_config, W25Q_config);

	this->retornoInicializacao = result;
}

bool FlashExterna::escreve(uintptr_t addr, uint8_t const *data,
		size_t const data_sz) {
	if (this->retornoInicializacao != 0)
		return false;

	this->retornoEscrita = mml_spix_write(addr, const_cast<uint8_t*>(data),
			data_sz, W25Q_config);

	if (this->retornoEscrita != 0)
		return false;

	return true;
}

int FlashExterna::leitura(const uintptr_t addr, uint8_t *data,
		const size_t data_sz) {

//	volatile uint8_t *p = (uint8_t*) addr;
	memcpy(data, (uint8_t*) addr, data_sz);
//	memcpy(data, p, data_sz);
	return 0;
}
