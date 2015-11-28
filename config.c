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
#include "kernel.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/


/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/
enum{
	COLUMN_NAME = 0,
	COLUMN_POINT,
	COLUMNS_AMOUNT
};
static char STR_TREE_VIEW_COLUMN[] = "Наименования";

#define WIDTH_COLUMN_TREE             100
static int width_column_tree = WIDTH_COLUMN_TREE;

static int tree_add_column(GtkTreeView * tree)
{
	GtkCellRenderer * render;
	GtkTreeViewColumn * column;

	render = gtk_cell_renderer_text_new();
	g_object_set(render,"editable",FALSE,NULL);
	g_object_set(render,"width",width_column_tree,NULL);

	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column,STR_TREE_VIEW_COLUMN);
	gtk_tree_view_column_pack_start(column,render,TRUE);
	gtk_tree_view_column_set_attributes(column,render,"text",COLUMN_NAME,NULL);
	gtk_tree_view_column_set_sizing (column,GTK_TREE_VIEW_COLUMN_FIXED);

	gtk_tree_view_append_column(tree,column);

	return SUCCESS;
}

static void row_activated_tree_view(GtkTreeView *tv,GtkTreePath *path,GtkTreeViewColumn *column,gpointer ud)
{
	g_debug("row_activated_tree_view");
}

static char STR_TREE_FRAME[] = "Группы";

static GtkWidget * create_block_tree(void)
{
	GtkWidget * frame;
	GtkWidget * scrwin;
	GtkWidget * treeview;

	GtkTreeStore * model;

	frame = gtk_frame_new(STR_TREE_FRAME);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	scrwin = gtk_scrolled_window_new(NULL,NULL);
	layout_widget(scrwin,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	model = gtk_tree_store_new(COLUMNS_AMOUNT,G_TYPE_STRING,G_TYPE_POINTER);
	treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	layout_widget(treeview,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	tree_add_column(GTK_TREE_VIEW(treeview));
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(treeview),FALSE);
	g_signal_connect(treeview,"row-activated",G_CALLBACK(row_activated_tree_view),NULL);
	g_object_unref(model);
	/*fill_treeview(GTK_TREE_VIEW(treeview));*/

	gtk_container_add(GTK_CONTAINER(frame),scrwin);
	gtk_container_add(GTK_CONTAINER(scrwin),treeview);

	gtk_widget_show(frame);
	gtk_widget_show(scrwin);
	gtk_widget_show(treeview);

	return frame;
}

static void clicked_button_add(GtkButton * b,gpointer ud)
{
	g_debug("clicked_button_add");
}

static char STR_BUTTON_ADD[] = "добавить";
static GtkWidget * create_block_option(void)
{
	GtkWidget * box;
	GtkWidget * but_add;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	but_add = gtk_button_new_with_label(STR_BUTTON_ADD);
	layout_widget(but_add,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_add,"clicked",G_CALLBACK(clicked_button_add),NULL);

	gtk_box_pack_start(GTK_BOX(box),but_add,FALSE,FALSE,0);

	gtk_widget_show(box);
	gtk_widget_show(but_add);

	return box;
}

static char STR_FRAME_CONFIG[] = "Конфигурирование";

static GtkWidget * create_block_config(void)
{
	GtkWidget * frame;
	GtkWidget * box;

	GtkWidget * block_tree;
	GtkWidget * block_option;

	frame = gtk_frame_new(STR_FRAME_CONFIG);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_tree = create_block_tree();
	block_option = create_block_option();

	gtk_container_add(GTK_CONTAINER(frame),box);
	gtk_box_pack_start(GTK_BOX(box),block_tree,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_option,TRUE,TRUE,0);

	gtk_widget_show(frame);
	gtk_widget_show(box);

	return frame;
}

struct _config_button_s
{
	GtkWidget * win_main;
	GtkWidget * win_config;
};
typedef struct _config_button_s config_button_s;

static void clicked_button_exit(GtkButton * b,gpointer ud)
{
	config_button_s * cb = (config_button_s*)ud;
	gtk_widget_show(cb->win_main);
	gtk_widget_destroy(cb->win_config);
	set_mode_work(MODE_CONTROL,cb->win_main);
}

/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
#define CONFIG_BLOCK_SPACING   5
static char STR_CONFIG_EXIT[] = "выход";
static config_button_s config_button;

int create_window_config(GtkWidget * win_main)
{
	GtkWidget * win_config;
	GtkWidget * box;
	GtkWidget * block_config;
	GtkWidget * exit;

	config_button.win_main = win_main;
	win_config = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(win_config),CONFIG_BLOCK_SPACING);
	gtk_window_set_title(GTK_WINDOW(win_config),STR_NAME_PROGRAMM);
	gtk_window_set_resizable(GTK_WINDOW(win_config),TRUE);
	gtk_window_set_position (GTK_WINDOW(win_config),GTK_WIN_POS_CENTER);
	/*gtk_window_set_default_size(GTK_WINDOW(win_config),300,300);*/
	config_button.win_config = win_config;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_config = create_block_config();

	exit = gtk_button_new_with_label(STR_CONFIG_EXIT);
	layout_widget(exit,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(exit,"clicked",G_CALLBACK(clicked_button_exit),&config_button);

	gtk_container_add(GTK_CONTAINER(win_config),box);
	gtk_box_pack_start(GTK_BOX(box),block_config,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),exit,FALSE,TRUE,0);

	gtk_widget_hide(win_main);
	gtk_widget_show(win_config);
	gtk_widget_show(box);
	gtk_widget_show(exit);

	return SUCCESS;
}

/*****************************************************************************/
