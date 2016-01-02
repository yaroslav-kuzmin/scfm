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
#include "database.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/
typedef struct _check_connect_s check_connect_s;
struct _block_setting_controller_s
{
	GtkEntryBuffer * address;
	GtkEntryBuffer * port;
	GtkEntryBuffer * id;

	link_s * link;
	config_controller_s * config;
	state_controller_s * state;
};
typedef struct _block_setting_controller_s block_setting_controller_s;

struct _block_controller_s
{
	GtkLabel * label;
};
typedef struct _block_controller_s block_controller_s;

struct _all_controller_s
{
	GSList * list;
};
typedef struct _all_controller_s all_controller_s;
/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/
static int check_link_controller(link_s * link,config_controller_s * config,state_controller_s * state)
{
	int rc;
	rc = link_connect_controller(link);
	if(rc == FAILURE){
		return rc;
	}
	rc = link_config_controller(link,config);
	if(rc == FAILURE){
		return rc;
	}
	rc = link_state_controller(link,state);
	if(rc == FAILURE){
		return rc;
	}
	return SUCCESS;
}

static int fill_block_setting_controller(block_setting_controller_s * bsc)
{
	return SUCCESS;
}

static char * check_address(const char * address)
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

static uint16_t check_port(const char * port)
{
	uint16_t p = 0;
	p = g_ascii_strtoull(port,NULL,10);
	return p;
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

static void clicked_button_check(GtkButton * button,gpointer ud)
{
	int rc;
	char * address = NULL;
	uint16_t port;
	uint8_t id;
	const char * str;
	link_s * link;
	config_controller_s * config;
	state_controller_s * state;
	block_setting_controller_s * bsc = (block_setting_controller_s*)ud;

	/*TODO при повторном нажатии утечка памяти*/
	bsc->link = NULL;
	/*TODO вывод сообщений*/
	str = gtk_entry_buffer_get_text(bsc->address);
	address = check_address(str);
	if(address == NULL){
		g_warning("Адрес контролера не корректный!");
		return ;
	}
	str = gtk_entry_buffer_get_text(bsc->port);
	port = check_port(str);
	if(port == 0){
		g_warning("Порт контролера не корректный!");
		return ;
	}
	str = gtk_entry_buffer_get_text(bsc->id);
	id = check_id(str);
	if(id == 0){
		g_warning("Индификатор контролера не корректный!");
		return ;
	}

	link = g_slice_alloc0(sizeof(link_s));
	config = g_slice_alloc0(sizeof(config_controller_s));
	state = g_slice_alloc0(sizeof(state_controller_s));

	/*TODO добавление других типов соединений*/
	link->type = TYPE_LINK_TCP;
	link->connect = NULL;
	link->address = address;
	link->port = port;
	link->id = id;

	rc = check_link_controller(link,config,state);
	if(rc == FAILURE){
		g_slice_free1(sizeof(link_s),link);
		g_slice_free1(sizeof(config_controller_s),config);
		g_slice_free1(sizeof(state_controller_s),state);
		return;
	}

	bsc->link = link;
	bsc->config = config;
	bsc->state = state;
	/*TODO сообщенийние что проверка корректноа*/
	fill_block_setting_controller(bsc);
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
static block_setting_controller_s block_setting_controller;

void * new_property_controller(void)
{
	controller_s * controller;
	link_s * link = block_setting_controller.link;
	config_controller_s * config = block_setting_controller.config;
	state_controller_s * state = block_setting_controller.state;

	/*TODO вывод сообщений*/
	if(link == NULL){
		g_warning("Соединение не проверено!");
		return NULL;
	}

	controller = g_slice_alloc0(sizeof(controller_s));
	controller->name = get_name_controller(config);
	controller->link = link;
	controller->config = config;
	controller->state = state;

	return controller;
}

static char STR_NAME_CONTROLLER[] = "Контроллер";
static char STR_NAME_ADDRESS[] = "Адрес";
static char STR_NAME_PORT[] = "Порт";
static char STR_NAME_ID[] = "Номер";
static char STR_NAME_CHECK[] = "Поиск";

GtkWidget * create_block_setting_controller(void)
{
	GtkWidget * grid;
	GtkWidget * label;
	GtkWidget * block_address;
	GtkWidget * block_port;
	GtkWidget * block_id;
	GtkEntryBuffer * buf;
	GtkWidget * but_check;

	block_setting_controller.link = NULL;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label = gtk_label_new(STR_NAME_CONTROLLER);
	layout_widget(label,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,FALSE);

	block_address = create_block_entry(STR_NAME_ADDRESS,&buf);
	block_setting_controller.address = buf;

	block_port = create_block_entry(STR_NAME_PORT,&buf);
	block_setting_controller.port = buf;

	block_id = create_block_entry(STR_NAME_ID,&buf);
	block_setting_controller.id = buf;

	but_check = gtk_button_new_with_label(STR_NAME_CHECK);
	layout_widget(but_check,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_check,"clicked",G_CALLBACK(clicked_button_check),&block_setting_controller);

	gtk_grid_attach(GTK_GRID(grid),label        ,0,0,2,1);
	gtk_grid_attach(GTK_GRID(grid),block_address,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_port   ,0,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_id     ,0,3,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_check    ,0,4,2,1);

	gtk_widget_show(grid);
	gtk_widget_show(label);
	gtk_widget_show(but_check);

	return grid;
}

/*****************************************************************************/
all_controller_s all_controller;
int init_all_controller(void)
{
	all_controller.list = g_slist_alloc();
	return SUCCESS;
}

int deinit_all_controller(void)
{
	g_slist_free(all_controller.list);
	return SUCCESS;
}

int connect_controller(controller_s * controller)
{
	int rc;
	link_s * link = controller->link;
	config_controller_s check;
	config_controller_s * config = controller->config;
	state_controller_s * state = controller->state;

	if(link->connect != NULL){
		g_warning("Контролер подключен!");
		return SUCCESS;
	}
	rc = check_link_controller(link,&check,state);
	if(rc == FAILURE){
		g_warning("Нет подключения к контроллеру!");
		return rc;
	}
	rc = check_config_controller(&check,config);
	if(rc == FAILURE){
		/*TODO перезапись базы данных*/
		g_warning("Данные из контроллера не совпадают с данными из базы данных!");
		link_disconnect_controller(link);
		return rc;
	}

	return SUCCESS;
}

/*TODO считывание данных из базыданных*/
controller_s * init_controller(uint32_t number)
{
	int rc;
	controller_s * controller = NULL;

	controller = g_slice_alloc0(sizeof(controller_s));
	controller->link = g_slice_alloc0(sizeof(link_s));
	controller->config = g_slice_alloc0(sizeof(config_controller_s));
	controller->state = g_slice_alloc0(sizeof(state_controller_s));
	/*память для обектов выделяется при чтении из базыданых*/
	rc = read_database_controller(number,controller);
	if(rc != SUCCESS){
		g_slice_free1(sizeof(controller_s),controller);
		g_slice_free1(sizeof(link_s),link);
		g_slice_free1(sizeof(config_controller_s),config);
		g_slice_free1(sizeof(state_controller_s),state);
		controller = NULL;
	}
	controller->name = get_name_controller(controller->config);
	all_controller.list = g_slist_append(all_controller.list,controller);
	return controller;
}

int del_property_controller(controller_s * property)
{
	char * str;
	link_s * link;
	config_controller_s * config;
	state_controller_s * state;

	if(property == NULL){
		return SUCCESS;
	}
	/*str = property->name;
	g_free(str);*/
	str = property->address;
	g_free(str);

	link = property->link;
	g_slice_free1(sizeof(link_s),link);
	config = property->config;
	g_slice_free1(sizeof(config_controller_s),config);
	state = property->state;
	g_slice_free1(sizeof(state_controller_s),state);
	g_slice_free1(sizeof(controller_s),property);

	return SUCCESS;
}

block_controller_s block_controller;

int fill_block_controller(controller_s * controller)
{
	GtkLabel * label = block_controller.label;
	g_string_printf(pub,"%#x",controller->id);
	gtk_label_set_text(label,pub->str);
	g_debug("name :> %s\nid :> %d\naddress :> %s\nport :> %d"
	       ,controller->name
	       ,controller->id
	       ,controller->address
	       ,controller->port);
	return SUCCESS;
}

GtkWidget * create_block_controller(void)
{
	GtkWidget * label;

	label = gtk_label_new(NULL);
	layout_widget(label,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_show(label);
	block_controller.label = GTK_LABEL(label);
	return label;
}
/*****************************************************************************/
