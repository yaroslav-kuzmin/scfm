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
#include <gtk/gtk.h>

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

#define FORMAT_NAME_TABLE_GROUP    "[g%07d]"
static int create_table_group(int number)
{
	g_string_printf(pub,"CREATE TABLE ");
	g_string_append_printf(pub,FORMAT_NAME_TABLE_GROUP,number);
	g_string_append(pub,"(number INTEGER PRIMARY KEY,name,type INTEGER)");
	return query_simple(pub);
}

static char STR_NAME_TABLE_TYPE_GROUP[] = "[group]";
static int create_table_type_group(void)
{
	g_string_printf(pub,"CREATE TABLE ");
	g_string_append(pub,STR_NAME_TABLE_TYPE_GROUP);
	g_string_append(pub,"(number INTEGER PRIMARY KEY,schema)");
	return query_simple(pub);
}

static char STR_NAME_TABLE_TYPE_VIDEOCAMERA[] = "[videocamera]";
static int create_table_type_videocamera(void)
{
	g_string_printf(pub,"CREATE TABLE ");
	g_string_append(pub,STR_NAME_TABLE_TYPE_VIDEOCAMERA);
	g_string_append(pub,"(number INTEGER PRIMARY KEY,protocol,address,port,access)");
	return query_simple(pub);
}

#define FIRST_NUMBER_GROUP     0
static int create_total_table(void)
{
	create_table_group(FIRST_NUMBER_GROUP);
	create_table_type_group();
	create_table_type_videocamera();

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
/*    Общие функции                                                          */
/*****************************************************************************/
int add_group(uint32_t number_group,uint32_t number,char * name,int type)
{
	g_string_printf(pub,"INSERT INTO ");
	g_string_append_printf(pub,FORMAT_NAME_TABLE_GROUP,number_group);
	g_string_append(pub," VALUES (");
	g_string_append_printf(pub,"%d,%s,%d)",number,name,type);
	return query_simple(pub);
}

static char STR_KEY_DATABASE[] = "database";
static char STR_DATABASE_FILE[] = "scfm.sqlite3";

int default_database_config(void)
{
	g_key_file_set_string(system_config,STR_GROUP_GLOBAL,STR_KEY_DATABASE,STR_DATABASE_FILE);
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
#if 0
	add_group(0,5,"\'тестовая группа0\'",0);
	add_group(0,2,"\'тестовая группа1\'",1);
	add_group(0,7,"\'тестовая группа3\'",1);
	add_group(0,5,"\'тестовая группа4\'",1);
#endif
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
