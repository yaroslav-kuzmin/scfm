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
#include "database.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/
struct _block_setting_controller_s
{
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
static void clicked_button_open_file(GtkButton * b,gpointer * ud)
{
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
static block_setting_controller_s block_setting_controller;

void * new_property_controller(void)
{
	controller_s * controller;
	controller = g_slice_alloc0(sizeof(controller_s));
	return controller;
}

int del_property_controller(controller_s * property)
{
	if(property == NULL){
		return SUCCESS;
	}
	g_slice_free1(sizeof(controller_s),property);

	return SUCCESS;
}

GtkWidget * create_block_setting_controller(void)
{
	GtkWidget * grid;

	GtkWidget * label;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label = gtk_label_new("лафет");
	layout_widget(label,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	gtk_grid_attach(GTK_GRID(grid),label ,0,0,1,1);

	gtk_widget_show(grid);
	gtk_widget_show(label);

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

/*TODO считывание данных из базыданных*/
controller_s * init_controller(uint32_t number)
{
	int rc;
	controller_s * controller = NULL;

	controller = g_slice_alloc0(sizeof(controller_s));
	/*память для обектов выделяется при чтении из базыданых*/
#if 0
	rc = read_database_controller(number,controller);
	if(rc != SUCCESS){
		g_slice_free1(sizeof(controller_s),controller);
		controller = NULL;
	}
#endif
	all_controller.list = g_slist_append(all_controller.list,controller);
	return controller;
}

block_controller_s block_controller;

int fill_controller(controller_s * controller)
{
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
