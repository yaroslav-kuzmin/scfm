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

typedef struct _block_tree_s block_tree_s;
struct _block_tree_s
{
	GtkTreeView * view;
	GdkPixbuf * image[AMOUNT_STATUS];
	uint32_t timeout_show;
	flag_t mode;
};
/*****************************************************************************/
/*    Локальные функции                                                      */
/*****************************************************************************/

static flag_t tree_add_column(block_tree_s * bt)
{
	GtkTreeView * treeview = bt->view;
	GtkCellRenderer * render;
	GtkTreeViewColumn * column;

	render = gtk_cell_renderer_pixbuf_new();
	/*column = gtk_tree_view_column_new_with_attributes("С",render,"pixbuf",COLUMN_IMAGE_TREE,NULL);*/
	g_object_set(render,"width",10,NULL);
	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column,"Статус");
	gtk_tree_view_column_pack_start(column,render,TRUE);
	gtk_tree_view_column_set_attributes(column,render,"pixbuf",COLUMN_IMAGE_TREE,NULL);
	gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column), GTK_TREE_VIEW_COLUMN_FIXED);

	gtk_tree_view_append_column (treeview,column);

	render = gtk_cell_renderer_text_new();
	g_object_set(render,"editable",FALSE,NULL);
	g_object_set(render,"width",70,NULL);
	/*размер шрифта*/
	/*g_object_set(render,"size",15000,NULL); */

	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column,"Наименования");
	gtk_tree_view_column_pack_start(column,render,TRUE);
	gtk_tree_view_column_set_attributes(column,render,"text",COLUMN_NAME_TREE,NULL);
	gtk_tree_view_column_set_sizing (column,GTK_TREE_VIEW_COLUMN_FIXED);

	gtk_tree_view_append_column(treeview,column);

	return SUCCESS;
}
/*TODO обеденить функции fill_treeview и fill_treeview_group */
static flag_t fill_treeview_group(GtkTreeStore * tree_model,GtkTreeIter * tree_iter,object_s * object,GdkPixbuf * image)
{
	GSList * list = NULL;
	GtkTreeIter child_iter;

	list = object->list;
	for(;list;){
		object_s * o = (object_s*)list->data;
		gtk_tree_store_append(tree_model,&child_iter,tree_iter);
		gtk_tree_store_set(tree_model,&child_iter
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

static flag_t fill_treeview(block_tree_s * bt)
{
	GtkTreeView * treeview = bt->view;
	GtkTreeSelection * select;
	GtkTreeModel * tree_model;
	GtkTreeIter tree_iter;
	GSList * list;
	GdkPixbuf * image = bt->image[STATUS_OFF];

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
#endif
}
static gboolean button_press_event_tree_view(GtkWidget *w,GdkEvent *event,gpointer ud)
{
	/*g_info("test button press");*/
	return FALSE;
}

static flag_t init_image(block_tree_s * bt)
{
	GdkPixbuf * buf;

	buf = get_resource_image(RESOURCE_STYLE,"tree-status-on-normal");
	bt->image[STATUS_ON_NORM] = buf;

	buf = get_resource_image(RESOURCE_STYLE,"tree-status-on-link-off");
	bt->image[STATUS_ON_LINK_OFF] = buf;

	buf = get_resource_image(RESOURCE_STYLE,"tree-status-on-crash");
	bt->image[STATUS_ON_CRASH] = buf;

	buf = get_resource_image(RESOURCE_STYLE,"tree-status-on-warning");
	bt->image[STATUS_ON_WARNING] = buf;

	buf = get_resource_image(RESOURCE_STYLE,"tree-status-off");
	bt->image[STATUS_OFF] = buf;

	return SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
static flag_t set_status_object(block_tree_s * bt,GtkTreeModel * tree_model,GtkTreeIter * iter,int status)
{
	GdkPixbuf * image;
	image = bt->image[status];
	gtk_tree_store_set(GTK_TREE_STORE(tree_model),iter,COLUMN_IMAGE_TREE,image,-1);
	return SUCCESS;
}

static flag_t set_status_all_objects(block_tree_s * bt,GtkTreeModel * model,GtkTreeIter * c_iter,GtkTreeIter * p_iter)
{
	int rc;
	object_s * object;
	GtkTreeIter cc_iter;

	rc = gtk_tree_model_iter_children(model,c_iter,p_iter);
	for(;rc;){
		gtk_tree_model_get(model,c_iter,COLUMN_POINT_TREE,&object,-1);
		set_status_object(bt,model,c_iter,object->status);
		rc = set_status_all_objects(bt,model,&cc_iter,c_iter);
		if(rc){
			break;
		}
		rc = gtk_tree_model_iter_next(model,c_iter);
	}

	return rc;
}

static flag_t tree_view_status(block_tree_s * bt)
{
	GtkTreeView * tree_view = bt->view;
	GtkTreeModel * tree_model;
	GtkTreeIter iter;

	tree_model = gtk_tree_view_get_model(tree_view);
	if(tree_model == NULL){
		return FAILURE;
	}

	set_status_all_objects(bt,tree_model,&iter,NULL);

	return SUCCESS;
}

static int show_block_tree(gpointer ud)
{
	block_tree_s * bt = (block_tree_s*)ud;

	if(bt->mode == MODE_CONTROL_OFF){
		return FALSE;
	}

	read_kernel_status();
	tree_view_status(bt);

	return TRUE;
}

static void tree_view_realize_main(GtkWidget * w,gpointer ud)
{
	/*block_tree_s * bt = (block_tree_s*)ud;*/
}

/*****************************************************************************/

static block_tree_s block_tree;

flag_t tree_check_status(flag_t mode)
{
	if(mode == MODE_CONTROL_ON){
		block_tree.mode = MODE_CONTROL_ON;
		g_timeout_add(block_tree.timeout_show,show_block_tree,&block_tree);
	}
	else{
		block_tree.mode = MODE_CONTROL_OFF;
		tree_view_status(&block_tree);
	}

	return SUCCESS;
}

flag_t reread_tree(void)
{
	GtkTreeModel * tree_model = gtk_tree_view_get_model(block_tree.view);

	if(tree_model == NULL){
		return SUCCESS;
	}
	gtk_tree_store_clear(GTK_TREE_STORE(tree_model));

	fill_treeview(&block_tree);
	return SUCCESS;
}


#define DEFAULT_TIMEOUT_SHOW          200    /*5 кадров в секунду отбражение информации*/

GtkWidget * create_block_tree_object(void)
{
	GtkWidget * frame;
	GtkWidget * scrwin;
	GtkWidget * treeview;
	GtkTreeStore * model;

	block_tree.timeout_show = DEFAULT_TIMEOUT_SHOW;
	block_tree.mode = MODE_CONTROL_OFF;

	init_image(&block_tree);

	frame = gtk_frame_new("Объекты");
	layout_widget(frame,GTK_ALIGN_START,GTK_ALIGN_FILL,FALSE,TRUE);
	gtk_widget_set_size_request(frame,DEFAULT_WIDTH_JOB_TREE,-1);
	gtk_container_set_border_width(GTK_CONTAINER(frame),DEFAULT_BORDER_JOB);
	gtk_frame_set_label_align(GTK_FRAME(frame),0,1);

	scrwin = gtk_scrolled_window_new(NULL,NULL);
	layout_widget(scrwin,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	model = gtk_tree_store_new(AMOUNT_COLUMN_TREE,G_TYPE_STRING,G_TYPE_POINTER,G_TYPE_OBJECT);
	/*TODO оформление дерева*/
	treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	layout_widget(treeview,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	g_signal_connect(treeview,"realize",G_CALLBACK(tree_view_realize_main),&block_tree);
	g_signal_connect(treeview,"row-activated",G_CALLBACK(row_activated_tree_view),NULL);
	g_signal_connect(treeview,"cursor-changed",G_CALLBACK(cursor_changed_tree_view),NULL);
	g_signal_connect(treeview,"button-press-event",G_CALLBACK(button_press_event_tree_view),NULL);
	g_object_unref(model);

	block_tree.view = GTK_TREE_VIEW(treeview);
	tree_add_column(&block_tree);
	fill_treeview(&block_tree);
	gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));

	gtk_container_add(GTK_CONTAINER(frame),scrwin);
	gtk_container_add(GTK_CONTAINER(scrwin),treeview);

	gtk_widget_show(frame);
	gtk_widget_show(scrwin);
	gtk_widget_show(treeview);

	return frame;
}

/*****************************************************************************/
