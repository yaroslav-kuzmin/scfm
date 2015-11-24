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

#include "pub.h"
#include "common.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

struct _logging_s{
	GString * error_name;
	GIOChannel * error_channel;
	GString * info_name;
	GIOChannel * info_channel;
	GString * buf;
	GtkTextBuffer * view_buf;
};
typedef struct _logging_s logging_s;

/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/

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
		g_printf("%s%s\n",STR_DEBUG,message);
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

	g_date_time_unref(cdt);
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
				g_error_free(err);
			}
			gtk_text_buffer_insert_at_cursor(log->view_buf,buf->str,-1);
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
			g_error_free(err);
		}
		else{
			g_io_channel_flush(log->error_channel,NULL);
		}
	}
	g_printf("%s\n",buf->str);
}

static char STR_LOG_DIR[] = "logs";
static char STR_ERROR_FILE[] = "error";

logging_s logging = {0};

int check_logging(GString * catalog)
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

/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

#define DEFAULT_TIMEOUT_FLUSH_INFO_CHANNEL    60000    /*в миллисекундах*/
static int timeout_flush_info_channel = DEFAULT_TIMEOUT_FLUSH_INFO_CHANNEL;

int init_logging(void)
{
	int rc = FAILURE;

	logging.view_buf = gtk_text_buffer_new(NULL);

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

int deinit_logging(void)
{
	int rc;
	GError * err = NULL;
	GIOChannel * c;
	GString * s;

	c = logging.info_channel;
	s = logging.info_name;
	if(c != NULL){
		rc = g_io_channel_shutdown(c,TRUE,&err);
		if(rc != G_IO_STATUS_NORMAL){
			g_string_printf(pub,"Ошибка ведения журнала %s!\n%s",s->str,err->message);
			dialog_error(pub->str);
			g_error_free(err);
		}
		g_string_free(s,TRUE);
		logging.info_name = NULL;
		logging.info_channel = NULL;
	}
	err = NULL;
	c = logging.error_channel;
	s = logging.error_name;
	if(c != NULL){
		rc = g_io_channel_shutdown(c,TRUE,&err);
		if(rc != G_IO_STATUS_NORMAL){
			g_string_printf(pub,"Ошибка ведения журнала %s!\n%s",s->str,err->message);
			dialog_error(pub->str);
			g_error_free(err);
		}
		g_string_free(s,TRUE);
		logging.error_name = NULL;
		logging.error_channel = NULL;
	}
	s = logging.buf;
	g_string_free(s,TRUE);
	g_object_unref(logging.view_buf);
	return SUCCESS;
}

/*****************************************************************************/
#define DEFAULT_SIZE_HEIGHT_VIEW_LOG      150
static int size_height_view_log = DEFAULT_SIZE_HEIGHT_VIEW_LOG;
static char STR_LOG_FRAME[] = "Журнал";

GtkWidget * create_block_log(void)
{
	GtkWidget * frame;
	GtkWidget * log;
	GtkTextBuffer * buf = logging.view_buf;

	frame = gtk_frame_new(STR_LOG_FRAME);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_END,TRUE,FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(frame),3);

	log = gtk_text_view_new_with_buffer(buf);
	layout_widget(log,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(log),FALSE);
	gtk_text_view_set_overwrite(GTK_TEXT_VIEW(log),FALSE);
	gtk_widget_set_size_request(log,-1,size_height_view_log);

	gtk_container_add(GTK_CONTAINER(frame),log);
	return frame;
}
