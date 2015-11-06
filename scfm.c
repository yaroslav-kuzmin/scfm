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
#include "common.h"

/*****************************************************************************/

/*****************************************************************************/
static void destroy_window_main(GtkWidget * w,gpointer ud)
{
	deinit_system();
	gtk_main_quit();
}

static gboolean key_press_event_window_main(GtkWidget * w,GdkEvent  *event,gpointer ud)
{
	GdkEventType type = event->type;
	gint state;

	if(type == GDK_KEY_PRESS){
		GdkEventKey * event_key = (GdkEventKey*)event;
		state = event_key->state;
		if( (state & GDK_SHIFT_MASK) && (state & GDK_CONTROL_MASK)){
			if( event_key->keyval == GDK_KEY_A){
				about_programm();
			}
		}
	}
	return FALSE;
}

#define MAIN_SPACING       3

GtkWidget * create_main_block(void)
{
	GtkWidget * win_main;

	win_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(win_main),MAIN_SPACING);
	gtk_window_set_title(GTK_WINDOW(win_main),STR_NAME_PROGRAMM);
	gtk_window_set_resizable(GTK_WINDOW(win_main),TRUE);
	gtk_window_set_position (GTK_WINDOW(win_main),GTK_WIN_POS_CENTER);

	gtk_window_set_default_size(GTK_WINDOW(win_main),300,300);
	g_signal_connect(win_main,"destroy",G_CALLBACK(destroy_window_main), NULL);
	g_signal_connect(win_main,"key-press-event",G_CALLBACK(key_press_event_window_main),NULL);

	gtk_widget_show_all(win_main);

	return win_main;
}
/*****************************************************************************/
int main(int argc,char * argv[])
{
	gtk_init(&argc,&argv);

	init_system();

	create_main_block();

	gtk_main();

	return SUCCESS;
}
/*****************************************************************************/
