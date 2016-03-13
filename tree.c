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
/*Основное дерево */
enum
{
	COLUMN_NAME_TREE = 0,  /*имя обеъкта*/
	COLUMN_POINT_TREE,     /*указатель на обект*/
	COLUMN_IMAGE_TREE,     /*указатель на рисунок*/
	AMOUNT_COLUMN_TREE
};

struct _block_tree_s
{
	GtkTreeView * view;
	GdkPixbuf * image[AMOUNT_STATUS];
};
typedef struct _block_tree_s block_tree_s;
/*****************************************************************************/
/*    Локальные функции                                                      */
/*****************************************************************************/

static int tree_add_column(block_tree_s * bt)
{
	GtkTreeView * treeview = bt->view;
	GtkCellRenderer * render;
	GtkTreeViewColumn * column;

	render = gtk_cell_renderer_pixbuf_new();
	/*column = gtk_tree_view_column_new_with_attributes("С",render,"pixbuf",COLUMN_IMAGE_TREE,NULL);*/
	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column,"С");
	gtk_tree_view_column_pack_start(column,render,TRUE);
	gtk_tree_view_column_set_attributes(column,render,"pixbuf",COLUMN_IMAGE_TREE,NULL);
	gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column), GTK_TREE_VIEW_COLUMN_FIXED);

	gtk_tree_view_append_column (treeview,column);

	render = gtk_cell_renderer_text_new();
	g_object_set(render,"editable",FALSE,NULL);
	g_object_set(render,"width",100,NULL);
	g_object_set(render,"size",15000,NULL); /*размер шрифта*/

	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column,"Наименования");
	gtk_tree_view_column_pack_start(column,render,TRUE);
	gtk_tree_view_column_set_attributes(column,render,"text",COLUMN_NAME_TREE,NULL);
	gtk_tree_view_column_set_sizing (column,GTK_TREE_VIEW_COLUMN_FIXED);

	gtk_tree_view_append_column(treeview,column);

	return SUCCESS;
}

static int fill_treeview_group(GtkTreeStore * tree_model,GtkTreeIter * tree_iter,object_s * object,GdkPixbuf * image)
{
	GSList * list = NULL;
	GtkTreeIter child_iter;

	list = object->list;
	for(;list;){
		object_s * o = (object_s*)list->data;
		gtk_tree_store_append(tree_model,&child_iter,tree_iter);
		gtk_tree_store_set(GTK_TREE_STORE(tree_model),&child_iter
		,COLUMN_NAME_TREE,o->name
		,COLUMN_POINT_TREE,o
		,COLUMN_IMAGE_TREE,image,-1);
		if(o->type == TYPE_GROUP){
			fill_treeview_group(tree_model,&child_iter,o,image);
		}
		list = g_slist_next(list);
	}
	return SUCCESS;
}

static int fill_treeview(block_tree_s * bt)
{
	GtkTreeView * treeview = bt->view;
	GtkTreeSelection * select;
	GtkTreeModel * tree_model;
	GtkTreeIter tree_iter;
	GSList * list;
	GdkPixbuf * image = bt->image[STATUS_WAIT];

	select =	gtk_tree_view_get_selection (treeview);
	gtk_tree_selection_get_selected(select,&tree_model,&tree_iter);

	list = list_kernel();
	for(;list;){
		object_s * o = (object_s*)list->data;
		gtk_tree_store_append(GTK_TREE_STORE(tree_model),&tree_iter,NULL);
		gtk_tree_store_set(GTK_TREE_STORE(tree_model),&tree_iter
		,COLUMN_NAME_TREE,o->name
		,COLUMN_POINT_TREE,o
		,COLUMN_IMAGE_TREE,image,-1);
		if(o->type == TYPE_GROUP){
			fill_treeview_group(GTK_TREE_STORE(tree_model),&tree_iter,o,image);
		}
		list = g_slist_next(list);
	}
	return SUCCESS;
}

static void row_activated_tree_view(GtkTreeView *tv,GtkTreePath *path,GtkTreeViewColumn *column,gpointer ud)
{
#if 1
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
#else
	g_debug("row_activated_tree_view");
#endif
}

static void cursor_changed_tree_view(GtkTreeView * tv,gpointer ud)
{
#if 0
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
#else
	g_debug("cursor_changed_tree_view");
#endif
}


static int init_image(block_tree_s * bt)
{
	GdkPixbuf * buf;
	buf = get_resource_image(RESOURCE_BASE,"tree_green");
	bt->image[STATUS_NORM] = buf;
	buf = get_resource_image(RESOURCE_BASE,"tree_red");
	bt->image[STATUS_ERROR] = buf;
	buf = get_resource_image(RESOURCE_BASE,"tree_yellow");
	bt->image[STATUS_WAIT] = buf;

	return SUCCESS;
}

static int find_object_tree(GtkTreeModel * model,GtkTreeIter * c_iter,GtkTreeIter * p_iter,object_s * object)
{
	int rc;
	object_s * check;

	rc = gtk_tree_model_iter_children(model,c_iter,p_iter);
	for(;rc;){
		GtkTreeIter cc_iter;
		gtk_tree_model_get(model,c_iter,COLUMN_POINT_TREE,&check,-1);
		if(check == object){
			rc = TRUE;
			break;
		}
		rc = find_object_tree(model,&cc_iter,c_iter,object);
		if(rc){
			break;
		}
		rc = gtk_tree_model_iter_next(model,c_iter);
	}

	return rc;
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
block_tree_s block_tree;

int status_tree(object_s * object,int status)
{
	int rc;
	GtkTreeView * tree_view = block_tree.view;
	GtkTreeModel * tree_model = gtk_tree_view_get_model(tree_view);
	GtkTreeIter iter;

	rc = find_object_tree(tree_model,&iter,NULL,object);
	if( rc ){
		GdkPixbuf * image = block_tree.image[status];
		gtk_tree_store_set(GTK_TREE_STORE(tree_model),&iter,COLUMN_IMAGE_TREE,image,-1);
	}

	return SUCCESS;
}

int reread_tree(void)
{
	GtkTreeModel * tree_model = gtk_tree_view_get_model(block_tree.view);

	if(tree_model == NULL){
		return SUCCESS;
	}
	gtk_tree_store_clear(GTK_TREE_STORE(tree_model));

	fill_treeview(&block_tree);
	return SUCCESS;
}

GtkWidget * create_block_tree_object(void)
{
	GtkWidget * frame;
	GtkWidget * scrwin;
	GtkWidget * treeview;
	GtkTreeStore * model;

	init_image(&block_tree);

	frame = gtk_frame_new("Объекты");
	layout_widget(frame,GTK_ALIGN_START,GTK_ALIGN_FILL,FALSE,TRUE);
	gtk_widget_set_size_request(frame,200,-1);

	scrwin = gtk_scrolled_window_new(NULL,NULL);
	layout_widget(scrwin,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	model = gtk_tree_store_new(AMOUNT_COLUMN_TREE,G_TYPE_STRING,G_TYPE_POINTER,G_TYPE_OBJECT);
	treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	layout_widget(treeview,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(treeview),FALSE);
	g_signal_connect(treeview,"row-activated",G_CALLBACK(row_activated_tree_view),NULL);
	g_signal_connect(treeview,"cursor-changed",G_CALLBACK(cursor_changed_tree_view),NULL);
	g_object_unref(model);

	block_tree.view = GTK_TREE_VIEW(treeview);
	tree_add_column(&block_tree);
	fill_treeview(&block_tree);

	gtk_container_add(GTK_CONTAINER(frame),scrwin);
	gtk_container_add(GTK_CONTAINER(scrwin),treeview);

	gtk_widget_show(frame);
	gtk_widget_show(scrwin);
	gtk_widget_show(treeview);

	return frame;
}

/*****************************************************************************/
