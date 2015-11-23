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
#include <stdint.h>
#include <gtk/gtk.h>

#include "pub.h"
#include "common.h"
#include "database.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/
struct _object_s
{
	uint8_t type;
	uint32_t number;
	char * name;
	void * property;
};
typedef struct _object_s object_s;

struct _group_s
{
	uint8_t type;
	uint32_t number;
	char * name;
	void * property;
	GSList * list;
};
typedef struct _group_s group_s;

struct _kernel_s
{
	GSList * list;
	uint32_t amount;
};
typedef struct _kernel_s kernel_s;

/*****************************************************************************/
/*    Локальные функции                                                      */
/*****************************************************************************/

static int full_gslict(kernel_s * k)
{
	int rc;
	void * query;
	uint32_t number;
	char * name;
	uint8_t type;
	object_s * o;
	group_s * g;

	query = prepare_group_database(FIRST_NUMBER_GROUP);
	if(query == NULL){
		return FAILURE;
	}
	for(;;){
		rc = next_group_database(query,&number,&name,&type);
		if(rc != SUCCESS){
			break;
		}
		name = g_strdup(name);
		g_debug(" number :> %d",number);
		g_debug(" name   :> %s",name);
		g_debug(" type   :> %d",type);
		o = NULL;
		g = NULL;
		switch(type){
			case TYPE_GROUP:
				g = (group_s*)g_slice_alloc(sizeof(group_s));
				g->number = number;
				g->type = type;
				g->name = name;
				g->property = NULL;
				g->list = NULL;
				break;
			case TYPE_VIDEOCAMERA:
				o = (object_s*)g_slice_alloc(sizeof(object_s));
				o->number = number;
				o->type = type;
				o->name = name;
				o->property = NULL;
				break;
			default:
				break;
		}
		if( o != NULL ){
			k->list = g_slist_append(k->list,o);
			k->amount++;
		}
		if( g != NULL ){
			k->list = g_slist_append(k->list,g);
			k->amount++;
		}
	}

	return SUCCESS;
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

kernel_s kernel;

int init_kernel(void)
{

#if 1
	schema_s schema = {"plan.png"};
	videocamera_s videocamera = {"rtsp","192.168.1.1",554,"/h_264"};
	add_object_database(0,1,"база 00",TYPE_GROUP,&schema);
	add_object_database(0,2,"видеокамера 00",TYPE_VIDEOCAMERA,&videocamera);
	add_object_database(0,3,"группа 01",TYPE_GROUP,&schema);
	add_object_database(0,4,"группа 02",TYPE_GROUP,&schema);
	add_object_database(3,5,"группа 05",TYPE_GROUP,&schema);
	add_object_database(3,6,"видео 01",TYPE_VIDEOCAMERA,&videocamera);

	/*del_object_database(0,3,TYPE_GROUP);*/
#endif

	kernel.list = g_slist_alloc();
	kernel.amount = 0;
	full_gslict(&kernel);

	return SUCCESS;
}

static void kernel_free(gpointer data)
{
	object_s * o = (object_s*)data;
	group_s * g = (group_s*)data;

	if(o == NULL){
		return;
	}

	if(o->name != NULL){
		g_free(o->name);
	}
	switch(o->type){
		case TYPE_GROUP:
			g_slice_free1(sizeof(group_s),g);
			break;
		case TYPE_VIDEOCAMERA:
			g_slice_free1(sizeof(object_s),o);
			break;
		default:
			break;
	}
}

int deinit_kernel(void)
{
	g_slist_free_full(kernel.list,kernel_free);
	return SUCCESS;
}
/*****************************************************************************/
