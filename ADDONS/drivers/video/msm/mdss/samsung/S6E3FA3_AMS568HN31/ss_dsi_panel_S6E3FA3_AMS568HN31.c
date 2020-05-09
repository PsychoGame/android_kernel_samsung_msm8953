/*
 * =================================================================
 *
 *
 *	Description:  samsung display panel file
 *
 *	Author: jb09.kim
 *	Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2012, Samsung Electronics. All rights reserved.

*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
*/
#include "ss_dsi_panel_S6E3FA3_AMS568HN31.h"
#include "ss_dsi_mdnie_S6E3FA3_AMS568HN31.h"

static int mdss_panel_on_pre(struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return false;
	}

	pr_info("%s %d\n", __func__, ndx);

	mdss_panel_attach_set(ctrl, true);

	return true;
}

extern int poweroff_charging;

static int mdss_panel_on_post(struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return false;
	}

	pr_info("%s %d\n", __func__, ndx);

	if(poweroff_charging) {
		pr_info("%s LPM Mode Enable, Add More Delay\n", __func__);
		msleep(300);
	}

	return true;
}

static char mdss_panel_revision(struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return false;
	}

	if (vdd->manufacture_id_dsi[ndx] == PBA_ID)
		mdss_panel_attach_set(ctrl, false);
	else
		mdss_panel_attach_set(ctrl, true);

	vdd->panel_revision = 'A' - 'A';

	return true;
}

static int mdss_manufacture_date_read(struct mdss_dsi_ctrl_pdata *ctrl)
{
	unsigned char date[2];
	int year, month, day;
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return false;
	}

	/* Read mtp (C8h 41,42th) for manufacture date */
	if (get_panel_rx_cmds(ctrl, RX_MANUFACTURE_DATE)->cmd_cnt) {
		mdss_samsung_panel_data_read(ctrl,
			get_panel_rx_cmds(ctrl, RX_MANUFACTURE_DATE),
			date, LEVEL2_KEY);

		year = date[0] & 0xf0;
		year >>= 4;
		year += 2011; // 0 = 2011 year
		month = date[0] & 0x0f;
		day = date[1] & 0x1f;

		pr_info("%s DSI%d manufacture_date = %d", __func__, ndx, year * 10000 + month * 100 + day);

		vdd->manufacture_date_dsi[ndx]  =   year * 10000 + month * 100 + day;
	} else {
		pr_err("%s DSI%d error", __func__, ndx);
		return false;
	}

	return true;
}

static int mdss_ddi_id_read(struct mdss_dsi_ctrl_pdata *ctrl)
{
	char ddi_id[5];
	int loop;
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return false;
	}

	/* Read mtp (D6h 1~5th) for ddi id */
	if (get_panel_rx_cmds(ctrl, RX_DDI_ID)->cmd_cnt) {
		mdss_samsung_panel_data_read(ctrl,
			get_panel_rx_cmds(ctrl, RX_DDI_ID),
			ddi_id, LEVEL2_KEY);

		for(loop = 0; loop < 5; loop++)
			vdd->ddi_id_dsi[ndx][loop] = ddi_id[loop];

		pr_info("%s DSI%d : %02x %02x %02x %02x %02x\n", __func__, ndx,
			vdd->ddi_id_dsi[ndx][0], vdd->ddi_id_dsi[ndx][1],
			vdd->ddi_id_dsi[ndx][2], vdd->ddi_id_dsi[ndx][3],
			vdd->ddi_id_dsi[ndx][4]);
	} else {
		pr_err("%s DSI%d error", __func__, ndx);
		return false;
	}

	return true;
}

static int mdss_cell_id_read(struct mdss_dsi_ctrl_pdata *ctrl)
{
	char cell_id_buffer[MAX_CELL_ID];
	int loop;
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return false;
	}

	/* Read Panel Unique Cell ID (C8h 41~51th) */
	if (get_panel_rx_cmds(ctrl, RX_CELL_ID)->cmd_cnt) {
		mdss_samsung_panel_data_read(ctrl,
			get_panel_rx_cmds(ctrl, RX_CELL_ID),
			cell_id_buffer, LEVEL2_KEY);

		for(loop = 0; loop < MAX_CELL_ID; loop++)
			vdd->cell_id_dsi[ndx][loop] = cell_id_buffer[loop];

		/* For AMS568HN31 exceptionally, MDNIE X&Y value(4bytes) should be get from A1h register read value */
		if(vdd->mdnie_x[ndx] && vdd->mdnie_y[ndx]) {
			vdd->cell_id_dsi[ndx][7] = vdd->mdnie_x[ndx] >> 8 & 0xff;
			vdd->cell_id_dsi[ndx][8] = vdd->mdnie_x[ndx] & 0xff;
			vdd->cell_id_dsi[ndx][9] = vdd->mdnie_y[ndx] >> 8 & 0xff;
			vdd->cell_id_dsi[ndx][10] = vdd->mdnie_y[ndx] & 0xff;
		} else
			pr_err("%s: MDNIE X,Y Value is Zero \n", __func__);

		pr_info("%s DSI_%d cell_id: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			__func__, ndx, vdd->cell_id_dsi[ndx][0],
			vdd->cell_id_dsi[ndx][1],	vdd->cell_id_dsi[ndx][2],
			vdd->cell_id_dsi[ndx][3],	vdd->cell_id_dsi[ndx][4],
			vdd->cell_id_dsi[ndx][5],	vdd->cell_id_dsi[ndx][6],
			vdd->cell_id_dsi[ndx][7],	vdd->cell_id_dsi[ndx][8],
			vdd->cell_id_dsi[ndx][9],	vdd->cell_id_dsi[ndx][10]);

	} else {
		pr_err("%s DSI%d error", __func__, ndx);
		return false;
	}

	return true;
}

static int get_hbm_candela_value(int level)
{
	if (level == 13)
		return 443;
	else if (level == 6)
		return 465;
	else if (level == 7)
		return 488;
	else if (level == 8)
		return 510;
	else if (level == 9)
		return 533;
	else if (level == 10)
		return 555;
	else if (level == 11)
		return 578;
	else if (level == 12)
		return 600;
	else
		return 600;
}

static int mdss_hbm_read(struct mdss_dsi_ctrl_pdata *ctrl)
{
	char hbm_buffer1[6] = {0,};
	char hbm_buffer2[29] = {0,};
	char hbm_elvss[2] ={0,};

	char V255R_0, V255R_1, V255G_0, V255G_1, V255B_0, V255B_1;
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);
	struct dsi_panel_cmds *hbm_gamma_cmds = get_panel_tx_cmds(ctrl, TX_HBM_GAMMA);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return false;
	}

	if (get_panel_rx_cmds(ctrl, RX_HBM)->cmd_cnt) {
		mdss_samsung_panel_data_read(ctrl,
			get_panel_rx_cmds(ctrl, RX_HBM),
			hbm_buffer1, LEVEL1_KEY);

		V255R_0 = (hbm_buffer1[0] & BIT(2))>>2;
		V255R_1 = hbm_buffer1[1];
		V255G_0 = (hbm_buffer1[0] & BIT(1))>>1;
		V255G_1 = hbm_buffer1[2];
		V255B_0 = hbm_buffer1[0] & BIT(0);
		V255B_1 = hbm_buffer1[3];
		hbm_buffer1[0] = V255R_0;
		hbm_buffer1[1] = V255R_1;
		hbm_buffer1[2] = V255G_0;
		hbm_buffer1[3] = V255G_1;
		hbm_buffer1[4] = V255B_0;
		hbm_buffer1[5] = V255B_1;

		pr_debug("mdss_hbm_read = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
			hbm_buffer1[0], hbm_buffer1[1], hbm_buffer1[2], hbm_buffer1[3], hbm_buffer1[4], hbm_buffer1[5]);
		memcpy(&hbm_gamma_cmds->cmds[0].payload[1], hbm_buffer1, 6);

	} else {
		pr_err("%s DSI%d error", __func__, ndx);
		return false;
	}

	if (get_panel_rx_cmds(ctrl, RX_HBM2)->cmd_cnt) {
		mdss_samsung_panel_data_read(ctrl,
			get_panel_rx_cmds(ctrl, RX_HBM2),
			hbm_buffer2, LEVEL1_KEY);

		memcpy(&hbm_gamma_cmds->cmds[0].payload[7], hbm_buffer2, 29);
	} else {
		pr_err("%s DSI%d error", __func__, ndx);
		return false;
	}

	if (get_panel_rx_cmds(ctrl, RX_ELVSS)->cmd_cnt) {
		/* Read mtp (B6h 22~23th) for hbm elvss */
		mdss_samsung_panel_data_read(ctrl,
			get_panel_rx_cmds(ctrl, RX_ELVSS),
			hbm_elvss, LEVEL1_KEY);
		vdd->display_status_dsi[ndx].elvss_value1 = hbm_elvss[0];

	} else {
		pr_err("%s DSI%d error", __func__, ndx);
		return false;
	}
	return true;
}

static struct dsi_panel_cmds *mdss_hbm_gamma(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);
	struct dsi_panel_cmds *hbm_gamma_cmds = get_panel_tx_cmds(ctrl, TX_HBM_GAMMA);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	pr_debug("%s vdd->auto_brightness : %d\n", __func__, vdd->auto_brightness);

	if (IS_ERR_OR_NULL(vdd->smart_dimming_dsi[ndx]->generate_gamma)) {
		pr_err("%s generate_gamma is NULL error", __func__);
		return NULL;
	} else {
		vdd->smart_dimming_dsi[ndx]->generate_hbm_gamma(
			vdd->smart_dimming_dsi[ndx],
			vdd->auto_brightness,
			&hbm_gamma_cmds->cmds[0].payload[1]);

		*level_key = LEVEL1_KEY;
		return hbm_gamma_cmds;
	}
}

static struct dsi_panel_cmds *mdss_hbm_etc(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	char elvss_2th_val, elvss_22th_val, elvss_23th_val;
	int ndx = display_ndx_check(ctrl);
	struct dsi_panel_cmds *hbm_etc_cmds = get_panel_tx_cmds(ctrl, TX_HBM_ETC);

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data ctrl : 0x%zx vdd : 0x%zx", (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	elvss_2th_val = elvss_22th_val = elvss_23th_val =0;

	elvss_22th_val = vdd->display_status_dsi[ndx].elvss_value1; // B6h 22th para : OTP Value
	elvss_23th_val = vdd->display_status_dsi[ndx].elvss_value2; // B6h 23th para

	*level_key = LEVEL1_KEY;

	/* 0xB6 2th*/
	if (vdd->auto_brightness == HBM_MODE) /* 465CD */
		elvss_2th_val = 0x11;
	else if (vdd->auto_brightness == HBM_MODE + 1) /* 488CD */
		elvss_2th_val = 0x0F;
	else if (vdd->auto_brightness == HBM_MODE + 2) /* 510CD */
		elvss_2th_val = 0x0E;
	else if (vdd->auto_brightness == HBM_MODE + 3) /* 533CD */
		elvss_2th_val = 0x0C;
	else if (vdd->auto_brightness == HBM_MODE + 4) /* 555CD */
		elvss_2th_val = 0x0B;
	else if (vdd->auto_brightness == HBM_MODE + 5) /* 578CD */
		elvss_2th_val = 0x0B;
	else if (vdd->auto_brightness == HBM_MODE + 6) /* 600CD */
		elvss_2th_val = 0x0A;
	else if (vdd->auto_brightness == HBM_MODE + 7) /* 443CD */
		elvss_2th_val = 0x12;

	/* ELVSS 0xB6 2th, 23th */
	hbm_etc_cmds->cmds[1].payload[2] = elvss_2th_val;
	hbm_etc_cmds->cmds[3].payload[1] = elvss_23th_val;

	/* ACL 15% in LDU/CCB mode */
	if (vdd->color_weakness_mode || vdd->ldu_correction_state)
		hbm_etc_cmds->cmds[4].payload[4] = 0x14; /* 0x14 : 15% */
	else
		hbm_etc_cmds->cmds[4].payload[4] = 0x0A; /* 0x0A  : 8% */

	if (vdd->auto_brightness == HBM_MODE + 6) {
		hbm_etc_cmds->cmds[7].payload[1] = 0xC0; // 600CD : HBM ON
	} else {
		hbm_etc_cmds->cmds[7].payload[1] = 0x00; // HBM OFF
	}

	LCD_INFO("0xB6 2th : 0x%x 0xB6 23th(elvss val) : 0x%x\n", elvss_2th_val, elvss_23th_val);
	return hbm_etc_cmds;
}

static int mdss_elvss_read(struct mdss_dsi_ctrl_pdata *ctrl)
{
	char elvss_b6[2];
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data ctrl : 0x%zx vdd : 0x%zx", (size_t)ctrl, (size_t)vdd);
		return false;
	}

	/* Read mtp (B6h 22th,23th) for elvss*/
	mdss_samsung_panel_data_read(ctrl,
		get_panel_rx_cmds(ctrl, RX_ELVSS),
		elvss_b6, LEVEL1_KEY);
	vdd->display_status_dsi[ndx].elvss_value1 = elvss_b6[0]; /*0xB6 22th OTP value*/
	vdd->display_status_dsi[ndx].elvss_value2 = elvss_b6[1]; /*0xB6 23th */

	return true;
}

static struct dsi_panel_cmds *mdss_hbm_off(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	*level_key = LEVEL2_KEY;

	pr_info("[MDSS] %s\n", __func__);
	return get_panel_tx_cmds(ctrl, TX_HBM_OFF);
}

#define COORDINATE_DATA_SIZE 6

#define F1(x,y) ((y)-((43*(x))/40)+45)
#define F2(x,y) ((y)-((310*(x))/297)-3)
#define F3(x,y) ((y)+((367*(x))/84)-16305)
#define F4(x,y) ((y)+((333*(x))/107)-12396)

/* Normal Mode */
static char coordinate_data_1[][COORDINATE_DATA_SIZE] = {
	{0xff, 0x00, 0xff, 0x00, 0xff, 0x00}, /* dummy */
	{0xff, 0x00, 0xfb, 0x00, 0xfb, 0x00}, /* Tune_1 */
	{0xff, 0x00, 0xfc, 0x00, 0xff, 0x00}, /* Tune_2 */
	{0xfb, 0x00, 0xfa, 0x00, 0xff, 0x00}, /* Tune_3 */
	{0xff, 0x00, 0xfe, 0x00, 0xfc, 0x00}, /* Tune_4 */
	{0xff, 0x00, 0xff, 0x00, 0xff, 0x00}, /* Tune_5 */
	{0xfb, 0x00, 0xfc, 0x00, 0xff, 0x00}, /* Tune_6 */
	{0xfd, 0x00, 0xff, 0x00, 0xfa, 0x00}, /* Tune_7 */
	{0xfc, 0x00, 0xff, 0x00, 0xfc, 0x00}, /* Tune_8 */
	{0xfb, 0x00, 0xff, 0x00, 0xff, 0x00}, /* Tune_9 */
};

/* sRGB/Adobe RGB Mode */
static char coordinate_data_2[][COORDINATE_DATA_SIZE] = {
	{0xff, 0x00, 0xff, 0x00, 0xff, 0x00}, /* dummy */
	{0xff, 0x00, 0xf6, 0x00, 0xec, 0x00}, /* Tune_1 */
	{0xff, 0x00, 0xf6, 0x00, 0xef, 0x00}, /* Tune_2 */
	{0xff, 0x00, 0xf8, 0x00, 0xf3, 0x00}, /* Tune_3 */
	{0xff, 0x00, 0xf8, 0x00, 0xec, 0x00}, /* Tune_4 */
	{0xff, 0x00, 0xf9, 0x00, 0xef, 0x00}, /* Tune_5 */
	{0xff, 0x00, 0xfb, 0x00, 0xf3, 0x00}, /* Tune_6 */
	{0xff, 0x00, 0xfb, 0x00, 0xec, 0x00}, /* Tune_7 */
	{0xff, 0x00, 0xfc, 0x00, 0xef, 0x00}, /* Tune_8 */
	{0xff, 0x00, 0xfd, 0x00, 0xf3, 0x00}, /* Tune_9 */
};

static char (*coordinate_data_multi[MAX_MODE])[COORDINATE_DATA_SIZE] = {
	coordinate_data_1, /* DYNAMIC - Normal */
	coordinate_data_2, /* STANDARD - sRGB/Adobe RGB */
	coordinate_data_2, /* NATURAL - sRGB/Adobe RGB */
	coordinate_data_1, /* MOVIE - Normal */
	coordinate_data_1, /* AUTO - Normal */
	coordinate_data_1, /* READING - Normal */
};

static int mdnie_coordinate_index(int x, int y)
{
	int tune_number = 0;

	if (F1(x,y) > 0) {
		if (F3(x,y) > 0) {
			tune_number = 3;
		} else {
			if (F4(x,y) < 0)
				tune_number = 1;
			else
				tune_number = 2;
		}
	} else {
		if (F2(x,y) < 0) {
			if (F3(x,y) > 0) {
				tune_number = 9;
			} else {
				if (F4(x,y) < 0)
					tune_number = 7;
				else
					tune_number = 8;
			}
		} else {
			if (F3(x,y) > 0)
				tune_number = 6;
			else {
				if (F4(x,y) < 0)
					tune_number = 4;
				else
					tune_number = 5;
			}
		}
	}

	return tune_number;
}

static int mdss_mdnie_read(struct mdss_dsi_ctrl_pdata *ctrl)
{
	char x_y_location[4];
	int mdnie_tune_index = 0;
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);
	
	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return false;
	}

	/* Read mtp (D6h 1~5th) for ddi id */
	if (get_panel_rx_cmds(ctrl, RX_MDNIE)->cmd_cnt) {
		mdss_samsung_panel_data_read(ctrl,
			get_panel_rx_cmds(ctrl, RX_MDNIE),
			x_y_location, LEVEL2_KEY);

		vdd->mdnie_x[ndx] = x_y_location[0] << 8 | x_y_location[1];	/* X */
		vdd->mdnie_y[ndx] = x_y_location[2] << 8 | x_y_location[3];	/* Y */

		mdnie_tune_index = mdnie_coordinate_index(vdd->mdnie_x[ndx], vdd->mdnie_y[ndx]);
		coordinate_tunning_multi(ndx, coordinate_data_multi, mdnie_tune_index,
			ADDRESS_SCR_WHITE_RED, COORDINATE_DATA_SIZE);

		pr_info("%s DSI%d : X-%d Y-%d \n", __func__, ndx,
			vdd->mdnie_x[ndx], vdd->mdnie_y[ndx]);
	} else {
		pr_err("%s DSI%d error", __func__, ndx);
		return false;
	}

	return true;
}

static int mdss_smart_dimming_init(struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return false;
	}

	vdd->smart_dimming_dsi[ndx] = vdd->panel_func.samsung_smart_get_conf();

	if (IS_ERR_OR_NULL(vdd->smart_dimming_dsi[ndx])) {
		pr_err("%s DSI%d error", __func__, ndx);
		return false;
	} else {
		if (get_panel_rx_cmds(ctrl, RX_SMART_DIM_MTP)->cmd_cnt){
			mdss_samsung_panel_data_read(ctrl,
				get_panel_rx_cmds(ctrl, RX_SMART_DIM_MTP),
				vdd->smart_dimming_dsi[ndx]->mtp_buffer, LEVEL2_KEY);

			/* Initialize smart dimming related things here */
			/* lux_tab setting for 350cd */
			vdd->smart_dimming_dsi[ndx]->lux_tab = vdd->dtsi_data[ndx].candela_map_table[vdd->panel_revision].cd;
			vdd->smart_dimming_dsi[ndx]->lux_tabsize = vdd->dtsi_data[ndx].candela_map_table[vdd->panel_revision].tab_size;
			vdd->smart_dimming_dsi[ndx]->man_id = vdd->manufacture_id_dsi[ndx];
			vdd->smart_dimming_dsi[ndx]->hbm_payload = &get_panel_tx_cmds(ctrl, TX_HBM_GAMMA)->cmds[0].payload[1];

			/* Just a safety check to ensure smart dimming data is initialised well */
			vdd->smart_dimming_dsi[ndx]->init(vdd->smart_dimming_dsi[ndx]);
			vdd->temperature = 20; // default temperature
			vdd->smart_dimming_loaded_dsi[ndx] = true;
		} else {
			pr_err("%s DSI%d error", __func__, ndx);
			return false;
		}
	}

	pr_info("%s DSI%d : --\n",__func__, ndx);

	return true;
}

static struct dsi_panel_cmds aid_cmd;
static struct dsi_panel_cmds *mdss_aid(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int cd_index = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	cd_index = vdd->bl_level;

	aid_cmd.cmds = &(get_panel_tx_cmds(ctrl, TX_AID_SUBDIVISION)->cmds[cd_index]);
	pr_info("%s : level(%d), aid(%x %x)\n", __func__, vdd->bl_level, aid_cmd.cmds->payload[1], aid_cmd.cmds->payload[2]);
	
	aid_cmd.cmd_cnt = 1;
	*level_key = LEVEL1_KEY;

	return &aid_cmd;
}

static struct dsi_panel_cmds vint_cmd;
static struct dsi_panel_cmds * mdss_vint(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);
	struct dsi_panel_cmds *vint_cmds = get_panel_tx_cmds(ctrl, TX_VINT);
	int cd_index = 0;
	int cmd_idx = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	cd_index = vdd->cd_idx;

	if (vdd->temperature <= -20)
		cd_index = 30;

	if (!vdd->dtsi_data[ndx].vint_map_table[vdd->panel_revision].size ||
		cd_index > vdd->dtsi_data[ndx].vint_map_table[vdd->panel_revision].size)
		goto end;

	cmd_idx = vdd->dtsi_data[ndx].vint_map_table[vdd->panel_revision].cmd_idx[cd_index];

	vint_cmd.cmds = &vint_cmds->cmds[cmd_idx];
	vint_cmd.cmd_cnt = 1;

	*level_key = LEVEL1_KEY;

	pr_debug("%s temp : %d 0xF4 : 0x%x\n", __func__, vdd->temperature,
		vint_cmd.cmds->payload[2] );

	return &vint_cmd;

end :
	pr_err("%s error", __func__);
		return NULL;
}

static struct dsi_panel_cmds * mdss_acl_on(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	struct dsi_panel_cmds *acl_on_cmds = get_panel_tx_cmds(ctrl, TX_ACL_ON);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	*level_key = LEVEL1_KEY;

	if (vdd->gradual_acl_val)
		acl_on_cmds->cmds[0].payload[4] = vdd->gradual_acl_val;

	return acl_on_cmds;
}

static struct dsi_panel_cmds * mdss_acl_off(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	*level_key = LEVEL1_KEY;
	pr_info("%s ", __func__);

	return get_panel_tx_cmds(ctrl, TX_ACL_OFF);
}


static struct dsi_panel_cmds elvss_cmd;
static struct dsi_panel_cmds * mdss_elvss(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);
	int cd_index = 0;
	int cmd_idx = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	cd_index = vdd->cd_idx;

	if (!vdd->dtsi_data[ndx].smart_acl_elvss_map_table[vdd->panel_revision].size ||
		cd_index > vdd->dtsi_data[ndx].smart_acl_elvss_map_table[vdd->panel_revision].size)
		goto end;

	cmd_idx = vdd->dtsi_data[ndx].smart_acl_elvss_map_table[vdd->panel_revision].cmd_idx[cd_index];

	elvss_cmd.cmds = &(get_panel_tx_cmds(ctrl, TX_SMART_ACL_ELVSS)->cmds[cmd_idx]);

	elvss_cmd.cmd_cnt = 1;

	*level_key = LEVEL1_KEY;

	return &elvss_cmd;

end :
	pr_err("%s error", __func__);
	return NULL;
}

static struct dsi_panel_cmds * mdss_elvss_temperature1(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	struct dsi_panel_cmds *elvss_lowtemp_cmds = get_panel_tx_cmds(ctrl, TX_ELVSS_LOWTEMP);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	if (vdd->temperature >= 0)
		elvss_lowtemp_cmds->cmds[0].payload[1] = vdd->temperature;
	else {
		elvss_lowtemp_cmds->cmds[0].payload[1] = (vdd->temperature*-1) | 0x80;
	}

	pr_debug("%s temp : %d 0xB8 : 0x%x\n", __func__, vdd->temperature,
		elvss_lowtemp_cmds->cmds[0].payload[1] );

	*level_key = LEVEL1_KEY;

	return elvss_lowtemp_cmds;
}

static struct dsi_panel_cmds * mdss_elvss_temperature2(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int candela_value = 0;
	int ndx = display_ndx_check(ctrl);
	struct dsi_panel_cmds *elvss_lowtemp2_cmds = get_panel_tx_cmds(ctrl, TX_ELVSS_LOWTEMP2);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	candela_value = vdd->candela_level;

	// T > 0
	if (vdd->temperature > 0) {
		if (candela_value <= 15)	/* 2 ~ 15 nit */
			elvss_lowtemp2_cmds->cmds[1].payload[1] = 0x0D;
		else				/* 16 ~ 420 nit */
			elvss_lowtemp2_cmds->cmds[1].payload[1]
				= vdd->display_status_dsi[ndx].elvss_value1;
	}
	// -20 <  T <= 0
	else if (vdd->temperature <= 0 && vdd->temperature > -20) {
		if (candela_value <= 15)	/* 2 ~ 15 nit */
			elvss_lowtemp2_cmds->cmds[1].payload[1] = 0x0D;
		else				/* 16 ~ 420 nit */
			elvss_lowtemp2_cmds->cmds[1].payload[1]
				= vdd->display_status_dsi[ndx].elvss_value1;
	}
	// T <= -20
	else {
		if (candela_value <= 15)	/* 2 ~ 15 nit */
			elvss_lowtemp2_cmds->cmds[1].payload[1] = 0x0D;
		else				/* 16 ~ 420 nit */
			elvss_lowtemp2_cmds->cmds[1].payload[1]
				= vdd->display_status_dsi[ndx].elvss_value1;
	}

	pr_debug("%s temp : %d 0xB0 : 0x%x 0xB6 : 0x%x\n", __func__, vdd->temperature,
		elvss_lowtemp2_cmds->cmds[0].payload[1],
		elvss_lowtemp2_cmds->cmds[1].payload[1]);

	*level_key = LEVEL1_KEY;
	return elvss_lowtemp2_cmds;
}

static struct dsi_panel_cmds * mdss_gamma(struct mdss_dsi_ctrl_pdata *ctrl, int *level_key)
{
	struct samsung_display_driver_data *vdd = check_valid_ctrl(ctrl);
	int ndx = display_ndx_check(ctrl);
	struct dsi_panel_cmds  *gamma_cmds = get_panel_tx_cmds(ctrl, TX_GAMMA);

	if (IS_ERR_OR_NULL(vdd)) {
		pr_err("%s: Invalid data ctrl : 0x%zx vdd : 0x%zx", __func__, (size_t)ctrl, (size_t)vdd);
		return NULL;
	}

	pr_debug("%s bl_level : %d candela : %dCD\n", __func__, vdd->bl_level, vdd->candela_level);

	if (IS_ERR_OR_NULL(vdd->smart_dimming_dsi[ndx]->generate_gamma)) {
		pr_err("%s generate_gamma is NULL error", __func__);
		return NULL;
	} else {
		vdd->smart_dimming_dsi[ndx]->generate_gamma(
			vdd->smart_dimming_dsi[ndx],
			vdd->candela_level,
			&gamma_cmds->cmds[0].payload[1]);

		*level_key = LEVEL1_KEY;

		return gamma_cmds;
	}
}

static void dsi_update_mdnie_data(void)
{
	/* Update mdnie command */
	mdnie_data.DSI0_COLOR_BLIND_MDNIE_2 = DSI0_COLOR_BLIND_MDNIE_2;
	mdnie_data.DSI0_RGB_SENSOR_MDNIE_1 = DSI0_RGB_SENSOR_MDNIE_1;
	mdnie_data.DSI0_RGB_SENSOR_MDNIE_2 = DSI0_RGB_SENSOR_MDNIE_2;

	mdnie_data.DSI0_BYPASS_MDNIE = DSI0_BYPASS_MDNIE;
	mdnie_data.DSI0_NEGATIVE_MDNIE = DSI0_NEGATIVE_MDNIE;
	mdnie_data.DSI0_COLOR_BLIND_MDNIE = DSI0_COLOR_BLIND_MDNIE;
	mdnie_data.DSI0_HBM_CE_MDNIE = DSI0_HBM_CE_MDNIE;
	mdnie_data.DSI0_HBM_CE_TEXT_MDNIE = DSI0_HBM_CE_TEXT_MDNIE;
	mdnie_data.DSI0_RGB_SENSOR_MDNIE = DSI0_RGB_SENSOR_MDNIE;
	mdnie_data.DSI0_CURTAIN = DSI0_CURTAIN;
	mdnie_data.DSI0_GRAYSCALE_MDNIE = DSI0_GRAYSCALE_MDNIE;
	mdnie_data.DSI0_GRAYSCALE_NEGATIVE_MDNIE = DSI0_GRAYSCALE_NEGATIVE_MDNIE;
	mdnie_data.DSI0_COLOR_BLIND_MDNIE_SCR = DSI0_COLOR_BLIND_MDNIE_2;
	mdnie_data.DSI0_RGB_SENSOR_MDNIE_SCR = DSI0_RGB_SENSOR_MDNIE_2;

	mdnie_data.mdnie_tune_value_dsi0 = mdnie_tune_value_dsi0;
	mdnie_data.light_notification_tune_value_dsi0 = light_notification_tune_value;

	/* Update MDNIE data related with size, offset or index */
	mdnie_data.dsi0_bypass_mdnie_size = ARRAY_SIZE(DSI0_BYPASS_MDNIE);
	mdnie_data.mdnie_color_blinde_cmd_offset = MDNIE_COLOR_BLINDE_CMD_OFFSET;
	mdnie_data.mdnie_step_index[MDNIE_STEP1] = MDNIE_STEP1_INDEX;
	mdnie_data.mdnie_step_index[MDNIE_STEP2] = MDNIE_STEP2_INDEX;
	mdnie_data.address_scr_white[ADDRESS_SCR_WHITE_RED_OFFSET] = ADDRESS_SCR_WHITE_RED;
	mdnie_data.address_scr_white[ADDRESS_SCR_WHITE_GREEN_OFFSET] = ADDRESS_SCR_WHITE_GREEN;
	mdnie_data.address_scr_white[ADDRESS_SCR_WHITE_BLUE_OFFSET] = ADDRESS_SCR_WHITE_BLUE;
	mdnie_data.dsi0_rgb_sensor_mdnie_1_size = DSI0_RGB_SENSOR_MDNIE_1_SIZE;
	mdnie_data.dsi0_rgb_sensor_mdnie_2_size = DSI0_RGB_SENSOR_MDNIE_2_SIZE;

	mdnie_data.dsi0_adjust_ldu_table = adjust_ldu_data;
	mdnie_data.dsi0_max_adjust_ldu = 6;
	mdnie_data.dsi0_scr_step_index = MDNIE_STEP2_INDEX;
	mdnie_data.dsi0_white_default_r = 0xff;
	mdnie_data.dsi0_white_default_g = 0xff;
	mdnie_data.dsi0_white_default_b = 0xff;
	mdnie_data.dsi0_white_rgb_enabled = 0;
}

static void mdss_panel_init(struct samsung_display_driver_data *vdd)
{
	pr_info("S6E3FA3_AMS568HN31 : %s\n", __func__);

	/* ON/OFF */
	vdd->panel_func.samsung_panel_on_pre = mdss_panel_on_pre;
	vdd->panel_func.samsung_panel_on_post = mdss_panel_on_post;

	/* DDI RX */
	vdd->panel_func.samsung_panel_revision = mdss_panel_revision;
	vdd->panel_func.samsung_manufacture_date_read = mdss_manufacture_date_read;
	vdd->panel_func.samsung_ddi_id_read = mdss_ddi_id_read;
	vdd->panel_func.samsung_cell_id_read = mdss_cell_id_read;
	vdd->panel_func.samsung_elvss_read = mdss_elvss_read;
	vdd->panel_func.samsung_hbm_read = mdss_hbm_read;
	vdd->panel_func.samsung_mdnie_read = mdss_mdnie_read;
	vdd->panel_func.samsung_smart_dimming_init = mdss_smart_dimming_init;
	vdd->panel_func.samsung_smart_get_conf = smart_get_conf_S6E3FA3_AMS568HN31;

	/* Brightness */
	vdd->panel_func.samsung_brightness_hbm_off = mdss_hbm_off;
	vdd->panel_func.samsung_brightness_aid = mdss_aid;
	vdd->panel_func.samsung_brightness_acl_on = mdss_acl_on;
	vdd->panel_func.samsung_brightness_acl_percent = NULL;
	vdd->panel_func.samsung_brightness_acl_off = mdss_acl_off;
	vdd->panel_func.samsung_brightness_elvss = mdss_elvss;
	vdd->panel_func.samsung_brightness_elvss_temperature1 = mdss_elvss_temperature1;
	vdd->panel_func.samsung_brightness_elvss_temperature2 = mdss_elvss_temperature2;
	vdd->panel_func.samsung_brightness_vint = mdss_vint;
	vdd->panel_func.samsung_brightness_gamma = mdss_gamma;

	/* default brightness */
	vdd->bl_level = 255;

	/* HBM */
	vdd->panel_func.samsung_hbm_gamma = mdss_hbm_gamma;
	vdd->panel_func.samsung_hbm_etc = mdss_hbm_etc;
	vdd->panel_func.get_hbm_candela_value = get_hbm_candela_value;

	vdd->manufacture_id_dsi[0] = PBA_ID;
	vdd->auto_brightness_level = 12;

	/* AID Interpolation */
	vdd->aid_subdivision_enable = true;

	/* MDNIE */
	vdd->support_mdnie_lite = true;
	vdd->support_mdnie_trans_dimming = false;
	/* for mdnie tuning */
	vdd->mdnie_tune_size[0] = sizeof(DSI0_BYPASS_MDNIE_1);
	vdd->mdnie_tune_size[1] = sizeof(DSI0_BYPASS_MDNIE_2);
	dsi_update_mdnie_data();

	/* Enable panic on first pingpong timeout */
	vdd->debug_data->panic_on_pptimeout = true;

	/* ACL default ON */
	vdd->acl_status = 1;
}

static int __init samsung_panel_init(void)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();
	char panel_string[] = "ss_dsi_panel_S6E3FA3_AMS568HN31_FHD";

	vdd->panel_name = mdss_mdp_panel + 8;
	pr_info("%s : %s\n", __func__, vdd->panel_name);

	if (!strncmp(vdd->panel_name, panel_string, strlen(panel_string)))
		vdd->panel_func.samsung_panel_init = mdss_panel_init;

	return 0;
}
early_initcall(samsung_panel_init);
