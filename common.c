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
#include <gtk/gtk.h>

#include <pub.h>

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

const char STR_NAME_PROGRAMM[] = "Система Управления Лафетными Стволами";
const char STR_COPYRIGHT[] = "(C) <2015> <Кузьмин Ярослав>";
const char STR_COMMENT[] =
"Программа позволяет управлять большой группой\n"
" лафетных стволов управляемых контролером <X>\n"
" подключеных по протоколу modbus\n"
" Может выступать в роли сервера или клиента.";
const char STR_LICENSE[] =
"  Эта программа является свободным программным обеспечением:           \n"
"  вы можете распространять и/или изменять это в соответствии с         \n"
"  условиями в GNU General Public License, опубликованной               \n"
"  Фондом свободного программного обеспечения, как версии 3 лицензии,   \n"
"  или (по вашему выбору) любой более поздней версии.                   \n"
"                                                                       \n"
"  Эта программа распространяется в надежде, что она будет полезной,    \n"
"  но БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ; даже без подразумеваемой гарантии        \n"
"  Или ПРИГОДНОСТИ ДЛЯ КОНКРЕТНЫХ ЦЕЛЕЙ. См GNU General Public License  \n"
"  для более подробной информации.                                      \n"
"                                                                       \n"
"  Вы должны были получить копию GNU General Public License             \n"
"  вместе с этой программой. Если нет, см <http://www.gnu.org/licenses/>\n"
"                                                                         ";
const char STR_EMAIL[] = "kuzmin.yaroslav@gmail.com";
const char STR_EMAIL_LABEL[] = "kuzmin.yaroslav@gmail.com";
const char * STR_AUTHORS[] = {"Кузьмин Ярослав",NULL};

GString * pub = NULL;

GKeyFile * system_config = NULL;

char STR_GROUP_GLOBAL[] = "global";

#ifdef G_OS_WIN32
static char STR_HOME_PATH[] = "HOMEPATH";
/*static char STR_HOME_PATH[] = "APPDATA";*/
#endif
#ifdef G_OS_UNIX
static char STR_HOME_PATH[] = "HOME";
#endif

static char STR_CONFIG_DIR[] = G_DIR_SEPARATOR_S".scfm"G_DIR_SEPARATOR_S"config"G_DIR_SEPARATOR_S;
static char STR_CACHE_DIR[] = G_DIR_SEPARATOR_S".scfm"G_DIR_SEPARATOR_S"cache"G_DIR_SEPARATOR_S;
static char STR_LOG_DIR[] = G_DIR_SEPARATOR_S".scfm"G_DIR_SEPARATOR_S"logs"G_DIR_SEPARATOR_S;
/*****************************************************************************/
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
/*****************************************************************************/
int dialog_error(char * message)
{
	GtkWidget * md_err =
	md_err = gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,message);
	gtk_dialog_run(GTK_DIALOG(md_err));
	gtk_widget_destroy(md_err);
	return FAILURE;
}

int layout_widget(GtkWidget * w,GtkAlign ha,GtkAlign va,gboolean he,gboolean ve)
{
	gtk_widget_set_halign(w,ha);
	gtk_widget_set_valign(w,va);
	gtk_widget_set_hexpand(w,he);
	gtk_widget_set_vexpand(w,ve);
	return SUCCESS;
}

int set_size_font(GtkWidget * w,int size)
{
	PangoContext * pancon_info;
	PangoFontDescription * panfondes_info;

	pancon_info = gtk_widget_get_pango_context(w);
	panfondes_info = pango_context_get_font_description(pancon_info);
	pango_font_description_set_size(panfondes_info,size);
	gtk_widget_override_font(w,panfondes_info);

	return SUCCESS;
}

/*****************************************************************************/
/*  конфиурирование системы                                                  */
/*****************************************************************************/

static int flag_save_config = NOT_OK;
int set_flag_save_config(void)
{
	flag_save_config = OK;
	return flag_save_config;
}

static int default_config(void)
{
	system_config = g_key_file_new();
	return SUCCESS;
}

static int save_config(GString * name)
{
	int rc;
	GError * err;

	if(flag_save_config != OK){
		return FAILURE;
	}

	if(system_config != NULL){
		rc = g_key_file_save_to_file(system_config,name->str,&err);
		if(rc == FALSE ){
			g_string_printf(pub,"Нет файла конфигурации %s \n %s",name->str,err->message);
			return dialog_error(pub->str);
		}
	}
	return SUCCESS;
}

static int read_config(GString * name)
{
	int rc;
	GError * err = NULL;

	if(name == NULL){
		return FAILURE;
	}

	system_config = g_key_file_new();
	rc = g_key_file_load_from_file(system_config,name->str,G_KEY_FILE_NONE,&err);
	if(rc == FALSE){
		g_key_file_free(system_config);
		system_config = NULL;
		return FAILURE;
	}
	return SUCCESS;
}

/*****************************************************************************/

static char STR_CONFIG_FILE[] = "ini";
static GString * config_name = NULL;

static int init_config(void)
{
	int rc;
	char * str = getenv(STR_HOME_PATH);
	if(str != NULL){
		config_name = g_string_new(str);
		g_string_append(config_name,STR_CONFIG_DIR);
		g_string_append(config_name,STR_CONFIG_FILE);
	}

	rc = read_config(config_name);
	if(rc != SUCCESS){
		default_config();
	}

	return SUCCESS;
}

static int deinit_config(void)
{
	save_config(config_name);
	if(system_config != NULL){
		g_key_file_free(system_config);
		system_config = NULL;
	}
	if(config_name != NULL){
		g_string_free(config_name,TRUE);
		config_name = NULL;
	}
	return SUCCESS;
}

/*****************************************************************************/
/* система логирования                                                       */
/*****************************************************************************/
static int open_logging(GString * name)
{

}

static GString * logging_name = NULL;

static int init_logging(void)
{
	GError * err = NULL;
	char * str;
	int rc;
	GTimeVal current_time;
	GDateTime * p_dt;


	str = getenv(STR_HOME_PATH);
	if(str != NULL){
		logging_name = g_string_new(str);
		g_string_append(logging_name,STR_LOG_DIR);
		g_get_current_time (&current_time);
		p_dt = g_date_time_new_from_timeval_local(&current_time);
		g_string_append_printf(logging_name,"%04d%02d%02d"
			               ,g_date_time_get_year(p_dt)
			               ,g_date_time_get_month(p_dt)
			               ,g_date_time_get_day_of_month(p_dt)
	}

	rc = open_logging(logging_name);
	if(rc == SUCCESS){

	}

	return SUCCESS;
}

static int deinit_logging(void)
{
	if(logging_name != NULL){
		g_string_free(logging_name,TRUE);
	 	logging_name = NULL;
	}
	 return SUCCESS;
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int init_common(void)
{
	pub = g_string_new(NULL);
	
	init_config();
	init_logging();
	return SUCCESS;
}

int deinit_common(void)
{
 	g_string_free(pub,TRUE);
	deinit_logging();
	deinit_config();

	return SUCCESS;
}
/*****************************************************************************/
