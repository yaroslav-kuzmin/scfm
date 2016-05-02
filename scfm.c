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

#include <stdlib.h>
#include "pub.h"
#include "common.h"
#include "log.h"
#include "menu.h"
#include "tree.h"
#include "object.h"

/*****************************************************************************/
/*  О Программе                                                              */
/*****************************************************************************/

#include "version.h"

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

static int about_programm(GtkWindow * parent_window)
{
	GtkWidget * dialog = gtk_about_dialog_new();
	GdkPixbuf * icon = get_default_icon();

	gtk_window_set_transient_for(GTK_WINDOW(dialog),parent_window);
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
	if(icon != NULL){
		gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog),icon);
	}
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	return SUCCESS;
}

/*****************************************************************************/
static flag_t default_size(GtkWidget * w)
{
	GtkRequisition min;
	GtkRequisition nat;
	gint pos_x;
	gint pos_y;

	gtk_window_get_position(GTK_WINDOW(w),&pos_x,&pos_y);
	gtk_widget_get_preferred_size(w,&min,&nat);

	g_message("Размер основного экрана  : %dx%d",nat.width,nat.height);
	g_message("Позиция основного экрана : %d %d",pos_x,pos_y);

	return SUCCESS;
}

static GtkWidget * create_block_job(void)
{
	GtkWidget * box;
	GtkWidget * block_tree;
	GtkWidget * block_object;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_set_size_request(box,DEFAULT_WIDTH_JOB,DEFAULT_HEIGHT_JOB);
	gtk_container_set_border_width(GTK_CONTAINER(box),0);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	block_tree = create_block_tree_object();
	block_object = create_block_object();

	gtk_box_pack_start(GTK_BOX(box),block_tree,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_object,TRUE,TRUE,0);
	gtk_widget_show(box);
	return box;
}

static void window_destroy_main(GtkWidget * w,gpointer ud)
{
	gtk_main_quit();
}

static gboolean window_key_press_event_main(GtkWidget * w,GdkEvent  *event,gpointer ud)
{
	GdkEventType type = event->type;
	gint state;

	if(type == GDK_KEY_PRESS){
		GdkEventKey * event_key = (GdkEventKey*)event;
		state = event_key->state;
		/*g_info(" key : %#x : %#x",event_key->keyval,state);*/
		if( (state & GDK_SHIFT_MASK) && (state & GDK_CONTROL_MASK)){
			if( event_key->keyval == GDK_KEY_A){
				about_programm(GTK_WINDOW(w));
			}
			if( event_key->keyval == GDK_KEY_S){
				default_size(w);
			}
		}
	}
	return FALSE;
}

static void window_realize_main(GtkWidget * w,gpointer ud)
{
}

static void window_show_main(GtkWidget * w,gpointer ud)
{
}

static GtkWidget * create_main_block(void)
{
	GtkWidget * win_main;
	GtkAccelGroup * accgro_main;
	GtkWidget * box;
	GtkWidget * block_menu;
	GtkWidget * block_job;
	GtkWidget * block_log;

	win_main = builder_widget(MODULE_MAIN_WINDOW,"window");
	gtk_window_set_title(GTK_WINDOW(win_main),STR_NAME_PROGRAMM);

	g_signal_connect(win_main,"destroy",G_CALLBACK(window_destroy_main), NULL);
	g_signal_connect(win_main,"key-press-event",G_CALLBACK(window_key_press_event_main),NULL);
	g_signal_connect(win_main,"realize",G_CALLBACK(window_realize_main),NULL);
	g_signal_connect(win_main,"show",G_CALLBACK(window_show_main),NULL);

	accgro_main = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(win_main),accgro_main);

	box = gtk_bin_get_child(GTK_BIN(win_main));

	block_menu = create_block_menu(win_main,accgro_main);
	block_job = create_block_job();
	block_log = create_block_log();

	gtk_box_pack_start(GTK_BOX(box),block_menu,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_job,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_log,FALSE,TRUE,0);

	gtk_widget_show(win_main);
	gtk_widget_show(box);

	return win_main;
}

static flag_t move_main_block(GtkWidget * win_main)
{
	/*default_size(win_main);*/
	return SUCCESS;
}

#define MIN_WIDTH_SCREEN         1680
#define MIN_HEIGHT_SCREEN        1050

static flag_t check_size_window(void)
{
	GdkDisplay * display = gdk_display_get_default();
	GdkScreen * screen = gdk_display_get_default_screen(display);
	int width = gdk_screen_get_width(screen);
	int height = gdk_screen_get_height(screen);

	if( (width < MIN_WIDTH_SCREEN) || (height < MIN_HEIGHT_SCREEN)){
		dialog_error("Минимальное разрешение экрана 1680x1050");
		exit(1);
	}
	return SUCCESS;
}
/*****************************************************************************/
int main(int argc,char * argv[])
{
	GtkWidget * win_main;

	gtk_init(&argc,&argv);

	check_size_window();

	init_system();

	win_main = create_main_block();

	apply_style_main(win_main,NULL);

	move_main_block(win_main);

	gtk_main();

	deinit_system();

	return SUCCESS;
}
/*****************************************************************************/
