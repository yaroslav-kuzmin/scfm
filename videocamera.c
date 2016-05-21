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
#include "database.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/
struct _block_setting_videocamera_s
{
	GtkEntryBuffer * protocol;
	GtkEntryBuffer * address;
	GtkEntryBuffer * port;
	GtkEntryBuffer * access;
};
typedef struct _block_setting_videocamera_s block_setting_videocamera_s;
struct _block_videocamera_s
{
	GtkLabel * label;
};
typedef struct _block_videocamera_s block_videocamera_s;
struct _all_videocamera_s
{
	GSList * list;
};
typedef struct _all_videocamera_s all_videocamera_s;
/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/
static char * check_buf_protocol(const char * protocol)
{
	/*TODO */
	if((protocol == NULL) || (*protocol == 0) ){
		return NULL;
	}
	return g_strdup(protocol);
}
static char * check_buf_address(const char * address)
{
	/*TODO*/
	if((address == NULL) || (*address == 0)){
		return NULL;
	}
	return g_strdup(address);
}
static uint32_t check_buf_port(const char * str_port)
{
	/*TODO */
	uint32_t port = 0;
	if((str_port == NULL) || (*str_port == 0)){
		return 0;
	}
	port = (uint32_t)g_strtod(str_port,NULL);
	if(port > 0xFFFF){
		return 0;
	}
	return port;
}
static char * check_buf_access(const char * access)
{
	/*TODO*/
	if((access == NULL) || (*access == 0)){
		return NULL;
	}
	return g_strdup(access);
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
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

/*  конфигурирование */
static block_setting_videocamera_s block_setting_videocamera;
void * new_property_videocamera(void)
{
	videocamera_s * property = NULL;
	GtkEntryBuffer * buf;
	const char * protocol;
	const char * address;
	const char * str_port;
	uint32_t port;
	const char * access;
	/*TODO вывод сообщения*/
	buf = block_setting_videocamera.protocol;
	protocol = gtk_entry_buffer_get_text(buf);
	protocol = check_buf_protocol(protocol);
	if(protocol == NULL){
		g_warning("Некорректный протокол!");
		return property;
	}
	buf = block_setting_videocamera.address;
	address = gtk_entry_buffer_get_text(buf);
	address = check_buf_address(address);
	if(address == NULL){
		g_warning("Некорректный адрес!");
		return property;
	}
	buf = block_setting_videocamera.port;
	str_port = gtk_entry_buffer_get_text(buf);
	port = check_buf_port(str_port);
	if(port == 0){
		g_warning("Некорректный порт!");
		return property;
	}
	buf = block_setting_videocamera.access;
	access = gtk_entry_buffer_get_text(buf);
	access = check_buf_access(access);
	if(access == NULL){
		g_warning("Некорректная строка доступа!");
		return property;
	}

	property = g_slice_alloc0(sizeof(videocamera_s));
	property->protocol = (char*)protocol;
	property->address = (char*)address;
	property->port = port;
	property->access = (char*)access;

	return property;
}

int del_property_videocamera(videocamera_s * property)
{
	char * str;
	if(property == NULL){
		return SUCCESS;
	}
	str = property->protocol;
	g_free(str);
	str = property->address;
	g_free(str);
	str = property->access;
	g_free(str);
	g_slice_free1(sizeof(videocamera_s),property);
	return SUCCESS;
}

GtkWidget * create_block_setting_videocamera(void)
{
	GtkWidget * box;
	GtkWidget * block_protocol;
	GtkWidget * block_addres;
	GtkWidget * block_port;
	GtkWidget * block_access;
	GtkEntryBuffer * buf;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_protocol = create_block_entry("протокол :",&buf);
	block_setting_videocamera.protocol = buf;
	block_addres = create_block_entry("адрес :",&buf);
	block_setting_videocamera.address = buf;
	block_port = create_block_entry("порт :",&buf);
	block_setting_videocamera.port = buf;
	block_access = create_block_entry("доступ :",&buf);
	block_setting_videocamera.access = buf;

	gtk_box_pack_start(GTK_BOX(box),block_protocol,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_addres,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_port,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_access,TRUE,TRUE,0);

	gtk_widget_show(box);

	return box;
}

/*****************************************************************************/
all_videocamera_s all_videocamera;
int init_all_videocameras(void)
{
	all_videocamera.list = g_slist_alloc();
	return SUCCESS;
}

int deinit_all_videcameras(void)
{
	g_slist_free(all_videocamera.list);
	return SUCCESS;
}

/*TODO считывание данных из базыданных*/
videocamera_s * init_videocamera(object_s * object)
{
	int rc;
	uint32_t number = object->number;
	videocamera_s * videocamera;

	videocamera = g_slice_alloc0(sizeof(videocamera_s));
	/*память для обектов выделяется при чтении из базыданых*/
	rc = read_database_videocamera(number,videocamera);
	if(rc != SUCCESS){
		g_slice_free1(sizeof(videocamera_s),videocamera);
		videocamera = NULL;
	}
	all_videocamera.list = g_slist_append(all_videocamera.list,videocamera);
	videocamera->object = object;
	return videocamera;
}

block_videocamera_s block_videocamera;

int select_block_videcamera(videocamera_s * videocamera)
{
	GtkLabel * label = block_videocamera.label;
	gtk_label_set_text(label,videocamera->access);
	return SUCCESS;
}
/*TODO блок становится видимым и не видимым  */
GtkWidget * create_block_videocamera(void)
{
	GtkWidget * label;

	label = gtk_label_new(NULL);
	layout_widget(label,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	block_videocamera.label = GTK_LABEL(label);

	gtk_widget_show(label);
	return label;
}
/*****************************************************************************/

flag_t videocamera_status(videocamera_s * videocamera)
{
	return STATUS_ERROR;
}
/*****************************************************************************/
