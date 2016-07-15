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
#include "log.h"
#include "kernel.h"
#include "config.h"
#include "database.h"
#include "group.h"
#include "videocamera.h"
#include "controller.h"
#include "object.h"
#include "tree.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

GString * pub = NULL;

GKeyFile * system_config = NULL;
char STR_GROUP_GLOBAL[] = "global";

/*****************************************************************************/
/*  Сообщения пользователю Диалоги                                           */
/*****************************************************************************/

void apply_style_main(GtkWidget * w,gpointer d);

flag_t dialog_info(char * message)
{
	GtkWidget * md_info;
	/*TODO передать указатель на основное окно */
	md_info = gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"%s",message);
	/*TODO применить свой стиль */
	gtk_window_set_decorated(GTK_WINDOW(md_info),FALSE);
	apply_style_main(md_info,NULL);
	gtk_dialog_run(GTK_DIALOG(md_info));
	gtk_widget_destroy(md_info);
	g_info("%s",message);
	return FAILURE;
}

flag_t dialog_error(char * message)
{
	GtkWidget * md_err;
	/*TODO передать указатель на основное окно */
	md_err = gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"%s",message);
	/*TODO применить свой стиль */
	gtk_window_set_decorated(GTK_WINDOW(md_err),FALSE);
	apply_style_main(md_err,NULL);
	/*gtk_dialog_run(GTK_DIALOG(md_err));*/
	gtk_dialog_run(GTK_DIALOG(md_err));
	gtk_widget_destroy(md_err);
	g_critical("%s",message);
	return FAILURE;
}

/*****************************************************************************/
/* Настройка  виджетов                                                       */
/*****************************************************************************/
flag_t layout_widget(GtkWidget * w,GtkAlign ha,GtkAlign va,gboolean he,gboolean ve)
{
	gtk_widget_set_halign(w,ha);
	gtk_widget_set_valign(w,va);
	gtk_widget_set_hexpand(w,he);
	gtk_widget_set_vexpand(w,ve);
	return SUCCESS;
}

flag_t set_size_font(GtkWidget * w,int size)
{
#if 0
	PangoContext * pancon_info;
	PangoFontDescription * panfondes_info;

	pancon_info = gtk_widget_get_pango_context(w);
	panfondes_info = pango_context_get_font_description(pancon_info);
	pango_font_description_set_size(panfondes_info,size);
	gtk_widget_override_font(w,panfondes_info);
#endif
	return SUCCESS;
}
/*****************************************************************************/
/* Время                                                                     */
/*****************************************************************************/
static GTimeVal current_time;
GDateTime * current_date_time(void)
{
	g_get_current_time (&current_time);
	return g_date_time_new_from_timeval_local(&current_time);
}

/*****************************************************************************/
/*  конфиурирование системы                                                  */
/*****************************************************************************/

static flag_t flag_save_config = NOT_OK;
flag_t set_flag_save_config(void)
{
	flag_save_config = OK;
	return flag_save_config;
}

static flag_t save_config(GString * name)
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

static flag_t read_config(GString * name)
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

static flag_t create_default_config(GString * name)
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
	}
	/*TODO создание каталога ресурсов и файла ресурсов*/

	/*TODO вызывать функции для значений по умолчанию из блоков*/
	default_database_config();

	set_flag_save_config()	;
	return save_config(name);
}

static char STR_CONFIG_FILE[] = "config";
static GString * config_name = NULL;

static flag_t check_config(GString * catalog)
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

static flag_t init_config(void)
{
	int rc = SUCCESS;

	if(system_config == NULL){
		rc = read_config(config_name);
	}
	return rc;
}

static flag_t deinit_config(void)
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
/*  Ресурсы                                                                  */
/*****************************************************************************/

static char STR_RESOURCE_DIR[] = "resource"G_DIR_SEPARATOR_S;
static char STR_RESOURCE_EXT[] = ".gresource";
static char STR_RESOURCE_IMAGE[] = "image";
static char STR_RESOURCE_STYLE[] = "style";
static char STR_RESOURCE_UI[] = "ui";
static GString * name_resource_image = NULL;
static GString * name_resource_style = NULL;
static GString * name_resource_ui = NULL;

static flag_t check_resource(void)
{
	int rc;
	/*файлы ресурсов находятся в каталоге программы в windows */
	name_resource_image = g_string_new(NULL);
	g_string_append(name_resource_image,STR_RESOURCE_DIR);
	g_string_append(name_resource_image,STR_RESOURCE_IMAGE);
	g_string_append(name_resource_image,STR_RESOURCE_EXT);
	rc = g_file_test(name_resource_image->str,G_FILE_TEST_IS_REGULAR);
	if(rc == FALSE){
		g_error("Нет файла ресурсов : %s",name_resource_image->str);
		return FAILURE;
	}
	name_resource_style = g_string_new(NULL);
	g_string_append(name_resource_style,STR_RESOURCE_DIR);
	g_string_append(name_resource_style,STR_RESOURCE_STYLE);
	g_string_append(name_resource_style,STR_RESOURCE_EXT);
	rc = g_file_test(name_resource_style->str,G_FILE_TEST_IS_REGULAR);
	if(rc == FALSE){
		g_error("Нет файла ресурсов : %s",name_resource_style->str);
		return FAILURE;
	}
	name_resource_ui = g_string_new(NULL);
	g_string_append(name_resource_ui,STR_RESOURCE_DIR);
	g_string_append(name_resource_ui,STR_RESOURCE_UI);
	g_string_append(name_resource_ui,STR_RESOURCE_EXT);
	rc = g_file_test(name_resource_ui->str,G_FILE_TEST_IS_REGULAR);
	if(rc == FALSE){
		g_error("Нет файла ресурсов : %s",name_resource_ui->str);
		return FAILURE;
	}
	return SUCCESS;
}

static GResource * resource_image;
static GResource * resource_style;
static GResource * resource_ui;

static flag_t init_resource(void)
{
	GError * err = NULL;
	resource_image = g_resource_load(name_resource_image->str,&err);
	if(resource_image == NULL){
		g_warning("Нет ресурсов : %s",err->message);
		g_error_free(err);
		return FAILURE;
	}

	err = NULL;
	resource_style = g_resource_load(name_resource_style->str,&err);
	if(resource_style == NULL){
		g_warning("Нет ресурсов : %s",err->message);
		g_error_free(err);
		return FAILURE;
	}

	err = NULL;
	resource_ui = g_resource_load(name_resource_ui->str,&err);
	if(resource_ui == NULL){
		g_warning("Нет ресурсов : %s",err->message);
		g_error_free(err);
		return FAILURE;
	}

	g_resources_register(resource_image);
	g_resources_register(resource_style);
	g_resources_register(resource_ui);

	return SUCCESS;
}

static flag_t deinit_resource(void)
{
	if(resource_image != NULL){
		g_resources_unregister(resource_image);
		g_resource_unref(resource_image);
		g_string_free(name_resource_image,TRUE);
	}
	if(resource_style != NULL){
		g_resources_unregister(resource_style);
		g_resource_unref(resource_style);
		g_string_free(name_resource_style,TRUE);
	}
	if(resource_ui != NULL){
		g_resources_unregister(resource_ui);
		g_resource_unref(resource_ui);
		g_string_free(name_resource_ui,TRUE);
	}
	return SUCCESS;
}

GdkPixbuf * get_resource_image(int res,const char * name_resource)
{
	GError * err = NULL;
	GdkPixbuf * image;

	switch(res){
		case RESOURCE_STYLE:
			g_string_printf(pub,"/%s/assets/",STR_RESOURCE_STYLE);
			break;
		case RESOURCE_IMAGE:
			g_string_printf(pub,"/%s/",STR_RESOURCE_IMAGE);
			break;
		default:
			g_string_printf(pub,"/%s/assets",STR_RESOURCE_STYLE);
			break;
	}
	g_string_append(pub,name_resource);
	g_string_append(pub,".png");
	image = gdk_pixbuf_new_from_resource(pub->str,&err);
	if(image == NULL){
		g_warning("Несмог загрузить рисунок : %s",err->message);
		g_error_free(err);
	}
	return image;
}

/*****************************************************************************/
/*  Стили                                                                    */
/*****************************************************************************/
static GtkStyleProvider * style_main;
void apply_style_main(GtkWidget * w,gpointer d)
{
	GtkStyleContext * style = gtk_widget_get_style_context(w);
	gtk_style_context_add_provider(style,style_main,GTK_STYLE_PROVIDER_PRIORITY_USER);
	if (GTK_IS_CONTAINER (w)){
		gtk_container_forall(GTK_CONTAINER (w),apply_style_main,NULL);
		/*gtk_container_foreach(GTK_CONTAINER (w),apply_style_main,NULL);*/
	}
}

static GtkStyleProvider * style_message_norm;
void apply_style_message_norm(GtkWidget * w,gpointer d)
{
	GtkStyleContext * style = gtk_widget_get_style_context(w);
	gtk_style_context_add_provider(style,style_message_norm,GTK_STYLE_PROVIDER_PRIORITY_USER + 1);
	if (GTK_IS_CONTAINER (w)){
		gtk_container_forall(GTK_CONTAINER (w),apply_style_message_norm,NULL);
		/*gtk_container_foreach(GTK_CONTAINER (w),apply_style_message_norm,NULL);*/
	}
}

static GtkStyleProvider * style_message_alarm;
void apply_style_message_alarm(GtkWidget * w,gpointer d)
{
	GtkStyleContext * style = gtk_widget_get_style_context(w);
	gtk_style_context_add_provider(style,style_message_alarm,GTK_STYLE_PROVIDER_PRIORITY_USER + 1);
	if (GTK_IS_CONTAINER (w)){
		gtk_container_forall(GTK_CONTAINER (w),apply_style_message_alarm,NULL);
		/*gtk_container_foreach(GTK_CONTAINER (w),apply_style_message_alarm,NULL);*/
	}
}

static void css_provider_parsing_error(GtkCssProvider *provider,GtkCssSection *section,GError *error,gpointer user_data)
{
	GFile * file = gtk_css_section_get_file(section);
	int line = gtk_css_section_get_end_line(section);
	int pos = gtk_css_section_get_end_position(section);

	g_warning("%s : line %d position %d : %s",g_file_get_basename(file),line,pos,error->message);
}

#if TEST_INTERFACE
/*static char STR_NAME_STYLE_MAIN[] = "resource/style/gtk.css";*/
static char STR_NAME_STYLE_MAIN[] =         "resource/style/main.css";
static char STR_NAME_STYLE_MESSAGE_NORM[] = "resource/style/message_norm.css";
static char STR_NAME_STYLE_MESSAGE_ALARM[] = "resource/style/message_alarm.css";
#else
static char STR_NAME_STYLE_MAIN[] = "/style/main.css";
static char STR_NAME_STYLE_MESSAGE_NORM[] = "/style/message_norm.css";
static char STR_NAME_STYLE_MESSAGE_ALARM[] = "/style/message_alarm.css";
#endif

static flag_t init_style(void)
{
	GtkCssProvider * css_provider;

 	css_provider = gtk_css_provider_new();
	g_signal_connect(css_provider,"parsing-error",G_CALLBACK(css_provider_parsing_error),NULL);
#if TEST_INTERFACE
	gtk_css_provider_load_from_path(css_provider,STR_NAME_STYLE_MAIN,NULL);
#else
	gtk_css_provider_load_from_resource(css_provider,STR_NAME_STYLE_MAIN);
#endif
	style_main = GTK_STYLE_PROVIDER(css_provider);

 	css_provider = gtk_css_provider_new();
	g_signal_connect(css_provider,"parsing-error",G_CALLBACK(css_provider_parsing_error),NULL);

#if TEST_INTERFACE
	gtk_css_provider_load_from_path(css_provider,STR_NAME_STYLE_MESSAGE_NORM,NULL);
#else
	gtk_css_provider_load_from_resource(css_provider,STR_NAME_STYLE_MESSAGE_NORM);
#endif
	style_message_norm = GTK_STYLE_PROVIDER(css_provider);

 	css_provider = gtk_css_provider_new();
	g_signal_connect(css_provider,"parsing-error",G_CALLBACK(css_provider_parsing_error),NULL);
#if TEST_INTERFACE
	gtk_css_provider_load_from_path(css_provider,STR_NAME_STYLE_MESSAGE_ALARM,NULL);
#else
	gtk_css_provider_load_from_resource(css_provider,STR_NAME_STYLE_MESSAGE_ALARM);
#endif
	style_message_alarm = GTK_STYLE_PROVIDER(css_provider);

	return SUCCESS;
}

static flag_t deinit_style(void)
{
	return SUCCESS;
}
/*****************************************************************************/
/*  Постройка интерфейса                                                     */
/*****************************************************************************/
static GtkBuilder * builder;

GtkWidget * builder_widget(flag_t module, char * name)
{
	GtkWidget * w;
	/*TODO без пересборки */
	g_string_printf(pub,"resource");
	switch(module){
		case MODULE_MAIN_WINDOW:
			g_string_append_printf(pub,"/ui/%s.ui",STR_MODULE_MAIN_WINDOW);
			break;
		default:
			ERROR_PROGRAM;
			return NULL;
	}

	/*builder = gtk_builder_new_from_resource(pub->str);*/
	builder = gtk_builder_new_from_file(pub->str);

	w = (GtkWidget*)gtk_builder_get_object(builder,name);

	g_object_unref(builder);
	return w;
}


/*****************************************************************************/
/*  Система                                                                  */
/*****************************************************************************/

static int mode_work = MODE_NOT_WORK;

flag_t set_mode_work(int mode,GtkWidget * win_main)
{
	mode_work = mode;
	switch(mode_work){
		case MODE_CONTROL_OFF:
			select_object(NULL);
		case MODE_CONTROL_ON:
			control_controllers(mode_work);
			tree_check_status(mode_work);
			break;
		case MODE_CONFIGURATION:
			create_window_config(win_main);
			tree_check_status(MODE_CONTROL_OFF);
			break;
		default:
			mode_work = MODE_NOT_WORK;
			tree_check_status(MODE_CONTROL_OFF);
			break;
	}
	return SUCCESS;
}

flag_t get_mode_work(void)
{
	return mode_work;
}

#ifdef G_OS_WIN32
/*static const char STR_HOME_PATH[] = "HOME";*/
/*static char STR_HOME_PATH[] = "APPDATA";*/
/*static char STR_HOME_PATH[] = "USERPROFILE";*/
#endif
#ifdef G_OS_UNIX
/*static const char STR_HOME_PATH[] = "HOME";*/
#endif

static char STR_WORK_CATALOG[] = G_DIR_SEPARATOR_S".scfm"G_DIR_SEPARATOR_S;
static GString * work_catalog = NULL;

static flag_t check_system(void)
{
 	int rc;
	const char * str = g_get_home_dir();

	g_printf("home dir : %s",str);

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
	check_resource();

	return SUCCESS;
}

static char RESOURCE_DEFAULT_ICON[] = "scfm";
static GdkPixbuf * default_icon = NULL;
GdkPixbuf * get_default_icon(void)
{
	return default_icon;
}

flag_t init_system(void)
{
	/*создание постояных блоков*/
	pub = g_string_new(NULL);

	/*проверка системных файлов*/
	check_system();

	init_config();
	init_logging();
	g_message("Запуск : %s",STR_NAME_PROGRAMM);
	g_info("Запуск : %s",STR_NAME_PROGRAMM);
	init_resource();
	init_style();
	init_database(work_catalog);
	init_all_groups();
	init_all_videocameras();
	init_all_controllers();
	init_kernel();

	default_icon = get_resource_image(RESOURCE_STYLE,RESOURCE_DEFAULT_ICON);
	if(default_icon != NULL){
		gtk_window_set_default_icon(default_icon);
	}
#if 0
	g_info(" HOME DIR :> %s",g_get_home_dir());
	g_info(" HOME DIR :> %s",g_getenv("HOME"));
	g_info(" HOME DIR :> %s",g_getenv("APPDATA"));
	g_info(" HOME DIR :> %s",g_getenv("USERPROFILE"));
#endif
	return SUCCESS;
}

flag_t deinit_system(void)
{
	deinit_database();
	deinit_all_controllers();
	deinit_all_videcameras();
	deinit_all_groups();
	deinit_kernel();
	deinit_style();
	deinit_resource();
	g_message("Останов системы.\n");
	g_info("Останов системы.\n");
	deinit_logging();
	deinit_config();
	g_string_free(pub,TRUE);
	g_string_free(work_catalog,TRUE);
	g_object_unref(default_icon);
	return SUCCESS;
}

/*****************************************************************************/
