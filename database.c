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

static char QUERY_CREATE_TABLE[] =
"CREATE TABLE object(number INTEGER PRIMARY KEY AUTOINCREMENT,name,type INTEGER)";

static char QUERY_INSERT_00_OBJECT[] = "INSERT INTO object VALUES (0,\"total\",0)";
/*static char QUERY_INSERT_OBJECT[] = "INSERT INTO object (name,type) VALUES";*/

static int create_total_table(void)
{
	int rc;
	char * error_message = NULL;

	rc = sqlite3_exec(database,QUERY_CREATE_TABLE,NULL,NULL, &error_message);
	if( rc != SQLITE_OK){
		g_critical("SQL error QUERY_CREATE_TABLE : %s!",error_message);
		sqlite3_free(error_message);
		return FAILURE;
	}

	rc = sqlite3_exec(database,QUERY_INSERT_00_OBJECT,NULL,NULL, &error_message);
	if( rc != SQLITE_OK){
		g_critical("SQL error QUERY_CREATE_TABLE : %s!",error_message);
		sqlite3_free(error_message);
		return FAILURE;
	}


	return SUCCESS;
}

static int create_default_database(GString * name)
{
	int rc;

	if(name == NULL){
		return FAILURE;
	}

	rc = sqlite3_open(name->str,&database); // откравает базу данных
	if(rc != SQLITE_OK){
		g_string_printf(pub,"Несмог открыть базу данных %s : %s!",name->str,sqlite3_errmsg(database));
		dialog_error(pub->str);
		sqlite3_close(database);
		return FAILURE;
	}

	create_total_table();

	return SUCCESS;
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

static char STR_KEY_DATABASE[] = "database";
static char STR_DATABASE_FILE[] = "database.sqlite3";

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
	check_database(work_catalog);

	return SUCCESS;
}

int deinit_database(void)
{
	return SUCCESS;
}
/*****************************************************************************/
