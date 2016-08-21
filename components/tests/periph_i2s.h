struct periph_i2s {
	uint32_t TX_FIFO;
	uint32_t reserved_4;
	union {
		struct {
			unsigned int tx_reset: 1;
			unsigned int rx_reset: 1;
			unsigned int tx_fifo_reset: 1;
			unsigned int rx_fifo_reset: 1;
			unsigned int tx_start: 1;
			unsigned int rx_start: 1;
			unsigned int tx_slave_mod: 1;
			unsigned int rx_slave_mod: 1;
			unsigned int tx_right_first: 1;
			unsigned int rx_right_first: 1;
			unsigned int tx_MSB_shift: 1;
			unsigned int rx_MSB_shift: 1;
			unsigned int tx_short_sync: 1;
			unsigned int rx_short_sync: 1;
			unsigned int tx_mono: 1;
			unsigned int rx_mono: 1;
			unsigned int tx_MSB_right: 1;
			unsigned int rx_MSB_right: 1;
			unsigned int sig_loopback: 1;
			unsigned int reserved19: 13;
		} fld;
		uint32_t val;
	} CONF;
	union {
		struct {
			unsigned int rx_take_data_int_raw: 1;
			unsigned int tx_put_data_int_raw: 1;
			unsigned int rx_wfull_int_raw: 1;
			unsigned int rx_rempty_int_raw: 1;
			unsigned int tx_wfull_int_raw: 1;
			unsigned int tx_rempty_int_raw: 1;
			unsigned int rx_hung_int_raw: 1;
			unsigned int tx_hung_int_raw: 1;
			unsigned int in_done_int_raw: 1;
			unsigned int in_suc_eof_int_raw: 1;
			unsigned int in_err_eof_int_raw: 1;
			unsigned int out_done_int_raw: 1;
			unsigned int out_eof_int_raw: 1;
			unsigned int in_dscr_err_int_raw: 1;
			unsigned int out_dscr_err_int_raw: 1;
			unsigned int in_dscr_empty_int_raw: 1;
			unsigned int out_total_eof_int_raw: 1;
			unsigned int reserved17: 15;
		} fld;
		uint32_t val;
	} INT_RAW;
	union {
		struct {
			unsigned int rx_take_data_int_st: 1;
			unsigned int tx_put_data_int_st: 1;
			unsigned int rx_wfull_int_st: 1;
			unsigned int rx_rempty_int_st: 1;
			unsigned int tx_wfull_int_st: 1;
			unsigned int tx_rempty_int_st: 1;
			unsigned int rx_hung_int_st: 1;
			unsigned int tx_hung_int_st: 1;
			unsigned int in_done_int_st: 1;
			unsigned int in_suc_eof_int_st: 1;
			unsigned int in_err_eof_int_st: 1;
			unsigned int out_done_int_st: 1;
			unsigned int out_eof_int_st: 1;
			unsigned int in_dscr_err_int_st: 1;
			unsigned int out_dscr_err_int_st: 1;
			unsigned int in_dscr_empty_int_st: 1;
			unsigned int out_total_eof_int_st: 1;
			unsigned int reserved17: 15;
		} fld;
		uint32_t val;
	} INT_ST;
	union {
		struct {
			unsigned int rx_take_data_int_ena: 1;
			unsigned int tx_put_data_int_ena: 1;
			unsigned int rx_wfull_int_ena: 1;
			unsigned int rx_rempty_int_ena: 1;
			unsigned int tx_wfull_int_ena: 1;
			unsigned int tx_rempty_int_ena: 1;
			unsigned int rx_hung_int_ena: 1;
			unsigned int tx_hung_int_ena: 1;
			unsigned int in_done_int_ena: 1;
			unsigned int in_suc_eof_int_ena: 1;
			unsigned int in_err_eof_int_ena: 1;
			unsigned int out_done_int_ena: 1;
			unsigned int out_eof_int_ena: 1;
			unsigned int in_dscr_err_int_ena: 1;
			unsigned int out_dscr_err_int_ena: 1;
			unsigned int in_dscr_empty_int_ena: 1;
			unsigned int out_total_eof_int_ena: 1;
			unsigned int reserved17: 15;
		} fld;
		uint32_t val;
	} INT_ENA;
	union {
		struct {
			unsigned int take_data_int_clr: 1;
			unsigned int put_data_int_clr: 1;
			unsigned int rx_wfull_int_clr: 1;
			unsigned int rx_rempty_int_clr: 1;
			unsigned int tx_wfull_int_clr: 1;
			unsigned int tx_rempty_int_clr: 1;
			unsigned int rx_hung_int_clr: 1;
			unsigned int tx_hung_int_clr: 1;
			unsigned int in_done_int_clr: 1;
			unsigned int in_suc_eof_int_clr: 1;
			unsigned int in_err_eof_int_clr: 1;
			unsigned int out_done_int_clr: 1;
			unsigned int out_eof_int_clr: 1;
			unsigned int in_dscr_err_int_clr: 1;
			unsigned int out_dscr_err_int_clr: 1;
			unsigned int in_dscr_empty_int_clr: 1;
			unsigned int out_total_eof_int_clr: 1;
			unsigned int reserved17: 15;
		} fld;
		uint32_t val;
	} INT_CLR;
	union {
		struct {
			unsigned int tx_bck_in_delay: 2;
			unsigned int tx_ws_in_delay: 2;
			unsigned int rx_bck_in_delay: 2;
			unsigned int rx_ws_in_delay: 2;
			unsigned int rx_sd_in_delay: 2;
			unsigned int tx_bck_out_delay: 2;
			unsigned int tx_ws_out_delay: 2;
			unsigned int tx_sd_out_delay: 2;
			unsigned int rx_ws_out_delay: 2;
			unsigned int rx_bck_out_delay: 2;
			unsigned int tx_dsync_sw: 1;
			unsigned int rx_dsync_sw: 1;
			unsigned int data_enable_delay: 2;
			unsigned int tx_bck_in_inv: 1;
			unsigned int reserved25: 7;
		} fld;
		uint32_t val;
	} TIMING;
	union {
		struct {
			unsigned int rx_data_num: 6;
			unsigned int tx_data_num: 6;
			unsigned int dscr_en: 1;
			unsigned int tx_fifo_mod: 3;
			unsigned int rx_fifo_mod: 3;
			unsigned int tx_fifo_mod_force_en: 1;
			unsigned int rx_fifo_mod_force_en: 1;
			unsigned int reserved21: 11;
		} fld;
		uint32_t val;
	} FIFO_CONF;
	uint32_t RXEOF_NUM;
	uint32_t CONF_SIGLE_DATA;
	union {
		struct {
			unsigned int tx_chan_mod: 3;
			unsigned int rx_chan_mod: 2;
			unsigned int reserved5: 27;
		} fld;
		uint32_t val;
	} CONF_CHAN;
	union {
		struct {
			unsigned int outlink_addr: 20;
			unsigned int reserved20: 8;
			unsigned int outlink_stop: 1;
			unsigned int outlink_start: 1;
			unsigned int outlink_restart: 1;
			unsigned int outlink_park: 1;
		} fld;
		uint32_t val;
	} OUT_LINK;
	union {
		struct {
			unsigned int inlink_addr: 20;
			unsigned int reserved20: 8;
			unsigned int inlink_stop: 1;
			unsigned int inlink_start: 1;
			unsigned int inlink_restart: 1;
			unsigned int inlink_park: 1;
		} fld;
		uint32_t val;
	} IN_LINK;
	uint32_t OUT_EOF_DES_ADDR;
	uint32_t IN_EOF_DES_ADDR;
	uint32_t OUT_EOF_BFR_DES_ADDR;
	union {
		struct {
			unsigned int ahb_testmode: 3;
			unsigned int reserved3: 1;
			unsigned int ahb_testaddr: 2;
			unsigned int reserved6: 26;
		} fld;
		uint32_t val;
	} AHB_TEST;
	uint32_t INLINK_DSCR;
	uint32_t INLINK_DSCR_BF0;
	uint32_t INLINK_DSCR_BF1;
	uint32_t OUTLINK_DSCR;
	uint32_t OUTLINK_DSCR_BF0;
	uint32_t OUTLINK_DSCR_BF1;
	union {
		struct {
			unsigned int in_rst: 1;
			unsigned int out_rst: 1;
			unsigned int ahbm_fifo_rst: 1;
			unsigned int ahbm_rst: 1;
			unsigned int out_loop_test: 1;
			unsigned int in_loop_test: 1;
			unsigned int out_auto_wrback: 1;
			unsigned int out_no_restart_clr: 1;
			unsigned int out_eof_mode: 1;
			unsigned int outdscr_burst_en: 1;
			unsigned int indscr_burst_en: 1;
			unsigned int out_data_burst_en: 1;
			unsigned int check_owner: 1;
			unsigned int mem_trans_en: 1;
			unsigned int reserved14: 18;
		} fld;
		uint32_t val;
	} LC_CONF;
	union {
		struct {
			unsigned int outfifo_wdata: 9;
			unsigned int reserved9: 7;
			unsigned int outfifo_push: 1;
			unsigned int reserved17: 15;
		} fld;
		uint32_t val;
	} OUTFIFO_PUSH;
	union {
		struct {
			unsigned int infifo_rdata: 12;
			unsigned int reserved12: 4;
			unsigned int infifo_pop: 1;
			unsigned int reserved17: 15;
		} fld;
		uint32_t val;
	} INFIFO_POP;
	uint32_t LC_STATE0;
	uint32_t LC_STATE1;
	union {
		struct {
			unsigned int lc_fifo_timeout: 8;
			unsigned int lc_fifo_timeout_shift: 3;
			unsigned int lc_fifo_timeout_ena: 1;
			unsigned int reserved12: 20;
		} fld;
		uint32_t val;
	} LC_HUNG_CONF;
	uint32_t reserved_78;
	uint32_t reserved_7c;
	union {
		struct {
			unsigned int cvsd_reg_y_max: 16;
			unsigned int cvsd_reg_y_min: 16;
		} fld;
		uint32_t val;
	} CVSD_CONF0;
	union {
		struct {
			unsigned int cvsd_reg_sigma_max: 16;
			unsigned int cvsd_reg_sigma_min: 16;
		} fld;
		uint32_t val;
	} CVSD_CONF1;
	union {
		struct {
			unsigned int cvsd_reg_K: 3;
			unsigned int cvsd_reg_J: 3;
			unsigned int cvsd_reg_beta: 10;
			unsigned int cvsd_reg_h: 3;
			unsigned int reserved19: 13;
		} fld;
		uint32_t val;
	} CVSD_CONF2;
	union {
		struct {
			unsigned int good_pack_max: 6;
			unsigned int N_err_seg: 3;
			unsigned int shift_rate: 3;
			unsigned int max_slide_sample: 8;
			unsigned int pack_len_8k: 5;
			unsigned int N_min_err: 3;
			unsigned int reserved28: 4;
		} fld;
		uint32_t val;
	} PLC_CONF0;
	union {
		struct {
			unsigned int bad_cef_atten_para: 8;
			unsigned int bad_cef_atten_para_shift: 4;
			unsigned int bad_ola_win2_para_shift: 4;
			unsigned int bad_ola_win2_para: 8;
			unsigned int slide_win_len: 8;
		} fld;
		uint32_t val;
	} PLC_CONF1;
	union {
		struct {
			unsigned int cvsd_seg_mod: 2;
			unsigned int min_period: 5;
			unsigned int reserved7: 25;
		} fld;
		uint32_t val;
	} PLC_CONF2;
	union {
		struct {
			unsigned int esco_en: 1;
			unsigned int esco_chan_mod: 1;
			unsigned int esco_cvsd_dec_pack_err: 1;
			unsigned int esco_cvsd_pack_len_8k: 5;
			unsigned int esco_cvsd_inf_en: 1;
			unsigned int cvsd_dec_start: 1;
			unsigned int cvsd_dec_reset: 1;
			unsigned int plc_en: 1;
			unsigned int plc2dma_en: 1;
			unsigned int reserved13: 19;
		} fld;
		uint32_t val;
	} ESCO_CONF0;
	union {
		struct {
			unsigned int sco_with_i2s_en: 1;
			unsigned int sco_no_i2s_en: 1;
			unsigned int cvsd_enc_start: 1;
			unsigned int cvsd_enc_reset: 1;
			unsigned int reserved4: 28;
		} fld;
		uint32_t val;
	} SCO_CONF0;
	union {
		struct {
			unsigned int tx_pcm_conf: 3;
			unsigned int tx_pcm_bypass: 1;
			unsigned int rx_pcm_conf: 3;
			unsigned int rx_pcm_bypass: 1;
			unsigned int tx_stop_en: 1;
			unsigned int tx_zeros_rm_en: 1;
			unsigned int reserved10: 22;
		} fld;
		uint32_t val;
	} CONF1;
	union {
		struct {
			unsigned int fifo_force_pd: 1;
			unsigned int fifo_force_pu: 1;
			unsigned int plc_mem_force_pd: 1;
			unsigned int plc_mem_force_pu: 1;
			unsigned int reserved4: 28;
		} fld;
		uint32_t val;
	} PD_CONF;
	union {
		struct {
			unsigned int camera_en: 1;
			unsigned int lcd_tx_wrx2_en: 1;
			unsigned int lcd_tx_sdx2_en: 1;
			unsigned int data_enable_test_en: 1;
			unsigned int data_enable: 1;
			unsigned int lcd_en: 1;
			unsigned int ext_adc_start_en: 1;
			unsigned int inter_valid_en: 1;
			unsigned int reserved8: 24;
		} fld;
		uint32_t val;
	} CONF2;
	union {
		struct {
			unsigned int clkm_div_num: 8;
			unsigned int clkm_div_b: 6;
			unsigned int clkm_div_a: 6;
			unsigned int clk_en: 1;
			unsigned int clka_ena: 1;
			unsigned int reserved22: 10;
		} fld;
		uint32_t val;
	} CLKM_CONF;
	union {
		struct {
			unsigned int tx_bck_div_num: 6;
			unsigned int rx_bck_div_num: 6;
			unsigned int tx_bits_mod: 6;
			unsigned int rx_bits_mod: 6;
			unsigned int reserved24: 8;
		} fld;
		uint32_t val;
	} SAMPLE_RATE_CONF;
	union {
		struct {
			unsigned int tx_pdm_en: 1;
			unsigned int rx_pdm_en: 1;
			unsigned int pcm2pdm_conv_en: 1;
			unsigned int pdm2pcm_conv_en: 1;
			unsigned int tx_pdm_sinc_osr2: 4;
			unsigned int tx_pdm_prescale: 8;
			unsigned int tx_pdm_hp_in_shift: 2;
			unsigned int tx_pdm_lp_in_shift: 2;
			unsigned int tx_pdm_sinc_in_shift: 2;
			unsigned int tx_pdm_sigmadelta_in_shift: 2;
			unsigned int rx_pdm_sinc_dsr_16_en: 1;
			unsigned int tx_pdm_hp_bypass: 1;
			unsigned int reserved26: 6;
		} fld;
		uint32_t val;
	} PDM_CONF;
	union {
		struct {
			unsigned int tx_pdm_fs: 10;
			unsigned int tx_pdm_fp: 10;
			unsigned int reserved20: 12;
		} fld;
		uint32_t val;
	} PDM_FREQ_CONF;
	union {
		struct {
			unsigned int tx_idle: 1;
			unsigned int tx_fifo_reset_back: 1;
			unsigned int rx_fifo_reset_back: 1;
			unsigned int reserved3: 29;
		} fld;
		uint32_t val;
	} STATE;
	uint32_t reserved_c0;
	uint32_t reserved_c4;
	uint32_t reserved_c8;
	uint32_t reserved_cc;
	uint32_t reserved_d0;
	uint32_t reserved_d4;
	uint32_t reserved_d8;
	uint32_t reserved_dc;
	uint32_t reserved_e0;
	uint32_t reserved_e4;
	uint32_t reserved_e8;
	uint32_t reserved_ec;
	uint32_t reserved_f0;
	uint32_t reserved_f4;
	uint32_t reserved_f8;
	uint32_t DATE;
};


extern volatile struct periph_i2s I2S0;

