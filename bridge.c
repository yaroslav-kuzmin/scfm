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
#include "log.h"

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
struct _block_bridge_s
{
	link_s server;
	link_s client;
};
typedef struct _block_bridge_s block_bridge_s;

static GtkEntryBuffer * create_block_state_input(char * label,uint16_t len,char * str_default)
{
	GtkWidget * label;
	GtkWidget * entry;
	GtkEntryBuffer * buf;

	return buf;
}

static GtkWidget * create_block_state_server(block_bridge_s * bb)
{
	GtkWidget * box;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_widget_show(box);
	return box;
}

static GtkWidget * create_block_state_client(block_bridge_s * bb)
{
	GtkWidget * box;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_widget_show(box);
	return box;
}

static GtkWidget * create_block_state(block_bridge_s * bb)
{
	GtkWidget * box;
	GtkWidget * block_state_server;
	GtkWidget * block_state_client;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

	block_state_server = create_block_state_server(bb);
	block_state_client = create_block_state_client(bb);

	gtk_box_pack_start(GTK_BOX(box),block_state_server,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_state_client,TRUE,TRUE,0);
	gtk_widget_show(box);
	return box;
}

static GtkWidget * create_block_info(block_bridge_s * bb)
{
	GtkWidget * grid;
	grid = gtk_grid_new();
	gtk_widget_show(grid);
	return grid;
}

static GtkWidget * create_block_control(block_bridge_s * bb)
{
	GtkWidget * box;
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_widget_show(box);
	return box;
}

static char STR_BRIDGE[] = "Мост";
static block_bridge_s block_bridge;
static GtkWidget * create_block_bridge()
{
	GtkWidget * frame;
	GtkWidget * box;
	GtkWidget * state;
	GtkWidget * info;
	GtkWidget * control;

	frame = gtk_frame_new(STR_BRIDGE);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

	state = create_block_state(&block_bridge);
	info = create_block_info(&block_bridge);
	control = create_block_control(&block_bridge);

	gtk_container_add(GTK_CONTAINER(frame),box);
	gtk_box_pack_start(GTK_BOX(box),state,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),info,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),control,TRUE,TRUE,0);

	gtk_widget_show(frame);
	gtk_widget_show(box);

	return frame;
}

static void clicked_button_exit(GtkButton * b,gpointer ud)
{
	GtkWidget * win = (GtkWidget*)ud;
	gtk_widget_destroy(win);
}
static gboolean key_press_event_window_bridge(GtkWidget * w,GdkEvent  *event,gpointer ud)
{
#if 0
	config_window_s * win = (config_window_s*)ud;
	GdkEventType type = event->type;
	gint state;
#endif
	return FALSE;
}
static void destroy_window_bridge(GtkWidget * w,gpointer ud)
{
}
/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
#define MIN_WIDTH_WIN_BRIDGE     500
#define MIN_HEIGHT_WIN_BRIDGE    500
static char STR_BRIDGE_EXIT[] = "Выход";
int create_windows_bridge(void)
{
	GtkWidget * win_bridge;
	GtkWidget * box;
	GtkWidget * block_bridge;
	GtkWidget * exit;

	win_bridge = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(win_bridge),5);
	gtk_window_set_title(GTK_WINDOW(win_bridge),STR_NAME_PROGRAMM);
	gtk_window_set_resizable(GTK_WINDOW(win_bridge),TRUE);
	gtk_window_set_position (GTK_WINDOW(win_bridge),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(win_bridge),MIN_WIDTH_WIN_BRIDGE,MIN_HEIGHT_WIN_BRIDGE);
	g_signal_connect(win_bridge,"destroy",G_CALLBACK(destroy_window_bridge),NULL);
	g_signal_connect(win_bridge,"key-press-event",G_CALLBACK(key_press_event_window_bridge),NULL);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_bridge = create_block_bridge();

	exit = gtk_button_new_with_label(STR_BRIDGE_EXIT);
	layout_widget(exit,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(exit,"clicked",G_CALLBACK(clicked_button_exit),win_bridge);

	gtk_container_add(GTK_CONTAINER(win_bridge),box);
	gtk_box_pack_start(GTK_BOX(box),block_bridge,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),exit,FALSE,TRUE,0);

	gtk_widget_show(win_bridge);
	gtk_widget_show(box);
	gtk_widget_show(exit);

	return SUCCESS;
}

/*****************************************************************************/
