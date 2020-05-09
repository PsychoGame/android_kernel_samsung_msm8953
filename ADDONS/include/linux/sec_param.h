/*
 * include/linux/sec_param.h
 *
 * COPYRIGHT(C) 2011-2016 Samsung Electronics Co., Ltd. All Right Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _SEC_PARAM_H_
#define _SEC_PARAM_H_

struct sec_param_data {
	unsigned int debuglevel;
	unsigned int uartsel;
	unsigned int rory_control;
	unsigned int movinand_checksum_done;
	unsigned int movinand_checksum_pass;
	unsigned int cp_debuglevel;
#ifdef CONFIG_GSM_MODEM_SPRD6500
	unsigned int update_cp_bin;
#else
	unsigned int reserved0;
#endif
	unsigned int reserved1[3]; /* CONFIG_RTC_AUTO_PWRON */
#ifdef CONFIG_SEC_MONITOR_BATTERY_REMOVAL
	unsigned int normal_poweroff;
#else
	unsigned int reserved2;
#endif
	unsigned int reserved3;
#ifdef CONFIG_BARCODE_PAINTER
/* [000] : SEC_IMEI= ... */
/* [080] : SEC_MEID= ... */
/* [160] : SEC_SN= ... */
/* [240] : SEC_PR_DATE= ... */
/* [320] : SEC_SKU= ... */
	char param_barcode_info[1024];
#else
	char reserved4[1024];
#endif
#ifdef CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE
	unsigned int wireless_charging_mode;
#else
	unsigned int reserved5;
#endif
#ifdef CONFIG_MUIC_HV
	unsigned int afc_disable;
#else
	unsigned int reserved6;
#endif
#ifdef CONFIG_SKU_THEME
	char param_skutheme_info[32];
#else
	unsigned int reserved7[32];
#endif
	unsigned int cp_reserved_mem;
	char param_carrierid[4]; //only use 3digits, 1 for null
	char param_sales[4]; //only use 3digits, 1 for null
	char param_lcd_resolution[8]; // Variable LCD resolution
        unsigned int UserPartitionFlashed;
	char prototype_serial[16];
	unsigned int apigpiotest;
	char apigpiotestresult[256];
	char reboot_recovery_cause[256];
#if defined (CONFIG_KEEP_JIG_LOW)
	unsigned int keep_jig_low;
#endif
	unsigned int force_upload_flag;
};

struct sec_param_data_s {
	struct delayed_work sec_param_work;
	struct completion work;
	void *value;
	unsigned int offset;
	unsigned int size;
	unsigned int direction;
};

enum sec_param_index {
	param_index_debuglevel,
	param_index_uartsel,
	param_rory_control,
	param_index_movinand_checksum_done,
	param_index_movinand_checksum_pass,
	param_cp_debuglevel,
#ifdef CONFIG_GSM_MODEM_SPRD6500
	param_update_cp_bin,
#endif
#ifdef CONFIG_SEC_MONITOR_BATTERY_REMOVAL
	param_index_normal_poweroff,
#endif
#ifdef CONFIG_BARCODE_PAINTER
	param_index_barcode_info,
#endif
#ifdef CONFIG_WIRELESS_CHARGER_HIGH_VOLTAGE
	param_index_wireless_charging_mode,
#endif
#ifdef CONFIG_MUIC_HV
	param_index_afc_disable,
#endif
#ifdef CONFIG_SKU_THEME
	param_index_skutheme_info,
#endif
	param_index_cp_reserved_mem,
#ifdef CONFIG_USER_RESET_DEBUG
	param_index_last_reset_reason,
#endif
#ifdef CONFIG_SEC_NAD
	param_index_qnad,
	param_index_qnad_ddr_result,
#endif	
        param_index_UserPartitionFlash,
	param_index_prototype_serial,
	param_index_apigpiotest,
	param_index_apigpiotestresult,
	param_index_reboot_recovery_cause,
#if defined (CONFIG_KEEP_JIG_LOW)
	param_index_keep_jig_low,
#endif
	param_index_max_sec_param_data,
};

extern bool sec_get_param(enum sec_param_index index, void *value);
extern bool sec_set_param(enum sec_param_index index, void *value);

#define SEC_PARAM_FILE_OFFSET	(param_file_size - 0x100000)

#define SEC_PARAM_LAST_RESET_REASON		(param_file_size - 4096)

#ifdef CONFIG_SEC_NAD
#define SEC_PARAM_NAD_OFFSET			(8*1024*1024)
#define SEC_PARAM_NAD_SIZE			(0x2000) /* 8KB */
#define SEC_PARAM_NAD_DDR_RESULT_OFFSET		(SEC_PARAM_NAD_OFFSET + SEC_PARAM_NAD_SIZE) /* 8MB + 8KB */
#endif

#endif /* _SEC_PARAM_H_ */
