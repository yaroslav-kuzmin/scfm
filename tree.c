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
#include "kernel.h"
#include "menu.h"
#include "object.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/
struct _block_tree_s
{
	GtkTreeView * view;
};
typedef struct _block_tree_s block_tree_s;
/*****************************************************************************/
/*    Локальные функции                                                      */
/*****************************************************************************/
char STR_TREE_VIEW_COLUMN[] = "Наименования";

#define WIDTH_COLUMN_TREE             100
int width_column_tree = WIDTH_COLUMN_TREE;

int tree_add_column(GtkTreeView * tree)
{
	GtkCellRenderer * render;
	GtkTreeViewColumn * column;

	render = gtk_cell_renderer_text_new();
	g_object_set(render,"editable",FALSE,NULL);
	g_object_set(render,"width",width_column_tree,NULL);

	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column,STR_TREE_VIEW_COLUMN);
	gtk_tree_view_column_pack_start(column,render,TRUE);
	gtk_tree_view_column_set_attributes(column,render,"text",COLUMN_NAME_TREE,NULL);
	gtk_tree_view_column_set_sizing (column,GTK_TREE_VIEW_COLUMN_FIXED);

	gtk_tree_view_append_column(tree,column);

	return SUCCESS;
}

int fill_treeview_group(GtkTreeStore * tree_model,GtkTreeIter * tree_iter,object_s * object)
{
	GSList * list = NULL;
	GtkTreeIter child_iter;

	list = object->list;
	for(;list;){
		object_s * o = (object_s*)list->data;
		gtk_tree_store_append(tree_model,&child_iter,tree_iter);
		gtk_tree_store_set(GTK_TREE_STORE(tree_model),&child_iter,COLUMN_NAME_TREE,o->name,COLUMN_POINT_TREE,o,-1);
		if(o->type == TYPE_GROUP){
			fill_treeview_group(tree_model,&child_iter,o);
		}
		list = g_slist_next(list);
	}
	return SUCCESS;
}

static int fill_treeview(GtkTreeView * treeview)
{
	GtkTreeSelection * select;
	GtkTreeModel * tree_model;
	GtkTreeIter tree_iter;
	GSList * list;

	select =	gtk_tree_view_get_selection (treeview);
	gtk_tree_selection_get_selected(select,&tree_model,&tree_iter);

	list = list_kernel();
	for(;list;){
		object_s * o = (object_s*)list->data;
		gtk_tree_store_append(GTK_TREE_STORE(tree_model),&tree_iter,NULL);
		gtk_tree_store_set(GTK_TREE_STORE(tree_model),&tree_iter,COLUMN_NAME_TREE,o->name,COLUMN_POINT_TREE,o,-1);
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
	int rc;
	object_s * object;
	GtkTreeModel * model;
	GtkTreeIter iter;
	GtkTreeSelection * select = gtk_tree_view_get_selection(tv);
	rc = gtk_tree_selection_get_selected(select,&model,&iter);
	if(rc != TRUE){
		return;
	}
	gtk_tree_model_get(model,&iter,COLUMN_POINT_TREE,&object,-1);

	if(object != NULL){
		select_object(object);
	}
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
block_tree_s block_tree;

int reread_tree(void)
{
	GtkTreeModel * tree_model = gtk_tree_view_get_model(block_tree.view);

	if(tree_model == NULL){
		return SUCCESS;
	}
	gtk_tree_store_clear(GTK_TREE_STORE(tree_model));

	fill_treeview(block_tree.view);
	return SUCCESS;
}

static char STR_TREE_FRAME[] = "Объекты";

GtkWidget * create_block_tree_object(void)
{
	GtkWidget * frame;
	GtkWidget * scrwin;
	GtkWidget * treeview;

	GtkTreeStore * model;

	frame = gtk_frame_new(STR_TREE_FRAME);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	scrwin = gtk_scrolled_window_new(NULL,NULL);
	layout_widget(scrwin,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	model = gtk_tree_store_new(AMOUNT_COLUMN_TREE,G_TYPE_STRING,G_TYPE_POINTER);
	treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	layout_widget(treeview,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	tree_add_column(GTK_TREE_VIEW(treeview));
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(treeview),FALSE);
	g_signal_connect(treeview,"row-activated",G_CALLBACK(row_activated_tree_view),NULL);
	g_signal_connect(treeview,"cursor-changed",G_CALLBACK(cursor_changed_tree_view),NULL);
	g_object_unref(model);
	fill_treeview(GTK_TREE_VIEW(treeview));
	block_tree.view = GTK_TREE_VIEW(treeview);
	gtk_container_add(GTK_CONTAINER(frame),scrwin);
	gtk_container_add(GTK_CONTAINER(scrwin),treeview);

	gtk_widget_show(frame);
	gtk_widget_show(scrwin);
	gtk_widget_show(treeview);

	return frame;
}

/*****************************************************************************/
