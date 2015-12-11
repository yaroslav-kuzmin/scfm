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
#include "group.h"
#include "videocamera.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/
struct _block_object_s
{
	GtkNotebook * notebook;
};
typedef struct _block_object_s block_object_s;
/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/
#define MAX_COLUMN_AMOUNT     1
static int column_amount = MAX_COLUMN_AMOUNT;
#define MAX_ROW_AMOUNT        1
static int row_amount = MAX_ROW_AMOUNT;

static int create_block_page(block_object_s * block_object,char * name)
{
	int rc;
	GtkNotebook * notebook = block_object->notebook;
	GtkWidget * grid;
	GtkWidget * label;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);



	label = gtk_label_new(name);

	gtk_widget_show(grid);
	gtk_widget_show(label);

	rc = gtk_notebook_append_page(notebook,grid,label);
	return rc;
}

/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

static char STR_BASE_PAGE[] = "основное";
block_object_s block_object;
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
