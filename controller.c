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
#include <gio/gio.h>
#include <string.h>

#include "pub.h"
#include "common.h"
#include "database.h"
#include "device.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

typedef struct _show_state_s show_state_s;
struct _show_state_s
{
	GtkImage * image_vertical;
	GdkPixbuf * frame_vertical;

	GtkImage * image_horizontal;
	GdkPixbuf * frame_horizontal;

	GtkImage * image_pipe;
	GdkPixbuf * frame_pipe;

	GtkImage * image_video;
	GdkPixbuf * frame_video;

	GtkLabel * lab_auto_work;
	GtkImage * image_auto_work;
	GdkPixbuf * frame_auto_work;

	GtkLabel * lab_mode;
	GtkLabel * lab_state;

	GtkImage * image_fire_alarm;
	GdkPixbuf * frame_fire_alarm;
	uint32_t counter_fire_alarm;
};

typedef struct _show_control_s show_control_s;
struct _show_control_s
{
	/*режим */
	GtkButton * but_mode;
	GtkWidget * image_mode_auto;
	GtkWidget * image_mode_manual;
	flag_t mode;

	/*лафет*/
	GtkButton * but_up;
	GtkButton * but_bottom;
	GtkButton * but_right;
	GtkButton * but_left;
	flag_t lafet_precise;
	uint16_t lafet_precise_value;

	/*актуатор*/
	GtkWidget * actuator_spray;
	GtkWidget * actuator_rate;

	/*задвижка*/
	GtkButton * but_valve_open;
	GtkButton * but_valve_close;
	uint16_t valve_precise;
	/*Осциляция*/
	GtkToggleButton * but_oscillation_vertical;
	GtkToggleButton * but_oscillation_horizontal;
	GtkToggleButton * but_oscillation_saw;
	GtkToggleButton * but_oscillation_step;
	flag_t oscillation_flag;
	command_u oscillation_command;
};

typedef struct _block_controller_s block_controller_s;
struct _block_controller_s
{
	/*отображение блока конторллера*/
	flag_t stop_show;
	flag_t run_show;
	uint32_t timeout_show;

	/*флаги управления*/

	/*потоки взаимодействия с конторлером*/
	controller_s * current;
	GSList * list_controllers;

	/*отображение блока управления в разных режимах*/
	show_state_s * state;
	show_control_s * control;
};

/*****************************************************************************/
/* Функции работы с изображениями                                            */
/*****************************************************************************/
/*номера рисунков*/
enum
{
 	VERTICAL_000 = 0,
	VERTICAL_006,
	VERTICAL_012,
	VERTICAL_018,
	VERTICAL_024,
	VERTICAL_030,
	VERTICAL_036,
	VERTICAL_042,
	VERTICAL_048,
	VERTICAL_054,
	VERTICAL_060,
	VERTICAL_066,
	VERTICAL_072,
	VERTICAL_078,
	VERTICAL_084,
	VERTICAL_090,
	VERTICAL_096,
	VERTICAL_102,
	VERTICAL_108,
	VERTICAL_114,
	VERTICAL_120,
	VERTICAL_126,
	VERTICAL_132,
	VERTICAL_138,
	VERTICAL_144,
	VERTICAL_150,
	VERTICAL_156,
	VERTICAL_162,
	VERTICAL_168,
	VERTICAL_174,
	VERTICAL_180,
	VERTICAL_BACKGROUND,

	HORIZONTAL_000,
	HORIZONTAL_006,
	HORIZONTAL_012,
	HORIZONTAL_018,
	HORIZONTAL_024,
	HORIZONTAL_030,
	HORIZONTAL_036,
	HORIZONTAL_042,
	HORIZONTAL_048,
	HORIZONTAL_054,
	HORIZONTAL_060,
	HORIZONTAL_066,
	HORIZONTAL_072,
	HORIZONTAL_078,
	HORIZONTAL_084,
	HORIZONTAL_090,
	HORIZONTAL_096,
	HORIZONTAL_102,
	HORIZONTAL_108,
	HORIZONTAL_114,
	HORIZONTAL_120,
	HORIZONTAL_126,
	HORIZONTAL_132,
	HORIZONTAL_138,
	HORIZONTAL_144,
	HORIZONTAL_150,
	HORIZONTAL_156,
	HORIZONTAL_162,
	HORIZONTAL_168,
	HORIZONTAL_174,
	HORIZONTAL_180,
	HORIZONTAL_186,
	HORIZONTAL_192,
	HORIZONTAL_198,
	HORIZONTAL_204,
	HORIZONTAL_210,
	HORIZONTAL_216,
	HORIZONTAL_222,
	HORIZONTAL_228,
	HORIZONTAL_234,
	HORIZONTAL_240,
	HORIZONTAL_246,
	HORIZONTAL_252,
	HORIZONTAL_258,
	HORIZONTAL_264,
	HORIZONTAL_270,
	HORIZONTAL_276,
	HORIZONTAL_282,
	HORIZONTAL_288,
	HORIZONTAL_294,
	HORIZONTAL_300,
	HORIZONTAL_306,
	HORIZONTAL_312,
	HORIZONTAL_318,
	HORIZONTAL_324,
	HORIZONTAL_330,
	HORIZONTAL_336,
	HORIZONTAL_342,
	HORIZONTAL_348,
	HORIZONTAL_354,
	HORIZONTAL_360,
	HORIZONTAL_BACKGROUND,

	PIPE_000,
	PIPE_025,
	PIPE_050,
	PIPE_075,
	PIPE_100,
	PIPE_BACKGROUND,

	FIRE_ALARM_ON_0,
	FIRE_ALARM_ON_1,
	FIRE_ALARM_OFF,

 	AMOUNT_IMAGE_STATE
};

static GdkPixbuf * images_state[AMOUNT_IMAGE_STATE] = {0};
static uint16_t period_vertical = 6;
static uint16_t max_vertical = 180;
static uint16_t period_horizontal = 6;
static uint16_t max_horizontal = 360;


static int init_image(block_controller_s * bc)
{
	/*TODO высвободить память из буфферов*/

	images_state[VERTICAL_000]        = get_resource_image(RESOURCE_IMAGE,"vertical_042");
	images_state[VERTICAL_006]        = get_resource_image(RESOURCE_IMAGE,"vertical_042");
	images_state[VERTICAL_012]        = get_resource_image(RESOURCE_IMAGE,"vertical_042");
	images_state[VERTICAL_018]        = get_resource_image(RESOURCE_IMAGE,"vertical_042");
	images_state[VERTICAL_024]        = get_resource_image(RESOURCE_IMAGE,"vertical_042");
	images_state[VERTICAL_030]        = get_resource_image(RESOURCE_IMAGE,"vertical_042");
	images_state[VERTICAL_036]        = get_resource_image(RESOURCE_IMAGE,"vertical_042");
	images_state[VERTICAL_042]        = get_resource_image(RESOURCE_IMAGE,"vertical_042");
	images_state[VERTICAL_048]        = get_resource_image(RESOURCE_IMAGE,"vertical_048");
	images_state[VERTICAL_054]        = get_resource_image(RESOURCE_IMAGE,"vertical_054");
	images_state[VERTICAL_060]        = get_resource_image(RESOURCE_IMAGE,"vertical_060");
	images_state[VERTICAL_066]        = get_resource_image(RESOURCE_IMAGE,"vertical_066");
	images_state[VERTICAL_072]        = get_resource_image(RESOURCE_IMAGE,"vertical_072");
	images_state[VERTICAL_078]        = get_resource_image(RESOURCE_IMAGE,"vertical_078");
	images_state[VERTICAL_084]        = get_resource_image(RESOURCE_IMAGE,"vertical_084");
	images_state[VERTICAL_090]        = get_resource_image(RESOURCE_IMAGE,"vertical_090");
	images_state[VERTICAL_096]        = get_resource_image(RESOURCE_IMAGE,"vertical_096");
	images_state[VERTICAL_102]        = get_resource_image(RESOURCE_IMAGE,"vertical_102");
	images_state[VERTICAL_108]        = get_resource_image(RESOURCE_IMAGE,"vertical_108");
	images_state[VERTICAL_114]        = get_resource_image(RESOURCE_IMAGE,"vertical_114");
	images_state[VERTICAL_120]        = get_resource_image(RESOURCE_IMAGE,"vertical_120");
	images_state[VERTICAL_126]        = get_resource_image(RESOURCE_IMAGE,"vertical_126");
	images_state[VERTICAL_132]        = get_resource_image(RESOURCE_IMAGE,"vertical_132");
	images_state[VERTICAL_138]        = get_resource_image(RESOURCE_IMAGE,"vertical_138");
	images_state[VERTICAL_144]        = get_resource_image(RESOURCE_IMAGE,"vertical_144");
	images_state[VERTICAL_150]        = get_resource_image(RESOURCE_IMAGE,"vertical_150");
	images_state[VERTICAL_156]        = get_resource_image(RESOURCE_IMAGE,"vertical_156");
	images_state[VERTICAL_162]        = get_resource_image(RESOURCE_IMAGE,"vertical_162");
	images_state[VERTICAL_168]        = get_resource_image(RESOURCE_IMAGE,"vertical_168");
	images_state[VERTICAL_174]        = get_resource_image(RESOURCE_IMAGE,"vertical_174");
	images_state[VERTICAL_180]        = get_resource_image(RESOURCE_IMAGE,"vertical_180");
	images_state[VERTICAL_BACKGROUND] = get_resource_image(RESOURCE_IMAGE,"vertical_background");

	images_state[HORIZONTAL_000]        = get_resource_image(RESOURCE_IMAGE,"horizontal_006");
	images_state[HORIZONTAL_006]        = get_resource_image(RESOURCE_IMAGE,"horizontal_006");
	images_state[HORIZONTAL_012]        = get_resource_image(RESOURCE_IMAGE,"horizontal_012");
	images_state[HORIZONTAL_018]        = get_resource_image(RESOURCE_IMAGE,"horizontal_018");
	images_state[HORIZONTAL_024]        = get_resource_image(RESOURCE_IMAGE,"horizontal_024");
	images_state[HORIZONTAL_030]        = get_resource_image(RESOURCE_IMAGE,"horizontal_030");
	images_state[HORIZONTAL_036]        = get_resource_image(RESOURCE_IMAGE,"horizontal_036");
	images_state[HORIZONTAL_042]        = get_resource_image(RESOURCE_IMAGE,"horizontal_042");
	images_state[HORIZONTAL_048]        = get_resource_image(RESOURCE_IMAGE,"horizontal_048");
	images_state[HORIZONTAL_054]        = get_resource_image(RESOURCE_IMAGE,"horizontal_054");
	images_state[HORIZONTAL_060]        = get_resource_image(RESOURCE_IMAGE,"horizontal_060");
	images_state[HORIZONTAL_066]        = get_resource_image(RESOURCE_IMAGE,"horizontal_066");
	images_state[HORIZONTAL_072]        = get_resource_image(RESOURCE_IMAGE,"horizontal_072");
	images_state[HORIZONTAL_078]        = get_resource_image(RESOURCE_IMAGE,"horizontal_078");
	images_state[HORIZONTAL_084]        = get_resource_image(RESOURCE_IMAGE,"horizontal_084");
	images_state[HORIZONTAL_090]        = get_resource_image(RESOURCE_IMAGE,"horizontal_090");
	images_state[HORIZONTAL_096]        = get_resource_image(RESOURCE_IMAGE,"horizontal_096");
	images_state[HORIZONTAL_102]        = get_resource_image(RESOURCE_IMAGE,"horizontal_102");
	images_state[HORIZONTAL_108]        = get_resource_image(RESOURCE_IMAGE,"horizontal_108");
	images_state[HORIZONTAL_114]        = get_resource_image(RESOURCE_IMAGE,"horizontal_114");
	images_state[HORIZONTAL_120]        = get_resource_image(RESOURCE_IMAGE,"horizontal_120");
	images_state[HORIZONTAL_126]        = get_resource_image(RESOURCE_IMAGE,"horizontal_126");
	images_state[HORIZONTAL_132]        = get_resource_image(RESOURCE_IMAGE,"horizontal_132");
	images_state[HORIZONTAL_138]        = get_resource_image(RESOURCE_IMAGE,"horizontal_138");
	images_state[HORIZONTAL_144]        = get_resource_image(RESOURCE_IMAGE,"horizontal_144");
	images_state[HORIZONTAL_150]        = get_resource_image(RESOURCE_IMAGE,"horizontal_150");
	images_state[HORIZONTAL_156]        = get_resource_image(RESOURCE_IMAGE,"horizontal_156");
	images_state[HORIZONTAL_162]        = get_resource_image(RESOURCE_IMAGE,"horizontal_162");
	images_state[HORIZONTAL_168]        = get_resource_image(RESOURCE_IMAGE,"horizontal_168");
	images_state[HORIZONTAL_174]        = get_resource_image(RESOURCE_IMAGE,"horizontal_174");
	images_state[HORIZONTAL_180]        = get_resource_image(RESOURCE_IMAGE,"horizontal_180");
	images_state[HORIZONTAL_186]        = get_resource_image(RESOURCE_IMAGE,"horizontal_186");
	images_state[HORIZONTAL_192]        = get_resource_image(RESOURCE_IMAGE,"horizontal_192");
	images_state[HORIZONTAL_198]        = get_resource_image(RESOURCE_IMAGE,"horizontal_198");
	images_state[HORIZONTAL_204]        = get_resource_image(RESOURCE_IMAGE,"horizontal_204");
	images_state[HORIZONTAL_210]        = get_resource_image(RESOURCE_IMAGE,"horizontal_210");
	images_state[HORIZONTAL_216]        = get_resource_image(RESOURCE_IMAGE,"horizontal_216");
	images_state[HORIZONTAL_222]        = get_resource_image(RESOURCE_IMAGE,"horizontal_222");
	images_state[HORIZONTAL_228]        = get_resource_image(RESOURCE_IMAGE,"horizontal_228");
	images_state[HORIZONTAL_234]        = get_resource_image(RESOURCE_IMAGE,"horizontal_234");
	images_state[HORIZONTAL_240]        = get_resource_image(RESOURCE_IMAGE,"horizontal_240");
	images_state[HORIZONTAL_246]        = get_resource_image(RESOURCE_IMAGE,"horizontal_246");
	images_state[HORIZONTAL_252]        = get_resource_image(RESOURCE_IMAGE,"horizontal_252");
	images_state[HORIZONTAL_258]        = get_resource_image(RESOURCE_IMAGE,"horizontal_258");
	images_state[HORIZONTAL_264]        = get_resource_image(RESOURCE_IMAGE,"horizontal_264");
	images_state[HORIZONTAL_270]        = get_resource_image(RESOURCE_IMAGE,"horizontal_270");
	images_state[HORIZONTAL_276]        = get_resource_image(RESOURCE_IMAGE,"horizontal_276");
	images_state[HORIZONTAL_282]        = get_resource_image(RESOURCE_IMAGE,"horizontal_282");
	images_state[HORIZONTAL_288]        = get_resource_image(RESOURCE_IMAGE,"horizontal_288");
	images_state[HORIZONTAL_294]        = get_resource_image(RESOURCE_IMAGE,"horizontal_294");
	images_state[HORIZONTAL_300]        = get_resource_image(RESOURCE_IMAGE,"horizontal_300");
	images_state[HORIZONTAL_306]        = get_resource_image(RESOURCE_IMAGE,"horizontal_306");
	images_state[HORIZONTAL_312]        = get_resource_image(RESOURCE_IMAGE,"horizontal_312");
	images_state[HORIZONTAL_318]        = get_resource_image(RESOURCE_IMAGE,"horizontal_318");
	images_state[HORIZONTAL_324]        = get_resource_image(RESOURCE_IMAGE,"horizontal_324");
	images_state[HORIZONTAL_330]        = get_resource_image(RESOURCE_IMAGE,"horizontal_330");
	images_state[HORIZONTAL_336]        = get_resource_image(RESOURCE_IMAGE,"horizontal_336");
	images_state[HORIZONTAL_342]        = get_resource_image(RESOURCE_IMAGE,"horizontal_342");
	images_state[HORIZONTAL_348]        = get_resource_image(RESOURCE_IMAGE,"horizontal_348");
	images_state[HORIZONTAL_354]        = get_resource_image(RESOURCE_IMAGE,"horizontal_354");
	images_state[HORIZONTAL_360]        = get_resource_image(RESOURCE_IMAGE,"horizontal_354");
	images_state[HORIZONTAL_BACKGROUND] = get_resource_image(RESOURCE_IMAGE,"horizontal_background");

	images_state[PIPE_000]        = get_resource_image(RESOURCE_IMAGE,"pipe_000");
	images_state[PIPE_025]        = get_resource_image(RESOURCE_IMAGE,"pipe_025");
	images_state[PIPE_050]        = get_resource_image(RESOURCE_IMAGE,"pipe_050");
	images_state[PIPE_075]        = get_resource_image(RESOURCE_IMAGE,"pipe_075");
	images_state[PIPE_100]        = get_resource_image(RESOURCE_IMAGE,"pipe_100");
	images_state[PIPE_BACKGROUND] = get_resource_image(RESOURCE_IMAGE,"pipe_background");

	images_state[FIRE_ALARM_ON_0] = get_resource_image(RESOURCE_IMAGE,"fire_alarm_on_0");
	images_state[FIRE_ALARM_ON_1] = get_resource_image(RESOURCE_IMAGE,"fire_alarm_on_1");
	images_state[FIRE_ALARM_OFF]  = get_resource_image(RESOURCE_IMAGE,"fire_alarm_off");

 	return SUCCESS;
}

#define IMAGE_BACKGROUND    0xFFFF

static GdkPixbuf * get_image_vertical(uint16_t angle)
{
	GdkPixbuf * buf = NULL;

	if(angle == IMAGE_BACKGROUND){
		return images_state[VERTICAL_BACKGROUND];
	}

	if(angle > max_vertical){
		angle = max_vertical;
	}

	angle = angle / period_vertical;
	angle = angle + VERTICAL_000;
	buf = images_state[angle];

 	return buf;
}
static GdkPixbuf * get_image_horizontal(uint16_t angle)
{
 	GdkPixbuf * buf = NULL;

	if(angle == IMAGE_BACKGROUND){
		return images_state[HORIZONTAL_BACKGROUND];
	}

	if(angle > max_horizontal){
		angle = max_horizontal;
	}
	angle = angle / period_horizontal;
	angle = angle + HORIZONTAL_000;
 	buf = images_state[angle];
	return buf;
}

static GdkPixbuf * get_image_pipe(uint16_t value)
{
	if(value == IMAGE_BACKGROUND){
		return images_state[PIPE_BACKGROUND];
	}
	return NULL;
}

static GdkPixbuf * get_image_valve_tic(flag_t valve)
{
	GdkPixbuf * buf;

	switch(valve){
		case STATE_VALVE_OPEN:
			buf = images_state[PIPE_100];
			break;
		case STATE_VALVE_CLOSE:
			buf = images_state[PIPE_000];
			break;
		case STATE_VALVE_OPEN_RUN:
			buf = images_state[PIPE_050];
			break;
		case STATE_VALVE_CLOSE_RUN:
			buf = images_state[PIPE_050];
			break;
		default:
				/*TODO поставить отдельный рисунок на ошибку*/
			buf = images_state[PIPE_000];
 			break;
	}

 	return buf;
}

static GdkPixbuf * get_image_valve_analog(uint16_t valve)
{
	return images_state[PIPE_000];
}

/*****************************************************************************/

#define MIN_VERTICAL_TIC      0
#define MAX_VERTICAL_TIC      30
#define MIN_VERTICAL_ANGLE   	0
#define MAX_VERTICAL_ANGLE    180
static uint16_t calculate_angle_tic_vertical(state_controller_s * state,config_controller_s * config)
{
	uint16_t angle;
	uint16_t tic = state->tic_vertical;
	gdouble rate = config->rate_tic_vertical;

	/*TODO для конкретного контролера */
	tic += 7;

	if(tic > MAX_VERTICAL_TIC){
		tic = MAX_VERTICAL_TIC;
	}
	rate = (gdouble)tic * rate;
	angle =(uint16_t)rate;
	/*если коэффициент не корректный*/
	if(angle > MAX_VERTICAL_ANGLE){
		angle = MAX_VERTICAL_ANGLE;
	}

 	return angle;
}
static uint16_t calculate_angle_encoder_vertical(state_controller_s * state,config_controller_s * config)
{
	int16_t angle = 0;
	return angle;
}
#define MIN_HORIZONTAL_TIC      0
#define MAX_HORIZONTAL_TIC      60

#define MIN_HORIZONTAL_ANGLE    0
#define MAX_HORIZONTAL_ANGLE    360
static uint16_t calculate_angle_tic_horizontal(state_controller_s * state,config_controller_s * config)
{
	uint16_t angle;
	uint16_t tic = state->tic_horizontal;
	gdouble rate = config->rate_tic_horizontal;

	if(tic > MAX_HORIZONTAL_TIC){
		tic = MAX_HORIZONTAL_TIC;
	}
	rate = (gdouble)tic * rate;
	angle = (uint16_t)rate;
	/*если коэффициент не корректный*/
	if(angle > MAX_HORIZONTAL_ANGLE){
		angle = MAX_HORIZONTAL_ANGLE;
	}

	return angle;
}

static uint16_t calculate_angle_encoder_horizontal(state_controller_s * state,config_controller_s * config)
{
	int16_t angle = 0;
	return angle;
}

static uint16_t calculate_valve(state_controller_s * state,config_controller_s * config)
{
	return 0;
}

#if 0
static uint16_t calculate_pressure(state_controller_s * state,config_controller_s * config)
{
	gdouble rate = config->rate_pressure;
	uint16_t pressure = state->pressure;

	rate = (gdouble)pressure * rate;

	if(rate < 0){
		pressure = STATE_PRESSURE_ERROR;
	}
	else{
		if(rate > UINT16_MAX){
			pressure = STATE_PRESSURE_ERROR;
		}
		else{
			pressure = (uint16_t)rate;
 		}
	}

	return pressure;
}
#endif


/*****************************************************************************/
/*                                                                           */
/*     Функции отрисовки информации по таймеру                               */
/*                                                                           */
/*****************************************************************************/

static int image_font_size = 16;

static flag_t image_paint_string(cairo_surface_t * cairo_surface,char * str,int x,int y)
{
	cairo_t * cairo = cairo_create(cairo_surface);

	cairo_select_font_face (cairo, "Luciida Console",/*"Courier",*/ /*"Georgia",*/
		    CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

	cairo_set_source_rgba(cairo,color_white.red,color_white.green,color_white.blue,color_white.alpha);

	cairo_set_font_size(cairo,image_font_size);

	cairo_move_to(cairo,x,y);

	cairo_show_text(cairo,str);

	cairo_destroy(cairo);
	return SUCCESS;
}

static GdkPixbuf * image_add_string(GdkPixbuf * buf_src,char * str,int x,int y)
{
	GdkPixbuf * buf_des;
	cairo_surface_t * cairo_surface = gdk_cairo_surface_create_from_pixbuf(buf_src,1,NULL);
	gint width = cairo_image_surface_get_width(cairo_surface);
	gint height = cairo_image_surface_get_height(cairo_surface);

	if(x > width ){
		return buf_src;
	}
	if(y > height){
		return buf_src;
	}

	image_paint_string(cairo_surface,str,x,y);

	g_object_unref(buf_src);
	buf_des = gdk_pixbuf_get_from_surface(cairo_surface,0,0,width,height);
	cairo_surface_destroy(cairo_surface);
	return buf_des;
}

static flag_t image_copy(GdkPixbuf * buf_des,GdkPixbuf * buf_src)
{
	int width = gdk_pixbuf_get_width(buf_des);
	int height = gdk_pixbuf_get_height(buf_des);
	gdk_pixbuf_copy_area(buf_src,0,0,width,height,buf_des,0,0);
	return SUCCESS;
}
static flag_t image_impose(GdkPixbuf * buf_des,GdkPixbuf * buf_src)
{
	int width = gdk_pixbuf_get_width(buf_des);
	int height = gdk_pixbuf_get_height(buf_des);
	gdk_pixbuf_composite(buf_src,buf_des,0,0,width,height,0,0,1,1,GDK_INTERP_NEAREST,255);
	return SUCCESS;
}

static flag_t show_console(show_state_s * show_state,show_control_s * show_control
                       ,state_controller_s * controller_state,config_controller_s * controller_config)
{
	flag_t mode = controller_mode(controller_state);
	GtkWidget * image = gtk_button_get_image(show_control->but_mode);

	switch(mode){
		case STATE_MODE_AUTO:
			show_control->mode = STATE_MODE_AUTO;
			if(image != show_control->image_mode_auto){
				gtk_button_set_image(show_control->but_mode,show_control->image_mode_auto);
			}
			break;
		case STATE_MODE_MANUAL:
			show_control->mode = STATE_MODE_MANUAL;
			if(image != show_control->image_mode_manual){
				gtk_button_set_image(show_control->but_mode,show_control->image_mode_manual);
			}
			break;
	}

	return SUCCESS;
}

static char STR_INFO_MODE_WAIT[]   = "Нет Соединения";
static char STR_INFO_MODE_AUTO[]   = "Автоматически режим работы";
static char STR_INFO_MODE_MANUAL[] = "Ручной режим работы";
static char STR_INFO_MODE_TEST[]   = "Режим тестирования";

static flag_t show_message_mode(GtkLabel * label,state_controller_s * state)
{
	flag_t mode = controller_mode(state);
	char * str = STR_INFO_MODE_WAIT;
	switch(mode){
		case STATE_MODE_AUTO:
			str = STR_INFO_MODE_AUTO;
			break;
		case STATE_MODE_MANUAL:
			str = STR_INFO_MODE_MANUAL;
			break;
		case STATE_MODE_TEST:
			str = STR_INFO_MODE_TEST;
			break;
	}
	gtk_label_set_text(label,str);

	if(mode == STATE_MODE_ERROR){
		apply_style_message_alarm(GTK_WIDGET(label),NULL);
	}
	else{
		apply_style_message_norm(GTK_WIDGET(label),NULL);
	}
	return SUCCESS;
}

static char STR_INFO_STATE_WAIT[]             = "Нет подключина!";
static char STR_INFO_STATE_NORM[]             = "Норма";
static char STR_INFO_STATE_LIMIT_VERTICAL[]   = "Предел по вертикале ";
static char STR_INFO_STATE_LIMIT_HORIZONTAL[] = "Предел по горизонтали";
static char STR_INFO_STATE_CRASH_VERTICAL[]   = "Авария вертикальной оси";
static char STR_INFO_STATE_CRASH_HORIZONTAL[] = "Авария горизонтальной оси";
static char STR_INFO_STATE_CRASH_SPARY[]      = "Авария актуатор распыла";
static char STR_INFO_STATE_CRASH_RATE[]       = "Авария актуатор литраж";
static char STR_INFO_STATE_CRASH_VALVE[]      = "Авария привода задвижки";
#define WAIT_MESSAGE_IN_SECOND      2
static uint32_t count_message = 0;
static uint32_t next_message = 0;

static flag_t show_message_state(GtkLabel * label,state_controller_s * state,uint32_t timeout)
{
	/*колличество сообщений об ошибке */
	flag_t all_info[] = {STATE_INFO_ERROR,STATE_INFO_ERROR,STATE_INFO_ERROR
	                    ,STATE_INFO_ERROR,STATE_INFO_ERROR,STATE_INFO_ERROR
	                    ,STATE_INFO_ERROR,STATE_INFO_ERROR,STATE_INFO_ERROR
	                    ,STATE_ERROR};
	flag_t * info = all_info;
	flag_t i;
	char * str;

	controller_info(state,info);
	if(*info == STATE_INFO_NORM){
		gtk_label_set_text(label,STR_INFO_STATE_NORM);
		apply_style_message_norm(GTK_WIDGET(label),NULL);
		return SUCCESS;
	}

	i = *(info + next_message);

	if((i == STATE_INFO_ERROR) || (i == STATE_ERROR)){
		next_message = 0;
		i = *info;
	}

	switch(i){
		case STATE_INFO_LIMIT_VERTICAL:
			str = STR_INFO_STATE_LIMIT_VERTICAL;
			break;
		case STATE_INFO_LIMIT_HORIZONTAL:
			str = STR_INFO_STATE_LIMIT_HORIZONTAL;
			break;
		case STATE_INFO_CRASH_VERTICAL:
			str = STR_INFO_STATE_CRASH_VERTICAL;
			break;
		case STATE_INFO_CRASH_HORIZONTAL:
			str = STR_INFO_STATE_CRASH_HORIZONTAL;
			break;
		case STATE_INFO_CRASH_SPARY:
			str = STR_INFO_STATE_CRASH_SPARY;
			break;
		case STATE_INFO_CRASH_RATE:
			str = STR_INFO_STATE_CRASH_RATE;
			break;
		case STATE_INFO_CRASH_VALVE:
			str = STR_INFO_STATE_CRASH_VALVE;
			break;
		default:
			str = STR_INFO_MODE_WAIT;
			break;
	}
	gtk_label_set_text(label,str);
	apply_style_message_alarm(GTK_WIDGET(label),NULL);

	count_message += timeout;
	if(count_message == (MILLISECOND_PER_SECOND * WAIT_MESSAGE_IN_SECOND)){
		count_message = 0;
		next_message ++;
	}
	return SUCCESS;
}


static flag_t show_message(show_state_s * show_state,show_control_s * show_control
                          ,state_controller_s * controller_state,config_controller_s * controller_config
													,uint32_t timeout)
{
	show_message_mode(show_state->lab_mode,controller_state);
	show_message_state(show_state->lab_state,controller_state,timeout);
	return SUCCESS;
}

static flag_t show_vertical(show_state_s * show_state,show_control_s * show_control
                        ,state_controller_s * controller_state,config_controller_s * controller_config)
{
	uint64_t flag = controller_config->flag;
	int16_t angle = 0;
	GdkPixbuf * frame = show_state->frame_vertical;
	GdkPixbuf * background;
	GdkPixbuf * angle_image;

	if(!ENGINE_VERTICAL(flag)){
		return SUCCESS;
	}
	if(TIC_VERTICAL(flag)){
		angle = calculate_angle_tic_vertical(controller_state,controller_config);
	}
	else{
		if(ENCODER_VERTICAL(flag)){
			angle = calculate_angle_encoder_vertical(controller_state,controller_config);
		}
	}

	background = get_image_vertical(IMAGE_BACKGROUND);
	image_copy(frame,background);

	angle_image = get_image_vertical(angle);
	image_impose(frame,angle_image);

	g_string_printf(pub,"%03d",angle);
	frame = image_add_string(frame,pub->str,34,465);

	gtk_image_set_from_pixbuf(show_state->image_vertical,frame);

	show_state->frame_vertical = frame;

	return SUCCESS;
}

static flag_t show_horizontal(show_state_s * show_state,show_control_s * show_control
                          ,state_controller_s * controller_state,config_controller_s * controller_config)
{
	uint64_t flag = controller_config->flag;
	int16_t angle = 0;
	GdkPixbuf * frame = show_state->frame_horizontal;
	GdkPixbuf * background;
	GdkPixbuf * angle_image;

	if(!ENGINE_HORIZONTAL(flag)){
		return SUCCESS;
	}
	if(TIC_HORIZONTAL(flag)){
		angle = calculate_angle_tic_horizontal(controller_state,controller_config);
	}
	else{
		if(ENCODER_HORIZONTAL(flag)){
			angle = calculate_angle_encoder_horizontal(controller_state,controller_config);
		}
	}

	background = get_image_horizontal(IMAGE_BACKGROUND);
	image_copy(frame,background);

	angle_image = get_image_horizontal(angle);
	image_impose(frame,angle_image);

	g_string_printf(pub,"%03d",angle);
	frame = image_add_string(frame,pub->str,34,465);

	gtk_image_set_from_pixbuf(show_state->image_horizontal,frame);
	show_state->frame_horizontal = frame;
	return SUCCESS;
}

static char STR_VALVE_OPEN[]      = "Открыта";
static char STR_VALVE_OPEN_RUN[]  = "Открывается";
static char STR_VALVE_CLOSE[]     = "Закрыта";
static char STR_VALVE_CLOSE_RUN[] = "Закрывается";
static char STR_VALVE_ERROR[]     = "Ошибка";

static char * get_state_valve_string(flag_t state)
{
	char * str = STR_VALVE_ERROR;
	switch(state){
		case STATE_VALVE_OPEN:
			str = STR_VALVE_OPEN;
			break;
		case STATE_VALVE_OPEN_RUN:
			str = STR_VALVE_OPEN_RUN;
			break;
		case STATE_VALVE_CLOSE:
			str = STR_VALVE_CLOSE;
			break;
		case STATE_VALVE_CLOSE_RUN:
			str = STR_VALVE_CLOSE_RUN;
			break;
	}
	return str;
}

static flag_t show_pipe(show_state_s * show_state,show_control_s * show_control
                     ,state_controller_s * controller_state,config_controller_s * controller_config)
{
	uint64_t flag = controller_config->flag;
	GdkPixbuf * frame = show_state->frame_pipe;
	GdkPixbuf * background;
	GdkPixbuf * image_valve;
	char * str_valve;
	flag_t state_valve;
	uint16_t analog_valve;

	if(!VALVE_DRY(flag)){
		if(!VALVE_ANALOG(flag)){
			return SUCCESS;
	 	}
	}

	state_valve = controller_state_valve(controller_state);

	if(VALVE_LIMIT(flag)){
	 	image_valve = get_image_valve_tic(state_valve);
		str_valve = get_state_valve_string(state_valve);
	}
	else{
	 	if(VALVE_FEEDBACK(flag)){
			analog_valve = calculate_valve(controller_state,controller_config);
			image_valve = get_image_valve_analog(analog_valve);
		}
		else{
			return SUCCESS;
		}
	}

	background = get_image_pipe(IMAGE_BACKGROUND);
	image_copy(frame,background);

	image_impose(frame,image_valve);

	frame = image_add_string(frame,str_valve,203,32);

	gtk_image_set_from_pixbuf(show_state->image_pipe,frame);

	show_state->frame_pipe = frame;
 	return SUCCESS;
}

static flag_t show_fire_alarm(show_state_s * show_state,show_control_s * show_control
                          ,state_controller_s * controller_state,config_controller_s * controller_config
													,uint32_t timeout)
{
 	uint32_t counter;
	flag_t fire = controller_state_fire_alarm(controller_state);
	GdkPixbuf * buf;

	if(fire == STATE_FIRE_ALARM_OFF){
		buf = images_state[FIRE_ALARM_OFF];
	}
	else{
 		counter = show_state->counter_fire_alarm;
		counter += timeout;
 		if((counter/MILLISECOND_PER_SECOND) % 2){
			buf = images_state[FIRE_ALARM_ON_0];
		}
		else{
			buf = images_state[FIRE_ALARM_ON_1];
		}
 		show_state->counter_fire_alarm = counter;
	}
	gtk_image_set_from_pixbuf(show_state->image_fire_alarm,buf);

	return SUCCESS;
}

static int show_block_controller(gpointer data)
{
	block_controller_s * bc = (block_controller_s *)data;
	controller_s * controller = bc->current;
	state_controller_s state;

	if(bc->stop_show == OK ){
		bc->run_show = NOT_OK;
		return FALSE; /*завершить работу*/
	}
	if(controller == NULL){
 		/*контролер не выбран*/
		bc->run_show = NOT_OK;
		return FALSE;
	}

	g_mutex_lock(&(controller->control->mutex));
	controller_copy_state(&state,controller->state);
	g_mutex_unlock(&(controller->control->mutex));

	show_console(bc->state,bc->control,&state,controller->config);
	show_message(bc->state,bc->control,&state,controller->config,bc->timeout_show);
	show_vertical(bc->state,bc->control,&state,controller->config);
	show_horizontal(bc->state,bc->control,&state,controller->config);
	show_pipe(bc->state,bc->control,&state,controller->config);
	show_fire_alarm(bc->state,bc->control,&state,controller->config,bc->timeout_show);

	return TRUE; /* продолжаем работу */
}

/*****************************************************************************/
/*                                                                           */
/*     Отображение состояние контроллера                                     */
/*                                                                           */
/*****************************************************************************/

static GtkWidget * create_block_state_message(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * frame_mode;
	GtkWidget * lab_mode;
	GtkWidget * frame_state;
	GtkWidget * box_state;
	GtkWidget * lab_static;
	GtkWidget * lab_state;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,TRUE);

	frame_mode = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame_mode),GTK_SHADOW_NONE);
	gtk_widget_set_size_request(frame_mode,-1,40);

	lab_mode = gtk_label_new(STR_INFO_MODE_WAIT);
	layout_widget(lab_mode,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(lab_mode,300,-1);
	bc->state->lab_mode = GTK_LABEL(lab_mode);

	frame_state = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame_state),GTK_SHADOW_NONE);
	gtk_widget_set_size_request(frame_state,-1,40);

	box_state = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

	lab_static = gtk_label_new("Cостояние установки :");

	lab_state = gtk_label_new(STR_INFO_STATE_WAIT);
	layout_widget(lab_state,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(lab_state,300,-1);
	bc->state->lab_state = GTK_LABEL(lab_state);

	gtk_box_pack_start(GTK_BOX(box),frame_mode,TRUE,TRUE,20);
	gtk_box_pack_start(GTK_BOX(box),frame_state,TRUE,TRUE,20);

	gtk_container_add(GTK_CONTAINER(frame_mode),lab_mode);
	gtk_container_add(GTK_CONTAINER(frame_state),box_state);

	gtk_box_pack_start(GTK_BOX(box_state),lab_static,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box_state),lab_state,TRUE,TRUE,0);

	gtk_widget_show(box);
	gtk_widget_show(frame_mode);
	gtk_widget_show(lab_mode);
	gtk_widget_show(frame_state);
	gtk_widget_show(box_state);
 	gtk_widget_show(lab_static);
	gtk_widget_show(lab_state);

	apply_style_message_norm(frame_mode,NULL);
	apply_style_message_norm(frame_state,NULL);
	apply_style_message_alarm(lab_mode,NULL);
	apply_style_message_norm(lab_static,NULL);
	apply_style_message_alarm(lab_state,NULL);
 	return box;
}

#define DEFAULT_SIZE_WIDTH_AXIS_VERTICAL    300
#define DEFAULT_SIZE_HEIGHT_AXIS_VERTICAL   300

static GtkWidget * create_block_state_vertical(show_state_s * state)
{
	/*GtkWidget * frame;*/
	GtkWidget * image;
	GdkPixbuf * buf;

	/*frame = gtk_frame_new(NULL);*/
	/*layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);*/

	/*TODO маштабирование */
	buf = get_resource_image(RESOURCE_IMAGE,"vertical_background");
	state->frame_vertical = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_AXIS_VERTICAL,DEFAULT_SIZE_HEIGHT_AXIS_VERTICAL);*/
	state->image_vertical = GTK_IMAGE(image);

	/*gtk_container_add(GTK_CONTAINER(frame),image);*/

	/*gtk_widget_show(frame);*/
	gtk_widget_show(image);

	/*return frame;*/
	return image;
}

#define DEFAULT_SIZE_WIDTH_AXIS_HORIZONTAL    300
#define DEFAULT_SIZE_HEIGHT_AXIS_HORIZONTAL   300
static GtkWidget * create_block_state_horizontal(show_state_s * state)
{
	/*GtkWidget * frame;*/
	GtkWidget * image;
	GdkPixbuf * buf;

	/*frame = gtk_frame_new(NULL);*/
	/*layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);*/

	buf = get_resource_image(RESOURCE_IMAGE,"horizontal_background");
	state->frame_horizontal = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование*/
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_AXIS_HORIZONTAL,DEFAULT_SIZE_HEIGHT_AXIS_HORIZONTAL);*/
	state->image_horizontal = GTK_IMAGE(image);

	/*gtk_container_add(GTK_CONTAINER(frame),image);*/

	/*gtk_widget_show(frame);*/
	gtk_widget_show(image);

	/*return frame;*/
	return image;
}

static GtkWidget * create_block_state_lafet(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_vertical;
	GtkWidget * block_horizontal;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

	block_vertical = create_block_state_vertical(bc->state);
	block_horizontal = create_block_state_horizontal(bc->state);

	gtk_box_pack_start(GTK_BOX(box),block_vertical,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_horizontal,TRUE,TRUE,0);

	gtk_widget_show(box);
	return box;
}

static GtkWidget * create_block_state_pipe(block_controller_s * bc)
{
	/*GtkWidget * frame;*/
	GtkWidget * image;
	GdkPixbuf * buf;

	/*frame = gtk_frame_new(NULL);*/
	/*layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);*/

	buf = get_resource_image(RESOURCE_IMAGE,"pipe_background");
	bc->state->frame_pipe = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование*/
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_PRESSURE_VALVE,DEFAULT_SIZE_HEIGHT_PRESSURE_VALVE);*/
	bc->state->image_pipe = GTK_IMAGE(image);

	/*gtk_container_add(GTK_CONTAINER(frame),image);*/

	/*gtk_widget_show(frame);*/
	gtk_widget_show(image);

	/*return frame;*/
	return image;
}

static GtkWidget * create_block_state_fire(block_controller_s * bc)
{
	GtkWidget * fire;
	GdkPixbuf * buf;

	buf = images_state[FIRE_ALARM_OFF];
	bc->state->frame_fire_alarm = buf;
	fire = gtk_image_new_from_pixbuf(buf);
	layout_widget(fire,GTK_ALIGN_END,GTK_ALIGN_START,FALSE,FALSE);
	bc->state->image_fire_alarm = GTK_IMAGE(fire);
	gtk_widget_show(fire);
	return fire;
}

static GtkWidget * create_block_state_info(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_pipe;
	GtkWidget * block_fire;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,FALSE);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	block_pipe = create_block_state_pipe(bc);
	block_fire = create_block_state_fire(bc);

	gtk_box_pack_start(GTK_BOX(box),block_pipe,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_fire,TRUE,TRUE,0);

	gtk_widget_show(box);
	return box;
}
static GtkWidget * create_block_state_left(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_state_message;
	GtkWidget * block_state_lafet;
	GtkWidget * block_state_info;


	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_state_message = create_block_state_message(bc);
	block_state_lafet = create_block_state_lafet(bc);
	block_state_info = create_block_state_info(bc);

	gtk_box_pack_start(GTK_BOX(box),block_state_message,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_state_lafet,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_state_info,TRUE,TRUE,0);

	gtk_widget_show(box);

	return box;
}

static GtkWidget * create_block_state_video(block_controller_s * bc)
{
	/*GtkWidget * frame;*/
	GtkWidget * image;
	GdkPixbuf * buf;

	/*frame = gtk_frame_new(NULL);*/
	/*layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);*/

	buf = get_resource_image(RESOURCE_IMAGE,"state_video_background");
	bc->state->frame_video = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование*/
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_PRESSURE_VALVE,DEFAULT_SIZE_HEIGHT_PRESSURE_VALVE);*/
	bc->state->image_video = GTK_IMAGE(image);

	/*gtk_container_add(GTK_CONTAINER(frame),image);*/

	/*gtk_widget_show(frame);*/
	gtk_widget_show(image);

	/*return frame;*/
	return image;
}

static void button_clicked_set_auto_work(GtkButton * b,gpointer ud)
{
	g_info("настройка автоматической работы");
}

static GtkWidget * create_block_control_auto_work(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * label;
	GtkWidget * but;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label = gtk_label_new("Сектор работы");
	layout_widget(label,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,TRUE);
	bc->state->lab_auto_work = GTK_LABEL(label);

	but = gtk_button_new_with_label("Настроить");
	layout_widget(but,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);
	g_signal_connect(but,"clicked",G_CALLBACK(button_clicked_set_auto_work),bc);

	gtk_box_pack_start(GTK_BOX(box),label,TRUE,TRUE,30);
	gtk_box_pack_start(GTK_BOX(box),but,TRUE,TRUE,10);

	gtk_widget_show(box);
	gtk_widget_show(label);
	gtk_widget_show(but);

	return box;
}

static GtkWidget * create_block_state_auto_work(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_control_auto_work;
	GtkWidget * image;
	GdkPixbuf * buf;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_control_auto_work = create_block_control_auto_work(bc);

	buf = get_resource_image(RESOURCE_IMAGE,"auto_work_background");
	bc->state->frame_auto_work = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование*/
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_PRESSURE_VALVE,DEFAULT_SIZE_HEIGHT_PRESSURE_VALVE);*/
	bc->state->image_auto_work = GTK_IMAGE(image);

	gtk_box_pack_start(GTK_BOX(box),block_control_auto_work,TRUE,TRUE,8);
	gtk_box_pack_start(GTK_BOX(box),image,TRUE,TRUE,0);
	gtk_widget_show(box);
	gtk_widget_show(image);

	return box;
}

static GtkWidget * create_block_state_right(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_state_video;
	GtkWidget * s_0;
	GtkWidget * block_state_auto_work;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	block_state_auto_work = create_block_state_auto_work(bc);

	s_0 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	layout_widget(s_0,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(s_0,300,-1);
	block_state_video = create_block_state_video(bc);

	gtk_box_pack_start(GTK_BOX(box),block_state_auto_work,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),s_0,FALSE,FALSE,50);
	gtk_box_pack_start(GTK_BOX(box),block_state_video,FALSE,TRUE,0);

	gtk_widget_show(box);
	gtk_widget_show(s_0);
	return box;
}

static GtkWidget * create_block_state(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_state_left;
	GtkWidget * block_state_right;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	block_state_left = create_block_state_left(bc);
	block_state_right = create_block_state_right(bc);

	gtk_box_pack_start(GTK_BOX(box),block_state_left,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_state_right,TRUE,TRUE,0);

	gtk_widget_show(box);

	return box;
}

/*****************************************************************************/
/*                                                                           */
/*     Функции отображения системы управления                                */
/*                                                                           */
/*****************************************************************************/

static flag_t push_command_queue(controller_s * controller,command_u command,flag_t rewrite)
{
	control_controller_s * control = controller->control;

	g_mutex_lock(&(control->mutex));
	if(control->thread == NULL){
		g_mutex_unlock(&(control->mutex));
		return FAILURE;
	}

	/*g_info("command : %d",control->command.part.value);*/
	if(control->command.part.value == COMMAND_EMPTY){
		control->command.all = command.all;
		/*g_info("command write");*/
	}
	else{
		if(rewrite == OK){
			control->command.all = command.all;
			/*g_info("command write");*/
		}
	}
	g_mutex_unlock(&(control->mutex));

	return SUCCESS;
}

static flag_t set_button_not_active(GtkButton * but)
{
	return SUCCESS;
}

static void button_clicked_control_mode(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};

	if(controller == NULL){
		g_info("Не выбран контролер");
		return ;
	}

	if(controller->type != TYPE_DEVICE_LSD ){
		g_info("Немогу перейти в ручной режим управления");
		return;
	}

	if(bc->control->mode == STATE_MODE_AUTO){
		command.part.value = COMMAND_MODE_MANUAL;
		push_command_queue(controller,command,NOT_OK);
	}
	else{
		command.part.value = COMMAND_MODE_AUTO;
		push_command_queue(controller,command,NOT_OK);
	}
}

static flag_t init_image_buton_mode(block_controller_s * bc)
{
	GtkWidget * image;
	GdkPixbuf * buf;

	buf = get_resource_image(RESOURCE_STYLE,"button-mode-auto");
	image = gtk_image_new_from_pixbuf(buf);
	bc->control->image_mode_auto = image;

	buf = get_resource_image(RESOURCE_STYLE,"button-mode-manual");
	image = gtk_image_new_from_pixbuf(buf);
	bc->control->image_mode_manual = image;

	return SUCCESS;
}

static GtkWidget * create_block_control_mode(block_controller_s * bc)
{
	GtkWidget * box;
	/*GtkWidget * label;*/
	GtkWidget * but;

	bc->control->mode = STATE_MODE_AUTO;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_CENTER,GTK_ALIGN_START,FALSE,FALSE);

	/*label = gtk_label_new("Режим управления");*/
	/*layout_widget(label,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);*/

	init_image_buton_mode(bc);
	but = gtk_button_new();
	layout_widget(but,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_button_set_image(GTK_BUTTON(but),bc->control->image_mode_auto);
	g_signal_connect(but,"clicked",G_CALLBACK(button_clicked_control_mode),bc);
	bc->control->but_mode = GTK_BUTTON(but);

	/*gtk_box_pack_start(GTK_BOX(box),label,TRUE,TRUE,5);*/
	gtk_box_pack_start(GTK_BOX(box),but,TRUE,TRUE,0);

	gtk_widget_show(box);
	/*gtk_widget_show(label);*/
	gtk_widget_show(but);
	return box;
}

static void button_clicked_lafet_precise(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s *)ud;
	flag_t state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
	if(state){
		bc->control->lafet_precise = OK;
	}
	else{
		bc->control->lafet_precise = NOT_OK;
	}
}

static void entry_activate_lafet_precise(GtkEntry * e,gpointer ud)
{
	/*block_controller_s * bc = (block_controller_s *)ud;*/
	/*GtkEntryBuffer * buf = gtk_entry_get_buffer(e);*/
}

#if 0
static void entry_backspace_lafet_precise(GtkEntry * e,gpointer ud)
{
	/*block_controller_s * bc = (block_controller_s *)ud;*/
	GtkEntryBuffer * buf = gtk_entry_get_buffer(e);
	g_info("%s",gtk_entry_buffer_get_text(buf));
}
static void entry_delete_from_cursor_lafet_precise(GtkEntry *e,GtkDeleteType type
                                                  ,gint count,gpointer ud)
{
	/*block_controller_s * bc = (block_controller_s *)ud;*/
	GtkEntryBuffer * buf = gtk_entry_get_buffer(e);
	g_info("%s",gtk_entry_buffer_get_text(buf));
}
static void entry_insert_at_cursor_lafet_precise(GtkEntry * e,gchar * str,gpointer ud)
{
	/*block_controller_s * bc = (block_controller_s *)ud;*/
	GtkEntryBuffer * buf = gtk_entry_get_buffer(e);
	g_info("%s",gtk_entry_buffer_get_text(buf));
}
static void entry_toggle_overwrite_lafet_precise(GtkEntry * e,gpointer ud)
{
	/*block_controller_s * bc = (block_controller_s *)ud;*/
	GtkEntryBuffer * buf = gtk_entry_get_buffer(e);
	g_info("%s",gtk_entry_buffer_get_text(buf));
}
static void entry_preedit_changed_lafet_precise(GtkEntry * e,gchar * str,gpointer ud)
{
	/*block_controller_s * bc = (block_controller_s *)ud;*/
	GtkEntryBuffer * buf = gtk_entry_get_buffer(e);
	g_info("%s",gtk_entry_buffer_get_text(buf));
}
#endif

static gboolean entry_key_press_event_lafet_precise(GtkEntry * e,GdkEvent *event,gpointer ud)
{
	/*block_controller_s * bc = (block_controller_s *)ud;*/
	/*GtkEntryBuffer * buf = gtk_entry_get_buffer(e);*/
	return FALSE;
}
static GtkWidget * create_block_lafet_precise(block_controller_s * bc)
{
	GtkWidget * grid;
	GtkWidget * label;
	GtkWidget * but_choise;
	GtkWidget * entry;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid),TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid),FALSE);
	gtk_grid_set_row_spacing(GTK_GRID(grid),2);

	label = gtk_label_new("Точное значение");

	but_choise = gtk_check_button_new();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(but_choise),FALSE);
	g_signal_connect(but_choise,"clicked",G_CALLBACK(button_clicked_lafet_precise),bc);
	bc->control->lafet_precise = NOT_OK;

	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry),"000");
	gtk_entry_set_max_length(GTK_ENTRY(entry),3);
	gtk_entry_set_width_chars(GTK_ENTRY(entry),3);
	gtk_entry_set_max_width_chars(GTK_ENTRY(entry),3);
	gtk_entry_set_input_purpose(GTK_ENTRY(entry),GTK_INPUT_PURPOSE_DIGITS);

	g_signal_connect(entry,"activate",G_CALLBACK(entry_activate_lafet_precise),bc);
#if 0
	g_signal_connect(entry,"backspace",G_CALLBACK(entry_backspace_lafet_precise),bc);
	g_signal_connect(entry,"delete-from-cursor",G_CALLBACK(entry_delete_from_cursor_lafet_precise),bc);
	g_signal_connect(entry,"insert-at-cursor",G_CALLBACK(entry_insert_at_cursor_lafet_precise),bc);
	g_signal_connect(entry,"toggle-overwrite",G_CALLBACK(entry_toggle_overwrite_lafet_precise),bc);
	g_signal_connect(entry,"preedit-changed",G_CALLBACK(entry_preedit_changed_lafet_precise),bc);
#endif
	g_signal_connect(entry,"key-press-event",G_CALLBACK(entry_key_press_event_lafet_precise),bc);

	gtk_grid_attach(GTK_GRID(grid),label     ,0,0,2,1);
	gtk_grid_attach(GTK_GRID(grid),but_choise,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),entry     ,1,1,1,1);

	gtk_widget_show(grid);
	gtk_widget_show(label);
	gtk_widget_show(but_choise);
	gtk_widget_show(entry);

	return grid;
}

static gboolean button_press_event_lafet_up(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_LAFET_UP;
	push_command_queue(controller,command,NOT_OK);
	g_info("Команда \"ВВЕРХ\"");
	return FALSE;
}
static gboolean button_press_event_lafet_bottom(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_LAFET_DOWN;
	push_command_queue(controller,command,NOT_OK);
	g_info("Команда \"ВНИЗ\"");
	return FALSE;
}
static gboolean button_press_event_lafet_right(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_LAFET_RIGHT;
	push_command_queue(controller,command,NOT_OK);
	g_info("Команда \"ВПРАВО\"");
	return FALSE;
}
static gboolean button_press_event_lafet_left(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_LAFET_LEFT;
	push_command_queue(controller,command,NOT_OK);
	g_info("Команда \"ВЛЕВО\"");
	return FALSE;
}
static gboolean button_release_event_lafet_stop(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_LAFET_STOP;
	push_command_queue(controller,command,OK);
	g_info("Команда \"СТОП\"");
	return FALSE;
}
#define BUTTON_WIDTH      70
#define BUTTON_HEIGHT     30

static GtkWidget * create_block_control_lafet(block_controller_s * bc)
{
#define BUTTON_LAFET_IMAGE    0
#if BUTTON_LAFET_IMAGE
	GdkPixbuf * buf;
	GtkWidget * img;
#endif
	GtkWidget * grid;
	GtkWidget * but_up;
	GtkWidget * but_bottom;
	GtkWidget * but_right;
	GtkWidget * but_left;
	GtkWidget * block_precise;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_CENTER,GTK_ALIGN_START,FALSE,FALSE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid),FALSE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid),FALSE);
	gtk_grid_set_row_spacing(GTK_GRID(grid),0);
	gtk_grid_set_column_spacing(GTK_GRID(grid),0);

#if BUTTON_LAFET_IMAGE
	buf = get_resource_image(RESOURCE_STYLE,"button-up");
	img = gtk_image_new_from_pixbuf(buf);
	but_up = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(but_up),img);
#else
	but_up = gtk_button_new_with_label("ВВЕРХ");
#endif
	layout_widget(but_up,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_up,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_up,"button-press-event",G_CALLBACK(button_press_event_lafet_up),bc);
	g_signal_connect(but_up,"button-release-event",G_CALLBACK(button_release_event_lafet_stop),bc);
	bc->control->but_up = GTK_BUTTON(but_up);

#if BUTTON_LAFET_IMAGE
	buf = get_resource_image(RESOURCE_STYLE,"button-bottom");
	img = gtk_image_new_from_pixbuf(buf);
	but_bottom = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(but_bottom),img);
#else
	but_bottom = gtk_button_new_with_label("ВНИЗ");
#endif
	layout_widget(but_bottom,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_bottom,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_bottom,"button-press-event",G_CALLBACK(button_press_event_lafet_bottom),bc);
	g_signal_connect(but_bottom,"button-release-event",G_CALLBACK(button_release_event_lafet_stop),bc);
	bc->control->but_bottom = GTK_BUTTON(but_bottom);

#if BUTTON_LAFET_IMAGE
	buf = get_resource_image(RESOURCE_STYLE,"button-right");
	img = gtk_image_new_from_pixbuf(buf);
	but_right = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(but_right),img);
#else
	but_right = gtk_button_new_with_label("ВПРАВО");
#endif
	layout_widget(but_right,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_right,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_right,"button-press-event",G_CALLBACK(button_press_event_lafet_right),bc);
	g_signal_connect(but_right,"button-release-event",G_CALLBACK(button_release_event_lafet_stop),bc);
	bc->control->but_right = GTK_BUTTON(but_right);

#if BUTTON_LAFET_IMAGE
	buf = get_resource_image(RESOURCE_STYLE,"button-left");
	img = gtk_image_new_from_pixbuf(buf);
	but_left = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(but_left),img);
#else
	but_left = gtk_button_new_with_label("ВЛЕВО");
#endif
	layout_widget(but_left,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_left,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_left,"button-press-event",G_CALLBACK(button_press_event_lafet_left),bc);
	g_signal_connect(but_left,"button-release-event",G_CALLBACK(button_release_event_lafet_stop),bc);
	bc->control->but_left = GTK_BUTTON(but_left);

	block_precise = create_block_lafet_precise(bc);

	gtk_grid_attach(GTK_GRID(grid),but_up       ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_bottom   ,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_left     ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_right    ,2,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_precise,0,2,3,1);

	gtk_widget_show(grid);
	gtk_widget_show(but_up);
	gtk_widget_show(but_bottom);
	gtk_widget_show(but_right);
	gtk_widget_show(but_left);

 	return grid;
}

static gboolean button_press_event_actuator_spray_less(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	uint64_t flag = controller->config->flag;
	if(!ACTUATOR_SPRAY(flag)){
		g_info("Нет привода актуатора распыл");
		return TRUE;
	}
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_SPRAY_LESS;
	push_command_queue(controller,command,NOT_OK);
	g_info("Команда \"Уже\"");
	return FALSE;
}
static gboolean button_press_event_actuator_spray_more(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	uint64_t flag = controller->config->flag;
	if(!ACTUATOR_SPRAY(flag)){
		g_info("Нет привода актуатора распыл");
		return TRUE;
	}
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_SPRAY_MORE;
	push_command_queue(controller,command,NOT_OK);
	g_info("Команда \"Шире\"");
	return FALSE;
}
static gboolean button_press_event_actuator_rate_less(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	uint64_t flag = controller->config->flag;
	if(!ACTUATOR_RATE(flag)){
		g_info("Нет привода актуатора литраж");
		return TRUE;
	}
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_RATE_LESS;
	push_command_queue(controller,command,NOT_OK);
	g_info("Команда \"Меньше\"");
	return FALSE;
}
static gboolean button_press_event_actuator_rate_more(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	uint64_t flag = controller->config->flag;
	if(!ACTUATOR_RATE(flag)){
		g_info("Нет привода актуатора литраж");
		return TRUE;
	}
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_RATE_MORE;
	push_command_queue(controller,command,NOT_OK);
	g_info("Команда \"Больше\"");
	return FALSE;
}
static gboolean button_release_event_actuator_stop(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}
	command.part.value = COMMAND_ACTUATOT_STOP;
	push_command_queue(controller,command,OK);
	g_info("Команда \"Стоп\"");
	return FALSE;
}

static GtkWidget * create_block_actuator(block_controller_s * bc)
{
	GdkPixbuf * buf;
	GtkWidget * box;

	GtkWidget * box_spray;
	GtkWidget * label_spray;
	GtkWidget * but_spray_less;
	GtkWidget * img_separator_spray;
	GtkWidget * but_spray_more;

	GtkWidget * box_rate;
	GtkWidget * label_rate;
	GtkWidget * img_separator_rate;
	GtkWidget * but_rate_less;
	GtkWidget * but_rate_more;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_CENTER,GTK_ALIGN_START,FALSE,FALSE);
	gtk_box_set_homogeneous(GTK_BOX(box),TRUE);

	box_spray = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box_spray,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	bc->control->actuator_spray = box_spray;

	label_spray = gtk_label_new("Распыл");
	layout_widget(label_spray,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_margin_start(label_spray,10);
	gtk_widget_set_margin_end(label_spray,10);

	but_spray_less = gtk_button_new_with_label("Уже");
	layout_widget(but_spray_less,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_spray_less,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_spray_less,"button-press-event",G_CALLBACK(button_press_event_actuator_spray_less),bc);
	g_signal_connect(but_spray_less,"button-release-event",G_CALLBACK(button_release_event_actuator_stop),bc);

	buf = get_resource_image(RESOURCE_STYLE,"actuator-separator-spray");
	img_separator_spray = gtk_image_new_from_pixbuf(buf);

	but_spray_more = gtk_button_new_with_label("Шире");
	layout_widget(but_spray_more,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_spray_more,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_spray_more,"button-press-event",G_CALLBACK(button_press_event_actuator_spray_more),bc);
	g_signal_connect(but_spray_more,"button-release-event",G_CALLBACK(button_release_event_actuator_stop),bc);

	box_rate = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box_rate,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	bc->control->actuator_rate = box_rate;

	label_rate = gtk_label_new("Литраж");
	layout_widget(label_rate,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	but_rate_more = gtk_button_new_with_label("Больше");
	layout_widget(but_rate_more,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_spray_more,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_rate_more,"button-press-event",G_CALLBACK(button_press_event_actuator_rate_more),bc);
	g_signal_connect(but_rate_more,"button-release-event",G_CALLBACK(button_release_event_actuator_stop),bc);

	buf = get_resource_image(RESOURCE_STYLE,"actuator-separator-rate");
	img_separator_rate = gtk_image_new_from_pixbuf(buf);

	but_rate_less = gtk_button_new_with_label("Меньше");
	layout_widget(but_rate_less,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_spray_less,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_rate_less,"button-press-event",G_CALLBACK(button_press_event_actuator_rate_less),bc);
	g_signal_connect(but_rate_less,"button-release-event",G_CALLBACK(button_release_event_actuator_stop),bc);

	gtk_box_pack_start(GTK_BOX(box),box_spray,TRUE,TRUE,20);
	gtk_box_pack_start(GTK_BOX(box),box_rate ,TRUE,TRUE,20);

	gtk_box_pack_start(GTK_BOX(box_spray),label_spray,FALSE,FALSE,10);
	gtk_box_pack_start(GTK_BOX(box_spray),but_spray_less,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(box_spray),img_separator_spray,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(box_spray),but_spray_more,FALSE,FALSE,0);

	gtk_box_pack_start(GTK_BOX(box_rate),label_rate         ,FALSE,FALSE,10);
	gtk_box_pack_start(GTK_BOX(box_rate),but_rate_more      ,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(box_rate),img_separator_rate ,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(box_rate),but_rate_less      ,FALSE,FALSE,0);

	gtk_widget_show(box);
	gtk_widget_show(box_spray);
	gtk_widget_show(label_spray);
	gtk_widget_show(img_separator_spray);
	gtk_widget_show(but_spray_less);
	gtk_widget_show(but_spray_more);
	gtk_widget_show(box_rate);
	gtk_widget_show(label_rate);
	gtk_widget_show(img_separator_rate);
	gtk_widget_show(but_rate_less);
	gtk_widget_show(but_rate_more);

 	return box;
}

static void button_clicked_valve_open(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};

	uint64_t flag = controller->config->flag;

	if(!VALVE_DRY(flag)){
		g_info("Нет зажвижки");
		return ;
	}
	if( controller == NULL){
		g_info("Не выбран контролер");
		return ;
	}
	command.part.value = COMMAND_VALVE_OPEN;
	push_command_queue(controller,command,NOT_OK);
}

static void button_clicked_valve_close(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};

	uint64_t flag = controller->config->flag;
	if(!VALVE_DRY(flag)){
		g_info("Нет зажвижки");
		return ;
	}
	if( controller == NULL){
		g_info("Не выбран контролер");
		return ;
	}
	command.part.value = COMMAND_VALVE_CLOSE;
	push_command_queue(controller,command,NOT_OK);
}

static void button_clicked_valve_run(GtkButton * b,gpointer ud)
{
	/*block_controller_s * bc = (block_controller_s*)ud;*/
	g_info("Запустить задвижку");
}
#define RATE_VALVE_ANALOG     4

static gdouble min_valve = 0;
static gdouble max_valve = 100;
static gdouble step_valve = 0.1;
static gboolean scale_change_value_valve(GtkRange * r,GtkScrollType s,gdouble v,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	gdouble percent;
	uint16_t precise;

	if(v > max_valve ){
		v = max_valve;
	}
	percent = v * 10;
	precise = percent * RATE_VALVE_ANALOG;

	/*g_info("Задвижка %e : %d",v,precise);*/
	bc->control->valve_precise = precise;

	gtk_range_set_value(r,v);
 	return TRUE;
}
static GtkWidget * create_block_control_valve(block_controller_s * bc)
{
	GtkWidget * grid;
	GtkWidget * label;
	GtkWidget * but_open;
	GtkWidget * but_close;
	GtkWidget * but_run;
	GtkWidget * scale;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid),FALSE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
	gtk_grid_set_row_spacing(GTK_GRID(grid),5);
	gtk_grid_set_column_spacing(GTK_GRID(grid),10);

	label = gtk_label_new("Задвижка");

	but_open = gtk_button_new_with_label("Открыть");
	layout_widget(but_open,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_open,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_open,"clicked",G_CALLBACK(button_clicked_valve_open),bc);
	bc->control->but_valve_open = GTK_BUTTON(but_open);

	but_close = gtk_button_new_with_label("Закрыть");
	layout_widget(but_close,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_close,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_close,"clicked",G_CALLBACK(button_clicked_valve_close),bc);
	bc->control->but_valve_close = GTK_BUTTON(but_close);

	but_run = gtk_button_new_with_label("Пуск");
	layout_widget(but_run,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_widget_set_size_request(but_run,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_run,"clicked",G_CALLBACK(button_clicked_valve_run),bc);

	scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,min_valve,max_valve,step_valve);
	layout_widget(scale,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_scale_set_digits(GTK_SCALE(scale),1); /*колличество знаков после запятой*/
	gtk_widget_set_size_request(scale,100,30);
	gtk_range_set_min_slider_size(GTK_RANGE(scale),20);
	/*gtk_scale_set_value_pos(GTK_SCALE(scale),GTK_POS_RIGHT);*/
	g_signal_connect(scale,"change-value",G_CALLBACK(scale_change_value_valve),bc);

	gtk_grid_attach(GTK_GRID(grid),label    ,0,0,2,1);
	gtk_grid_attach(GTK_GRID(grid),but_open ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_close,0,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_run  ,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),scale    ,1,2,1,1);

	gtk_widget_show(grid);
	gtk_widget_show(label);
	gtk_widget_show(but_open);
	gtk_widget_show(but_close);
	gtk_widget_show(but_run);
	gtk_widget_show(scale);

	return grid;
}

static void button_clicked_oscillation_run(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command ={0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	if(bc->control->oscillation_flag == COMMAND_OSCILLATION_STOP){
		command.part.value = bc->control->oscillation_command.part.value;
		bc->control->oscillation_flag = COMMAND_OSCILLATION_RUN;
		push_command_queue(controller,command,NOT_OK);
	}
}
static void button_clicked_oscillation_stop(GtkButton * b,gpointer ud)
{
 	block_controller_s * bc = (block_controller_s*)ud;
	controller_s * controller = bc->current;
	command_u command = {0};
	if( controller == NULL){
	 	g_info("Не выбран контролер");
		return;
	}
	bc->control->oscillation_flag = COMMAND_OSCILLATION_STOP;
	command.part.value = COMMAND_OSCILLATION_STOP;
	push_command_queue(controller,command,OK);
}
static void button_clicked_oscillation_vertical(GtkToggleButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	flag_t state = gtk_toggle_button_get_active(b);
	GtkToggleButton * set;

	if(state){
		set = bc->control->but_oscillation_horizontal;
		gtk_toggle_button_set_active(set,FALSE);
		set = bc->control->but_oscillation_saw;
		gtk_toggle_button_set_active(set,FALSE);
		set = bc->control->but_oscillation_step;
		gtk_toggle_button_set_active(set,FALSE);
		bc->control->oscillation_command.part.value = COMMAND_OSCILLATION_VERTICAL;
	}
}
static void button_clicked_oscillation_horizontal(GtkToggleButton * b,gpointer ud)
{
 	block_controller_s * bc = (block_controller_s*)ud;
	flag_t state = gtk_toggle_button_get_active(b);
	GtkToggleButton * set;

	if(state){
		set = bc->control->but_oscillation_vertical;
		gtk_toggle_button_set_active(set,FALSE);
		set = bc->control->but_oscillation_saw;
		gtk_toggle_button_set_active(set,FALSE);
		set = bc->control->but_oscillation_step;
		gtk_toggle_button_set_active(set,FALSE);
		bc->control->oscillation_command.part.value = COMMAND_OSCILLATION_HORIZONTAL;
	}
}

static void button_clicked_oscillation_saw(GtkToggleButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	flag_t state = gtk_toggle_button_get_active(b);
	GtkToggleButton * set;

	if(state){
		set = bc->control->but_oscillation_vertical;
		gtk_toggle_button_set_active(set,FALSE);
		set = bc->control->but_oscillation_horizontal;
		gtk_toggle_button_set_active(set,FALSE);
		set = bc->control->but_oscillation_step;
		gtk_toggle_button_set_active(set,FALSE);
		bc->control->oscillation_command.part.value = COMMAND_OSCILLATION_SAW;
	}
}
static void button_clicked_oscillation_step(GtkToggleButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	flag_t state = gtk_toggle_button_get_active(b);
	GtkToggleButton * set;

	if(state){
		set = bc->control->but_oscillation_vertical;
		gtk_toggle_button_set_active(set,FALSE);
		set = bc->control->but_oscillation_horizontal;
		gtk_toggle_button_set_active(set,FALSE);
		set = bc->control->but_oscillation_saw;
		gtk_toggle_button_set_active(set,FALSE);
		bc->control->oscillation_command.part.value = COMMAND_OSCILLATION_STEP;
	}
}

static GtkWidget * create_block_oscillation(block_controller_s * bc)
{
	GtkWidget * grid;
	GtkWidget * label;
	GtkWidget * but_run;
	GtkWidget * but_stop;

	GtkWidget * but_vertical;
	GtkWidget * but_horizontal;
	GtkWidget * but_saw;
	GtkWidget * but_step;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_grid_set_row_spacing(GTK_GRID(grid),5);
	gtk_grid_set_column_spacing(GTK_GRID(grid),10);

	label = gtk_label_new("Осциляция");

	bc->control->oscillation_command.part.value = COMMAND_OSCILLATION_STOP;
	bc->control->oscillation_flag = COMMAND_OSCILLATION_STOP;

	but_run = gtk_button_new_with_label("Запустить");
	gtk_widget_set_size_request(but_run,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_run,"clicked",G_CALLBACK(button_clicked_oscillation_run),bc);

	but_stop = gtk_button_new_with_label("Остановить");
	gtk_widget_set_size_request(but_stop,BUTTON_WIDTH,BUTTON_HEIGHT);
	g_signal_connect(but_stop,"clicked",G_CALLBACK(button_clicked_oscillation_stop),bc);

	but_vertical = gtk_check_button_new_with_label("Вертикальная");
	g_signal_connect(but_vertical,"clicked",G_CALLBACK(button_clicked_oscillation_vertical),bc);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(but_vertical),FALSE);
	bc->control->but_oscillation_vertical = GTK_TOGGLE_BUTTON(but_vertical);

	but_horizontal = gtk_check_button_new_with_label("Горизонтальная");
	g_signal_connect(but_horizontal,"clicked",G_CALLBACK(button_clicked_oscillation_horizontal),bc);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(but_vertical),FALSE);
	bc->control->but_oscillation_horizontal = GTK_TOGGLE_BUTTON(but_horizontal);

	but_saw = gtk_check_button_new_with_label("Пила");
	g_signal_connect(but_saw,"clicked",G_CALLBACK(button_clicked_oscillation_saw),bc);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(but_saw),FALSE);
	bc->control->but_oscillation_saw = GTK_TOGGLE_BUTTON(but_saw);

	but_step = gtk_check_button_new_with_label("Ступенька");
	g_signal_connect(but_step,"clicked",G_CALLBACK(button_clicked_oscillation_step),bc);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(but_step),FALSE);
	bc->control->but_oscillation_step = GTK_TOGGLE_BUTTON(but_step);

	gtk_grid_attach(GTK_GRID(grid),label         ,0,0,2,1);
	gtk_grid_attach(GTK_GRID(grid),but_run       ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_stop      ,1,1,1,1);

	gtk_grid_attach(GTK_GRID(grid),but_vertical  ,0,2,2,1);
	gtk_grid_attach(GTK_GRID(grid),but_horizontal,0,3,2,1);
	gtk_grid_attach(GTK_GRID(grid),but_saw       ,0,4,2,1);
	gtk_grid_attach(GTK_GRID(grid),but_step      ,0,5,2,1);

	gtk_widget_show(grid);
	gtk_widget_show(label);
	gtk_widget_show(but_run);
	gtk_widget_show(but_stop);
	gtk_widget_show(but_vertical);
	gtk_widget_show(but_horizontal);
	gtk_widget_show(but_saw);
	gtk_widget_show(but_step);

	return grid;
}

static GtkWidget * create_block_control(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_mode;
	GtkWidget * s_0;
	GtkWidget * s_1;
	GtkWidget * s_2;
	GtkWidget * s_3;
	GtkWidget * block_lafet;
	GtkWidget * block_actuator;
	GtkWidget * block_valve;
	GtkWidget * block_oscillation;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
  layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_set_size_request(box,DEFAULT_WIDTH_CONTROLLER_CONTROL,-1);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	block_mode = create_block_control_mode(bc);
	s_0 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_size_request(s_0,170,-1);
	layout_widget(s_0,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	block_lafet = create_block_control_lafet(bc);
	s_1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_size_request(s_1,150,-1);
	layout_widget(s_1,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	block_actuator = create_block_actuator(bc);
	s_2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_size_request(s_2,150,-1);
	layout_widget(s_2,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	block_valve = create_block_control_valve(bc);
	s_3 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_size_request(s_3,150,-1);
	layout_widget(s_3,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	block_oscillation = create_block_oscillation(bc);

	gtk_box_pack_start(GTK_BOX(box),block_mode,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(box),s_0,FALSE,FALSE,30);
	gtk_box_pack_start(GTK_BOX(box),block_lafet,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(box),s_1,FALSE,FALSE,10);
	gtk_box_pack_start(GTK_BOX(box),block_actuator,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(box),s_2,FALSE,FALSE,10);
	gtk_box_pack_start(GTK_BOX(box),block_valve,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(box),s_3,FALSE,FALSE,10);
	gtk_box_pack_start(GTK_BOX(box),block_oscillation,FALSE,FALSE,5);

	gtk_widget_show(box);
	gtk_widget_show(s_0);
	gtk_widget_show(s_1);
	gtk_widget_show(s_2);
	gtk_widget_show(s_3);

	return box;
}

/*****************************************************************************/
/*                                                                           */
/* Создание блока отображения свойств контроллера                            */
/*                                                                           */
/* Выбор контроллера                                                         */
/*                                                                           */
/*****************************************************************************/

static flag_t	changed_block_controller(block_controller_s * bc
                                      ,config_controller_s * controller_config
                                      ,state_controller_s * controller_state)
{
	show_control_s * control = bc->control;
	show_state_s * state = bc->state;
	uint64_t flag = controller_config->flag;

	if(!ENGINE_VERTICAL(flag)){
		set_button_not_active(control->but_up);
		set_button_not_active(control->but_bottom);
		image_copy(state->frame_vertical,images_state[VERTICAL_BACKGROUND]);
		gtk_image_set_from_pixbuf(state->image_vertical,state->frame_vertical);
	}
	else{
		/*TODO сделать функции активные, не активные кнопки */
	}

	if(!ENGINE_HORIZONTAL(flag)){
		set_button_not_active(control->but_left);
		set_button_not_active(control->but_right);
		image_copy(state->frame_horizontal,images_state[HORIZONTAL_BACKGROUND]);
		gtk_image_set_from_pixbuf(state->image_horizontal,state->frame_horizontal);
	}
	else{
	}

	if(!VALVE_DRY(flag)){
		/*set_button_not_active(control->but_valve_open);*/
		/*set_button_not_active(control->but_valve_close);*/
	}
	else{
	}
#if 0
	if(PRESSURE(flag)){
		gtk_widget_show(GTK_WIDGET(state->pressure));
	}
	else{
		gtk_widget_hide(GTK_WIDGET(state->pressure));
	}
	/*TODO сделать функции активные не активные */
	if(ACTUATOR_SPRAY(flag)){
		gtk_widget_show(control->actuator_spray);
	}
	else{
		gtk_widget_hide(control->actuator_spray);
	}
	if(ACTUATOR_RATE(flag)){
		gtk_widget_show(control->actuator_rate);
	}
	else{
		gtk_widget_hide(control->actuator_rate);
	}

	if(ACTUATOR_VEIL(flag)){
	}
	if(LIMIT_VERTICAL(flag)){
	}
	if(TIC_VERTICAL(flag)){
	}
	if(ENCODER_VERTICAL(flag)){
	}
	if(AMPERAGE_VERTICAL(flag)){
	}
	if(LIMIT_HORIZONTAL(flag)){
	}
	if(TIC_HORIZONTAL(flag)){
	}
	if(ENCODER_HORIZONTAL(flag)){
	}
	if(AMPERAGE_HORIZONTAL(flag)){
	}
	if(CONSOLE(flag)){
	}
	if(POST(flag)){
	}
	if(SENSOR_FIRE_DRY(flag)){
	}
	if(SENSOR_FIRE_485(flag)){
	}
	if(SENSOR_FIRE_ETHERNET(flag)){
	}
	if(SENSOR_DRY_485(flag)){
	}
	if(SENSOR_DRY_ETHERNET(flag)){
	}
	if(VALVE_ANALOG(flag)){
	}
	if(VALVE_LIMIT(flag)){
	}
	if(VALVE_FEEDBACK(flag)){
	}
	if(CAM_ANALOG_DC(flag)){
	}
	if(CAM_DIGITAL_DC(flag)){
	}
	if(CAM_DIGITAL_POE(flag)){
	}
	if(FIRE_ALARM_01(flag)){
	}
	if(FIRE_ALARM_02(flag)){
	}
	if(FIRE_ALARM_03(flag)){
	}
	if(FIRE_ALARM_04(flag)){
	}
	if(FIRE_ALARM_05(flag)){
	}
	if(FIRE_ALARM_06(flag)){
	}
	if(FIRE_ALARM_07(flag)){
	}
	if(FIRE_ALARM_08(flag)){
	}
	if(FIRE_ALARM_09(flag)){
	}
	if(FIRE_ALARM_10(flag)){
	}
	if(FIRE_ALARM_11(flag)){
	}
	if(FIRE_ALARM_12(flag)){
	}
	if(FIRE_ALARM_13(flag)){
	}
	if(FIRE_ALARM_14(flag)){
	}
	if(FIRE_ALARM_15(flag)){
	}
	if(FIRE_ALARM_16(flag)){
	}
	if(DEVICE_01_STATE_0(flag)){
	}
	if(DEVICE_01_STATE_1(flag)){
	}
	if(DEVICE_02_STATE_0(flag)){
	}
	if(DEVICE_02_STATE_1(flag)){
	}
	if(DEVICE_03_STATE_0(flag)){
	}
	if(DEVICE_03_STATE_1(flag)){
	}
	if(DEVICE_04_STATE_0(flag)){
	}
	if(DEVICE_04_STATE_1(flag)){
	}
	if(DEVICE_05_STATE_0(flag)){
	}
	if(DEVICE_05_STATE_1(flag)){
	}
	if(DEVICE_06_STATE_0(flag)){
	}
	if(DEVICE_06_STATE_1(flag)){
	}
	if(DEVICE_07_STATE_0(flag)){
	}
	if(DEVICE_07_STATE_1(flag)){
	}
	if(DEVICE_08_STATE_0(flag)){
	}
	if(DEVICE_08_STATE_1(flag)){
	}
#endif
	return SUCCESS;
}

/* 1 000 000  микросекунд == 1 секунда*/
#define DEFAULT_TIMEOUT_CURRENT        500000       /*1000000*/      /*   250000        250 милесекунд */
#define DEFAULT_TIMEOUT_ALL            3000000      /* 3 секунды */

static show_state_s show_state;
static show_control_s show_control;
static block_controller_s block_controller;

int select_block_controller(controller_s * controller)
{
	controller_s * old_controller;
	control_controller_s * control;
	GThread * thread;

	if(controller == NULL){
		block_controller.stop_show = OK;
		old_controller = block_controller.current;
		block_controller.current = NULL;
		if( old_controller ){
			control = old_controller->control;
			g_mutex_lock(&(control->mutex));
			control->timeout = DEFAULT_TIMEOUT_ALL;
			g_mutex_unlock(&(control->mutex));
		g_info("контроллер не выбран : %s",old_controller->name);
		}
		return SUCCESS;
	}

	control = controller->control;
	g_mutex_lock(&(control->mutex));
	thread = control->thread;
	g_mutex_unlock(&(control->mutex));
	if(thread == NULL){
		g_info("Поток управления контроллером %s не запушен!",controller->name);
		block_controller.current = NULL;
		return FAILURE;
	}

	changed_block_controller(&block_controller,controller->config,controller->state);

	block_controller.current = controller;

	g_mutex_lock(&(control->mutex));
	control->command.all = COMMAND_EMPTY;
	control->timeout = DEFAULT_TIMEOUT_CURRENT;
	g_mutex_unlock(&(control->mutex));

	if(block_controller.run_show == NOT_OK){
	 	block_controller.run_show = OK;
		block_controller.stop_show = NOT_OK;
 		block_controller.state->counter_fire_alarm = 0;
		g_timeout_add(block_controller.timeout_show,show_block_controller,&block_controller);
	}

	return SUCCESS;
}

/* 1000 миллесекунд == 1 секунде */
#define DEFAULT_TIMEOUT_SHOW          200    /*5 кадров в секунду отбражение информации*/
GtkWidget * create_block_controller(void)
{
 	GtkWidget * box;
	GtkWidget * frame_state;
	GtkWidget * frame_control;

	block_controller.stop_show = NOT_OK;
	block_controller.run_show = NOT_OK;
	block_controller.timeout_show = DEFAULT_TIMEOUT_SHOW;
	block_controller.state = &show_state;
	block_controller.control = &show_control;
	block_controller.current = NULL;

	init_image(&block_controller);

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	frame_control = create_block_control(&block_controller);
	frame_state = create_block_state(&block_controller);

	gtk_box_pack_start(GTK_BOX(box),frame_control,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),frame_state,TRUE,TRUE,0);

	gtk_widget_show(box);

	return box;
}

/*****************************************************************************/
/*                                                                           */
/* Функции взаимодействия с конторлером отдельный поток вывод в основном окне*/
/*                                                                           */
/*****************************************************************************/
static flag_t controller_link(controller_s * controller)
{
	int rc;
	link_s * link = controller->link;
	config_controller_s check_config;
	config_controller_s * config = controller->config;
	state_controller_s new_state;
	state_controller_s * state = controller->state;
	control_controller_s * control = controller->control;

	rc = link_controller(link,&check_config,&new_state);
	if(rc == FAILURE){
		g_mutex_lock(&(control->mutex));
		controller->status = STATUS_ERROR;
		g_mutex_unlock(&(control->mutex));
		return rc;
	}

	rc = controller_check_config(&check_config,config);
	if(rc == FAILURE){
		link_controller_disconnect(link);
		g_mutex_lock(&(control->mutex));
		controller->status = STATUS_ERROR;
		g_mutex_unlock(&(control->mutex));
		return rc;
	}

	g_mutex_lock(&(control->mutex));
	controller_copy_state(state,&new_state);
	controller->status = STATUS_NORM;
	g_mutex_unlock(&(control->mutex));
	return SUCCESS;
}

static flag_t controller_write_read(link_s * link,state_controller_s * state,control_controller_s * control)
{
	flag_t rc;
	command_u command;
	state_controller_s new_state;

	rc = link_controller_state(link,&new_state);
	if(rc == FAILURE){
		return rc;
	}
	g_mutex_lock(&(control->mutex));
	controller_copy_state(state,&new_state);
	command.all = control->command.all;
	control->command.part.value = COMMAND_EMPTY;
	g_mutex_unlock(&(control->mutex));
	if(command.part.value != COMMAND_EMPTY){
		rc = link_controller_command(link,command);
		if(rc == FAILURE){
			return rc;
		}
	}
	return SUCCESS;
}

/*****************************************************************************/
/* функция  потока комуникации с контролерами                                */
/*****************************************************************************/

static gpointer controller_communication(gpointer ud)
{
	controller_s * controller = (controller_s *)ud;
	uint32_t timeout;
	flag_t rc;
	link_s * link = controller->link;
	state_controller_s * state = controller->state;
	control_controller_s * control = controller->control;
	GThread * thread;

	for(;;){
		g_mutex_lock(&(control->mutex));
		timeout = control->timeout;
		g_mutex_unlock(&(control->mutex));

		if(timeout == 0){
			g_mutex_lock(&(control->mutex));
			controller->status = STATUS_WAIT;
			g_mutex_unlock(&(control->mutex));

			link_controller_disconnect(link);

			g_mutex_lock(&(control->mutex));
			controller->status = STATUS_ERROR;
			g_mutex_unlock(&(control->mutex));

	g_mutex_lock(&(control->mutex));
	thread = control->thread;
	control->thread = NULL;
	controller->status = STATUS_OFF;
	g_mutex_unlock(&(control->mutex));
	g_thread_exit(thread);

			break;
		}
		rc = link_check_connect(link);
		if(rc == STATUS_ERROR){
			/*нет соединения*/
			g_mutex_lock(&(control->mutex));
			controller->status = STATUS_WAIT;
			g_mutex_unlock(&(control->mutex));
			rc = controller_link(controller);
			if(rc == SUCCESS){
				g_mutex_lock(&(control->mutex));
				controller->status = STATUS_NORM;
				g_mutex_unlock(&(control->mutex));
			}
		}
		else{
			rc = controller_write_read(link,state,control);
			if(rc == FAILURE){
				g_mutex_lock(&(control->mutex));
				controller->status = STATUS_ERROR;
				g_mutex_unlock(&(control->mutex));
			}
		}
		g_usleep(timeout);
	}
	/*TODO возможна колизия*/

	g_mutex_lock(&(control->mutex));
	thread = control->thread;
	control->thread = NULL;
	controller->status = STATUS_OFF;
	g_mutex_unlock(&(control->mutex));
	g_thread_exit(thread);
	return NULL;
}

/*****************************************************************************/
/* создание потоков подключения и взаимодействия                             */
/*****************************************************************************/

static flag_t control_controllers_on(block_controller_s * bc)
{
	int i;
	GSList * list = bc->list_controllers;
	GThread * thread;

	if( list == NULL){
		g_info("Нет контролеров 1");
		return FAILURE;
	}

	for(i = 0;list;i++){
		controller_s * controller = (controller_s*)list->data;
		control_controller_s * control = controller->control;

		g_mutex_lock(&(control->mutex));
		thread = control->thread;
		g_mutex_unlock(&(control->mutex));
		if(thread == NULL){
			control->timeout = DEFAULT_TIMEOUT_ALL;
			g_string_printf(pub,"controller_%04d",i);
			control->thread = g_thread_new(pub->str,controller_communication,controller);
			g_info("Контролер инициализирован : %s",controller->name);
		}
		else{
			g_mutex_lock(&(control->mutex));
			control->timeout = DEFAULT_TIMEOUT_ALL;
			g_mutex_unlock(&(control->mutex));
			g_info("Контролер инициализирован повторно : %s",controller->name);
		}
		list = g_slist_next(list);
	}
	return SUCCESS;
}

static flag_t control_controllers_off(block_controller_s * bc)
{
	GSList * list = bc->list_controllers;
	GThread * thread;

	if(list == NULL){
		g_info("Нет контролеров 2");
		return SUCCESS;
	}

	for(;list;){
		controller_s * controller = (controller_s*)list->data;
		control_controller_s *control = controller->control;

		g_mutex_lock(&(control->mutex));
		thread = control->thread;
		g_mutex_unlock(&(control->mutex));
		if(thread != NULL){
			g_mutex_lock(&(control->mutex));
			control->timeout = 0; /*функция потока завершит свою работу и закроет соединение*/
			g_mutex_unlock(&(control->mutex));
			g_info("Контроллер деинициализирован : %s",controller->name);
		}
		list = g_slist_next(list);
	}
	return FAILURE;
}

flag_t control_controllers(flag_t mode)
{
	flag_t rc = FAILURE;
	switch(mode){
		case MODE_CONTROL_ON:
			rc = control_controllers_on(&block_controller);
			break;
		case MODE_CONTROL_OFF:
			rc = control_controllers_off(&block_controller);
			break;
	}
	return rc;
}

flag_t controller_status(controller_s * controller)
{
	flag_t flag = STATUS_OFF;
	control_controller_s * control;
	GThread * thread;

	if(controller == NULL){
		return flag;
	}

	control = controller->control;

	g_mutex_lock(&(control->mutex));
	thread = control->thread;
	g_mutex_unlock(&(control->mutex));
	if(thread == NULL){
		controller->object->status = flag;
		return flag;
	}

	flag = controller->object->status;

	g_mutex_lock(&(control->mutex));
	flag = controller->status;
	g_mutex_unlock(&(control->mutex));
	/*TODO если флаг изменился вывести сообшение*/
	controller->object->status = flag;

	return flag;
}

int init_all_controllers(void)
{
	block_controller.list_controllers = NULL;
	return SUCCESS;
}

int deinit_all_controllers(void)
{
	GSList * list = block_controller.list_controllers;
	g_slist_free(list);
	return SUCCESS;
}

/*****************************************************************************/
/* Выделение и высвобождения памяти для структур контролера                  */
/*****************************************************************************/

/*Выделение памяти и считывание из базы данных*/
controller_s * init_controller(object_s * object)
{
	int rc;
	uint32_t number = object->number;
	controller_s * controller = NULL;
	GSList * list = block_controller.list_controllers;

	controller = g_slice_alloc0(sizeof(controller_s));
	controller->status = STATUS_OFF;
	controller->link = g_slice_alloc0(sizeof(link_s));
	controller->config = g_slice_alloc0(sizeof(config_controller_s));
	controller->state = g_slice_alloc0(sizeof(state_controller_s));
	controller->control = g_slice_alloc0(sizeof(control_controller_s));
	g_mutex_init(&(controller->control->mutex));
	controller->control->thread = NULL;
	controller->control->command.all =  COMMAND_EMPTY;
	/*память для обектов выделяется при чтении из базыданых*/
	rc = read_database_controller(number,controller);
	if(rc != SUCCESS){
		g_slice_free1(sizeof(controller_s),controller);
		g_slice_free1(sizeof(link_s),controller->link);
		g_slice_free1(sizeof(config_controller_s),controller->config);
		g_slice_free1(sizeof(state_controller_s),controller->state);
		g_mutex_clear(&(controller->control->mutex));
		g_slice_free1(sizeof(control_controller_s),controller->control);
		controller = NULL;
		return NULL;
	}
	controller->name = controller_name(controller->config);
	controller->type = controller_type_device(controller->config);

	list = g_slist_append(list,controller);

	block_controller.list_controllers = list;

	controller->object = object;
#if 0
	g_info(" %s ",controller->name);
	g_debug("type               :> %d",controller->config->type);
	g_debug("flag               :> %#lx",controller->config->flag);
	g_debug("tic vertical       :> %g",controller->config->rate_tic_vertical);
	g_debug("encoder vertical   :> %g",controller->config->rate_encoder_vertical);
	g_debug("amprage vertical   :> %g",controller->config->rate_amperage_vertical);
	g_debug("ti horizontal      :> %g",controller->config->rate_tic_horizontal);
	g_debug("encoder horizontal :> %g",controller->config->rate_encoder_horizontal);
	g_debug("amprage horizontal :> %g",controller->config->rate_amperage_horizontal);
	g_debug("pressure           :> %g",controller->config->rate_pressure);
	g_debug("valve              :> %g",controller->config->rate_valve_analog);
#endif
	return controller;
}
/*Высвобождение памяти   */
int del_property_controller(controller_s * property)
{
	char * str;
	link_s * link;
	config_controller_s * config;
	state_controller_s * state;
	control_controller_s * control;

	if(property == NULL){
		return SUCCESS;
	}
	str = property->name;
	g_free(str);

	link = property->link;
	link_controller_disconnect(link);
	str = link->address;
	g_free(str);
	g_slice_free1(sizeof(link_s),link);

	config = property->config;
	g_slice_free1(sizeof(config_controller_s),config);
	state = property->state;
	g_slice_free1(sizeof(state_controller_s),state);
	control = property->control;
	g_mutex_clear(&(control->mutex));
	g_slice_free1(sizeof(control_controller_s),control);

	g_slice_free1(sizeof(controller_s),property);

	return SUCCESS;
}
/*****************************************************************************/
