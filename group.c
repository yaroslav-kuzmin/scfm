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
struct _block_setting_group_s
{
	GtkEntryBuffer * image;
};
typedef struct _block_setting_group_s block_setting_group_s;

struct _block_group_s
{
	GtkLabel * label;
};
typedef struct _block_group_s block_group_s;

struct _all_group_s
{
	GSList * list;
};
typedef struct _all_group_s all_group_s;
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
static block_setting_group_s block_setting_group;

void * new_property_group(void)
{
	group_s * group;
	char * image = (char*)gtk_entry_buffer_get_text(block_setting_group.image);

	if( (image == NULL) || (*image == 0) ){
		g_warning("Нет имени рисунка");
		return NULL;
	}
	/*TODO проверка изображения*/
	group = g_slice_alloc0(sizeof(group_s));
	group->image = g_strdup(image);
	return group;
}

int del_property_group(group_s * property)
{
	char * image;
	if(property == NULL){
		return SUCCESS;
	}
	image = property->image;
	g_free(image);
	g_slice_free1(sizeof(group_s),property);

	return SUCCESS;
}

static char STR_NAME_FILE[] = "имя файла";
static char STR_ADD_GROUP[] = "открыть файл";
/*static group_s group;*/

GtkWidget * create_block_setting_group(void)
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
	block_setting_group.image = buf;
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

/*****************************************************************************/
all_group_s all_group;
int init_all_groups(void)
{
	all_group.list = g_slist_alloc();
	return SUCCESS;
}

int deinit_all_groups(void)
{
	g_slist_free(all_group.list);
	return SUCCESS;
}

/*TODO считывание данных из базыданных*/
group_s * init_group(uint32_t number)
{
	int rc;
	group_s * group = NULL;

	group = g_slice_alloc0(sizeof(group_s));
	/*память для обектов выделяется при чтении из базыданых*/
	rc = read_database_group(number,group);
	if(rc != SUCCESS){
		g_slice_free1(sizeof(group_s),group);
		group = NULL;
	}
	all_group.list = g_slist_append(all_group.list,group);
	return group;
}

block_group_s block_group;
int select_block_group(group_s * group)
{
	GtkLabel * image = block_group.label;
	gtk_label_set_text(image,group->image);
	return SUCCESS;
}

GtkWidget * create_block_group(void)
{
	GtkWidget * label;

	label = gtk_label_new(NULL);
	layout_widget(label,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_show(label);
	block_group.label = GTK_LABEL(label);
	return label;
}
/*****************************************************************************/
