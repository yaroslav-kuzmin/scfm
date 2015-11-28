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
#include "group.h"
#include "videocamera.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

/*****************************************************************************/
/*    Локальные функции                                                      */
/*****************************************************************************/

static GSList * fill_gslict(uint32_t number_group,uint32_t * total_amount)
{
	int rc;
	GSList * list = NULL;
	void * query;
	uint32_t number;
	char * name;
	uint8_t type;
	object_s * object;
	uint32_t amount = *total_amount;

	query = prepare_group_database(number_group);
	if(query == NULL){
		return NULL;
	}
	for(;;){
		rc = next_group_database(query,&number,&name,&type);
		if(rc != SUCCESS){
			break;
		}
		object = (object_s*)g_slice_alloc(sizeof(object_s));
		object->number = number;
		if(amount < number){
			amount = number;
		}
		object->type = type;
		object->name = g_strdup(name);
		object->property = NULL;
		object->list = NULL;

		switch(type){
			case TYPE_GROUP:
				object->list = fill_gslict(number,&amount);
				fill_group(object);
				break;
			case TYPE_VIDEOCAMERA:
				fill_videocamera(object);
				break;
			default:
				g_slice_free1(sizeof(object_s),object);
				object = NULL;
				break;
		}
		if(object != NULL){
			list = g_slist_append(list,object);
		}

		g_debug("\n amount :> %d",amount);
		g_debug(" number :> %d",object->number);
		g_debug(" type   :> %#x",object->type);
		g_debug(" name   :> %s",object->name);
	}
	*total_amount = amount;

	return list;
}

int add_object(object_s * parent,object_s * child)
{
	int rc;
	uint32_t number = FIRST_NUMBER_GROUP;

	if( (parent->type != TYPE_KERNEL) && (parent->type != TYPE_GROUP)){
		return FAILURE;
	}

	if(parent->type == TYPE_GROUP){
		number = parent->number;
	}
	rc = add_object_database(number,child->number,child->name,child->type,child->property);
	if(rc != SUCCESS){
		return FAILURE;
	}
	parent->list = g_slist_append(parent->list,child);
	return SUCCESS;
}

int del_object(object_s * parent,object_s * child)
{
	return SUCCESS;
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

#define MAX_NUMBER_OBJECT    0x000fffff
object_s kernel;

GSList * kernel_list(void)
{
	return kernel.list;
}

int init_kernel(void)
{
	uint32_t number = 0;

	kernel.type = TYPE_KERNEL;
	kernel.list = fill_gslict(FIRST_NUMBER_GROUP,&number);
	kernel.number = number;
	kernel.name = STR_NAME_PROGRAMM;
	kernel.property = NULL;

	return SUCCESS;
}

static void kernel_free(gpointer data)
{
	object_s * o = (object_s*)data;

	if(o == NULL){
		return;
	}

	if(o->name != NULL){
		g_free(o->name);
	}
	switch(o->type){
		case TYPE_GROUP:
			break;
		case TYPE_VIDEOCAMERA:
			break;
		default:
			break;
	}
	g_slice_free1(sizeof(object_s),o);
}

int deinit_kernel(void)
{
	g_slist_free_full(kernel.list,kernel_free);
	return SUCCESS;
}
/*****************************************************************************/
