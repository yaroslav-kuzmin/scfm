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
/*    локальные переменые                                                    */
/*****************************************************************************/
struct _config_s
{
	GtkTreeView * tree_view;
	GtkLabel * select;

	int type;
	object_s * group;
	object_s * object;
};
typedef struct _config_s config_s;

/*****************************************************************************/
/*    локальные функции                                                      */
/*****************************************************************************/
/*TODO обеденить с основным деревом*/
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
static int fill_treeview_group(GtkTreeStore * tree_model,GtkTreeIter * tree_iter,object_s * object)
{
	GSList * list = NULL;
	GtkTreeIter child_iter;

	list = object->list;
	for(;list;){
		object_s * o = (object_s*)list->data;
		gtk_tree_store_append(tree_model,&child_iter,tree_iter);
		gtk_tree_store_set(GTK_TREE_STORE(tree_model),&child_iter,COLUMN_NAME,o->name,COLUMN_POINT,o,-1);
		if(o->type == TYPE_GROUP){
			fill_treeview_group(tree_model,&child_iter,o);
		}
		list = g_slist_next(list);
	}
	return SUCCESS;
}

static char STR_ROOT_TREE[] = "основа";

static int fill_treeview(GtkTreeView * treeview)
{
	GtkTreeSelection * select;
	GtkTreeModel * tree_model;
	GtkTreeIter tree_iter_root;
	GtkTreeIter tree_iter;
	GSList * list;

	select =	gtk_tree_view_get_selection (treeview);
	gtk_tree_selection_get_selected(select,&tree_model,&tree_iter);

	gtk_tree_store_append(GTK_TREE_STORE(tree_model),&tree_iter_root,NULL);
	gtk_tree_store_set(GTK_TREE_STORE(tree_model),&tree_iter_root,COLUMN_NAME,STR_ROOT_TREE,COLUMN_POINT,NULL,-1);

	list = kernel_list();
	for(;list;){
		object_s * o = (object_s*)list->data;
		gtk_tree_store_append(GTK_TREE_STORE(tree_model),&tree_iter,&tree_iter_root);
		gtk_tree_store_set(GTK_TREE_STORE(tree_model),&tree_iter,COLUMN_NAME,o->name,COLUMN_POINT,o,-1);
		if(o->type == TYPE_GROUP){
			fill_treeview_group(GTK_TREE_STORE(tree_model),&tree_iter,o);
		}
		list = g_slist_next(list);
	}
	return SUCCESS;
}

static void row_activated_tree_view(GtkTreeView *tv,GtkTreePath *path,GtkTreeViewColumn *column,gpointer ud)
{
	g_debug("row_activated_tree_view");
}

static void cursor_changed_tree_view(GtkTreeView * tv,gpointer ud)
{
	g_debug("cursor_changed_tree_view");
#if 0
	int rc;
	config_s * c = (config_s*)ud;
	GtkTreeModel * model;
	GtkTreeIter * iter_parent = c->tree_iter_parent;
	GtkTreeIter * iter = c->tree_iter;
	GtkTreeSelection * select = gtk_tree_view_get_selection (tv);
	rc = gtk_tree_selection_get_selected(select,&model,iter);
	if(rc == TRUE){
		object_s * object;
		c->tree_model = model;
		gtk_tree_model_get(model,iter,COLUMN_POINT,&object,-1);
		if(object == NULL){ /*основа*/
			c->group = get_kernel();
			c->object = NULL;
			c->tree_root = OK;
		}
		else{
			if(object->type == TYPE_GROUP){
				c->group = object;
				c->object = NULL;
				rc = gtk_tree_model_iter_parent(model,iter_parent,iter);
				if(rc == TRUE){
					c->tree_root = OK;
				}
				else{
					c->tree_root = NOT_OK;
				}
			}
		}
	}
#endif
}

static char STR_TREE_FRAME[] = "Группы";

static GtkWidget * create_block_tree(config_s * config)
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
	config->tree_view = GTK_TREE_VIEW(treeview);
	g_signal_connect(treeview,"row-activated",G_CALLBACK(row_activated_tree_view),config);
	g_signal_connect(treeview,"cursor-changed",G_CALLBACK(cursor_changed_tree_view),config);
	g_object_unref(model);
	fill_treeview(GTK_TREE_VIEW(treeview));

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

static GtkWidget * create_combobox(config_s * config)
{
	GtkWidget * combox;

	
	return combox;
}

static GtkWidget * create_block_option(config_s * config)
{
	GtkWidget * box;
	GtkWidget * lab_select;
	GtkWidget * combox;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	lab_select = gtk_label_new(STR_ROOT_TREE);
	layout_widget(lab_select,GTK_ALIGN_FILL,GTK_ALIGN_CENTER,TRUE,TRUE);
	config->select = GTK_LABEL(lab_select);

	combox = create_combobox(config);

	/*but_add = gtk_button_new_with_label(STR_BUTTON_ADD);*/
	/*layout_widget(but_add,GTK_ALIGN_CENTER,GTK_ALIGN_END,FALSE,FALSE);*/
	/*g_signal_connect(but_add,"clicked",G_CALLBACK(clicked_button_add),config);*/

	gtk_box_pack_start(GTK_BOX(box),lab_select,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),combox,TRUE,TRUE,0);

	/*gtk_box_pack_start(GTK_BOX(box),but_add,FALSE,FALSE,0);*/

	gtk_widget_show(box);
	gtk_widget_show(lab_select);

	/*gtk_widget_show(but_add);*/

	return box;
}

static char STR_FRAME_CONFIG[] = "Конфигурирование";

static GtkWidget * create_block_config(config_s * config)
{
	GtkWidget * frame;
	GtkWidget * box;

	GtkWidget * block_tree;
	GtkWidget * block_option;

	frame = gtk_frame_new(STR_FRAME_CONFIG);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_tree = create_block_tree(config);
	block_option = create_block_option(config);

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
	int flag_exit;
};
typedef struct _config_button_s config_button_s;

static void clicked_button_exit(GtkButton * b,gpointer ud)
{
	config_button_s * cb = (config_button_s*)ud;
	gtk_widget_show(cb->win_main);
	cb->flag_exit = NOT_OK;
	gtk_widget_destroy(cb->win_config);
	set_mode_work(MODE_CONTROL,cb->win_main);
}

static gboolean key_press_event_window_config(GtkWidget * w,GdkEvent  *event,gpointer ud)
{
	config_button_s * cb = (config_button_s*)ud;
	GdkEventType type = event->type;
	gint state;

	if(type == GDK_KEY_PRESS){
		GdkEventKey * event_key = (GdkEventKey*)event;
		state = event_key->state;
		if( (state & GDK_MOD1_MASK) ){
			if( event_key->keyval == GDK_KEY_F4){
				gtk_widget_destroy(cb->win_config);
			}
		}
	}
	return FALSE;
}

static void destroy_window_config(GtkWidget * w,gpointer ud)
{
	config_button_s * cb = (config_button_s*)ud;
	if(cb->flag_exit == OK){
		gtk_widget_destroy(cb->win_main);
	}
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
#define CONFIG_BLOCK_SPACING   5
static char STR_CONFIG_EXIT[] = "выход";
static config_button_s config_button;

#define MIN_WIDTH_WIN_CONFIG      400
#define MIN_HEIGHT_WIN_CONFIG     400
static config_s total_config;

int create_window_config(GtkWidget * win_main)
{
	GtkWidget * win_config;
	GtkWidget * box;
	GtkWidget * block_config;
	GtkWidget * exit;

	total_config.tree_view = NULL;
	total_config.group = NULL;
	total_config.object = NULL;

	config_button.flag_exit = OK;
	config_button.win_main = win_main;
	win_config = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	config_button.win_config = win_config;
	gtk_container_set_border_width(GTK_CONTAINER(win_config),CONFIG_BLOCK_SPACING);
	gtk_window_set_title(GTK_WINDOW(win_config),STR_NAME_PROGRAMM);
	gtk_window_set_resizable(GTK_WINDOW(win_config),TRUE);
	gtk_window_set_position (GTK_WINDOW(win_config),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(win_config),MIN_WIDTH_WIN_CONFIG,MIN_HEIGHT_WIN_CONFIG);
	g_signal_connect(win_config,"destroy",G_CALLBACK(destroy_window_config),&config_button);
	g_signal_connect(win_config,"key-press-event",G_CALLBACK(key_press_event_window_config),&config_button);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_config = create_block_config(&total_config);

	exit = gtk_button_new_with_label(STR_CONFIG_EXIT);
	layout_widget(exit,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(exit,"clicked",G_CALLBACK(clicked_button_exit),&config_button);

	gtk_container_add(GTK_CONTAINER(win_config),box);
	gtk_box_pack_start(GTK_BOX(box),block_config,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),exit,FALSE,TRUE,0);
	/*TODO обработка alt-f4*/
	gtk_widget_hide(win_main);
	gtk_widget_show(win_config);
	gtk_widget_show(box);
	gtk_widget_show(exit);

	return SUCCESS;
}

/*****************************************************************************/
