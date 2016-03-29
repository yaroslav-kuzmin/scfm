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
#include "bridge.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/


/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/
static void activate_menu_bridge_view(GtkMenuItem * b,gpointer ud)
{
	create_windows_bridge();
}

static int mode_control = MODE_CONTROL_OFF;
static void activate_menu_job_control(GtkMenuItem * b,gpointer ud)
{
	GtkWidget * w = (GtkWidget *)ud;
	if(mode_control == MODE_CONTROL_ON){
		mode_control = MODE_CONTROL_OFF;
	}
	else{
		mode_control = MODE_CONTROL_ON;
	}
	set_mode_work(mode_control,w);
}

static void activate_menu_job_config(GtkMenuItem * b,gpointer ud)
{
	GtkWidget * w = (GtkWidget *)ud;
	set_mode_work(MODE_CONFIGURATION,w);
}

static void activate_menu_job_exit(GtkMenuItem * b,gpointer ud)
{
	GtkWidget * w = (GtkWidget *)ud;
	gtk_widget_destroy(w);
}

/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

static GtkWidget * create_menu_bridge(GtkWidget * win_main,GtkAccelGroup * main_accgro)
{
	GtkWidget * menite_bridge;
	GtkWidget * men_bridge;
	GtkWidget * menite_view;

	menite_bridge = gtk_menu_item_new_with_label("Мост");
	men_bridge = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menite_bridge),men_bridge);

	menite_view = gtk_menu_item_new_with_label("Отобразить");
	g_signal_connect(menite_view,"activate",G_CALLBACK(activate_menu_bridge_view),NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(men_bridge),menite_view);

	gtk_widget_show(menite_bridge);
	gtk_widget_show(men_bridge);
	gtk_widget_show(menite_view);

	return menite_bridge;
}

static GtkWidget * create_menu_job(GtkWidget * win_main,GtkAccelGroup * main_accgro)
{
	GtkWidget * menite_job;
	GtkWidget * men_job;
	GtkWidget * menite_control;
	GtkWidget * menite_config;
	GtkWidget * menite_s0;
	GtkWidget * menite_exit;

	menite_job = gtk_menu_item_new_with_label("Работа");

	men_job = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menite_job),men_job);

	menite_control = gtk_menu_item_new_with_label("Управление");
	g_signal_connect(menite_control,"activate",G_CALLBACK(activate_menu_job_control),win_main);
	gtk_widget_add_accelerator(menite_control,"activate",main_accgro
	                          ,GDK_KEY_E,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(men_job),menite_control);


	menite_config = gtk_menu_item_new_with_label("Конфигурирование");
	g_signal_connect(menite_config,"activate",G_CALLBACK(activate_menu_job_config),win_main);
	gtk_widget_add_accelerator(menite_config,"activate",main_accgro
	                          ,GDK_KEY_C,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(men_job),menite_config);

	menite_s0 = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(men_job),menite_s0);

	menite_exit = gtk_menu_item_new_with_label("Выход");
	g_signal_connect(menite_exit,"activate",G_CALLBACK(activate_menu_job_exit),win_main);
	gtk_widget_add_accelerator(menite_exit,"activate",main_accgro
	                          ,GDK_KEY_Q,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(men_job),menite_exit);

	g_info(" Ctrl+E : %#x : %#x",GDK_KEY_E,'E');
	g_info(" Ctrl+Q : %#x : %#x",GDK_KEY_Q,'Q');

	gtk_widget_show(menite_job);
	gtk_widget_show(men_job);
	gtk_widget_show(menite_control);
	gtk_widget_show(menite_config);
	gtk_widget_show(menite_s0);
	gtk_widget_show(menite_exit);

	return menite_job;
}

GtkWidget * create_block_menu(GtkWidget * win_main,GtkAccelGroup * main_accgro)
{
	GtkWidget * menbar_main;
	GtkWidget * menite_job;
	GtkWidget * menite_bridge;

	menbar_main = gtk_menu_bar_new();
	layout_widget(menbar_main,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,FALSE);

	menite_job = create_menu_job(win_main,main_accgro);
	gtk_menu_shell_append(GTK_MENU_SHELL(menbar_main),menite_job);

	menite_bridge = create_menu_bridge(win_main,main_accgro);
	gtk_menu_shell_append(GTK_MENU_SHELL(menbar_main),menite_bridge);

	gtk_widget_show(menbar_main);
	return menbar_main;
}

/*****************************************************************************/
