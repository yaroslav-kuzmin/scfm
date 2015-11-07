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
#include <glib/gprintf.h>
#include <gtk/gtk.h>

#include "version.h"
#include "pub.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

char STR_NAME_PROGRAMM[] = "Система Управления Лафетными Стволами";

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
			g_error_free(err);
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
	rc = g_key_file_load_from_file(system_config,name->str,G_KEY_FILE_NONE,&err);
	if(rc == FALSE){
		g_key_file_free(system_config);
		system_config = NULL;
		g_string_printf(pub,"Несмог считать файл конфигурации %s!\n%s",name->str,err->message);
		dialog_error(pub->str);
		g_error("%s",pub->str);
		g_error_free(err);
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

	set_flag_save_config()	;
	return save_config(name);
}

static char STR_CONFIG_FILE[] = G_DIR_SEPARATOR_S"config";
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

	if(system_config != NULL){
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
/* система логирования                                                       */
/*****************************************************************************/

struct _logging_s{
	GString * error_name;
	GIOChannel * error_channel;
	GString * info_name;
	GIOChannel * info_channel;
	GString * buf;
};
typedef struct _logging_s logging_s;


static GIOChannel * open_channel_append(GString * name)
{
	GIOChannel * channel;
	GError * err = NULL;
	if(name == NULL){
		return NULL;
	}
	channel = g_io_channel_new_file(name->str,"a",&err);
	if(channel == NULL){
		g_string_printf(pub,"Несмог открыть файл %s!\n%s",name->str,err->message);
		dialog_error(pub->str);
		g_critical("%s",pub->str);
		g_error_free(err);
	}

	return channel;
}

static int flush_info_channel(gpointer ud)
{
	int rc;
	GError * err = NULL;
	logging_s * l = (logging_s *)ud;
	GIOChannel * channel = l->info_channel;

	if(channel == NULL){
		return FALSE;
	}

	rc = g_io_channel_flush(channel,&err);
	if(rc != G_IO_STATUS_NORMAL){
		g_string_printf(pub,"Ошибка введении журнала %s!\n%s",l->info_name->str,err->message);
		dialog_error(pub->str);
		g_critical("%s",pub->str);
		g_error_free(err);
		g_io_channel_shutdown(channel,TRUE,NULL);
		g_io_channel_unref(channel);
		l->info_channel = NULL;
		return FALSE;
	}
	return TRUE;
}

static char STR_ERROR[]    = "ОШИБКА СИСТЕМЫ : ";
static char STR_CRITICAL[] = "ОШИБКА : ";
static char STR_WARNING[]  = "ПРЕДУПРЕЖДЕНИЕ : ";
static char STR_DEBUG[]    = "debug : ";

static void print_logging(const gchar *log_domain,GLogLevelFlags log_level,
             const gchar *message,gpointer user_data)
{
	int rc;
	GError * err;
	gsize bw;
	logging_s * log = (logging_s *)user_data;
	GString * buf = log->buf;
	GDateTime * cdt;

	if(log_level == G_LOG_LEVEL_DEBUG){
		g_printf("%s%s",STR_DEBUG,message);
		return;
	}

	cdt = current_date_time();

	g_string_printf(buf,"\n[%02d:%02d:%02d %02d.%02d.%04d] "
	               ,g_date_time_get_hour(cdt)
	               ,g_date_time_get_minute(cdt)
	               ,g_date_time_get_second(cdt)
	               ,g_date_time_get_day_of_month(cdt)
	               ,g_date_time_get_month(cdt)
	               ,g_date_time_get_year(cdt));

	if(log_level != G_LOG_LEVEL_INFO){
		g_string_append_printf(buf,"(%s) ",log_domain);
	}

	if(log_level == G_LOG_LEVEL_WARNING){
		g_string_append(buf,STR_WARNING);
	}
	else{
		if(log_level == G_LOG_LEVEL_CRITICAL){
			g_string_append(buf,STR_CRITICAL);
		}
		else{
			if(log_level == G_LOG_LEVEL_ERROR){
				g_string_append(buf,STR_ERROR);
			}
		}
	}

	g_string_append(buf,message);

	if(log_level == G_LOG_LEVEL_INFO){
		err = NULL;
		if(log->info_channel != NULL){
			rc = g_io_channel_write_chars(log->info_channel,buf->str,-1,&bw,&err);
			if(rc != G_IO_STATUS_NORMAL){
				g_io_channel_shutdown(log->info_channel,TRUE,NULL);
				g_io_channel_unref(log->info_channel);
				log->info_channel = NULL;
				g_string_printf(pub,"Ошибка ведения журнала %s!\n%s",log->info_name->str,err->message);
				dialog_error(pub->str);
				g_critical("%s",pub->str);
				g_error_free(err);
			}
		}
		return;
	}

	err = NULL;
	if(log->error_channel != NULL){
		rc = g_io_channel_write_chars(log->error_channel,buf->str,-1,&bw,&err);
		if(rc != G_IO_STATUS_NORMAL){
			g_io_channel_shutdown(log->error_channel,TRUE,NULL);
			g_io_channel_unref(log->error_channel);
			log->error_channel = NULL;
			g_string_printf(pub,"Ошибка ведения журнала %s!\n%s",log->error_name->str,err->message);
			dialog_error(pub->str);
			g_critical("%s",pub->str);
			g_error_free(err);
		}
		else{
			g_io_channel_flush(log->error_channel,NULL);
		}
	}
	g_printf("%s",buf->str);
}

#define MODE_WORK_CATALOG             0755

static char STR_LOG_DIR[] = G_DIR_SEPARATOR_S"logs";
static char STR_ERROR_FILE[] = G_DIR_SEPARATOR_S"error";

logging_s logging = {0};

static int check_logging(GString * catalog)
{
	int rc;
	GDateTime * cdt;

	logging.error_name = g_string_new(catalog->str);
	g_string_append(logging.error_name,STR_ERROR_FILE);

	logging.info_name = g_string_new(catalog->str);
	g_string_append(logging.info_name,STR_LOG_DIR);
	rc = g_mkdir_with_parents(logging.info_name->str,MODE_WORK_CATALOG);
	if(rc == -1){
		g_string_printf(pub,"Несмог создать каталог журналов %s!",logging.info_name->str);
		dialog_error(pub->str);
		g_critical("%s",pub->str);
		return FAILURE;
	}
	cdt = current_date_time();
	g_string_append(logging.info_name,G_DIR_SEPARATOR_S);
	g_string_append_printf(logging.info_name,"%04d%02d%02d.log"
	                      ,g_date_time_get_year(cdt)
	                      ,g_date_time_get_month(cdt)
	                      ,g_date_time_get_day_of_month(cdt));
	g_date_time_unref(cdt);
	return SUCCESS;
}

#define DEFAULT_TIMEOUT_FLUSH_INFO_CHANNEL    60000    /*в миллисекундах*/
static int timeout_flush_info_channel = DEFAULT_TIMEOUT_FLUSH_INFO_CHANNEL;

static int init_logging(void)
{
	int rc = FAILURE;

	if(logging.error_name == NULL){
		return rc;
	}
	if(logging.info_name == NULL){
		return rc;
	}

	if(logging.error_channel == NULL){
		logging.error_channel = open_channel_append(logging.error_name);
		if(logging.error_channel == NULL){
			return rc;
		}
	}
	if(logging.info_channel == NULL){
		logging.info_channel = open_channel_append(logging.info_name);
		if(logging.info_channel == NULL){
			return rc;
		}
	}

	g_timeout_add(timeout_flush_info_channel,flush_info_channel,&logging);
	logging.buf = g_string_new(NULL);
	g_log_set_default_handler(print_logging,&logging);

	return SUCCESS;
}

static int deinit_logging(void)
{
	int rc;
	GError * err = NULL;
	GIOChannel * c;
	GString * s;

	s = logging.info_name;
	c = logging.info_channel;

	if(c != NULL){
		rc = g_io_channel_shutdown(c,TRUE,&err);
		if(rc != G_IO_STATUS_NORMAL){
			g_string_printf(pub,"Ошибка ведения журнала %s!\n%s",s->str,err->message);
			dialog_error(pub->str);
			g_critical("%s",pub->str);
			g_error_free(err);
		}
		g_io_channel_unref(c);
		g_string_free(s,TRUE);
		logging.info_name = NULL;
		logging.info_channel = NULL;
	}

	s = logging.error_name;
	c = logging.error_channel;
	if(c != NULL){
		rc = g_io_channel_shutdown(c,TRUE,&err);
		if(rc != G_IO_STATUS_NORMAL){
			g_string_printf(pub,"Ошибка ведения журнала %s!\n%s",s->str,err->message);
			dialog_error(pub->str);
			g_critical("%s",pub->str);
			g_error_free(err);
		}
		g_io_channel_unref(c);
		g_string_free(s,TRUE);
		logging.error_name = NULL;
		logging.error_channel = NULL;
	}
	s = logging.buf;
	g_string_free(s,TRUE);
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

static char STR_WORK_CATALOG[] = G_DIR_SEPARATOR_S".scfm";
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
static GdkPixbuf * default_icon = NULL;

int init_system(void)
{
	/*создание постояных блоков*/
	pub = g_string_new(NULL);

	default_icon = gdk_pixbuf_new_from_resource(RESOURCE_DEFAULT_ICON,NULL);
	if(default_icon != NULL){
		gtk_window_set_default_icon(default_icon);
	}

	/*проверка системных файлов*/
	check_system();

	init_config();
	init_logging();
	g_message("Запуск : %s",STR_NAME_PROGRAMM);
	g_info("Запуск : %s",STR_NAME_PROGRAMM);

	return SUCCESS;
}

int deinit_system(void)
{
	g_message("Останов системы.\n");
	g_info("Останов системы.\n");
	deinit_logging();
	deinit_config();
 	g_string_free(pub,TRUE);
	return SUCCESS;
}

/*****************************************************************************/
/*  О Программе                                                              */
/*****************************************************************************/

static const char STR_COPYRIGHT[] = "(C) <2015> <Кузьмин Ярослав>";
static const char STR_COMMENT[] =
"Программа позволяет управлять большой группой\n"
" лафетных стволов, задвижками, видеокамерами.";
static const char STR_LICENSE[] =
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
"                                                                        ";
static const char STR_EMAIL[] = "kuzmin.yaroslav@gmail.com";
static const char STR_EMAIL_LABEL[] = "kuzmin.yaroslav@gmail.com";
static const char * STR_AUTHORS[] = {"Кузьмин Ярослав",NULL};

int about_programm(void)
{
	GtkWidget * dialog = gtk_about_dialog_new();

	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog),STR_NAME_PROGRAMM);
	g_string_printf(pub,"%d.%03d - %x",VERSION_MAJOR,VERSION_MINOR,VERSION_GIT);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog),pub->str);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),STR_COPYRIGHT);
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),STR_COMMENT);
	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog),STR_LICENSE);
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog),GTK_LICENSE_CUSTOM);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),NULL);
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog),STR_EMAIL_LABEL);
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog),STR_AUTHORS);
	gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(dialog),STR_AUTHORS);
	gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(dialog),NULL);
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(dialog),NULL);
	if(default_icon != NULL){
		gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog),default_icon);
	}
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	return SUCCESS;
}

/*****************************************************************************/
