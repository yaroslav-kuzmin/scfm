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
#include <gtk/gtk.h>

#include "pub.h"
#include "common.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/
struct _block_videocamera_s
{
	GtkEntryBuffer * protocol;
	GtkEntryBuffer * addres;
	GtkEntryBuffer * port;
	GtkEntryBuffer * access;
};
typedef struct _block_videocamera_s block_videocamera_s;
/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/

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

	gtk_widget_show(box);
	gtk_widget_show(label);
	gtk_widget_show(entry);
	return box;
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

void * new_property_videocamera(void)
{
	return NULL;
}

int del_property_videocamera(videocamera_s * property)
{
	return SUCCESS;
}


static char STR_NAME_PROTOCOL[] = "протокол :";
static char STR_NAME_ADDRES[] = "адрес :";
static char STR_NAME_PORT[] = "порт :";
static char STR_NAME_ACCESS[] = "доступ :";
static block_videocamera_s block_videocamera;

GtkWidget * create_setting_videocamera(void)
{
	GtkWidget * box;
	GtkWidget * block_protocol;
	GtkWidget * block_addres;
	GtkWidget * block_port;
	GtkWidget * block_access;
	GtkEntryBuffer * buf;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_protocol = create_block_entry(STR_NAME_PROTOCOL,&buf);
	block_videocamera.protocol = buf;
	block_addres = create_block_entry(STR_NAME_ADDRES,&buf);
	block_videocamera.addres = buf;
	block_port = create_block_entry(STR_NAME_PORT,&buf);
	block_videocamera.port = buf;
	block_access = create_block_entry(STR_NAME_ACCESS,&buf);
	block_videocamera.access = buf;

	gtk_box_pack_start(GTK_BOX(box),block_protocol,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_addres,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_port,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_access,TRUE,TRUE,0);

	gtk_widget_show(box);
	return box;
}

/*TODO считывание данных из базыданных*/
int fill_videocamera(object_s * object)
{

	return SUCCESS;
}

/*****************************************************************************/
