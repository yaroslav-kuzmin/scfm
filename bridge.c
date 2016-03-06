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

#include "pub.h"
#include "common.h"
#include "log.h"
#include "modbus.h"
#include "device.h"

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
enum
{
	CELL_SERVER = 0,
	CELL_CLIENT,
	AMOUNT_CELL
};

struct _cell_s
{
	GtkEntryBuffer * device;
	GtkEntryBuffer * baud;
	GtkEntryBuffer * parity;
	GtkEntryBuffer * data_bit;
	GtkEntryBuffer * stop_bit;
	GtkEntryBuffer * id;
	GtkEntryBuffer * begin_register;
	GtkEntryBuffer * amount_register;

	link_s * link;

	uint16_t b_register;
	uint16_t a_register;
	modbus_mapping_t * mb_mapping;
};
typedef struct _cell_s cell_s;
struct _block_bridge_s
{
	int connect;
	cell_s * cell[AMOUNT_CELL];
	GThread * t_bridge;
	GMutex m_bridge;
};
typedef struct _block_bridge_s block_bridge_s;

/*****************************************************************************/
static GtkWidget * create_block_state_input(char * name,uint16_t len,char * str_default
                                           ,GtkEntryBuffer ** buff,uint16_t size_buff)
{
	GtkWidget * box;
	GtkWidget * label;
	GtkWidget * entry;
	GtkEntryBuffer * entry_buff;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),TRUE);

	label = gtk_label_new(name);
	layout_widget(label,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,TRUE);
	gtk_widget_set_size_request(label,len,-1);

	entry_buff = gtk_entry_buffer_new(str_default,-1);
	gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(entry_buff),size_buff);
	entry = gtk_entry_new_with_buffer(entry_buff);
	gtk_entry_set_max_length(GTK_ENTRY(entry),size_buff);
	gtk_entry_set_width_chars(GTK_ENTRY(entry),size_buff);
	layout_widget(entry,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,TRUE);
	*buff = GTK_ENTRY_BUFFER(entry_buff);

	gtk_box_pack_start(GTK_BOX(box),label,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),entry,TRUE,TRUE,0);

	gtk_widget_show(box);
	gtk_widget_show(label);
	gtk_widget_show(entry);
	return box;
}

#define SIZE_MIN_CELL_INPUT      60
static GtkWidget * create_block_state_cell(block_bridge_s * bb,int number_cell)
{
	GtkWidget * frame;
	GtkWidget * box;
	GtkWidget * block_device;
	GtkWidget * block_baud;
	GtkWidget * block_parity;
	GtkWidget * block_data_bit;
	GtkWidget * block_stop_bit;
	GtkWidget * block_id;
	GtkWidget * block_begin_register;
	GtkWidget * block_amount_register;
	GtkEntryBuffer * buf;
	cell_s * cell = bb->cell[number_cell];

	if(number_cell == CELL_SERVER){
		frame = gtk_frame_new("Сервер");
	}
	else{
		frame = gtk_frame_new("Клиент");
	}
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,FALSE);
	gtk_frame_set_label_align(GTK_FRAME(frame),0.5,0);
	gtk_container_set_border_width(GTK_CONTAINER(frame),3);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),TRUE);

	block_device = create_block_state_input("Устройство",SIZE_MIN_CELL_INPUT,"\\\\.\\COM7",&buf,10);
	cell->device = buf;

	block_baud = create_block_state_input("Скорость",SIZE_MIN_CELL_INPUT,"9600",&buf,7);
	cell->baud = buf;

	block_parity = create_block_state_input("Четность",SIZE_MIN_CELL_INPUT,"E",&buf,2);
	cell->parity = buf;

	block_data_bit = create_block_state_input("Бит данных",SIZE_MIN_CELL_INPUT,"8",&buf,2);
	cell->data_bit = buf;

	block_stop_bit = create_block_state_input("Стоп бит",SIZE_MIN_CELL_INPUT,"2",&buf,2);
	cell->stop_bit = buf;

	block_id = create_block_state_input("Номер контролера",SIZE_MIN_CELL_INPUT,"2",&buf,2);
	cell->id = buf;

	block_begin_register = create_block_state_input("Начальный регистр",SIZE_MIN_CELL_INPUT,"0x1064",&buf,7);
	cell->begin_register = buf;

	block_amount_register = create_block_state_input("Количество регистров",SIZE_MIN_CELL_INPUT,"0x100",&buf,7);
	cell->amount_register = buf;


	gtk_container_add(GTK_CONTAINER(frame),box);
	gtk_box_pack_start(GTK_BOX(box),block_device,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_baud,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_parity,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_data_bit,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_stop_bit,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_id,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_begin_register,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_amount_register,TRUE,TRUE,0);

	gtk_widget_show(frame);
	gtk_widget_show(box);

	return frame;
}

static GtkWidget * create_block_state(block_bridge_s * bb)
{
	GtkWidget * box;
	GtkWidget * block_state_server;
	GtkWidget * block_state_client;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,FALSE);
	gtk_box_set_homogeneous(GTK_BOX(box),TRUE);

	block_state_server = create_block_state_cell(bb,CELL_SERVER);
	block_state_client = create_block_state_cell(bb,CELL_CLIENT);

	gtk_box_pack_start(GTK_BOX(box),block_state_server,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_state_client,TRUE,TRUE,0);

	gtk_widget_show(box);
	return box;
}

/*****************************************************************************/
static char * check_uart_device(const char * device)
{
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
static uint16_t check_register(char * reg)
{
	uint16_t br = g_ascii_strtoull(reg,NULL,16);
	return br;
}
static int check_overflow(uint16_t b,uint16_t a)
{
	uint32_t c = (uint32_t)b + (uint32_t)a;
	if(c > G_MAXUINT16){
		c = 0;
	}
	return c;
}
static int fill_state(cell_s * cell)
{
	int rc;
	link_s * link = cell->link;
	char * str;
	uint8_t id;
	char * device = NULL;
	uint32_t baud;
	int8_t parity;
	uint8_t data_bit;
	uint8_t stop_bit;
	uint16_t begin_register;
	uint16_t amount_register;

	str = cell->link->device;
	if(str != NULL){
		g_free(str);
	}
	str = (char *)gtk_entry_buffer_get_text(cell->device);
	device = check_uart_device(str);
	if(device == NULL){
		return FAILURE;
	}
	str = (char*)gtk_entry_buffer_get_text(cell->baud);
	baud = check_uart_baud(str);
	if(baud == 0){
		goto exit_fill_srate_error;
	}
	str = (char*)gtk_entry_buffer_get_text(cell->parity);
	parity = check_uart_parity(str);
	if(parity == 0){
		goto exit_fill_srate_error;
	}
	str = (char*)gtk_entry_buffer_get_text(cell->data_bit);
	data_bit = check_uart_data_bit(str);
	if(data_bit == 0){
		goto exit_fill_srate_error;
	}
	str = (char*)gtk_entry_buffer_get_text(cell->stop_bit);
	stop_bit = check_uart_stop_bit(str);
	if(stop_bit == 0){
		goto exit_fill_srate_error;
	}
	str = (char*)gtk_entry_buffer_get_text(cell->id);
	id = check_id(str);
	if(id == 0){
		goto exit_fill_srate_error;
	}
	str = (char*)gtk_entry_buffer_get_text(cell->begin_register);
	begin_register = check_register(str);
	if(begin_register == 0){
		goto exit_fill_srate_error;
	}
	str = (char*)gtk_entry_buffer_get_text(cell->amount_register);
	amount_register = check_register(str);
	if(amount_register == 0){
		goto exit_fill_srate_error;
	}
	rc = check_overflow(begin_register,amount_register);
	if(rc == FAILURE){
		goto exit_fill_srate_error;
	}
	cell->b_register = begin_register;
	cell->a_register = amount_register;
	cell->mb_mapping = NULL;
	link->connect = NULL;
	link->id = id;
	link->type = TYPE_LINK_UART;
	link->dest = NULL;
	link->device = device;
	link->baud = baud;
	link->parity = parity;
	link->data_bit = data_bit;
	link->stop_bit = stop_bit;

	return SUCCESS;

exit_fill_srate_error:
	g_free(device);
	return FAILURE;
}
static int connect_bridge(block_bridge_s * bb)
{
	int rc;
	cell_s * cell_server = bb->cell[CELL_SERVER];
	cell_s * cell_client = bb->cell[CELL_CLIENT];
	link_s * link;
	uint16_t begin_registers = cell_server->b_register;
	uint16_t amount_registers = cell_server->a_register;

	link = cell_server->link;
	rc = link_connect_controller(link);
	if(rc != SUCCESS){
		return FAILURE;
	}
	link = cell_client->link;
	rc = link_connect_controller(link);
	if(rc != SUCCESS){
		link = cell_server->link;
		link_disconnect_controller(link);
		return FAILURE;
	}
	cell_server->mb_mapping = modbus_mapping_new(0,0,begin_registers + amount_registers,0);

	return SUCCESS;
}
static int disconnect_bridge(block_bridge_s * bb)
{
	cell_s * cell_server = bb->cell[CELL_SERVER];
	cell_s * cell_client = bb->cell[CELL_CLIENT];
	link_s * link;
	link = cell_server->link;
	link_disconnect_controller(link);
	link = cell_client->link;
	link_disconnect_controller(link);

	return SUCCESS;
}
static char STR_CONTROL_CONNECT[] = "Подключить";
static char STR_CONTROL_DISCONNECT[] = "Отключить";
static void clicked_button_connect(GtkButton * b,gpointer ud)
{
	int rc;
	block_bridge_s * bb = (block_bridge_s*)ud;
	cell_s * cell_server = bb->cell[CELL_SERVER];
	cell_s * cell_client = bb->cell[CELL_CLIENT];

	if(bb->connect != OK){

		rc = fill_state(cell_server);
		if(rc != SUCCESS){
			dialog_info("Не корректные данные сервера");
			return ;
		}
		rc = fill_state(cell_client);
		if(rc != SUCCESS){
			dialog_info("Не корректные данные клиента");
			return ;
		}
		rc = connect_bridge(bb);
		if(rc != SUCCESS){
			dialog_info("Несмог подключится");
			return ;
		}
		/*запуск потока проброса*/
		g_mutex_init(&(bb->m_bridge));
		bb->t_bridge = g_thread_new("bridge",work_bridge,bb);

		gtk_button_set_label(b,STR_CONTROL_DISCONNECT);
		bb->connect = OK;
	}
	else{

		bb->connect = NOT_OK;

		disconnect_bridge(bb);
		gtk_button_set_label(b,STR_CONTROL_CONNECT);
	}
}
static GtkWidget * create_block_control(block_bridge_s * bb)
{
	GtkWidget * box;
	GtkWidget * but;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,FALSE);

	but = gtk_button_new_with_label(STR_CONTROL_CONNECT);
	layout_widget(but,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but,"clicked",G_CALLBACK(clicked_button_connect),bb);
	bb->connect = NOT_OK;
	gtk_box_pack_start(GTK_BOX(box),but,TRUE,TRUE,0);
	gtk_widget_show(box);
	gtk_widget_show(but);
	return box;
}

/*****************************************************************************/

static GtkWidget * create_block_info(block_bridge_s * bb)
{
	GtkWidget * grid;
	grid = gtk_grid_new();
	gtk_widget_show(grid);
	return grid;
}
/*****************************************************************************/
static block_bridge_s block_bridge;
static GtkWidget * create_block_bridge()
{
	GtkWidget * frame;
	GtkWidget * box;
	GtkWidget * state;
	GtkWidget * info;
	GtkWidget * control;

	block_bridge.cell[CELL_SERVER] = g_malloc0(sizeof(cell_s));
	block_bridge.cell[CELL_CLIENT] = g_malloc0(sizeof(cell_s));
	block_bridge.cell[CELL_SERVER]->link = g_malloc0(sizeof(link_s));
	block_bridge.cell[CELL_CLIENT]->link = g_malloc0(sizeof(link_s));

	frame = gtk_frame_new("МОСТ");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_frame_set_label_align(GTK_FRAME(frame),0.5,1);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,3);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	state = create_block_state(&block_bridge);
	control = create_block_control(&block_bridge);
	info = create_block_info(&block_bridge);

	gtk_container_add(GTK_CONTAINER(frame),box);
	gtk_box_pack_start(GTK_BOX(box),state,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),control,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),info,FALSE,TRUE,0);

	gtk_widget_show(frame);
	gtk_widget_show(box);

	return frame;
}

/*****************************************************************************/
/* Основное окно для подключения моста                                       */
/*****************************************************************************/
static void clicked_button_exit(GtkButton * b,gpointer ud)
{
	GtkWidget * win = (GtkWidget*)ud;
	gtk_widget_destroy(win);
}
static gboolean key_press_event_window_bridge(GtkWidget * w,GdkEvent  *event,gpointer ud)
{
#if 0
	config_window_s * win = (config_window_s*)ud;
	GdkEventType type = event->type;
	gint state;
#endif
	return FALSE;
}
static void destroy_window_bridge(GtkWidget * w,gpointer ud)
{
}

int create_windows_bridge(void)
{
	GtkWidget * win_bridge;
	GtkWidget * box;
	GtkWidget * block_bridge;
	GtkWidget * exit;

	win_bridge = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(win_bridge),3);
	gtk_window_set_title(GTK_WINDOW(win_bridge),STR_NAME_PROGRAMM);
	gtk_window_set_resizable(GTK_WINDOW(win_bridge),TRUE);
	gtk_window_set_position (GTK_WINDOW(win_bridge),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(win_bridge),500,500);
	g_signal_connect(win_bridge,"destroy",G_CALLBACK(destroy_window_bridge),NULL);
	g_signal_connect(win_bridge,"key-press-event",G_CALLBACK(key_press_event_window_bridge),NULL);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_bridge = create_block_bridge();

	exit = gtk_button_new_with_label("ВЫХОД");
	layout_widget(exit,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(exit,"clicked",G_CALLBACK(clicked_button_exit),win_bridge);

	gtk_container_add(GTK_CONTAINER(win_bridge),box);
	gtk_box_pack_start(GTK_BOX(box),block_bridge,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),exit,FALSE,TRUE,5);

	gtk_widget_show(win_bridge);
	gtk_widget_show(box);
	gtk_widget_show(exit);

	return SUCCESS;
}

/*****************************************************************************/
