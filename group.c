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

/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/
static char STR_SCHEMA_GROUP[] = "Схема группы";
static void clicked_button_open_file(GtkButton * b,gpointer * ud)
{
	int rc;
	GtkEntryBuffer * buf = (GtkEntryBuffer*)ud;
	GtkWidget * dialog = gtk_file_chooser_dialog_new(STR_SCHEMA_GROUP
	                     ,NULL,GTK_FILE_CHOOSER_ACTION_OPEN
	                     ,"Открыть",GTK_RESPONSE_OK
	                     ,"Закрыть",GTK_RESPONSE_CLOSE,NULL);

	rc = gtk_dialog_run(GTK_DIALOG(dialog));
	if(rc == GTK_RESPONSE_OK){
		char * str;
		str = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		if(str != NULL){
			gtk_entry_buffer_set_text(buf,str,-1);
			g_free(str);
		}
	}

	gtk_widget_destroy(dialog);
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
static char STR_NAME_FILE[] = "имя файла";
static char STR_ADD_GROUP[] = "открыть файл";
/*static group_s group;*/

GtkWidget * create_setting_group(void)
{
	GtkWidget * grid;

	GtkWidget * label;
	GtkWidget * entry;
	GtkWidget * button;
	GtkEntryBuffer * buf;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label = gtk_label_new(STR_NAME_FILE);
	layout_widget(label,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	entry = gtk_entry_new();
	layout_widget(entry,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	buf = gtk_entry_get_buffer(GTK_ENTRY(entry));

	button = gtk_button_new_with_label(STR_ADD_GROUP);
	layout_widget(button,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(button,"clicked",G_CALLBACK(clicked_button_open_file),buf);

	gtk_grid_attach(GTK_GRID(grid),label ,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),entry ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),button,2,0,1,1);

	gtk_widget_show(grid);
	gtk_widget_show(label);
	gtk_widget_show(entry);
	gtk_widget_show(button);

	return grid;
}

int fill_group(object_s * object)
{
	return SUCCESS;
}
/*****************************************************************************/
