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
#include "group.h"
#include "videocamera.h"
#include "controller.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

/*****************************************************************************/
/*    Локальные функции                                                      */
/*****************************************************************************/

static GSList * fill_gslict(uint32_t number_group,uint32_t * total_amount,object_s * parent)
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
		object->parent = parent;

		switch(type){
			case TYPE_GROUP:
				object->list = fill_gslict(number,&amount,object);
				object->property = init_group(object);
				break;
			case TYPE_VIDEOCAMERA:
				object->property = init_videocamera(object);
				break;
			case TYPE_CONTROLLERE:
				object->property = init_controller(object);
				break;
			default:
				g_slice_free1(sizeof(object_s),object);
				object = NULL;
				break;
		}
		if(object != NULL){
			list = g_slist_append(list,object);
		}

	}
	*total_amount = amount;

	return list;
}

/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

int add_object(object_s * parent,object_s * child)
{
	int rc;
	uint32_t number = FIRST_NUMBER_GROUP;

	if(parent == NULL){
		return FAILURE;
	}
	if(child == NULL){
		return FAILURE;
	}
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

int del_property(int type,void * property)
{
	switch(type){
		case TYPE_GROUP:
			del_property_group(property);
			break;
		case TYPE_VIDEOCAMERA:
			del_property_videocamera(property);
			break;
		case TYPE_CONTROLLERE:
			del_property_controller(property);
			break;
		case TYPE_UNKNOWN:
		default:
			break;
	}
	return SUCCESS;
}

int del_object(object_s * parent,object_s * child)
{
	int rc;
	uint32_t number = FIRST_NUMBER_GROUP;
	GSList * list;

	if(child == NULL){
		return FAILURE;
	}
	if( (parent == NULL) && (child->type != TYPE_KERNEL) ){
		return FAILURE;
	}
	if( (parent == NULL) && (child->type == TYPE_KERNEL)){
		/*удаление всех обектов из ядра*/
		object_s * object;
		list = child->list;
		for(;list;){
			object = list->data;
			del_object(child,object);
			list = child->list;
		}
		return SUCCESS;
	}
	if( child->type == TYPE_GROUP ){
		object_s * object;
		list = child->list;
		for(;list;){
			object = list->data;
			del_object(child,object);
			list = child->list;
		}
	}

	if( (parent->type != TYPE_KERNEL) && (parent->type != TYPE_GROUP)){
		return FAILURE;
	}

	if(parent->type == TYPE_GROUP){
		number = parent->number;
	}
	rc = del_object_database(number,child->number,child->type);
	if(rc != SUCCESS){
		return FAILURE;
	}
	parent->list = g_slist_remove(parent->list,child);
	del_property(child->type,child->property);
	g_slice_free1(sizeof(object_s),child);

	return SUCCESS;
}

#define MAX_NUMBER_OBJECT    0x000fffff
object_s kernel;

object_s * get_kernel(void)
{
	return &kernel;
}

GSList * list_kernel(void)
{
	return kernel.list;
}

uint32_t next_number_kernel(void)
{
	kernel.number ++;
	return kernel.number;
}

int init_kernel(void)
{
	uint32_t number = FIRST_NUMBER_GROUP;

	kernel.type = TYPE_KERNEL;
	kernel.list = fill_gslict(FIRST_NUMBER_GROUP,&number,NULL);
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
	del_property(o->type,o->property);
	g_slice_free1(sizeof(object_s),o);
}

int deinit_kernel(void)
{
	g_slist_free_full(kernel.list,kernel_free);
	return SUCCESS;
}
/*****************************************************************************/
