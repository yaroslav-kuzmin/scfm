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
	GtkFrame * frame;

	GtkWidget * empty;
	GtkWidget * group;
	GtkWidget * videocamera;
	GtkWidget * controller;
};
typedef struct _block_object_s block_object_s;

/*****************************************************************************/
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
		case TYPE_CONTROLLER:
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


/*****************************************************************************/
/*****************************************************************************/
block_object_s block_object;

flag_t select_object(object_s * object)
{
#if !(TEST_INTERFACE)
	flag_t rc;
#endif

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

	gtk_frame_set_label(block_object.frame,object->name);

	select_block_controller(NULL);
	switch(object->type){
		case TYPE_GROUP:
			select_block_group((group_s*)object->property);
			break;
		case TYPE_VIDEOCAMERA:
			select_block_videcamera((videocamera_s*)object->property);
			break;
		case TYPE_CONTROLLER:
			select_block_controller((controller_s*)object->property);
			break;
		default:
			break;
	}
	change_object(&block_object,object->type);
	return SUCCESS;
}

GtkWidget * create_block_object(void)
{
	GtkWidget * frame;
	GtkWidget * grid;

	frame = gtk_frame_new("Нет обекта");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_set_size_request(frame,DEFAULT_WIDTH_JOB_OBJECT,-1);
	gtk_container_set_border_width(GTK_CONTAINER(frame),DEFAULT_BORDER_JOB);
	gtk_frame_set_label_align(GTK_FRAME(frame),0,1);

	block_object.frame = GTK_FRAME(frame);

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_object.empty = create_block_empty();
	block_object.group = create_block_group();
	block_object.videocamera = create_block_videocamera();
	block_object.controller = create_block_controller();

	gtk_container_add(GTK_CONTAINER(frame),grid);

	gtk_grid_attach(GTK_GRID(grid),block_object.empty,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_object.group,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_object.videocamera,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_object.controller,0,0,1,1);

	gtk_widget_show(frame);
	gtk_widget_show(grid);

	change_object(&block_object,TYPE_UNKNOWN);

	return frame;
}

/*****************************************************************************/
flag_t object_status(object_s * object)
{
	flag_t status = STATUS_OFF;
	int type = object->type;

	switch (type){
		case TYPE_CONTROLLER:
			status = controller_status(object->property);
			break;
		case TYPE_VIDEOCAMERA:
			status = videocamera_status(object->property);
			break;
		case TYPE_GROUP:
		case TYPE_KERNEL:
		case TYPE_UNKNOWN:
		default:
			break;
	}

	if(status != object->status){
		char * str;
		switch(status){
			case STATUS_OFF:
				str = MESSAGE_STATUS_OFF;
			 	break;
			case STATUS_ERROR:
				str = MESSAGE_STATUS_ERROR;
				break;
			case STATUS_WAIT:
				str = MESSAGE_STATUS_WAIT;
				break;
			case STATUS_NORM:
				str = MESSAGE_STATUS_NORM;
				break;
			default:
				str = MESSAGE_STATUS_OFF;
				break;
		}
		g_info("Состояние %s : %s",object->name,str);
	}

	return status;
}
/*****************************************************************************/

