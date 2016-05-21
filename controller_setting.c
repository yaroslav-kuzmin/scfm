/*****************************************************************************/
/*                                                                           */
/* сулс - система управления лафетными стволами                              */
/*  Авторское Право (C) <2015> <Кузьмин Ярослав>                             */
/*                                                                           */
/*  Эта программа является свободным программным обеспечением:               */
/*  вы можете распространять и/или изменять это в соответствии с             */
/*  условиями в GNU General Public License, опубликованной                   */
/*  Фондом свободного программного обеспечения, как версии 3 лицензии,       */
/*  или (по вашему выбору) любой более поздней версии.                       */
/*                                                                           */
/*  Эта программа распространяется в надежде, что она будет полезной,        */
/*  но БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ; даже без подразумеваемой гарантии            */
/*  Или ПРИГОДНОСТИ ДЛЯ КОНКРЕТНЫХ ЦЕЛЕЙ. См GNU General Public License      */
/*  для более подробной информации.                                          */
/*                                                                           */
/*  Вы должны были получить копию GNU General Public License                 */
/*  вместе с этой программой. Если нет, см <http://www.gnu.org/licenses/>    */
/*                                                                           */
/*  Адрес для контактов: kuzmin.yaroslav@gmail.com                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* scfm - system control fire monitors                                       */
/*  Copyright (C) <2015> <Kuzmin Yaroslav>                                   */
/*                                                                           */
/*  This program is free software: you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation, either version 3 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*                                                                           */
/*  Email contact: kuzmin.yaroslav@gmail.com                                 */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
#include <gio/gio.h>

#include "pub.h"
#include "common.h"
#include "device.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

struct _block_info_controller_s
{
	GtkLabel * label_name;
	GtkWidget * box_engine_vertical;
	GtkWidget * box_engine_horizontal;
	GtkWidget * box_actuator_spray;
	GtkWidget * box_actuator_rate;
	GtkWidget * box_actuator_veil;
	GtkWidget * box_limit_vertical;
	GtkWidget * box_tic_vertical;
	GtkEntryBuffer * buf_tic_vertical;
	GtkWidget * box_encoder_vertical;
	GtkEntryBuffer * buf_encoder_vertical;
	GtkWidget * box_amperage_vertical;
	GtkEntryBuffer * buf_amperage_vertical;
	GtkWidget * box_limit_horizontal;
	GtkWidget * box_tic_horizontal;
	GtkEntryBuffer * buf_tic_horizontal;
	GtkWidget * box_encoder_horizontal;
	GtkEntryBuffer * buf_encoder_horizontal;
	GtkWidget * box_amperage_horizontal;
	GtkEntryBuffer * buf_amperage_horizontal;
	GtkWidget * box_pressure;
	GtkEntryBuffer * buf_pressure;
	GtkWidget * box_console;
	GtkWidget * box_post;
	GtkWidget * box_sensor_fire_dry;
	GtkWidget * box_sensor_fire_485;
	GtkWidget * box_sensor_fire_ethernet;
	GtkWidget * box_sensor_dry_485;
	GtkWidget * box_sensor_dry_etherner;
	GtkWidget * box_valve_dry;
	GtkWidget * box_valve_analog;
	GtkEntryBuffer * buf_valve_analog;
	GtkWidget * box_valve_limit;
	GtkWidget * box_valve_feedback;
	GtkWidget * box_cam_analog_dc;
	GtkWidget * box_cam_digital_dc;
	GtkWidget * box_cam_digital_poe;
	GtkWidget * box_fire_alarm_01;
	GtkWidget * box_fire_alarm_02;
	GtkWidget * box_fire_alarm_03;
	GtkWidget * box_fire_alarm_04;
	GtkWidget * box_fire_alarm_05;
	GtkWidget * box_fire_alarm_06;
	GtkWidget * box_fire_alarm_07;
	GtkWidget * box_fire_alarm_08;
	GtkWidget * box_fire_alarm_09;
	GtkWidget * box_fire_alarm_10;
	GtkWidget * box_fire_alarm_11;
	GtkWidget * box_fire_alarm_12;
	GtkWidget * box_fire_alarm_13;
	GtkWidget * box_fire_alarm_14;
	GtkWidget * box_fire_alarm_15;
	GtkWidget * box_fire_alarm_16;
	GtkWidget * box_device_01_state_0;
	GtkWidget * box_device_01_state_1;
	GtkWidget * box_device_02_state_0;
	GtkWidget * box_device_02_state_1;
	GtkWidget * box_device_03_state_0;
	GtkWidget * box_device_03_state_1;
	GtkWidget * box_device_04_state_0;
	GtkWidget * box_device_04_state_1;
	GtkWidget * box_device_05_state_0;
	GtkWidget * box_device_05_state_1;
	GtkWidget * box_device_06_state_0;
	GtkWidget * box_device_06_state_1;
	GtkWidget * box_device_07_state_0;
	GtkWidget * box_device_07_state_1;
	GtkWidget * box_device_08_state_0;
	GtkWidget * box_device_08_state_1;
};
typedef struct _block_info_controller_s block_info_controller_s;

#define SELECT_BLOCK_FIND_TCP    1
#define SELECT_BLOCK_FIND_UART   2
struct _block_setting_controller_s
{
	int select_block_find;
	GtkWidget * block_find_type_tcp;
	GtkEntryBuffer * tcp_id;
	GtkEntryBuffer * tcp_address;
	GtkEntryBuffer * tcp_port;

	GtkWidget * block_find_type_uart;
	GtkEntryBuffer * uart_id;
	GtkEntryBuffer * uart_device;
	GtkEntryBuffer * uart_baud;
	GtkEntryBuffer * uart_parity;
	GtkEntryBuffer * uart_data_bit;
	GtkEntryBuffer * uart_stop_bit;

	block_info_controller_s * block_info;

	link_s * link;
	char * name;
	config_controller_s * config;
	state_controller_s * state;
};
typedef struct _block_setting_controller_s block_setting_controller_s;

/*****************************************************************************/
/* Блок отображения конфигурированием контролером                            */
/*****************************************************************************/

static int fill_block_info(block_setting_controller_s * bsc)
{
	GtkLabel * label;
	GtkWidget * box;
	char * name;
	block_info_controller_s * block_info = bsc->block_info;
	config_controller_s * config = bsc->config;
	uint64_t flag;

	if(config == NULL){
		flag = 0;
	}
	else{
		flag = config->flag;
	}
	name = bsc->name;
	label = block_info->label_name;
	gtk_label_set_text(label,name);
	box = block_info->box_engine_vertical;
	if(ENGINE_VERTICAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_engine_horizontal;
	if(ENGINE_HORIZONTAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_actuator_spray;
	if(ACTUATOR_SPRAY(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_actuator_rate;
	if(ACTUATOR_RATE(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_actuator_veil;
	if(ACTUATOR_VEIL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_limit_vertical;
	if(LIMIT_VERTICAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_tic_vertical;
	if(TIC_VERTICAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_encoder_vertical;
	if(ENCODER_VERTICAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_amperage_vertical;
	if(AMPERAGE_VERTICAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_limit_horizontal;
	if(LIMIT_HORIZONTAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_tic_horizontal;
	if(TIC_HORIZONTAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_encoder_horizontal;
	if(ENCODER_HORIZONTAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_amperage_horizontal;
	if(AMPERAGE_HORIZONTAL(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_pressure;
	if(PRESSURE(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_console;
	if(CONSOLE(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_post;
	if(POST(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_sensor_fire_dry;
	if(SENSOR_FIRE_DRY(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_sensor_fire_485;
	if(SENSOR_FIRE_485(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_sensor_fire_ethernet;
	if(SENSOR_FIRE_ETHERNET(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_sensor_dry_485;
	if(SENSOR_DRY_485(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_sensor_dry_etherner;
	if(SENSOR_DRY_ETHERNET(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_valve_dry;
	if(VALVE_DRY(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_valve_analog;
	if(VALVE_ANALOG(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_valve_limit;
	if(VALVE_LIMIT(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_valve_feedback;
	if(VALVE_FEEDBACK(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_cam_analog_dc;
	if(CAM_ANALOG_DC(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_cam_digital_dc;
	if(CAM_DIGITAL_DC(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_cam_digital_poe;
	if(CAM_DIGITAL_POE(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_01;
	if(FIRE_ALARM_01(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_02;
	if(FIRE_ALARM_02(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_03;
	if(FIRE_ALARM_03(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_04;
	if(FIRE_ALARM_04(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_05;
	if(FIRE_ALARM_05(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_06;
	if(FIRE_ALARM_06(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_07;
	if(FIRE_ALARM_07(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_08;
	if(FIRE_ALARM_08(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_09;
	if(FIRE_ALARM_09(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_10;
	if(FIRE_ALARM_10(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_11;
	if(FIRE_ALARM_11(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_12;
	if(FIRE_ALARM_12(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_13;
	if(FIRE_ALARM_13(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_14;
	if(FIRE_ALARM_14(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_15;
	if(FIRE_ALARM_15(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_fire_alarm_16;
	if(FIRE_ALARM_16(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_01_state_0;
	if(DEVICE_01_STATE_0(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_01_state_1;
	if(DEVICE_01_STATE_1(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_02_state_0;
	if(DEVICE_02_STATE_0(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_02_state_1;
	if(DEVICE_02_STATE_1(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_03_state_0;
	if(DEVICE_03_STATE_0(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_03_state_1;
	if(DEVICE_03_STATE_1(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_04_state_0;
	if(DEVICE_04_STATE_0(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_04_state_1;
	if(DEVICE_04_STATE_1(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_05_state_0;
	if(DEVICE_05_STATE_0(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_05_state_1;
	if(DEVICE_05_STATE_1(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_06_state_0;
	if(DEVICE_06_STATE_0(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_06_state_1;
	if(DEVICE_06_STATE_1(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_07_state_0;
	if(DEVICE_07_STATE_0(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_07_state_1;
	if(DEVICE_07_STATE_1(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_08_state_0;
	if(DEVICE_08_STATE_0(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	box = block_info->box_device_08_state_1;
	if(DEVICE_08_STATE_1(flag)){
		gtk_widget_show(box);
	}
	else{
		gtk_widget_hide(box);
	}
	return SUCCESS;
}

static int check_rate_controller(block_info_controller_s * block_info,config_controller_s * config)
{
	const char * str;
	double rate;
	GtkEntryBuffer * buf;

	buf = block_info->buf_tic_vertical;
	str = gtk_entry_buffer_get_text(buf);
	rate = g_strtod(str,NULL);
	config->rate_tic_vertical = rate;

	buf = block_info->buf_encoder_vertical;
	str = gtk_entry_buffer_get_text(buf);
	rate = g_strtod(str,NULL);
	config->rate_encoder_vertical = rate;

	buf = block_info->buf_amperage_vertical;
	str = gtk_entry_buffer_get_text(buf);
	rate = g_strtod(str,NULL);
	config->rate_amperage_vertical = rate;

	buf = block_info->buf_tic_horizontal;
	str = gtk_entry_buffer_get_text(buf);
	rate = g_strtod(str,NULL);
	config->rate_tic_horizontal = rate;

	buf = block_info->buf_encoder_horizontal;
	str = gtk_entry_buffer_get_text(buf);
	rate = g_strtod(str,NULL);
	config->rate_encoder_horizontal = rate;

	buf = block_info->buf_amperage_horizontal;
	str = gtk_entry_buffer_get_text(buf);
	rate = g_strtod(str,NULL);
	config->rate_amperage_horizontal = rate;

	buf = block_info->buf_pressure;
	str = gtk_entry_buffer_get_text(buf);
	rate = g_strtod(str,NULL);
	config->rate_pressure = rate;

	buf = block_info->buf_valve_analog;
	str = gtk_entry_buffer_get_text(buf);
	rate = g_strtod(str,NULL);
	config->rate_valve_analog = rate;

	return SUCCESS;
}

static GtkWidget * create_block_info_line(char * str,int len,GtkEntryBuffer ** buf,char * default_buf,int len_buf)
{
	GtkWidget * box;
	GtkWidget * label;
	GtkEntryBuffer * entry_buf;
	GtkWidget * entry;
	int width = len_buf;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),TRUE);

	label = gtk_label_new(str);
	layout_widget(label,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,TRUE);
	gtk_widget_set_size_request(label,len,-1);

	if(buf != NULL){
		entry_buf = gtk_entry_buffer_new(default_buf,-1);
		gtk_entry_buffer_set_max_length(entry_buf,width);
		entry = gtk_entry_new_with_buffer(entry_buf);
		gtk_entry_set_max_length(GTK_ENTRY(entry),width);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),width);
		layout_widget(entry,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,TRUE);
		*buf = entry_buf;
	}

	gtk_box_pack_start(GTK_BOX(box),label,TRUE,TRUE,0);
	if(buf != NULL){
		gtk_box_pack_start(GTK_BOX(box),entry,TRUE,TRUE,0);
	}

	gtk_widget_show(box);
	gtk_widget_show(label);
	if(buf != NULL){
		gtk_widget_show(entry);
	}

	return box;
}
static block_info_controller_s block_info_controller;

static int32_t min_width_block_info = 300;
static int32_t min_height_block_info = 400;

static GtkWidget * create_block_info(block_setting_controller_s * bsc)
{
	GtkWidget * scrwin;
	GtkWidget * box_main;
	GtkWidget * box;
	GtkWidget * label;
	GtkEntryBuffer * buf;
	block_info_controller_s * block_info = &block_info_controller;

	scrwin = gtk_scrolled_window_new(NULL,NULL);
	layout_widget(scrwin,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrwin),min_width_block_info);
	gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrwin),min_height_block_info);

	box_main = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box_main,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box_main),TRUE);

	/*наименование*/
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);
	gtk_widget_show(box);
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,0);
	label = gtk_label_new("Наименование : ");
	layout_widget(label,GTK_ALIGN_START,GTK_ALIGN_START,FALSE,TRUE);
	gtk_box_pack_start(GTK_BOX(box),label,FALSE,TRUE,0);
	gtk_widget_show(label);
	label = gtk_label_new("Нет информации");
	layout_widget(label,GTK_ALIGN_START,GTK_ALIGN_START,FALSE,TRUE);
	gtk_box_pack_start(GTK_BOX(box),label,FALSE,TRUE,0);
	gtk_widget_show(label);
	block_info->label_name = GTK_LABEL(label);

#define LEN_INFO_LINE        100
#define SPACING_INFO_LINE    2

	box = create_block_info_line("Двигатель вертикальной оси",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_engine_vertical = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Двигатель горизонтальной оси",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_engine_horizontal = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Актуатор (Распыл)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_actuator_spray = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Актуатор (Расход)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_actuator_rate = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Актуатор (Завеса)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_actuator_veil = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Концевики вертикальной оси",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_limit_vertical = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик импульсов вертикальной оси",LEN_INFO_LINE,&buf,"6",5);
	block_info->box_tic_vertical = box;
	block_info->buf_tic_vertical = buf;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Энкодер вертикальной оси",LEN_INFO_LINE,&buf,"1",5);
	block_info->box_encoder_vertical = box;
	block_info->buf_encoder_vertical = buf;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик тока вертикальной оси",LEN_INFO_LINE,&buf,"1",5);
	block_info->box_amperage_vertical = box;
	block_info->buf_amperage_vertical = buf;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Концевики горизонтальной оси",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_limit_horizontal = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик импульсов горизонтальной оси",LEN_INFO_LINE,&buf,"6",5);
	block_info->box_tic_horizontal = box;
	block_info->buf_tic_horizontal = buf;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Энкодер горизонтальной оси",LEN_INFO_LINE,&buf,"1",5);
	block_info->box_encoder_horizontal = box;
	block_info->buf_encoder_horizontal = buf;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик тока горизонтальной оси",LEN_INFO_LINE,&buf,"1",5);
	block_info->box_amperage_horizontal = box;
	block_info->buf_amperage_horizontal = buf;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик давления",LEN_INFO_LINE,&buf,"1",5);
	block_info->box_pressure = box;
	block_info->buf_pressure = buf;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Пульт ПДУ-П",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_console = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Пост ПДУ с пультом ПДУ-П",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_post = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик пламени (сухой контакт)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_sensor_fire_dry = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик пламени (интерфейс RS-485)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_sensor_fire_485 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик пламени (интерфейс Ethernet)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_sensor_fire_ethernet = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик (Сухой контакт + Интерфейс RS-485)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_sensor_dry_485 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Датчик (Сухой контакт + Интерфейс Ethernet)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_sensor_dry_etherner = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Заслонка (Сухой контакт)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_valve_dry = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Заслонка (Аналоговый сигнал)",LEN_INFO_LINE,&buf,"0.5",5);
	block_info->box_valve_analog = box;
	block_info->buf_valve_analog = buf;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Заслонка (Концевики)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_valve_limit = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Заслонка (Обратная связь)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_valve_feedback = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Аналоговая видеокамера (DC)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_cam_analog_dc = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Цифровая видеокамера (DC)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_cam_digital_dc = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Цифровая видеокамера (POE)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_cam_digital_poe = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("1 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_01 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("2 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_02 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("3 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_03 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("4 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_04 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("5 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_05 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("6 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_06 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("7 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_07 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("8 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_08 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("9 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_09 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("10 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_10 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("11 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_11 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("12 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_12 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("13 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_13 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("14 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_14 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("15 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_15 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("16 шлейф ПС",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_fire_alarm_16 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 1 (Состояние А)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_01_state_0 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 1 (Состояние Б)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_01_state_1 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 2 (Состояние А)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_02_state_0 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 2 (Состояние Б)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_02_state_1 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 3 (Состояние А)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_03_state_0 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 3 (Состояние Б)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_03_state_1 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 4 (Состояние А)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_04_state_0 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 4 (Состояние Б)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_04_state_1 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 5 (Состояние А)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_05_state_0 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 5 (Состояние Б)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_05_state_1 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 6 (Состояние А)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_06_state_0 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 6 (Состояние Б)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_06_state_1 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 7 (Состояние А)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_07_state_0 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 7 (Состояние Б)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_07_state_1 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 8 (Состояние А)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_08_state_0 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	box = create_block_info_line("Устройство 8 (Состояние Б)",LEN_INFO_LINE,NULL,NULL,0);
	block_info->box_device_08_state_1 = box;
	gtk_box_pack_start(GTK_BOX(box_main),box,TRUE,TRUE,SPACING_INFO_LINE);

	gtk_container_add(GTK_CONTAINER(scrwin),box_main);

	gtk_widget_show(box_main);
	gtk_widget_show(scrwin);

	bsc->block_info = block_info;
	fill_block_info(bsc);

	return scrwin;
}

static uint8_t check_id(const char * id)
{
	uint8_t t = 0;
	t = g_ascii_strtoull(id,NULL,10);
	if(t < MIN_ID){
		t = 0;
	}
	else{
		if(t > MAX_ID){
			t = 0;
		}
	}
	return t;
}

static char * check_tcp_address(const char * address)
{
	char * str;
	GInetAddress * inet_address = g_inet_address_new_from_string(address);
	if(inet_address == NULL){
		return NULL;
	}
	str = g_inet_address_to_string(inet_address);
	g_object_unref(inet_address);
	return str;
}

static uint16_t check_tcp_port(const char * port)
{
	uint16_t p = 0;
	p = g_ascii_strtoull(port,NULL,10);
	return p;
}

static link_s * check_option_tcp(block_setting_controller_s * bsc)
{
	link_s * link = NULL;
	uint8_t id;
	char * address = NULL;
	uint16_t port;
	const char * str;

	/*TODO вывод сообщений*/
	str = gtk_entry_buffer_get_text(bsc->tcp_id);
	id = check_id(str);
	if(id == 0){
		g_warning("Индификатор контролера не корректный!");
		return link;
	}
	str = gtk_entry_buffer_get_text(bsc->tcp_address);
	address = check_tcp_address(str);
	if(address == NULL){
		g_warning("Адрес контролера не корректный!");
		return link;
	}
	str = gtk_entry_buffer_get_text(bsc->tcp_port);
	port = check_tcp_port(str);
	if(port == 0){
		g_warning("Порт контролера не корректный!");
		return link;
	}

	link = g_slice_alloc0(sizeof(link_s));
	link->id = id;
	link->connect = NULL;

	link->type = TYPE_LINK_TCP;
	link->address = address;
	link->port = port;

	link->device = STR_EMPTY;
	link->baud = 0;
	link->parity = 0;
	link->data_bit = 0;
	link->stop_bit = 0;

	return link;
}

static char * check_uart_device(const char * device)
{
	/*TODO в Linux и Windos разные название файлов*/
	return g_strdup(device);
}

static uint32_t check_uart_baud(const char * baud)
{
	uint32_t speed = g_ascii_strtoull(baud,NULL,10);
	return speed;
}

static int8_t check_uart_parity(const char * parity)
{
	int8_t symbol = parity[0];
	/*N нет четности : E четно : O не четно */
	if(symbol != 'N'){
		if(symbol != 'E'){
			if(symbol != 'O'){
				symbol = 0;
			}
		}
	}
	return symbol;
}

static uint8_t check_uart_data_bit(const char * data_bit)
{
	uint8_t bit = g_ascii_strtoull(data_bit,NULL,10);
	if( (bit < 5) || (bit > 8)){
		bit = 0;
	}
	return bit;
}

static uint8_t check_uart_stop_bit(const char * stop_bit)
{
	uint8_t bit = g_ascii_strtoull(stop_bit,NULL,10);
	if(bit != 1){
		if(bit != 2){
			bit = 0;
		}
	}
	return bit;
}

static link_s * check_option_uart(block_setting_controller_s * bsc)
{
	link_s * link = NULL;
	uint8_t id;
	char * device = NULL;
	uint32_t baud;
	int8_t parity;
	uint8_t data_bit;
	uint8_t stop_bit;
	const char * str;

	/*TODO вывод сообщений*/
	str = gtk_entry_buffer_get_text(bsc->uart_id);
	id = check_id(str);
	if(id == 0){
		g_warning("Индификатор контролера не корректный!");
		return link;
	}
	str = gtk_entry_buffer_get_text(bsc->uart_device);
	device = check_uart_device(str);
	if(device == NULL){
		g_warning("Название файла не корректно!");
		return link;
	}
	str = gtk_entry_buffer_get_text(bsc->uart_baud);
	baud = check_uart_baud(str);
	if(baud == 0){
		g_warning("Скорость не корректна!");
		return link;
	}
	str = gtk_entry_buffer_get_text(bsc->uart_parity);
	parity = check_uart_parity(str);
	if(parity == 0){
		g_warning("Четность не корректна!");
		return link;
	}
	str = gtk_entry_buffer_get_text(bsc->uart_data_bit);
	data_bit = check_uart_data_bit(str);
	if(data_bit == 0){
		g_warning("Бит дата не корректна!");
		return link;
	}
	str = gtk_entry_buffer_get_text(bsc->uart_stop_bit);
	stop_bit = check_uart_stop_bit(str);
	if(stop_bit == 0){
		g_warning("Стоп бит не корректна!");
		return link;
	}

	link = g_slice_alloc0(sizeof(link_s));
	link->id = id;
	link->connect = NULL;
	link->type = TYPE_LINK_UART;
	link->device = device;
	link->baud = baud;
	link->parity = parity;
	link->data_bit = data_bit;
	link->stop_bit = stop_bit;

	link->address = STR_EMPTY;
	link->port = 0;

	return link;
}

static void clicked_button_check(GtkButton * button,gpointer ud)
{
	int rc;
	block_setting_controller_s * bsc = (block_setting_controller_s*)ud;
	link_s * link = NULL;
	config_controller_s * config;
	state_controller_s * state;

	if(bsc->select_block_find == SELECT_BLOCK_FIND_TCP){
		link = check_option_tcp(bsc);
	}
	else{
		link = check_option_uart(bsc);
	}
	if(link == NULL){
		return ;
	}

	/*TODO при повторном нажатии утечка памяти*/
	bsc->link = NULL;

	config = g_slice_alloc0(sizeof(config_controller_s));
	state = g_slice_alloc0(sizeof(state_controller_s));

	rc = link_controller(link,config,state);
	if(rc == FAILURE){
		g_slice_free1(sizeof(link_s),link);
		g_slice_free1(sizeof(config_controller_s),config);
		g_slice_free1(sizeof(state_controller_s),state);
		return;
	}

	bsc->link = link;
	bsc->config = config;
	bsc->state = state;
	bsc->name = controller_name(config);
	/*TODO сообщенийние что проверка корректна*/
	fill_block_info(bsc);

	link_controller_disconnect(link);
}

static void clicked_radio_button_tcp(GtkRadioButton * rb,gpointer ud)
{
	block_setting_controller_s * bsc = (block_setting_controller_s*)ud;
	GtkWidget * block_tcp = bsc->block_find_type_tcp;
	GtkWidget * block_uart = bsc->block_find_type_uart;
	bsc->select_block_find = SELECT_BLOCK_FIND_TCP;
	gtk_widget_hide(block_uart);
	gtk_widget_show(block_tcp);
}

static void clicked_radio_button_uart(GtkRadioButton * rb,gpointer ud)
{
	block_setting_controller_s * bsc = (block_setting_controller_s*)ud;
	GtkWidget * block_tcp = bsc->block_find_type_tcp;
	GtkWidget * block_uart = bsc->block_find_type_uart;
	bsc->select_block_find = SELECT_BLOCK_FIND_UART;
	gtk_widget_hide(block_tcp);
	gtk_widget_show(block_uart);
}

static GtkWidget * create_block_select_type(block_setting_controller_s * bsc)
{
	GtkWidget * grid;
	GtkWidget * lab_type;
	GtkWidget * lab_tcp;
	GtkWidget * but_tcp;
	GtkWidget * lab_uart;
	GtkWidget * but_uart;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	lab_type = gtk_label_new("Тип соединения");
	layout_widget(lab_type,GTK_ALIGN_START,GTK_ALIGN_START,FALSE,FALSE);

	lab_tcp = gtk_label_new("Интернет");
	layout_widget(lab_tcp,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);

	but_tcp = gtk_radio_button_new(NULL);
	g_signal_connect(but_tcp,"clicked",G_CALLBACK(clicked_radio_button_tcp),bsc);
	layout_widget(but_tcp,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	lab_uart = gtk_label_new("последовательный порт");
	layout_widget(lab_uart,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);

	but_uart = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(but_tcp));
	g_signal_connect(but_uart,"clicked",G_CALLBACK(clicked_radio_button_uart),bsc);
	layout_widget(but_uart,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	gtk_grid_attach(GTK_GRID(grid),lab_type,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),lab_tcp ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_tcp ,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),lab_uart,2,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_uart,2,1,1,1);

	gtk_widget_show(grid);
	gtk_widget_show(lab_type);
	gtk_widget_show(lab_tcp);
	gtk_widget_show(but_tcp);
	gtk_widget_show(lab_uart);
	gtk_widget_show(but_uart);

	return grid;
}

static GtkWidget * create_block_entry(char * name,GtkEntryBuffer ** buf)
{
	GtkWidget * box;
	GtkWidget * label;
	GtkWidget * entry;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_CENTER,TRUE,FALSE);

	label = gtk_label_new(name);
	layout_widget(label,GTK_ALIGN_START,GTK_ALIGN_CENTER,FALSE,FALSE);

	entry = gtk_entry_new();
	layout_widget(entry,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,FALSE);
	*buf = gtk_entry_get_buffer(GTK_ENTRY(entry));

	gtk_box_pack_start(GTK_BOX(box),label,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),entry,TRUE,TRUE,0);

	gtk_widget_show(box);
	gtk_widget_show(label);
	gtk_widget_show(entry);

	return box;
}

static GtkWidget * create_block_find_type_tcp(block_setting_controller_s * bsc)
{
	GtkWidget * box;
	GtkWidget * block_id;
	GtkWidget * block_address;
	GtkWidget * block_port;
	GtkEntryBuffer * buf;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_id = create_block_entry("Номер контролера",&buf);
	gtk_entry_buffer_set_text(buf,"2",-1);
	bsc->tcp_id = buf;
	block_address = create_block_entry("Адрес",&buf);
	gtk_entry_buffer_set_text(buf,"127.0.0.1",-1);
	bsc->tcp_address = buf;
	block_port = create_block_entry("Порт",&buf);
	gtk_entry_buffer_set_text(buf,"1502",-1);
	bsc->tcp_port = buf;

	gtk_box_pack_start(GTK_BOX(box),block_id,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_address,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_port,TRUE,TRUE,0);

	gtk_widget_show(box);
	return box;
}

static GtkWidget * create_block_find_type_uart(block_setting_controller_s * bsc)
{
	GtkWidget * box;
	GtkWidget * block_id;
	GtkWidget * block_device;
	GtkWidget * block_baud;
	GtkWidget * block_parity;
	GtkWidget * block_data_bit;
	GtkWidget * block_stop_bit;
	GtkEntryBuffer * buf;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_id = create_block_entry("Номер контролера",&buf);
	gtk_entry_buffer_set_text(buf,"2",-1);
	bsc->uart_id = buf;
	block_device = create_block_entry("Устройство",&buf);
	gtk_entry_buffer_set_text(buf,"\\\\.\\COM7",-1);
	bsc->uart_device = buf;
	block_baud = create_block_entry("Скорость",&buf);
	gtk_entry_buffer_set_text(buf,"9600",-1);
	bsc->uart_baud = buf;
	block_parity = create_block_entry("Четность",&buf);
	gtk_entry_buffer_set_text(buf,"E",-1);
	bsc->uart_parity = buf;
	block_data_bit = create_block_entry("Бит данных",&buf);
	gtk_entry_buffer_set_text(buf,"8",-1);
	bsc->uart_data_bit = buf;
	block_stop_bit = create_block_entry("Стоп бит",&buf);
	gtk_entry_buffer_set_text(buf,"1",-1);
	bsc->uart_stop_bit = buf;

	gtk_box_pack_start(GTK_BOX(box),block_id,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_device,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_baud,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_parity,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_data_bit,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_stop_bit,TRUE,TRUE,0);

	gtk_widget_show(box);
	return box;
}

static GtkWidget * create_block_find_type(block_setting_controller_s * bsc)
{
	GtkWidget * grid;
	GtkWidget * block_find_type_tcp;
	GtkWidget * block_find_type_uart;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_find_type_tcp = create_block_find_type_tcp(bsc);
	bsc->block_find_type_tcp = block_find_type_tcp;

	block_find_type_uart = create_block_find_type_uart(bsc);
	bsc->block_find_type_uart = block_find_type_uart;

	gtk_grid_attach(GTK_GRID(grid),block_find_type_tcp,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_find_type_uart,0,0,1,1);

	gtk_widget_show(grid);
	bsc->select_block_find = SELECT_BLOCK_FIND_TCP;
	gtk_widget_show(block_find_type_tcp);
	gtk_widget_hide(block_find_type_uart);

	return grid;
}

static GtkWidget * create_block_find(block_setting_controller_s * bsc)
{
	GtkWidget * box;
	GtkWidget * block_select_type;
	GtkWidget * block_find_type;
	GtkWidget * but_check;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_select_type = create_block_select_type(bsc);

	block_find_type = create_block_find_type(bsc);

	but_check = gtk_button_new_with_label("Поиск");
	layout_widget(but_check,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_check,"clicked",G_CALLBACK(clicked_button_check),bsc);

	gtk_box_pack_start(GTK_BOX(box),block_select_type,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_find_type,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),but_check,FALSE,FALSE,0);

	gtk_widget_show(box);
	gtk_widget_show(but_check);

	return box;
}
static block_setting_controller_s block_setting_controller;

static char STR_NAME_DEFAULT[] = "НЕТ ИНФОРМАЦИИ";

GtkWidget * create_block_setting_controller(void)
{
	GtkWidget * box;
	GtkWidget * block_find;
	GtkWidget * block_info;

	block_setting_controller.link = NULL;
	block_setting_controller.config = NULL;
	block_setting_controller.name = STR_NAME_DEFAULT;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_find = create_block_find(&block_setting_controller);

	block_info = create_block_info(&block_setting_controller);

	gtk_box_pack_start(GTK_BOX(box),block_find,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_info,TRUE,TRUE,0);

	gtk_widget_show(box);
	return box;
}

/*****************************************************************************/
/* Выделение и высвобождения памяти для структур контролера                  */
/*****************************************************************************/
/*выделение памяти при конфигурировании */
void * new_property_controller(void)
{
	int rc;
	controller_s * controller;
	link_s * link = block_setting_controller.link;
	config_controller_s * config = block_setting_controller.config;
	state_controller_s * state = block_setting_controller.state;
	block_info_controller_s * block_info = block_setting_controller.block_info;

	/*TODO вывод сообщений*/
	if(link == NULL){
		g_warning("Соединение не проверено!");
		return NULL;
	}

	rc = check_rate_controller(block_info,config);
	if(rc == FAILURE){
		g_warning("Некоректный коэффициенты");
		return NULL;
	}
	controller = g_slice_alloc0(sizeof(controller_s));
	controller->name = block_setting_controller.name;
	controller->link = link;
	controller->config = config;
	controller->state = state;
	controller->control = g_slice_alloc0(sizeof(control_controller_s));
	controller->control->command.all = COMMAND_EMPTY;

	return controller;
}

/*****************************************************************************/
