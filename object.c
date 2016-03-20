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
#include "group.h"
#include "videocamera.h"
#include "controller.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/
#if 0
TODO несколько страниц с любым количествои статических или динамических элементов
#define MAX_COLUMN_AMOUNT     1
static int column_amount = MAX_COLUMN_AMOUNT;
#define MAX_ROW_AMOUNT        1
static int row_amount = MAX_ROW_AMOUNT;
struct _block_page_s
{
};
#endif
struct _block_object_s
{
	GtkNotebook * notebook;

	GtkWidget * empty;
	GtkWidget * group;
	GtkWidget * videocamera;
	GtkWidget * controller;
};
typedef struct _block_object_s block_object_s;
/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/
static int change_object(block_object_s * block_object,int type)
{
	switch(type){
		case TYPE_GROUP:
			gtk_widget_hide(block_object->empty);
			gtk_widget_hide(block_object->videocamera);
			gtk_widget_hide(block_object->controller);
			gtk_widget_show(block_object->group);
			break;
		case TYPE_VIDEOCAMERA:
			gtk_widget_hide(block_object->empty);
			gtk_widget_hide(block_object->group);
			gtk_widget_hide(block_object->controller);
			gtk_widget_show(block_object->videocamera);
			break;
		case TYPE_CONTROLLERE:
			gtk_widget_hide(block_object->empty);
			gtk_widget_hide(block_object->group);
			gtk_widget_hide(block_object->videocamera);
			gtk_widget_show(block_object->controller);
			break;
		case TYPE_UNKNOWN:
		default:
			gtk_widget_hide(block_object->group);
			gtk_widget_hide(block_object->videocamera);
			gtk_widget_hide(block_object->controller);
			gtk_widget_show(block_object->empty);
			break;
	}
	return SUCCESS;
}

static GtkWidget * create_block_empty(void)
{
	GtkWidget * label;
	label = gtk_label_new("Выберите объект отображения");
	layout_widget(label,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_show(label);
	return label;
}

static int create_block_page(block_object_s * block_object,char * name)
{
	int rc;
	GtkNotebook * notebook = block_object->notebook;
	GtkWidget * grid;
	GtkWidget * label;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label = gtk_label_new(name);

	block_object->empty = create_block_empty();
	block_object->group = create_block_group();
	block_object->videocamera = create_block_videocamera();
	block_object->controller = create_block_controller();

	gtk_grid_attach(GTK_GRID(grid),block_object->empty,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_object->group,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_object->videocamera,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_object->controller,0,0,1,1);

	gtk_widget_show(grid);
	gtk_widget_show(label);
	change_object(block_object,TYPE_UNKNOWN);
	rc = gtk_notebook_append_page(notebook,grid,label);
	return rc;
}

/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
block_object_s block_object;

flag_t select_object(object_s * object)
{
	flag_t rc;
	GtkWidget * child;
	GtkWidget * label;

	if(object == NULL){
		change_object(&block_object,TYPE_UNKNOWN);
		return FAILURE;
	}

#if !(TEST_INTERFACE)
	rc = get_mode_work();
	if(rc != MODE_CONTROL_ON){
		g_info("Нет режима управления!");
		change_object(&block_object,TYPE_UNKNOWN);
		return SUCCESS;
	}
#endif

	rc = gtk_notebook_get_current_page(block_object.notebook);
	if(rc == -1){
		change_object(&block_object,TYPE_UNKNOWN);
		return FAILURE;
	}
	child = gtk_notebook_get_nth_page(block_object.notebook,rc);
	if(child == NULL){
		change_object(&block_object,TYPE_UNKNOWN);
		return FAILURE;
	}

	label = gtk_notebook_get_tab_label(block_object.notebook,child);
	gtk_label_set_text(GTK_LABEL(label),object->name);

	select_block_controller(NULL);
	switch(object->type){
		case TYPE_GROUP:
			select_block_group((group_s*)object->property);
			break;
		case TYPE_VIDEOCAMERA:
			select_block_videcamera((videocamera_s*)object->property);
			break;
		case TYPE_CONTROLLERE:
			select_block_controller((controller_s*)object->property);
			break;
		default:
			break;
	}
	change_object(&block_object,object->type);
	return SUCCESS;
}

static char STR_BASE_PAGE[] = "основное";
GtkWidget * create_block_object(void)
{
	GtkWidget * notebook;

	notebook = gtk_notebook_new();
	layout_widget(notebook,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	block_object.notebook = GTK_NOTEBOOK(notebook);

	create_block_page(&block_object,STR_BASE_PAGE);

	gtk_widget_show(notebook);

	return notebook;
}

/*****************************************************************************/
