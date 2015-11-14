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

#include "pub.h"
#include "log.h"
#include "database.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

char STR_NAME_PROGRAMM[] = "Система Управления Лафетными Стволами";

generic_s generic;
GString * pub = NULL;

GKeyFile * system_config = NULL;
char STR_GROUP_GLOBAL[] = "global";

/*****************************************************************************/
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
/*****************************************************************************/

int dialog_error(char * message)
{
	GtkWidget * md_err;
	md_err = gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"%s",message);
	gtk_dialog_run(GTK_DIALOG(md_err));
	gtk_widget_destroy(md_err);
	g_critical("%s",message);
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

static GTimeVal current_time;
GDateTime * current_date_time(void)
{
	g_get_current_time (&current_time);
	return g_date_time_new_from_timeval_local(&current_time);
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

static int save_config(GString * name)
{
	int rc;
	GError * err = NULL;

	if(flag_save_config != OK){
		return FAILURE;
	}
	if(name == NULL){
		return FAILURE;
	}

	if(system_config != NULL){
		rc = g_key_file_save_to_file(system_config,name->str,&err);
		if(rc == FALSE ){
			g_string_printf(pub,"Несмог сохранить файл конфигурации %s!\n%s",name->str,err->message);
			dialog_error(pub->str);
			g_error("%s",pub->str);
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
	if(system_config != NULL){
		return SUCCESS;
	}
	system_config = g_key_file_new();
	rc = g_key_file_load_from_file(system_config,name->str,G_KEY_FILE_KEEP_COMMENTS,&err);
	if(rc == FALSE){
		g_key_file_free(system_config);
		system_config = NULL;
		g_string_printf(pub,"Несмог считать файл конфигурации %s!\n%s",name->str,err->message);
		dialog_error(pub->str);
		g_error("%s",pub->str);
	}
	return SUCCESS;
}

static const char STR_LICENSE_CONFIG[] =
	"##############################################################################\n"
	"                                                                             #\n"
	" сулс - система управления лафетными стволами                                #\n"
	"  Авторское Право (C) <2015> <Кузьмин Ярослав>                               #\n"
	"                                                                             #\n"
	"  Эта программа является свободным программным обеспечением:                 #\n"
	"  вы можете распространять и/или изменять это в соответствии с               #\n"
	"  условиями в GNU General Public License, опубликованной                     #\n"
	"  Фондом свободного программного обеспечения, как версии 3 лицензии,         #\n"
	"  или (по вашему выбору) любой более поздней версии.                         #\n"
	"                                                                             #\n"
	"  Эта программа распространяется в надежде, что она будет полезной,          #\n"
	"  но БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ; даже без подразумеваемой гарантии              #\n"
	"  Или ПРИГОДНОСТИ ДЛЯ КОНКРЕТНЫХ ЦЕЛЕЙ. См GNU General Public License        #\n"
	"  для более подробной информации.                                            #\n"
	"                                                                             #\n"
	"  Вы должны были получить копию GNU General Public License                   #\n"
	"  вместе с этой программой. Если нет, см <http://www.gnu.org/licenses/>      #\n"
	"                                                                             #\n"
	"  Адрес для контактов: kuzmin.yaroslav@gmail.com                             #\n"
	"                                                                             #\n"
	"##############################################################################\n"
	"                                                                             #\n"
	" scfm - system control fire monitors                                         #\n"
	"  Copyright (C) <2015> <Kuzmin Yaroslav>                                     #\n"
	"                                                                             #\n"
	"  This program is free software: you can redistribute it and/or modify       #\n"
	"  it under the terms of the GNU General Public License as published by       #\n"
	"  the Free Software Foundation, either version 3 of the License, or          #\n"
	"  (at your option) any later version.                                        #\n"
	"                                                                             #\n"
	"  This program is distributed in the hope that it will be useful,            #\n"
	"  but WITHOUT ANY WARRANTY; without even the implied warranty of             #\n"
	"  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              #\n"
	"  GNU General Public License for more details.                               #\n"
	"                                                                             #\n"
	"  You should have received a copy of the GNU General Public License          #\n"
	"  along with this program.  If not, see <http://www.gnu.org/licenses/>.      #\n"
	"                                                                             #\n"
	"  Email contact: kuzmin.yaroslav@gmail.com                                   #\n"
	"                                                                             #\n"
	"##############################################################################\n";

static int create_default_config(GString * name)
{
	int rc;
	GError * err = NULL;

	if(name == NULL){
		return FAILURE;
	}
	if(system_config != NULL){
		return SUCCESS;
	}
	system_config = g_key_file_new();

	rc = g_key_file_set_comment(system_config,NULL,NULL,STR_LICENSE_CONFIG,&err);
	if(rc == FALSE){
		g_string_printf(pub,"Несмог создать файл конфигурации %s!\n%s",name->str,err->message);
		dialog_error(pub->str);
		g_error("%s",pub->str);
		g_error_free(err);
	}
	/*TODO вызывать функции для значений по умолчанию из блоков*/
	default_database_config();

	set_flag_save_config()	;
	return save_config(name);
}

static char STR_CONFIG_FILE[] = "config";
static GString * config_name = NULL;

static int check_config(GString * catalog)
{
	int rc;
	config_name = g_string_new(catalog->str);
	g_string_append(config_name,STR_CONFIG_FILE);
	rc = g_file_test(config_name->str,G_FILE_TEST_IS_REGULAR);
	if(rc == FALSE){
		return create_default_config(config_name);
	}
	return SUCCESS;
}

static int init_config(void)
{
	int rc = SUCCESS;

	if(system_config == NULL){
		rc = read_config(config_name);
	}
	return rc;
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
/*  Система                                                                  */
/*****************************************************************************/

#ifdef G_OS_WIN32
static const char STR_HOME_PATH[] = "HOME";
/*static char STR_HOME_PATH[] = "APPDATA";*/
/*static char STR_HOME_PATH[] = "USERPROFILE";*/
#endif
#ifdef G_OS_UNIX
static const char STR_HOME_PATH[] = "HOME";
#endif

static char STR_WORK_CATALOG[] = G_DIR_SEPARATOR_S".scfm"G_DIR_SEPARATOR_S;
static GString * work_catalog = NULL;

static int check_system(void)
{
 	int rc;
	const char * str = g_getenv(STR_HOME_PATH);/*g_get_home_dir();*/

	work_catalog = g_string_new(str);
	g_string_append(work_catalog,STR_WORK_CATALOG);

	rc = g_file_test(work_catalog->str,G_FILE_TEST_IS_DIR);
	if(rc == FALSE){
		rc = g_mkdir_with_parents(work_catalog->str,MODE_WORK_CATALOG);
		if(rc == -1){
			g_string_printf(pub,"Несмог создать рабочий каталог %s!",work_catalog->str);
			dialog_error(pub->str);
			g_error("%s",pub->str);
		}
	}

	check_config(work_catalog);
	check_logging(work_catalog);

	return SUCCESS;
}

static char RESOURCE_DEFAULT_ICON[] = "/resource/scfm.png";

generic_s * init_system(void)
{
	GdkPixbuf * default_icon = NULL;
	/*создание постояных блоков*/
	pub = g_string_new(NULL);
	generic.pub = pub;

	default_icon = gdk_pixbuf_new_from_resource(RESOURCE_DEFAULT_ICON,NULL);
	if(default_icon != NULL){
		gtk_window_set_default_icon(default_icon);
	}
	generic.default_icon = default_icon;

	/*проверка системных файлов*/
	check_system();

	init_config();
	init_logging();
	g_message("Запуск : %s",STR_NAME_PROGRAMM);
	g_info("Запуск : %s",STR_NAME_PROGRAMM);
	init_database(work_catalog);

	return &generic;
}

int deinit_system(void)
{
	deinit_database();
	g_message("Останов системы.\n");
	g_info("Останов системы.\n");
	deinit_logging();
	deinit_config();
 	g_string_free(pub,TRUE);
	return SUCCESS;
}

