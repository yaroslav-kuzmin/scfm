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
#include <sqlite3.h>

#include "pub.h"
#include "common.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

static sqlite3 * database = NULL;

/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/

static int query_simple(GString * query)
{
	int rc;
	char * error_message = NULL;
	rc = sqlite3_exec(database,query->str,NULL,NULL, &error_message);
	if( rc != SQLITE_OK){
		g_critical("SQL error : %s : %s!",query->str,error_message);
		sqlite3_free(error_message);
		return FAILURE;
	}
	return SUCCESS;
}

static int create_table_object(int number)
{
	g_string_printf(pub,"CREATE TABLE ");
	g_string_append_printf(pub,FORAMT_NAME_TABLE_OBJECT,number);
	g_string_append(pub,"(number INTEGER PRIMARY KEY,name,type INTEGER)");
	return query_simple(pub);
}
static char STR_NAME_TABLE_GROUP[] = "[group]";
static int create_table_group(void)
{
	g_string_printf(pub,"CREATE TABLE ");
	g_string_append(pub,STR_NAME_TABLE_GROUP);
	g_string_append(pub,"(number INTEGER PRIMARY KEY,image)");
	return query_simple(pub);
}
static char STR_NAME_TABLE_VIDEOCAMERA[] = "[videocamera]";
static int create_table_videocamera(void)
{
	g_string_printf(pub,"CREATE TABLE ");
	g_string_append(pub,STR_NAME_TABLE_VIDEOCAMERA);
	g_string_append(pub,"(number INTEGER PRIMARY KEY,protocol,address,port,access)");
	return query_simple(pub);
}

static char STR_NAME_TABLE_CONTROLLER[] = "[controller]";
static int create_table_controller(void)
{
	g_string_printf(pub,"CREATE TABLE ");
	g_string_append(pub,STR_NAME_TABLE_CONTROLLER);
	g_string_append(pub,"(number INTEGER PRIMARY KEY \
,type_link,id,address,port,device,baud,parity,data_bit,stop_bit \
,type,flag \
,tic_vertical FLOAT,encoder_vertical FLOAT,amperage_vertical FLOAT \
,tic_horizontal FLOAT,encoder_horizontal FLOAT,amperage_horizontal FLOAT\
,pressure FLOAT,valve_analog FLOAT)");
	return query_simple(pub);
}
static int delete_table_object(int number)
{
	g_string_printf(pub,"DROP TABLE ");
	g_string_append_printf(pub,FORAMT_NAME_TABLE_OBJECT,number);
	return query_simple(pub);
}

static int create_total_table(void)
{
	create_table_object(FIRST_NUMBER_GROUP);
	create_table_group();
	create_table_videocamera();
	create_table_controller();

	return SUCCESS;
}

static int database_open(GString * name)
{
	int rc;
	if(name == NULL){
		return FAILURE;
	}

	rc = sqlite3_open(name->str,&database); // откравает базу данных
	if(rc != SQLITE_OK){
		g_string_printf(pub,"Несмог открыть базу данных %s : %s!",name->str,sqlite3_errmsg(database));
		dialog_error(pub->str);
		return FAILURE;
	}
	return SUCCESS;
}

static int create_default_database(GString * name)
{
	int rc;

	rc = database_open(name);
	if(rc != SUCCESS){
		return rc;
	}
	create_total_table();

	return SUCCESS;
}
/*****************************************************************************/

static int add_info_object_table(uint32_t number_group,uint32_t number_object,char *name,uint8_t type)
{
	g_string_printf(pub,"INSERT INTO ");
	g_string_append_printf(pub,FORAMT_NAME_TABLE_OBJECT,number_group);
	g_string_append(pub," VALUES (");
	g_string_append_printf(pub,"%d,\'%s\',%d)",number_object,name,type);
	return query_simple(pub);
}

static int del_info_object_table(uint32_t number_group,uint32_t number_object)
{
	g_string_printf(pub,"DELETE FROM ");
	g_string_append_printf(pub,FORAMT_NAME_TABLE_OBJECT,number_group);
	g_string_append_printf(pub," WHERE number=%d",number_object);
	return query_simple(pub);
}

static int add_table_group(uint32_t number,group_s * group)
{
	int rc;

	rc = create_table_object(number);
	if(rc != SUCCESS ){
		return rc;
	}

	g_string_printf(pub,"INSERT INTO ");
	g_string_append(pub,STR_NAME_TABLE_GROUP);
	g_string_append_printf(pub," VALUES (%d,\'%s\')",number,group->image);

	return query_simple(pub);
}

static int del_table_group(uint32_t number)
{
	int rc;

	rc = delete_table_object(number);
	if(rc != SUCCESS){
		return rc;
	}

	g_string_printf(pub,"DELETE FROM ");
	g_string_append(pub,STR_NAME_TABLE_GROUP);
	g_string_append_printf(pub," WHERE number=%d",number);

	return query_simple(pub);
}

static int add_table_videcamera(uint32_t n,videocamera_s * v)
{
	g_string_printf(pub,"INSERT INTO ");
	g_string_append(pub,STR_NAME_TABLE_VIDEOCAMERA);
	g_string_append_printf(pub," VALUES (%d,\'%s\',\'%s\',%d,\'%s\')"
	                      ,n,v->protocol,v->address,v->port,v->access);
	return query_simple(pub);
}

static int del_table_videcamera(uint32_t number)
{
	g_string_printf(pub,"DELETE FROM ");
	g_string_append(pub,STR_NAME_TABLE_VIDEOCAMERA);
	g_string_append_printf(pub," WHERE number=%d",number);

	return query_simple(pub);
}

static int add_table_controller(uint32_t number,controller_s * controller)
{
	link_s * link = controller->link;
	config_controller_s * config = controller->config;
	double temp;
	g_string_printf(pub,"INSERT INTO ");
	g_string_append(pub,STR_NAME_TABLE_CONTROLLER);
/*
	g_string_append_printf(pub," VALUES (%d,%d,%d,\'%s\',%d,\'%s\',%d,%d,%d,%d \
,%d,%lld,\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\',\'%f\')"
	                      ,number
	                      ,link->type,link->id,link->address,link->port
	                      ,link->device,link->baud,link->parity,link->data_bit,link->stop_bit
	                      ,config->type,config->flag
	                      ,config->rate_tic_vertical
	                      ,config->rate_encoder_vertical
	                      ,config->rate_amperage_vertical
	                      ,config->rate_tic_horizontal
	                      ,config->rate_encoder_horizontal
	                      ,config->rate_amperage_horizontal
	                      ,config->rate_pressure
	                      ,config->rate_valve_analog);
*/
	g_string_append_printf(pub," VALUES (%d,",number);
	g_string_append_printf(pub,"%d,%d,",link->type,link->id);
	g_string_append_printf(pub,"%s,%d,",link->address,link->port);
	g_string_append_printf(pub,"%s,%d,%d,%d,%d,",link->device,link->baud,link->parity,link->data_bit,link->stop_bit);
	g_string_append_printf(pub,"%d,%ld,",config->type,config->flag);

	g_string_append_printf(pub,"%f,",config->rate_tic_vertical);
	g_string_append_printf(pub,"%f,",config->rate_encoder_vertical);
	g_string_append_printf(pub,"%f,",config->rate_amperage_vertical);
	g_string_append_printf(pub,"%f,",config->rate_tic_horizontal);
	g_string_append_printf(pub,"%f,",config->rate_encoder_horizontal);
	g_string_append_printf(pub,"%f,",config->rate_amperage_horizontal);
	g_string_append_printf(pub,"%f,",config->rate_pressure);
	g_string_append_printf(pub,"%f)",config->rate_valve_analog);
	g_debug(":> %s",pub->str);
	temp = 0.256;
	g_string_printf(pub,":: %f : %f",temp,config->rate_pressure);
	printf(":> %s\n",pub->str);
	printf(":> %f : %f\n",temp,config->rate_pressure);
	/*return query_simple(pub);*/
	return FAILURE;
}

static int del_table_controller(uint32_t number)
{
	g_string_printf(pub,"DELETE FROM ");
	g_string_append(pub,STR_NAME_TABLE_CONTROLLER);
	g_string_append_printf(pub," WHERE number=%d",number);

	return query_simple(pub);
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

void* prepare_group_database(uint32_t number)
{
	int rc;
	sqlite3_stmt * stmt;

	g_string_printf(pub,"SELECT * FROM ");
	g_string_append_printf(pub,FORAMT_NAME_TABLE_OBJECT,number);

	rc = sqlite3_prepare_v2(database,pub->str,(pub->len+1) ,&stmt,NULL);
	if(rc != SQLITE_OK){
		const char * error_message = sqlite3_errmsg(database);
		g_critical("SQL prepare_v2 : %s : %s\n",pub->str,error_message);
		return NULL;
	}
	return (void*)stmt;
}

int next_group_database(void * s,uint32_t * number,char **name,uint8_t * type)
{
	int rc;
	sqlite3_stmt * stmt = (sqlite3_stmt*)s;
	int amount_column = 0;
	*number = 0;
	*name = NULL;
	*type = TYPE_UNKNOWN;

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_DONE){
		/* данных в запросе нет*/
		sqlite3_finalize(stmt);
		return FAILURE;
	}
	if(rc == SQLITE_ERROR ){
		g_critical("SQL step : %s",sqlite3_errmsg(database));
		sqlite3_finalize(stmt);
		return FAILURE;
	}
	if(rc == SQLITE_ROW){
		/*строка запроса*/
		amount_column = sqlite3_data_count(stmt);
		if(amount_column != COLUMN_TABLE_OBJECT_AMOUNT){
			g_critical("SQL step : Некорректная таблица групп : %d",amount_column);
			sqlite3_finalize(stmt);
			return FAILURE;
		}
		/*TODO корректный типы таблицы*/
		*number = sqlite3_column_int64(stmt,COLUMN_TABLE_OBJECT_NUMBER);
		*name = (char*)sqlite3_column_text(stmt,COLUMN_TABLE_OBJECT_NAME);
		*type = sqlite3_column_int(stmt,COLUMN_TABLE_OBJECT_TYPE);
	}
	return SUCCESS;
}

int add_object_database(uint32_t number_group,uint32_t number_object,char * name,uint8_t type,void * property)
{
	int rc;
	switch(type){
		case TYPE_GROUP:
			rc = add_table_group(number_object,(group_s*)property);
			if(rc != SUCCESS){
				return rc;
			}
			break;
		case TYPE_VIDEOCAMERA:
			rc = add_table_videcamera(number_object,(videocamera_s*)property);
			if(rc != SUCCESS){
				return rc;
			}
			break;
		case TYPE_CONTROLLERE:
			rc = add_table_controller(number_object,(controller_s*)property);
			if(rc != SUCCESS){
				return rc;
			}
			break;
		default :
			return FAILURE;
	}

	/*TODO проверка на неполную вставку обектов*/
	return add_info_object_table(number_group,number_object,name,type);
}
/*TODO добавить функцию update*/
int del_object_database(uint32_t number_group,uint32_t number_object,uint8_t type)
{
	int rc;
	switch(type){
		case TYPE_GROUP:
			rc = del_table_group(number_object);
			if(rc != SUCCESS){
				return rc;
			}
			break;
		case TYPE_VIDEOCAMERA:
			rc = del_table_videcamera(number_object);
			if(rc != SUCCESS){
				return rc;
			}
			break;
		case TYPE_CONTROLLERE:
			rc = del_table_controller(number_object);
			if(rc != SUCCESS){
				return rc;
			}
			break;
		default :
			return FAILURE;
	}

	return del_info_object_table(number_group,number_object);
}

int read_database_group(uint32_t number,group_s * group)
{
	int rc;
	sqlite3_stmt * stmt;
	int amount_column = 0;
	const char * image;

	g_string_printf(pub,"SELECT * FROM ");
	g_string_append_printf(pub,"%s WHERE number=%d",STR_NAME_TABLE_GROUP,number);

	rc = sqlite3_prepare_v2(database,pub->str,(pub->len+1),&stmt,NULL);
	if(rc != SQLITE_OK){
		const char * error_message = sqlite3_errmsg(database);
		g_critical("SQL prepare_v2 : %s : %s\n",pub->str,error_message);
		return FAILURE;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_DONE){
		/* данных в запросе нет*/
		g_critical("Ошибка ведения таблицы [group]!");
		sqlite3_finalize(stmt);
		/*TODO функция исправления базы данных*/
		return FAILURE;
	}
	if(rc == SQLITE_ERROR ){
		g_critical("SQL step : %s",sqlite3_errmsg(database));
		sqlite3_finalize(stmt);
		return FAILURE;
	}
	if(rc == SQLITE_ROW){
		amount_column = sqlite3_data_count(stmt);
		if(amount_column != COLUMN_TABLE_GROUP_AMOUNT){
			g_critical("SQL step : Некорректная таблица групп : %d",amount_column);
			sqlite3_finalize(stmt);
			return FAILURE;
		}
		/*TODO проверка на тип колонок и название */
		/*TODO где делать проверки на корректность*/
		image = (const char *)sqlite3_column_text(stmt,COLUMN_TABLE_GROUP_IMAGE);
		group->image = g_strdup(image);
	}
	sqlite3_finalize(stmt);
	return SUCCESS;
}

int read_database_videocamera(uint32_t number,videocamera_s * videocamera)
{
	int rc;
	sqlite3_stmt * stmt;
	int amount_column = 0;
	const char * protocol;
	const char * address;
	const char * access;

	g_string_printf(pub,"SELECT * FROM ");
	g_string_append_printf(pub,"%s WHERE number=%d",STR_NAME_TABLE_VIDEOCAMERA,number);

	rc = sqlite3_prepare_v2(database,pub->str,(pub->len+1),&stmt,NULL);
	if(rc != SQLITE_OK){
		const char * error_message = sqlite3_errmsg(database);
		g_critical("SQL prepare_v2 : %s : %s\n",pub->str,error_message);
		return FAILURE;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_DONE){
		/* данных в запросе нет*/
		g_critical("Ошибка ведения таблицы [videocamera]!");
		sqlite3_finalize(stmt);
		/*TODO функция исправления базы данных*/
		return FAILURE;
	}
	if(rc == SQLITE_ERROR ){
		g_critical("SQL step : %s",sqlite3_errmsg(database));
		sqlite3_finalize(stmt);
		return FAILURE;
	}
	if(rc == SQLITE_ROW){
		amount_column = sqlite3_data_count(stmt);
		if(amount_column != COLUMN_TABLE_VIDEOCAMERA_AMOUNT){
			g_critical("SQL step : Некорректная таблица видеокамер : %d",amount_column);
			sqlite3_finalize(stmt);
			return FAILURE;
		}
		/*TODO проверка на тип колонок и название */
		/*TODO где делать проверки на корректность*/
		protocol = (const char*)sqlite3_column_text(stmt,COLUMN_TABLE_VIDEOCAMERA_PROTOCOL);
		videocamera->protocol = g_strdup(protocol);
		address = (const char *)sqlite3_column_text(stmt,COLUMN_TABLE_VIDEOCAMERA_ADDRESS);
		videocamera->address = g_strdup(address);
		videocamera->port = sqlite3_column_int64(stmt,COLUMN_TABLE_VIDEOCAMERA_PORT);
		access = (const char *)sqlite3_column_text(stmt,COLUMN_TABLE_VIDEOCAMERA_ACCESS);
		videocamera->access = g_strdup(access);
	}
	sqlite3_finalize(stmt);
	return SUCCESS;
}

int read_database_controller(uint32_t number,controller_s * controller)
{
	int rc;
	sqlite3_stmt * stmt;
	int amount_column = 0;
	const char * address;
	const char * device;
	double temp;
	const char * str;

	g_string_printf(pub,"SELECT * FROM ");
	g_string_append_printf(pub,"%s WHERE number=%d",STR_NAME_TABLE_CONTROLLER,number);

	rc = sqlite3_prepare_v2(database,pub->str,(pub->len+1),&stmt,NULL);
	if(rc != SQLITE_OK){
		const char * error_message = sqlite3_errmsg(database);
		g_critical("SQL prepare_v2 : %s : %s\n",pub->str,error_message);
		return FAILURE;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_DONE){
		/* данных в запросе нет*/
		g_critical("Ошибка ведения таблицы [controller]!");
		sqlite3_finalize(stmt);
		/*TODO функция исправления базы данных*/
		return FAILURE;
	}
	if(rc == SQLITE_ERROR ){
		g_critical("SQL step : %s",sqlite3_errmsg(database));
		sqlite3_finalize(stmt);
		return FAILURE;
	}
	if(rc == SQLITE_ROW){
		amount_column = sqlite3_data_count(stmt);
		if(amount_column != COLUMN_TABLE_CONTROLLER_AMOUNT){
			g_critical("SQL step : Некорректная таблица контроллеров : %d",amount_column);
			sqlite3_finalize(stmt);
			return FAILURE;
		}
		/*TODO проверка на тип колонок и название */
		/*TODO где делать проверки на корректность*/
		controller->link->type = sqlite3_column_int64(stmt,COLUMN_TABLE_CONTROLLER_TYPE_LINK);
		controller->link->id = sqlite3_column_int64(stmt,COLUMN_TABLE_CONTROLLER_ID);
		address = (const char*)sqlite3_column_text(stmt,COLUMN_TABLE_CONTROLLER_ADDRESS);
		controller->link->address = g_strdup(address);
		controller->link->port = sqlite3_column_int64(stmt,COLUMN_TABLE_CONTROLLER_PORT);
		device = (const char*)sqlite3_column_text(stmt,COLUMN_TABLE_CONTROLLER_DEVICE);
		controller->link->device = g_strdup(device);
		controller->link->baud = sqlite3_column_int64(stmt,COLUMN_TABLE_CONTROLLER_BAUD);
		controller->link->parity = sqlite3_column_int64(stmt,COLUMN_TABLE_CONTROLLER_PARITY);
		controller->link->data_bit = sqlite3_column_int64(stmt,COLUMN_TABLE_CONTROLLER_DATA_BIT);
		controller->link->stop_bit = sqlite3_column_int64(stmt,COLUMN_TABLE_CONTROLLER_STOP_BIT);
		controller->config->type = sqlite3_column_int64(stmt,COLUMN_TABLE_CONTROLLER_TYPE);
		controller->config->flag = sqlite3_column_int64(stmt,COLUMN_TABLE_CONTROLLER_FLAG);
		controller->config->rate_tic_vertical = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_TIC_VERTICAL);
		controller->config->rate_encoder_vertical = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_ENCODER_VERTICAL);
		controller->config->rate_amperage_vertical = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_AMPERAGE_VERTICAL);
		controller->config->rate_tic_horizontal = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_TIC_HORIZONTAL);
		controller->config->rate_encoder_horizontal = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_ENCODER_HORIZONTAL);
		controller->config->rate_amperage_horizontal = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_AMPERAGE_HORIZONTAL);

		/*controller->config->rate_pressure = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_PRESSURE);*/
		temp = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_PRESSURE);
		g_debug("temp :> %f : %g",temp,temp);
		str = (const char *)sqlite3_column_text(stmt,COLUMN_TABLE_CONTROLLER_PRESSURE);
		g_debug("str :> %s",str);
		temp = g_strtod(str,NULL);
		g_debug("temp :> %f : %g",temp,temp);
		controller->config->rate_pressure = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_PRESSURE);

		controller->config->rate_valve_analog = sqlite3_column_double(stmt,COLUMN_TABLE_CONTROLLER_VALVE_ANALOG);
	}
	sqlite3_finalize(stmt);
	return SUCCESS;
}
/*****************************************************************************/
static char STR_KEY_DATABASE[] = "database";
static char STR_DATABASE_FILE[] = "scfm.sqlite3";

int default_database_config(void)
{
	g_key_file_set_string(system_config,STR_GROUP_GLOBAL,STR_KEY_DATABASE,STR_DATABASE_FILE);
	return SUCCESS;
}
int reindex_database(void)
{
/*TODO сделать преобразование идентификаторов в базе данных*/
/*TODO прверка на корректность данных */
	return SUCCESS;
}

static GString * database_name = NULL;

int check_database(GString * work_catalog)
{
	int rc;
	char * str = NULL;
	GError * err = NULL;

	database_name = g_string_new(work_catalog->str);

	str = g_key_file_get_string(system_config,STR_GROUP_GLOBAL,STR_KEY_DATABASE,&err);
	if(str == NULL){
		g_string_append(database_name,STR_DATABASE_FILE);
		default_database_config();
		set_flag_save_config();
	}
	else{
		g_string_append(database_name,str);
	}
	rc = g_file_test(database_name->str,G_FILE_TEST_IS_REGULAR);
	if(rc == FALSE){
		return create_default_database(database_name);
	}
	return SUCCESS;
}

int init_database(GString * work_catalog)
{
	int rc;

	check_database(work_catalog);

	if(database == NULL){
		rc = database_open(database_name);
		if(rc != SUCCESS){
			return rc;
		}
	}
	return SUCCESS;
}

int deinit_database(void)
{
	if(database != NULL){
		sqlite3_close(database);
	}
	return SUCCESS;
}
/*****************************************************************************/
