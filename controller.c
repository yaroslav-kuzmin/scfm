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
/*структура взаимодействия основного потока и потока взаимодействия с конторллерами*/
typedef struct _communication_controller_s communication_controller_s;
struct _communication_controller_s
{
	int exit;

	GThread * tid;
	GMutex mutex;

	GSList * list;
	controller_s * current;

	uint32_t timeout_current;
	uint32_t timeout_all;
	uint32_t timeout_config;
};

typedef struct _show_state_s show_state_s;
struct _show_state_s
{
	GtkImage * image_vertical;
	GdkPixbuf * frame_vertical;

	GtkImage * image_horizontal;
	GdkPixbuf * frame_horizontal;

	GtkImage * valve;
	GdkPixbuf * buf_valve;

	GtkImage * pressure;
	GdkPixbuf * buf_pressure;

	GtkLabel * lab_mode;
	GtkLabel * lab_state;

	GtkImage * fire_alarm;
};

enum
{
	MODE_AUTO=1,
	MODE_MANUAL,
	MODE_CONFIG,
	AMOUNT_MODE
};

typedef struct _show_control_s show_control_s;
struct _show_control_s
{
	/*режим */
	GtkLabel * lab_console;
	GtkButton * but_console;
	flag_t mode;

	/*лафет*/
	GtkButton * but_up;
	GtkButton * but_down;
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
	/*название блока */
	GtkLabel * name;
	flag_t type_device;
	/*отображение блока конторллера*/
	flag_t stop_show;
	flag_t run_show;
	uint32_t timeout_show;

	/*флаги управления*/

	/*поток взыимодействия с конторлером*/
	communication_controller_s * communication_controller;

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

	PRESSURE_00,
	PRESSURE_15,
	PRESSURE_20,
	PRESSURE_25,
	PRESSURE_ERROR,
	VALVE_OPEN,
	VALVE_CLOSE,
	VALVE_OPEN_RUN,
	VALVE_CLOSE_RUN,
	VALVE_ERROR,
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

	images_state[PRESSURE_00]    = get_resource_image(RESOURCE_IMAGE,"pressure_00");
	images_state[PRESSURE_15]    = get_resource_image(RESOURCE_IMAGE,"pressure_15");
	images_state[PRESSURE_20]    = get_resource_image(RESOURCE_IMAGE,"pressure_20");
	images_state[PRESSURE_25]    = get_resource_image(RESOURCE_IMAGE,"pressure_25");
	images_state[PRESSURE_ERROR] = get_resource_image(RESOURCE_IMAGE,"pressure_xx");

	images_state[VALVE_OPEN]      = get_resource_image(RESOURCE_IMAGE,"valve_open");
	images_state[VALVE_CLOSE]     = get_resource_image(RESOURCE_IMAGE,"valve_close");
	images_state[VALVE_OPEN_RUN]  = get_resource_image(RESOURCE_IMAGE,"valve_open_run");
	images_state[VALVE_CLOSE_RUN] = get_resource_image(RESOURCE_IMAGE,"valve_close_run");
	images_state[VALVE_ERROR]     = get_resource_image(RESOURCE_IMAGE,"valve_error");

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
#define STATE_PRESSURE_ERROR     0xFFFF
#if 0
static GdkPixbuf * get_image_pressure(uint16_t pressure)
{
	if(pressure == STATE_PRESSURE_ERROR){
		return images_state[PRESSURE_ERROR];
	}
	if(pressure == 0){
		return images_state[PRESSURE_00];
	}
	if(pressure == 1){
		return images_state[PRESSURE_15];
	}
	if(pressure == 2){
		return images_state[PRESSURE_20];
	}
	return images_state[PRESSURE_25];
}

static GdkPixbuf * get_image_valve_tic(flag_t valve)
{
	GdkPixbuf * buf;

	switch(valve){
		case STATE_VALVE_OPEN:
			buf = images_state[VALVE_OPEN];
			break;
		case STATE_VALVE_CLOSE:
			buf = images_state[VALVE_CLOSE];
			break;
		case STATE_VALVE_OPEN_RUN:
			buf = images_state[VALVE_OPEN_RUN];
			break;
		case STATE_VALVE_CLOSE_RUN:
			buf = images_state[VALVE_CLOSE_RUN];
			break;
		default:
			/*TODO поставить отдельный рисунок на ошибку*/
			buf = images_state[VALVE_ERROR];
			break;
	}

	return buf;
}

static GdkPixbuf * get_image_valve_analog(uint16_t valve)
{
	return images_state[VALVE_ERROR];
}
#endif
#define MIN_VERTICAL_TIC      0
#define MAX_VERTICAL_TIC      30
#define MIN_VERTICAL_ANGLE   	0
#define MAX_VERTICAL_ANGLE    180
static uint16_t calculate_angle_tic_vertical(state_controller_s * state,config_controller_s * config)
{
	uint16_t angle;
	uint16_t tic = state->tic_vertical;
	gdouble rate = config->rate_tic_vertical;

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
/* Функции взаимодействия с конторлером отдельный поток вывод в основном окне*/
/*****************************************************************************/
static int connect_controller(controller_s * controller)
{
	int rc;
	link_s * link = controller->link;
	config_controller_s check;
	config_controller_s * config = controller->config;
	state_controller_s * state = controller->state;

	if(link->connect != NULL){
		g_info("Контролер уже подключен : %s",controller->name);
		return SUCCESS;
	}
	rc = check_link_controller(link,&check,state);
	if(rc == FAILURE){
		g_info("Нет подключения к контроллеру : %s",controller->name);
		return rc;
	}
	rc = check_config_controller(&check,config);
	if(rc == FAILURE){
		/*TODO перезапись базы данных*/
		g_info("Данные из контроллера %s не совпадают с данными из базы данных!",controller->name);
		link_disconnect_controller(link);
		return rc;
	}
	return SUCCESS;
}

static int disconnect_controller(controller_s * controller)
{
	link_s * link = controller->link;
	if(link->connect == NULL){
		g_info("Контролер не подключен : %s",controller->name);
		return FAILURE;
	}
	return link_disconnect_controller(link);
}

static flag_t copy_state(state_controller_s * des,state_controller_s * src)
{
	memmove(des,src,sizeof(state_controller_s));
	return SUCCESS;
}
/*uint32_t debug_id = 0;*/
/* функция  потока комуникации с контролерами */
static gpointer controllers_communication(gpointer ud)
{
	int rc;
	communication_controller_s * cc = (communication_controller_s *)ud;
	controller_s * controller;
	link_s * link;
	state_controller_s state;
	command_u command;
	/*GSList * list;*/

	for(;;){
		g_mutex_lock(&(cc->mutex));
		controller = cc->current;
		g_mutex_unlock(&(cc->mutex));
		if(controller != NULL){
			link = controller->link;
			rc = link_state_controller(link,&state);
			if(rc == FAILURE){
				/*TODO сделать реконнект*/
				controller->object->status = STATUS_ERROR;
			}
			else{
				/*g_debug("read controller");*/
				g_mutex_lock(&(cc->mutex));
				copy_state(controller->state,&state);
				command.all = controller->control->command.all;
				controller->control->command.part.value = COMMAND_EMPTY;
				g_mutex_unlock(&(cc->mutex));
				if(command.part.value != COMMAND_EMPTY){
					/*g_debug(" :> %ld ",command);*/
					rc = command_controller(link,command);
					if(rc == FAILURE){
						/*TODO сделать реконнект*/
						controller->object->status = STATUS_ERROR;
					}
				}
			} /*else*/
		} /*if(controller != NULL)*/
#if 0
		/*проверка всех */
		list = ac->list;
		for(;list;){
			controller = (controller_s*)list->data;
			control = controller->control;
			g_mutex_lock(&(cc->flag));
			if(control->select == OK ){
				g_mutex_unlock(&(cc->mutex));
				link = controller->link;
				state = controller->state;
				g_mutex_lock(&(cc->m_state));
				/*TODO вынести заполнение структуры в отдельную функцию*/
				rc = link_state_controller(link,state);
				g_mutex_unlock(&(cc->m_state));
				if(rc == FAILURE){
					/*TODO сделать реконнект*/
					g_debug("reconnect");
				}
			}
			g_mutex_unlock(&(cc->flag));
			list = g_slist_next(list);
		}
#endif
		/*debug_id ++;*/
		/*g_debug("state controller : %d",debug_id);*/
		/*TODO сделать возможное в реальном режиме менять таймаут*/
		g_usleep(cc->timeout_current);
		g_mutex_lock(&(cc->mutex));
		if(cc->exit == OK){
			g_mutex_unlock(&(cc->mutex));
			g_thread_exit(cc->tid);
		}
	 	g_mutex_unlock(&(cc->mutex));
	}
	cc->tid = NULL;
	return NULL;
}

static flag_t control_controllers_on(communication_controller_s * cc)
{
	int rc;
	GSList * list = cc->list;

	if(list == NULL){
		g_info("Нет контролеров");
		return FAILURE;
	}

	if(cc->tid != NULL){
		g_info("Поток коммуникации уже запущен!");
		return FAILURE;
	}

	for(;list;){
		controller_s * controller = (controller_s*)list->data;
		rc = connect_controller(controller);
		if(rc == SUCCESS){
			g_info("Подключился к %s",controller->name);
			/*TODO установить статус */
			controller->object->status = STATUS_NORM;
		}
		list = g_slist_next(list);
	}

	cc->exit = NOT_OK;
	g_mutex_init(&(cc->mutex));
	cc->tid = g_thread_new("controller",controllers_communication,cc);

	return FAILURE;
}

static flag_t control_controllers_off(communication_controller_s * cc)
{
	int rc;
	GSList * list = cc->list;

	if(cc->tid != NULL){/*поток запущен*/
		cc->exit = OK;
		g_thread_join(cc->tid);
		g_mutex_clear(&(cc->mutex));
		cc->tid = NULL;
	}

	if(list == NULL){
		g_info("Нет контролеров");
		return SUCCESS;
	}

	for(;list;){
		controller_s * controller = (controller_s*)list->data;
		rc = disconnect_controller(controller);
		if(rc == SUCCESS){
			g_info("Отключился от %s",controller->name);
			controller->object->status = STATUS_WAIT;
		}
		list = g_slist_next(list);
	}
	return FAILURE;
}

static communication_controller_s communication_controller;

flag_t control_controllers(flag_t mode)
{
	flag_t rc = FAILURE;
	switch(mode){
		case MODE_CONTROL_ON:
			rc = control_controllers_on(&communication_controller);
			break;
		case MODE_CONTROL_OFF:
			rc = control_controllers_off(&communication_controller);
			break;
	}
	return rc;
}

/* 1 000 000  микросекунд == 1 секунда*/
#define DEFAULT_TIMEOUT_CURRENR        1000000      /*   250000        250 милесекунд */
#define DEFAULT_TIMEOUT_ALL            3000000      /* 3 секунды */
#define DEFAULT_TIMEOUT_CONFIG         600000000    /* 600 секунд*/

int init_all_controllers(void)
{
	communication_controller.list = NULL;
	communication_controller.current = NULL;
	communication_controller.timeout_current =  DEFAULT_TIMEOUT_CURRENR;
	communication_controller.timeout_all = DEFAULT_TIMEOUT_ALL;
	communication_controller.timeout_config = DEFAULT_TIMEOUT_CONFIG;
	communication_controller.exit = OK;
	communication_controller.tid = NULL;

	return SUCCESS;
}

int deinit_all_controllers(void)
{
	g_slist_free(communication_controller.list);
	return SUCCESS;
}

/*****************************************************************************/
/* Блок отображение основного окна управления контролером                    */
/*****************************************************************************/

/***** Функции отрисовки информации по таймеру *******************************/
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

static int show_vertical(show_state_s * show_state,show_control_s * show_controls
                        ,state_controller_s * controller_state,config_controller_s * controller_config)
{
	uint64_t flag = controller_config->flag;
	int16_t angle = 0;

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
	image_copy(show_state->frame_vertical,background);

	angle_image = get_image_vertical(angle);
	image_impose(show_state->frame_vertical,angle_image);

	gtk_image_set_from_pixbuf(show_state->image_vertical,show_state->frame_vertical);

	return SUCCESS;
}

static int show_horizontal(show_state_s * show_state,show_control_s * show_controls
                          ,state_controller_s * controller_state,config_controller_s * controller_config)
{
	uint64_t flag = controller_config->flag;
	int16_t angle = 0;
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
	image_copy(show_state->frame_horizontal,background);

	angle_image = get_image_horizontal(angle);
	image_impose(show_state->frame_horizontal,angle_image);

	gtk_image_set_from_pixbuf(show_state->image_horizontal,show_state->frame_horizontal);
	return SUCCESS;
}

static int show_valve(show_state_s * show_state,show_control_s * show_controls
                     ,state_controller_s * controller_state,config_controller_s * controller_config)
{
#if 0
	uint64_t flag = controller_config->flag;
	GdkPixbuf * image_valve;

	if(!VALVE_DRY(flag)){
		if(!VALVE_ANALOG(flag)){
			return SUCCESS;
		}
	}

	if(VALVE_LIMIT(flag)){
		flag_t s = get_state_valve(controller_state);
		image_valve = get_image_valve_tic(s);
	}
	else{
		if(VALVE_FEEDBACK(flag)){
			image_valve = get_image_valve_analog(0);
		}
		else{
			image_valve = images_state[VALVE_ERROR];
		}
	}

	/*set_image(show_state->valve,show_state->buf_valve,image_valve);*/
#endif
	return SUCCESS;
}

static int show_pressure(show_state_s * show_state,show_control_s * show_controls
                     ,state_controller_s * controller_state,config_controller_s * controller_config)
{
#if 0
 	uint64_t flag = controller_config->flag;
	GdkPixbuf * image_pressure;
	uint16_t pressure;

	if(!PRESSURE(flag)){
			return SUCCESS;
	}

	pressure = calculate_pressure(controller_state,controller_config);
	image_pressure = get_image_pressure(pressure);
	/*set_image(show_state->pressure,show_state->buf_pressure,image_pressure);*/
#endif
	return SUCCESS;
}

#if 0
static int show_file_sensor(block_controller_s * bc)
{
	return SUCCESS;
}

static int show_fire_alarm(block_controller_s * bc)
{
	return SUCCESS;
}
#endif
static int show_block_controller(gpointer data)
{
 	/*GtkLabel * label;*/
	block_controller_s * bc = (block_controller_s *)data;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * controller = cc->current;

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
	if(cc->tid == NULL){
		/*поток не запушен*/
		bc->run_show = NOT_OK;
		return FALSE;
	}
	g_mutex_lock(&(cc->mutex));
	copy_state(&state,controller->state);
	g_mutex_unlock(&(cc->mutex));

	show_vertical(bc->state,bc->control,&state,controller->config);
	show_horizontal(bc->state,bc->control,&state,controller->config);
	show_valve(bc->state,bc->control,&state,controller->config);
	show_pressure(bc->state,bc->control,&state,controller->config);

#if 0
	show_file_sensor(bc);
	show_fire_alarm(bc);
#endif
	return TRUE; /* продолжаем работу */
}

/***** Функции отображение информации ****************************************/

#define DEFAULT_SIZE_WIDTH_AXIS_VERTICAL    300
#define DEFAULT_SIZE_HEIGHT_AXIS_VERTICAL   300

static GtkWidget * create_block_state_vertical(show_state_s * state)
{
	GtkWidget * frame;
	GtkWidget * image;
	GdkPixbuf * buf;

	frame = gtk_frame_new(NULL);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	/*TODO маштабирование */
	buf = get_resource_image(RESOURCE_IMAGE,"vertical_background");
	state->frame_vertical = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_AXIS_VERTICAL,DEFAULT_SIZE_HEIGHT_AXIS_VERTICAL);*/
	state->image_vertical = GTK_IMAGE(image);

	gtk_container_add(GTK_CONTAINER(frame),image);

	gtk_widget_show(frame);
	gtk_widget_show(image);

	return frame;
}

#define DEFAULT_SIZE_WIDTH_AXIS_HORIZONTAL    300
#define DEFAULT_SIZE_HEIGHT_AXIS_HORIZONTAL   300
static GtkWidget * create_block_state_horizontal(show_state_s * state)
{
	GtkWidget * frame;
	GtkWidget * image;
	GdkPixbuf * buf;

	frame = gtk_frame_new(NULL);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	buf = get_resource_image(RESOURCE_IMAGE,"horizontal_background");
	state->frame_horizontal = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование*/
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_AXIS_HORIZONTAL,DEFAULT_SIZE_HEIGHT_AXIS_HORIZONTAL);*/
	state->image_horizontal = GTK_IMAGE(image);

	gtk_container_add(GTK_CONTAINER(frame),image);

	gtk_widget_show(frame);
	gtk_widget_show(image);

	return frame;
}
#if 0
static GtkWidget * create_block_state_valve(show_state_s * state)
{
	GtkWidget * frame;
	GtkWidget * image;
	GdkPixbuf * buf;

	frame = gtk_frame_new("Задвижка");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	buf = images_state[VALVE_ERROR];
	state->buf_valve = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование*/
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_PRESSURE_VALVE,DEFAULT_SIZE_HEIGHT_PRESSURE_VALVE);*/
	state->valve = GTK_IMAGE(image);

	gtk_container_add(GTK_CONTAINER(frame),image);

	gtk_widget_show(frame);
	gtk_widget_show(image);

	return frame;
}

static GtkWidget * create_block_state_pressure(show_state_s * state)
{
 	GtkWidget * frame;
	GtkWidget * image;
	GdkPixbuf * buf;

	frame = gtk_frame_new("Давление");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	buf = images_state[PRESSURE_ERROR];
	state->buf_pressure = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование*/
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_PRESSURE_VALVE,DEFAULT_SIZE_HEIGHT_PRESSURE_VALVE);*/
	state->pressure = GTK_IMAGE(image);

	gtk_container_add(GTK_CONTAINER(frame),image);

	gtk_widget_show(frame);
	gtk_widget_show(image);

	return frame;
}
/*static char STR_FIRE_SENSOR_TRIGGER[] = "Пожар";*/
static char STR_FIRE_SENSOR_NORM[] = "НОРМА";
static GtkWidget * create_block_fire_sensor(block_controller_s * block)
{
	GtkWidget * frame;
	GtkWidget * grid;
	GtkWidget * label;

	frame = gtk_frame_new("Датчик Пламени");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label = gtk_label_new(STR_FIRE_SENSOR_NORM);
	layout_widget(label,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);

	gtk_container_add(GTK_CONTAINER(frame),grid);
	gtk_grid_attach(GTK_GRID(grid),label,0,0,1,1);

	gtk_widget_show(frame);
	gtk_widget_show(grid);
	gtk_widget_show(label);

	return frame;
}

static GtkWidget * create_block_fire_alarm_one(uint16_t number)
{
	GtkWidget * label;
	g_string_printf(pub,"%02d НОРМА",number);
	label = gtk_label_new(pub->str);
	layout_widget(label,GTK_ALIGN_START,GTK_ALIGN_FILL,FALSE,TRUE);
	return label;
}

static GtkWidget * create_block_fire_alarm(block_controller_s * block)
{
	int i;
	GtkWidget * frame;
	GtkWidget * box;
	GtkWidget * block_fire_alarm;

	frame = gtk_frame_new("Пожарная Сигнализация");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	for(i = 0;i < AMOUNT_FIRE_SENSOR;i++){
		block_fire_alarm = create_block_fire_alarm_one(i+1);
		gtk_box_pack_start(GTK_BOX(box),block_fire_alarm,TRUE,TRUE,0);
		gtk_widget_show(block_fire_alarm);
	}

	gtk_container_add(GTK_CONTAINER(frame),box);

	gtk_widget_show(frame);
	gtk_widget_show(box);
	return frame;
}
#endif

static GtkWidget * create_block_info_fire(show_state_s * state)
{
	GtkWidget * fire;
	GdkPixbuf * buf = get_resource_image(RESOURCE_IMAGE,"fire_alarm_norm");

	fire = gtk_image_new_from_pixbuf(buf);
	layout_widget(fire,GTK_ALIGN_END,GTK_ALIGN_START,FALSE,FALSE);
	state->fire_alarm = GTK_IMAGE(fire);
	gtk_widget_show(fire);
	return fire;
}

static char STR_INFO_MODE_WAIT[] = "Ожидание";
static char STR_INFO_STATE_NORM[] = "Норма";

static GtkWidget * create_block_info_state(show_state_s * state)
{
	GtkWidget * box;
	GtkWidget * lab_mode;
	GtkWidget * lab_state;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,TRUE);
	lab_mode = gtk_label_new(STR_INFO_MODE_WAIT);
	state->lab_mode = GTK_LABEL(lab_mode);

	lab_state = gtk_label_new(STR_INFO_STATE_NORM);
	state->lab_state = GTK_LABEL(lab_state);

	gtk_box_pack_start(GTK_BOX(box),lab_mode,TRUE,TRUE,10);
	gtk_box_pack_start(GTK_BOX(box),lab_state,TRUE,TRUE,10);

	gtk_widget_show(box);
	gtk_widget_show(lab_mode);
	gtk_widget_show(lab_state);

 	return box;
}

static GtkWidget * create_block_state_message(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_info_state;
	GtkWidget * block_info_fire;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,FALSE);

	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	block_info_state = create_block_info_state(bc->state);
	block_info_fire = create_block_info_fire(bc->state);

	gtk_box_pack_start(GTK_BOX(box),block_info_state,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_info_fire,TRUE,TRUE,0);

	gtk_widget_show(box);

	return box;
}

static GtkWidget * create_block_state_lafet(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_vertical;
	GtkWidget * block_horizontal;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);

	block_vertical = create_block_state_vertical(bc->state);
	block_horizontal = create_block_state_horizontal(bc->state);

	gtk_box_pack_start(GTK_BOX(box),block_vertical,TRUE,TRUE,5);
	gtk_box_pack_start(GTK_BOX(box),block_horizontal,TRUE,TRUE,5);

	gtk_widget_show(box);
	return box;
}
static GtkWidget * create_block_state_pipe(block_controller_s * bc)
{
	GtkWidget * box;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
	gtk_widget_show(box);
	return box;
}

static GtkWidget * create_block_state(block_controller_s * bc)
{
	GtkWidget * frame;
	GtkWidget * box;
	GtkWidget * label_name;
	GtkWidget * block_state_message;
	GtkWidget * block_state_lafet;
	GtkWidget * block_state_pipe;

	frame = gtk_frame_new("Информация");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label_name = gtk_label_new("Нет подключения к контролеру!");
	layout_widget(label_name,GTK_ALIGN_CENTER,GTK_ALIGN_START,TRUE,TRUE);
	bc->name = GTK_LABEL(label_name);

	block_state_message = create_block_state_message(bc);
	block_state_lafet = create_block_state_lafet(bc);
	block_state_pipe = create_block_state_pipe(bc);

	gtk_box_pack_start(GTK_BOX(box),label_name,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_state_message,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_state_lafet,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_state_pipe,TRUE,TRUE,0);

	gtk_container_add(GTK_CONTAINER(frame),box);

	gtk_widget_show(frame);
	gtk_widget_show(box);
	gtk_widget_show(label_name);

	return frame;
}

/***** Функции отображения системы управления ********************************/


static flag_t push_command_queue(communication_controller_s * cc,controller_s * controller,command_u command,flag_t rewrite)
{
	control_controller_s * control = controller->control;

	g_mutex_lock(&(cc->mutex));
	if(control->command.part.value == COMMAND_EMPTY){
		control->command.all = command.all;
	}
	else{
		if(rewrite == OK){
			control->command.all = command.all;
		}
	}
	g_mutex_unlock(&(cc->mutex));

	return SUCCESS;
}

static flag_t set_button_not_active(GtkButton * but)
{
	return SUCCESS;
}

static void button_clicked_control_mode(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};

 	if(bc->type_device == TYPE_DEVICE_LSD){
		g_info("Режим управления ручной");
		command.part.value = COMMAND_MODE_MANUAL;
		push_command_queue(communication_controller,controller,command,NOT_OK);
		return;
	}

	g_info("Режим не установлен");
	/*TODO если тип устанвки позволяет устновить автоматический или ручной режим*/
}

static GtkWidget * create_block_control_mode(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * label;
	GtkWidget * but;

	bc->control->mode = MODE_MANUAL;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	layout_widget(box,GTK_ALIGN_CENTER,GTK_ALIGN_START,TRUE,TRUE);

	label = gtk_label_new("Режим управления");
	layout_widget(label,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	/*gtk_label_set_xalign(GTK_LABEL(label),0);*/
	bc->control->lab_console = GTK_LABEL(label);

	but = gtk_button_new_with_label("Изменить");
	layout_widget(but,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but,"clicked",G_CALLBACK(button_clicked_control_mode),bc);
	bc->control->but_console = GTK_BUTTON(but);

	gtk_box_pack_start(GTK_BOX(box),label,TRUE,TRUE,5);
	gtk_box_pack_start(GTK_BOX(box),but,TRUE,TRUE,5);

	gtk_widget_show(box);
	gtk_widget_show(label);
	gtk_widget_show(but);
	return box;
}

static void button_clicked_valve_open(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};

	if( controller == NULL){
		g_info("Не выбран контролер");
		return ;
	}
	command.part.value = COMMAND_VALVE_OPEN;
	push_command_queue(communication_controller,controller,command,NOT_OK);
}

static void button_clicked_valve_close(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};

	if( controller == NULL){
		g_info("Не выбран контролер");
		return ;
	}
	command.part.value = COMMAND_VALVE_CLOSE;
	push_command_queue(communication_controller,controller,command,NOT_OK);
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

	g_info("Задвижка %e : %d",v,precise);
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

	label = gtk_label_new("Задвижка");

	but_open = gtk_button_new_with_label("Открыть");
	layout_widget(but_open,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_open,"clicked",G_CALLBACK(button_clicked_valve_open),bc);
	bc->control->but_valve_open = GTK_BUTTON(but_open);

	but_close = gtk_button_new_with_label("Закрыть");
	layout_widget(but_close,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_close,"clicked",G_CALLBACK(button_clicked_valve_close),bc);
	bc->control->but_valve_close = GTK_BUTTON(but_close);

	but_run = gtk_button_new_with_label("Установить");
	layout_widget(but_run,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_run,"clicked",G_CALLBACK(button_clicked_valve_run),bc);

	scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,min_valve,max_valve,step_valve);
	layout_widget(scale,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	gtk_scale_set_digits(GTK_SCALE(scale),1); /*колличество знаков после запятой*/
	gtk_widget_set_size_request(scale,100,-1);
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

static void button_press_event_lafet_up(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_LAFET_UP;
	push_command_queue(communication_controller,controller,command,NOT_OK);
}
static void button_press_event_lafet_down(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_LAFET_DOWN;
	push_command_queue(communication_controller,controller,command,NOT_OK);
}
static void button_press_event_lafet_right(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_LAFET_RIGHT;
	push_command_queue(communication_controller,controller,command,NOT_OK);
}
static void button_press_event_lafet_left(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_LAFET_LEFT;
	push_command_queue(communication_controller,controller,command,NOT_OK);
}
static void button_release_event_lafet_stop(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_LAFET_STOP;
	push_command_queue(communication_controller,controller,command,OK);
}

static GtkWidget * create_block_control_lafet(block_controller_s * bc)
{
	GtkWidget * grid;
	GtkWidget * but_up;
	GtkWidget * but_down;
	GtkWidget * but_right;
	GtkWidget * but_left;
	GtkWidget * block_precise;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_CENTER,GTK_ALIGN_START,TRUE,TRUE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid),FALSE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid),FALSE);

	but_up = gtk_button_new_with_label("ВВЕРХ");
	layout_widget(but_up,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_up,"button-press-event",G_CALLBACK(button_press_event_lafet_up),bc);
	g_signal_connect(but_up,"button-release-event",G_CALLBACK(button_release_event_lafet_stop),bc);
	bc->control->but_up = GTK_BUTTON(but_up);

	but_down = gtk_button_new_with_label("ВНИЗ");
	layout_widget(but_down,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_down,"button-press-event",G_CALLBACK(button_press_event_lafet_down),bc);
	g_signal_connect(but_down,"button-release-event",G_CALLBACK(button_release_event_lafet_stop),bc);
	bc->control->but_down = GTK_BUTTON(but_down);

	but_right = gtk_button_new_with_label("ВПРАВО");
	layout_widget(but_right,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_right,"button-press-event",G_CALLBACK(button_press_event_lafet_right),bc);
	g_signal_connect(but_right,"button-release-event",G_CALLBACK(button_release_event_lafet_stop),bc);
	bc->control->but_right = GTK_BUTTON(but_right);

	but_left = gtk_button_new_with_label("ВЛЕВО");
	layout_widget(but_left,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_left,"button-press-event",G_CALLBACK(button_press_event_lafet_left),bc);
	g_signal_connect(but_left,"button-release-event",G_CALLBACK(button_release_event_lafet_stop),bc);
	bc->control->but_left = GTK_BUTTON(but_left);

	block_precise = create_block_lafet_precise(bc);

	gtk_grid_attach(GTK_GRID(grid),but_up       ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_down     ,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_left     ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_right    ,2,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_precise,0,2,3,1);

	gtk_widget_show(grid);
	gtk_widget_show(but_up);
	gtk_widget_show(but_down);
	gtk_widget_show(but_right);
	gtk_widget_show(but_left);

 	return grid;
}

static void button_press_event_actuator_spray_less(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_SPRAY_LESS;
	push_command_queue(communication_controller,controller,command,NOT_OK);
}
static void button_press_event_actuator_spray_more(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_SPRAY_MORE;
	push_command_queue(communication_controller,controller,command,NOT_OK);
}
static void button_press_event_actuator_rate_less(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_RATE_LESS;
	push_command_queue(communication_controller,controller,command,NOT_OK);
}
static void button_press_event_actuator_rate_more(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_RATE_MORE;
	push_command_queue(communication_controller,controller,command,NOT_OK);
}
static void button_release_event_actuator_stop(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	command.part.value = COMMAND_ACTUATOT_STOP;
	push_command_queue(communication_controller,controller,command,OK);
}

static GtkWidget * create_block_actuator(block_controller_s * bc)
{
	GdkPixbuf * buf;
	GtkWidget * box;

	GtkWidget * grid_spray;
	GtkWidget * label_spray;
	GtkWidget * but_spray_less;
	GtkWidget * img_separator_spray;
	GtkWidget * but_spray_more;

	GtkWidget * grid_rate;
	GtkWidget * label_rate;
	GtkWidget * img_separator_rate;
	GtkWidget * but_rate_less;
	GtkWidget * but_rate_more;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_CENTER,GTK_ALIGN_START,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),TRUE);

	grid_spray = gtk_grid_new();
	layout_widget(grid_spray,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid_spray),TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid_spray),TRUE);
	bc->control->actuator_spray = grid_spray;

	label_spray = gtk_label_new("Распыл");
	layout_widget(label_spray,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	buf = get_resource_image(RESOURCE_STYLE,"actuator-separator-spray");
	img_separator_spray = gtk_image_new_from_pixbuf(buf);

	but_spray_less = gtk_button_new_with_label("Уже");
	layout_widget(but_spray_less,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_spray_less,"button-press-event",G_CALLBACK(button_press_event_actuator_spray_less),bc);
	g_signal_connect(but_spray_less,"button-release-event",G_CALLBACK(button_release_event_actuator_stop),bc);
	but_spray_more = gtk_button_new_with_label("Шире");
	layout_widget(but_spray_more,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_spray_more,"button-press-event",G_CALLBACK(button_press_event_actuator_spray_more),bc);
	g_signal_connect(but_spray_more,"button-release-event",G_CALLBACK(button_release_event_actuator_stop),bc);

	grid_rate = gtk_grid_new();
	layout_widget(grid_rate,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid_rate),TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid_rate),TRUE);
	bc->control->actuator_rate = grid_rate;

	label_rate = gtk_label_new("Литраж");
	layout_widget(label_rate,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);

	buf = get_resource_image(RESOURCE_STYLE,"actuator-separator-rate");
	img_separator_rate = gtk_image_new_from_pixbuf(buf);
	but_rate_less = gtk_button_new_with_label("Больше");
	layout_widget(but_rate_less,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_rate_less,"button-press-event",G_CALLBACK(button_press_event_actuator_rate_less),bc);
	g_signal_connect(but_rate_less,"button-release-event",G_CALLBACK(button_release_event_actuator_stop),bc);
	but_rate_more = gtk_button_new_with_label("Меньше");
	layout_widget(but_rate_more,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_rate_more,"button-press-event",G_CALLBACK(button_press_event_actuator_rate_more),bc);
	g_signal_connect(but_rate_more,"button-release-event",G_CALLBACK(button_release_event_actuator_stop),bc);

	gtk_box_pack_start(GTK_BOX(box),grid_spray,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),grid_rate ,TRUE,TRUE,0);

	gtk_grid_attach(GTK_GRID(grid_spray),label_spray        ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid_spray),but_spray_less     ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid_spray),img_separator_spray,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid_spray),but_spray_more     ,2,1,1,1);

	gtk_grid_attach(GTK_GRID(grid_rate),label_rate         ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid_rate),but_rate_less      ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid_rate),img_separator_rate,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid_rate),but_rate_more      ,2,1,1,1);

	gtk_widget_show(box);
	gtk_widget_show(grid_spray);
	gtk_widget_show(label_spray);
	gtk_widget_show(img_separator_spray);
	gtk_widget_show(but_spray_less);
	gtk_widget_show(but_spray_more);
	gtk_widget_show(grid_rate);
	gtk_widget_show(label_rate);
	gtk_widget_show(img_separator_rate);
	gtk_widget_show(but_rate_less);
	gtk_widget_show(but_rate_more);

 	return box;
}

static void button_clicked_oscillation_run(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command ={0};
	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	if(bc->control->oscillation_flag == COMMAND_OSCILLATION_STOP){
		command.part.value = bc->control->oscillation_command.part.value;
		bc->control->oscillation_flag = COMMAND_OSCILLATION_RUN;
		push_command_queue(communication_controller,controller,command,NOT_OK);
	}
}
static void button_clicked_oscillation_stop(GtkButton * b,gpointer ud)
{
 	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	if( controller == NULL){
	 	g_info("Не выбран контролер");
		return;
	}
	bc->control->oscillation_flag = COMMAND_OSCILLATION_STOP;
	command.part.value = COMMAND_OSCILLATION_STOP;
	push_command_queue(communication_controller,controller,command,OK);
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

	label = gtk_label_new("Осциляция");

	bc->control->oscillation_command.part.value = COMMAND_OSCILLATION_VERTICAL; /*выбор по умолчанию */
	bc->control->oscillation_flag = COMMAND_OSCILLATION_STOP;

	but_run = gtk_button_new_with_label("Запустить");
	g_signal_connect(but_run,"clicked",G_CALLBACK(button_clicked_oscillation_run),bc);

	but_stop = gtk_button_new_with_label("Остановить");
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
	gtk_grid_attach(GTK_GRID(grid),but_vertical  ,0,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_horizontal,1,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_saw       ,0,3,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_step      ,1,3,1,1);

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
static GtkWidget * create_block_control_console(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_mode;
	GtkWidget * block_lafet;
	GtkWidget * block_actuator;
	GtkWidget * block_valve;
	GtkWidget * block_oscillation;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
  layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_mode = create_block_control_mode(bc);
	block_lafet = create_block_control_lafet(bc);
	block_actuator = create_block_actuator(bc);
	block_valve = create_block_control_valve(bc);
	block_oscillation = create_block_oscillation(bc);

	gtk_box_pack_start(GTK_BOX(box),block_mode,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_lafet,TRUE,TRUE,5);
	gtk_box_pack_start(GTK_BOX(box),block_actuator,TRUE,TRUE,5);
	gtk_box_pack_start(GTK_BOX(box),block_valve,TRUE,TRUE,5);
	gtk_box_pack_start(GTK_BOX(box),block_oscillation,TRUE,TRUE,5);

	gtk_widget_show(box);

	return box;
}

static GtkWidget * create_block_control(block_controller_s * bc)
{
	GtkWidget * frame;
	GtkWidget * block_control_console;

	frame = gtk_frame_new("Управление");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_frame_set_label_align(GTK_FRAME(frame),0.5,1);
	gtk_container_set_border_width(GTK_CONTAINER(frame),5);

	block_control_console = create_block_control_console(bc);

	gtk_container_add(GTK_CONTAINER(frame),block_control_console);

	gtk_widget_show(frame);

	return frame;
}

static flag_t	changed_block_controller(block_controller_s * bc
                                      ,config_controller_s * controller_config
                                      ,state_controller_s * controller_state)
{
	show_control_s * control = bc->control;
	show_state_s * state = bc->state;
	uint64_t flag = controller_config->flag;

	bc->type_device = get_type_device(controller_config);

	if(!ENGINE_VERTICAL(flag)){
		set_button_not_active(control->but_up);
		set_button_not_active(control->but_down);
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

/*****************************************************************************/

static show_state_s show_state;
static show_control_s show_control;
static block_controller_s block_controller;

int select_block_controller(controller_s * controller)
{
	communication_controller_s * cc = block_controller.communication_controller;
	GtkLabel * label;

	if(cc->tid == NULL){
		g_info("Нет подключения!");
		cc->current = NULL;
		return FAILURE;
	}

	if(controller == NULL){
		block_controller.stop_show = OK;
		g_mutex_lock(&(cc->mutex));
		cc->current = NULL;
		g_mutex_unlock(&(cc->mutex));
		return SUCCESS;
	}

	changed_block_controller(&block_controller,controller->config,controller->state);

	g_mutex_lock(&(cc->mutex));
	controller->control->command.all = COMMAND_EMPTY;
	cc->current = controller;
	g_mutex_unlock(&(cc->mutex));

	if(block_controller.run_show == NOT_OK){
	 	block_controller.run_show = OK;
		block_controller.stop_show = NOT_OK;
		g_timeout_add(block_controller.timeout_show,show_block_controller,&block_controller);
	}

	label = block_controller.name;
	gtk_label_set_text(label,controller->name);

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
	block_controller.communication_controller = &communication_controller;
	block_controller.state = &show_state;
	block_controller.control = &show_control;

	init_image(&block_controller);

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	frame_control = create_block_control(&block_controller);
	frame_state = create_block_state(&block_controller);

	gtk_box_pack_start(GTK_BOX(box),frame_control,TRUE,TRUE,5);
	gtk_box_pack_start(GTK_BOX(box),frame_state,TRUE,TRUE,5);

	gtk_widget_show(box);

	return box;
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

	controller = g_slice_alloc0(sizeof(controller_s));
	controller->link = g_slice_alloc0(sizeof(link_s));
	controller->config = g_slice_alloc0(sizeof(config_controller_s));
	controller->state = g_slice_alloc0(sizeof(state_controller_s));
	controller->control = g_slice_alloc0(sizeof(control_controller_s));
	controller->control->command.all =  COMMAND_EMPTY;
	/*память для обектов выделяется при чтении из базыданых*/
	rc = read_database_controller(number,controller);
	if(rc != SUCCESS){
		g_slice_free1(sizeof(controller_s),controller);
		g_slice_free1(sizeof(link_s),controller->link);
		g_slice_free1(sizeof(config_controller_s),controller->config);
		g_slice_free1(sizeof(state_controller_s),controller->state);
		controller = NULL;
		return NULL;
	}
	controller->name = get_name_controller(controller->config);

	communication_controller.list = g_slist_append(communication_controller.list,controller);
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
	link_disconnect_controller(link);
	str = link->address;
	g_free(str);
	g_slice_free1(sizeof(link_s),link);

	config = property->config;
	g_slice_free1(sizeof(config_controller_s),config);
	state = property->state;
	g_slice_free1(sizeof(state_controller_s),state);
	control = property->control;
	g_slice_free1(sizeof(control_controller_s),control);

	g_slice_free1(sizeof(controller_s),property);

	return SUCCESS;
}
/*****************************************************************************/
