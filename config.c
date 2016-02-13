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
#include "tree.h"
#include "group.h"
#include "videocamera.h"
#include "controller.h"
#include "setting_controller.h"

/*****************************************************************************/
/*    локальные переменые                                                    */
/*****************************************************************************/
/*Основное дерево */
enum
{
	COLUMN_NAME_TREE = 0,  /*имя обеъкта*/
	COLUMN_POINT_TREE,     /*указатель на обект*/
	AMOUNT_COLUMN_TREE
};

struct _block_config_s
{
	/*название группы*/
	GtkTreeModel * model_group;
	GtkTreeIter * iter_group;
	GtkTreeIter * iter_parent_group;
	GtkLabel * select_group;
	object_s * parent_group;
	object_s * group;
	object_s * object;
	/*название объекта*/
	GtkEntryBuffer * name;
	/*тип объекта*/
	int type;
	/*TODO пересикается с pub.h*/
	GtkWidget * setting_unknown;
	GtkWidget * setting_group;
 	GtkWidget * setting_videocamera;
	GtkWidget * setting_controller;
};
typedef struct _block_config_s block_config_s;

/*****************************************************************************/
/*    локальные функции                                                      */
/*****************************************************************************/

static char STR_TREE_VIEW_COLUMN_NAME[] = "Наименования";

#define WIDTH_COLUMN_TREE             100
static int width_column_tree = WIDTH_COLUMN_TREE;

static int tree_add_column(GtkTreeView * treeview)
{
	GtkCellRenderer * render;
	GtkTreeViewColumn * column;

	render = gtk_cell_renderer_text_new();
	g_object_set(render,"editable",FALSE,NULL);
	g_object_set(render,"width",width_column_tree,NULL);
	g_object_set(render,"size",15000,NULL); /*размер шрифта*/

	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column,STR_TREE_VIEW_COLUMN_NAME);
	gtk_tree_view_column_pack_start(column,render,TRUE);
	gtk_tree_view_column_set_attributes(column,render,"text",COLUMN_NAME_TREE,NULL);
	gtk_tree_view_column_set_sizing (column,GTK_TREE_VIEW_COLUMN_FIXED);

	gtk_tree_view_append_column(treeview,column);

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
		gtk_tree_store_set(GTK_TREE_STORE(tree_model),&child_iter
		,COLUMN_NAME_TREE,o->name
		,COLUMN_POINT_TREE,o,-1);
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
	object_s * object;

	select =	gtk_tree_view_get_selection (treeview);
	gtk_tree_selection_get_selected(select,&tree_model,&tree_iter_root);

	object = get_kernel();
	gtk_tree_store_append(GTK_TREE_STORE(tree_model),&tree_iter_root,NULL);
	gtk_tree_store_set(GTK_TREE_STORE(tree_model),&tree_iter_root
	                  ,COLUMN_NAME_TREE,STR_ROOT_TREE
	                  ,COLUMN_POINT_TREE,object,-1);

	list = list_kernel();
	for(;list;){
		object = (object_s*)list->data;
		gtk_tree_store_append(GTK_TREE_STORE(tree_model),&tree_iter,&tree_iter_root);
		gtk_tree_store_set(GTK_TREE_STORE(tree_model),&tree_iter
		                  ,COLUMN_NAME_TREE,object->name
		                  ,COLUMN_POINT_TREE,object,-1);
		if(object->type == TYPE_GROUP){
			fill_treeview_group(GTK_TREE_STORE(tree_model),&tree_iter,object);
		}
		list = g_slist_next(list);
	}
	return SUCCESS;
}

static int add_object_treeview(block_config_s * config,object_s * object)
{
	GtkTreeModel * model = config->model_group;
	GtkTreeIter * iter_parent = config->iter_parent_group;
	GtkTreeIter * iter = config->iter_group ;

	gtk_tree_store_append(GTK_TREE_STORE(model),iter,iter_parent);
	gtk_tree_store_set(GTK_TREE_STORE(model),iter
		                  ,COLUMN_NAME_TREE,object->name
		                  ,COLUMN_POINT_TREE,object,-1);
	return SUCCESS;
}

static int del_object_treeview(block_config_s * config)
{
	object_s * group = config->group;
	object_s * object = config->object;
	GtkTreeModel * model = config->model_group;
	GtkTreeIter * iter = config->iter_group;

	if(group == NULL){
		return FAILURE;
	}

	if( object == NULL){
		iter = config->iter_parent_group;
	}

	gtk_tree_store_remove(GTK_TREE_STORE(model),iter);

	return SUCCESS;
}

static void row_activated_tree_view(GtkTreeView *tv,GtkTreePath *path,GtkTreeViewColumn *column,gpointer ud)
{
	g_debug("row_activated_tree_view");
}

static void cursor_changed_tree_view(GtkTreeView * tv,gpointer ud)
{
	int rc;
	block_config_s * config = (block_config_s*)ud;
	object_s * group;
	object_s * object;
	GtkTreeModel * model;
	GtkTreeIter * iter_parent = config->iter_parent_group;
	GtkTreeIter * iter = config->iter_group ;
	GtkTreeSelection * select = gtk_tree_view_get_selection(tv);

	gtk_label_set_text(config->select_group,STR_ROOT_TREE);
	config->parent_group = NULL;
	config->group = NULL;
	config->object = NULL;

	rc = gtk_tree_selection_get_selected(select,&model,iter_parent);
	if(rc != TRUE){
		return;
	}

	config->model_group = model;
	gtk_tree_model_get(model,iter_parent,COLUMN_POINT_TREE,&group,-1);
	if(group == NULL){
		g_warning("Некорректное дерево объектов 0!");
		return ;
	}

	gtk_tree_model_iter_children(model,iter,iter_parent);
	config->group = group;
	if(group->type == TYPE_KERNEL){
		return;
	}

	if(group->type == TYPE_GROUP){
		GtkTreeIter iter_parent_group;
		gtk_tree_model_iter_parent(model,&iter_parent_group,iter_parent);
		gtk_tree_model_get(model,&iter_parent_group,COLUMN_POINT_TREE,&object,-1);
		if(group == NULL){
			g_warning("Некорректное дерево объектов 1!");
			return ;
		}
		config->parent_group = object;
		gtk_label_set_text(config->select_group,group->name);
		return;
	}
	else{
		gtk_tree_selection_get_selected(select,&model,iter);
		config->model_group = model;
		gtk_tree_model_iter_parent(model,iter_parent,iter);
		gtk_tree_model_get(model,iter_parent,COLUMN_POINT_TREE,&group,-1);
		gtk_tree_model_get(model,iter,COLUMN_POINT_TREE,&object,-1);
		if( (group == NULL) || (object == NULL)){
			g_warning("Некорректное дерево объектов 2!");
			return ;
		}
		config->group = group;
		config->object = object;
		if(group->type == TYPE_KERNEL){
			return;
		}
		if(group->type == TYPE_GROUP){
			gtk_label_set_text(config->select_group,group->name);
		}
		else{
			g_warning("Некорректное дерево объектов 3!");
			config->group = NULL;
			config->object = NULL;
		}
	}
}

static char STR_TREE_FRAME[] = "Группы";
GtkTreeIter iter = {0};
GtkTreeIter iter_parent = {0};
static GtkWidget * create_block_tree(block_config_s * config)
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
	g_signal_connect(treeview,"row-activated",G_CALLBACK(row_activated_tree_view),config);
	g_signal_connect(treeview,"cursor-changed",G_CALLBACK(cursor_changed_tree_view),config);
	g_object_unref(model);
	fill_treeview(GTK_TREE_VIEW(treeview));
	config->model_group = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	config->iter_group = &iter;
	config->iter_parent_group = &iter_parent;
	config->group = NULL;

	gtk_container_add(GTK_CONTAINER(frame),scrwin);
	gtk_container_add(GTK_CONTAINER(scrwin),treeview);

	gtk_widget_show(frame);
	gtk_widget_show(scrwin);
	gtk_widget_show(treeview);

	return frame;
}

enum
{
	COLUMN_COMBOBOX_NAME = 0,
	COLUMN_COMBOBOX_TYPE,
	COLUMN_AMOUNT_COMBOBOX
};

static GtkTreeModel * create_model_combobox(void)
{
	GtkListStore * model;
	GtkTreeIter iter;
	model = gtk_list_store_new(COLUMN_AMOUNT_COMBOBOX,G_TYPE_STRING,G_TYPE_INT);
	gtk_list_store_append(model,&iter);
	gtk_list_store_set(model,&iter
	                  ,COLUMN_COMBOBOX_NAME,STR_TYPE_GROUP
	                  ,COLUMN_COMBOBOX_TYPE,TYPE_GROUP
	                  ,-1);
	gtk_list_store_append(model,&iter);
	gtk_list_store_set(model,&iter
	                  ,COLUMN_COMBOBOX_NAME,STR_TYPE_VIDEOCAMERE
	                  ,COLUMN_COMBOBOX_TYPE,TYPE_VIDEOCAMERA
	                  ,-1);
	gtk_list_store_append(model,&iter);
	gtk_list_store_set(model,&iter
	                  ,COLUMN_COMBOBOX_NAME,STR_TYPE_CONTROLLER
	                  ,COLUMN_COMBOBOX_TYPE,TYPE_CONTROLLERE
	                  ,-1);
	return GTK_TREE_MODEL(model);
}

static int select_setting(block_config_s * config)
{
	switch(config->type){
		case TYPE_UNKNOWN:
			gtk_widget_show(config->setting_unknown);
			gtk_widget_hide(config->setting_group);
			gtk_widget_hide(config->setting_videocamera);
			gtk_widget_hide(config->setting_controller);
			break;
		case TYPE_GROUP:
			gtk_widget_hide(config->setting_unknown);
			gtk_widget_show(config->setting_group);
			gtk_widget_hide(config->setting_videocamera);
			gtk_widget_hide(config->setting_controller);
			break;
		case TYPE_VIDEOCAMERA:
			gtk_widget_hide(config->setting_unknown);
			gtk_widget_hide(config->setting_group);
			gtk_widget_show(config->setting_videocamera);
			gtk_widget_hide(config->setting_controller);
			break;
		case TYPE_CONTROLLERE:
			gtk_widget_hide(config->setting_unknown);
			gtk_widget_hide(config->setting_group);
			gtk_widget_hide(config->setting_videocamera);
			gtk_widget_show(config->setting_controller);
			break;
		default:
			gtk_widget_show(config->setting_unknown);
			gtk_widget_hide(config->setting_group);
			gtk_widget_hide(config->setting_videocamera);
			gtk_widget_hide(config->setting_controller);
			break;
	}
	return SUCCESS;
}

static void changed_combobox(GtkComboBox *cb, gpointer ud)
{
	block_config_s * config = (block_config_s*)ud;
	GtkTreeModel * model = gtk_combo_box_get_model(cb);
	GtkTreeIter iter;
	int rc;

	rc = gtk_combo_box_get_active_iter(cb,&iter);
	if(rc == TRUE){
		gtk_tree_model_get(model,&iter,COLUMN_COMBOBOX_TYPE,&rc,-1);
		config->type = rc;
		select_setting(config);
	}
}
static GtkWidget * create_combobox(block_config_s * config)
{
	GtkWidget * combox;
	GtkTreeModel * model = create_model_combobox();
	GtkCellRenderer *renderer;

	combox = gtk_combo_box_new_with_model(model);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combox),-1);
	layout_widget(combox,GTK_ALIGN_FILL,GTK_ALIGN_CENTER,TRUE,FALSE);
	g_signal_connect(combox,"changed",G_CALLBACK(changed_combobox),config);
	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combox),renderer,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combox),renderer
	                              ,"text",COLUMN_COMBOBOX_NAME,NULL);
	g_object_unref(model);

	gtk_widget_show(combox);
	return combox;
}

static GtkWidget * create_setting_unknown(void)
{
	GtkWidget * label;
	label = gtk_label_new("Блок выбора");
	layout_widget(label,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_hide(label);
	return label;
}


static char STR_SETTING[] = "Свойства";
static GtkWidget * create_block_setting(block_config_s * config)
{
	GtkWidget * frame;
	GtkWidget * grid;

	frame = gtk_frame_new(STR_SETTING);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	config->setting_unknown = create_setting_unknown();
	config->setting_group = create_block_setting_group();
	config->setting_videocamera = create_block_setting_videocamera();
	config->setting_controller = create_block_setting_controller();
	config->type = TYPE_UNKNOWN;

	gtk_container_add(GTK_CONTAINER(frame),grid);

	gtk_grid_attach(GTK_GRID(grid),config->setting_unknown    ,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),config->setting_group      ,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),config->setting_videocamera,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),config->setting_controller ,0,0,1,1);

	gtk_widget_show(frame);
	gtk_widget_show(grid);
	select_setting(config);
	return frame;
}

static void * new_property(int type)
{
	void * property = NULL;
	switch(type){
		case TYPE_GROUP:
			property = new_property_group();
			break;
		case TYPE_VIDEOCAMERA:
			property = new_property_videocamera();
			break;
		case TYPE_CONTROLLERE:
			property = new_property_controller();
			break;
		case TYPE_UNKNOWN:
		default:
			break;
	}
	return property;
}

static void clicked_button_add(GtkButton * b,gpointer ud)
{
	int rc;
	block_config_s * config = (block_config_s*)ud;
	object_s * object;
	void * property = NULL;
	char * name = (char*)gtk_entry_buffer_get_text(config->name);

	if(config->group == NULL){
		g_warning("Не выбрана группа");
		return;
	}
	if( (name == NULL) || (*name == 0)){
		g_warning("Невведено имя объекта");
		return ;
	}
	/*TODO проверка на корректность имени*/

	if(config->type == TYPE_UNKNOWN){
		g_warning("Не выбран тип");
		return ;
	}

	property = new_property(config->type);
	if(property == NULL){
		g_warning("Нет свойств объекта");
		return;
	}

	object = g_slice_alloc0(sizeof(object_s));
	object->name = g_strdup(name);
	object->type = config->type;
	object->number = next_number_kernel();
	object->property = property;

	rc = add_object(config->group,object);
	if(rc == FAILURE){
		del_property(config->type,property);
		g_slice_free1(sizeof(object_s),object);
		return;
	}
	add_object_treeview(config,object);
}

static void clicked_button_del(GtkButton * b,gpointer ud)
{
	block_config_s * config = (block_config_s*)ud;
	object_s * parent_group = config->parent_group;
	object_s * group = config->group;
	object_s * object = config->object;

	if(group == NULL){
		return;
	}
	if( (object == NULL) && (parent_group == NULL)){
		return ; /*основу не удаляем*/
	}
	if(parent_group == NULL){
		del_object(group,object);
		goto exit_clicked_button_del;
	}
	if(object == NULL){
		del_object(parent_group,group);
		goto exit_clicked_button_del;
	}
exit_clicked_button_del:
	del_object_treeview(config);
}

static char STR_BUTTON_ADD[] = "добавить";
static char STR_BUTTON_DEL[] = "удалить";

static GtkWidget * create_block_button(block_config_s * config)
{
	GtkWidget * box;
	GtkWidget * but_add;
	GtkWidget * but_del;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_CENTER,TRUE,FALSE);

	but_add = gtk_button_new_with_label(STR_BUTTON_ADD);
	layout_widget(but_add,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,FALSE);
	g_signal_connect(but_add,"clicked",G_CALLBACK(clicked_button_add),config);

	but_del = gtk_button_new_with_label(STR_BUTTON_DEL);
	layout_widget(but_del,GTK_ALIGN_END,GTK_ALIGN_CENTER,TRUE,FALSE);
	g_signal_connect(but_del,"clicked",G_CALLBACK(clicked_button_del),config);

	gtk_box_pack_start(GTK_BOX(box),but_add,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(box),but_del,FALSE,FALSE,0);

	gtk_widget_show(box);
	gtk_widget_show(but_add);
	gtk_widget_show(but_del);

	return box;
}

static char STR_NAME_GROUP[] = "Группа";
static char STR_NAME_OBJECT[] = "Название";
static char STR_TYPE_OBJECT[] = "Тип";

static GtkWidget * create_block_option(block_config_s * config)
{
	GtkWidget * grid;

	GtkWidget * lab_name_group;
	GtkWidget * lab_select;

	GtkWidget * lab_name_object;
	GtkWidget * ent_name_object;

	GtkWidget * lab_name_type;
	GtkWidget * combox;

	GtkWidget * block_setting;
	GtkWidget * block_button;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	lab_name_group = gtk_label_new(STR_NAME_GROUP);
	layout_widget(lab_name_group,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,FALSE);

	lab_select = gtk_label_new(STR_ROOT_TREE);
	layout_widget(lab_select,GTK_ALIGN_FILL,GTK_ALIGN_CENTER,TRUE,FALSE);
	config->select_group = GTK_LABEL(lab_select);

	lab_name_object = gtk_label_new(STR_NAME_OBJECT);
	layout_widget(lab_name_object,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,FALSE);

	ent_name_object = gtk_entry_new();
	layout_widget(ent_name_object,GTK_ALIGN_FILL,GTK_ALIGN_CENTER,TRUE,FALSE);
	config->name = gtk_entry_get_buffer(GTK_ENTRY(ent_name_object));

	lab_name_type = gtk_label_new(STR_TYPE_OBJECT);
	layout_widget(lab_name_type,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,FALSE);
	combox = create_combobox(config);

	block_setting = create_block_setting(config);

	block_button = create_block_button(config);

	gtk_grid_attach(GTK_GRID(grid),lab_name_group ,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),lab_select     ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),lab_name_object,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),ent_name_object,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),lab_name_type  ,0,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),combox         ,1,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_setting  ,0,3,2,1);
	gtk_grid_attach(GTK_GRID(grid),block_button   ,0,4,2,1);

	gtk_widget_show(grid);
	gtk_widget_show(lab_name_group);
	gtk_widget_show(lab_select);
	gtk_widget_show(lab_name_object);
	gtk_widget_show(ent_name_object);
	gtk_widget_show(lab_name_type);
	gtk_widget_show(ent_name_object);

	return grid;
}

static char STR_FRAME_CONFIG[] = "Конфигурирование";

static GtkWidget * create_block_config(block_config_s * config)
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

struct _config_window_s
{
	GtkWidget * main;
	GtkWidget * config;
	int main_exit;
};
typedef struct _config_window_s config_window_s;

static void clicked_button_exit(GtkButton * b,gpointer ud)
{
	config_window_s * win = (config_window_s*)ud;
	reread_tree();
	gtk_widget_show(win->main);
	win->main_exit = NOT_OK;
	gtk_widget_destroy(win->config);
}

static gboolean key_press_event_window_config(GtkWidget * w,GdkEvent  *event,gpointer ud)
{
	config_window_s * win = (config_window_s*)ud;
	GdkEventType type = event->type;
	gint state;

	if(type == GDK_KEY_PRESS){
		GdkEventKey * event_key = (GdkEventKey*)event;
		state = event_key->state;
		if( (state & GDK_MOD1_MASK) ){
			if( event_key->keyval == GDK_KEY_F4){
				gtk_widget_destroy(win->config);
			}
		}
	}
	return FALSE;
}

static void destroy_window_config(GtkWidget * w,gpointer ud)
{
	config_window_s * win = (config_window_s*)ud;
	if(win->main_exit == OK){
		gtk_widget_destroy(win->main);
	}
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
#define CONFIG_BLOCK_SPACING   5
static char STR_CONFIG_EXIT[] = "выход";
static config_window_s config_window;

#define MIN_WIDTH_WIN_CONFIG      500
#define MIN_HEIGHT_WIN_CONFIG     500

static block_config_s total_config = {0};

int create_window_config(GtkWidget * win_main)
{
	GtkWidget * win_config;
	GtkWidget * box;
	GtkWidget * block_config;
	GtkWidget * exit;

	config_window.main_exit = OK;
	config_window.main = win_main;
	win_config = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	config_window.config = win_config;
	gtk_container_set_border_width(GTK_CONTAINER(win_config),CONFIG_BLOCK_SPACING);
	gtk_window_set_title(GTK_WINDOW(win_config),STR_NAME_PROGRAMM);
	gtk_window_set_resizable(GTK_WINDOW(win_config),TRUE);
	gtk_window_set_position (GTK_WINDOW(win_config),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(win_config),MIN_WIDTH_WIN_CONFIG,MIN_HEIGHT_WIN_CONFIG);
	g_signal_connect(win_config,"destroy",G_CALLBACK(destroy_window_config),&config_window);
	g_signal_connect(win_config,"key-press-event",G_CALLBACK(key_press_event_window_config),&config_window);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_config = create_block_config(&total_config);

	exit = gtk_button_new_with_label(STR_CONFIG_EXIT);
	layout_widget(exit,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(exit,"clicked",G_CALLBACK(clicked_button_exit),&config_window);

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
