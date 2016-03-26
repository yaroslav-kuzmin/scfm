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
	GtkImage * axis_vertical;
	GdkPixbuf * buf_axis_vertical;

	GtkImage * axis_horizontal;
	GdkPixbuf * buf_axis_horizontal;

	GtkImage * valve;
	GdkPixbuf * buf_valve;
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
	/*лафет*/
	GtkButton * but_up;
	GtkButton * but_down;
	GtkButton * but_right;
	GtkButton * but_left;

	GtkButton * but_valve;

	flag_t mode;
	GtkWidget * console;
};

typedef struct _block_controller_s block_controller_s;
struct _block_controller_s
{
	/*название блока */
	GtkLabel * name;
	/*отображение блока конторллера*/
	flag_t stop_show;
	flag_t run_show;
	uint32_t timeout_show;

	/**/
	communication_controller_s * communication_controller;

	/*отображение блока управления в разных режимах*/

	show_state_s * state;
	show_control_s * control;
};

/*****************************************************************************/
/* Функции работы с изображениями                                            */
/*****************************************************************************/
enum
{
	VERTICAL_0000 = 0,
	VERTICAL_0100,
	VERTICAL_0200,
	VERTICAL_0300,
	VERTICAL_0400,
	VERTICAL_0500,
	VERTICAL_0600,
	VERTICAL_0700,
	VERTICAL_0800,
	VERTICAL_0900,
	VERTICAL_UP,
	VERTICAL_DOWN,
	VERTICAL_NO_ENGINE, /*Нет двигателя */
	VERTICAL_NO_CONNECT, /*Нет сигнала*/
	HORIZONTAL_0000,
	HORIZONTAL_0100,
	HORIZONTAL_0200,
	HORIZONTAL_0300,
	HORIZONTAL_0400,
	HORIZONTAL_0500,
	HORIZONTAL_0600,
	HORIZONTAL_0700,
	HORIZONTAL_0800,
	HORIZONTAL_0900,
	HORIZONTAL_1000,
	HORIZONTAL_1100,
	HORIZONTAL_1200,
	HORIZONTAL_1300,
	HORIZONTAL_1400,
	HORIZONTAL_1500,
	HORIZONTAL_1600,
	HORIZONTAL_1700,
	HORIZONTAL_1800,
	HORIZONTAL_1900,
	HORIZONTAL_2000,
	HORIZONTAL_2100,
	HORIZONTAL_2200,
	HORIZONTAL_2300,
	HORIZONTAL_2400,
	HORIZONTAL_2500,
	HORIZONTAL_2600,
	HORIZONTAL_2700,
	HORIZONTAL_2800,
	HORIZONTAL_2900,
	HORIZONTAL_3000,
	HORIZONTAL_3100,
	HORIZONTAL_3200,
	HORIZONTAL_3300,
	HORIZONTAL_3400,
	HORIZONTAL_3500,
	HORIZONTAL_3600,
	HORIZONTAL_RIGHT,
	HORIZONTAL_LEFT,
	HORIZONTAL_NO_ENGINE,  /*нет двигателя*/
	HORIZONTAL_NO_CONNECT,  /*нет сигнала*/
	PRESSURE_NORM,
	PRESSURE_NOTNORM,
	VALVE_OPEN,
	VALVE_CLOSE,
	VALVE_OPEN_RUN,
	VALVE_CLOSE_RUN,
	VALVE_ERROR,
 	AMOUNT_IMAGE_STATE
};

static GdkPixbuf * images_state[AMOUNT_IMAGE_STATE] = {0};
static int16_t period_vertical = 10;
static int16_t min_vertical = 0;
static int16_t max_vertical = 90;
static gdouble period_horizontal = 10;
static int16_t min_horizontal = 0;
static int16_t max_horizontal = 360;


static int init_image(block_controller_s * bc)
{
	/*TODO высвободить память из буфферов*/

	images_state[VERTICAL_0000]       = get_resource_image(RESOURCE_IMAGE,"v0000");
	images_state[VERTICAL_0100]       = get_resource_image(RESOURCE_IMAGE,"v0100");
	images_state[VERTICAL_0200]       = get_resource_image(RESOURCE_IMAGE,"v0200");
	images_state[VERTICAL_0300]       = get_resource_image(RESOURCE_IMAGE,"v0300");
	images_state[VERTICAL_0400]       = get_resource_image(RESOURCE_IMAGE,"v0400");
	images_state[VERTICAL_0500]       = get_resource_image(RESOURCE_IMAGE,"v0500");
	images_state[VERTICAL_0600]       = get_resource_image(RESOURCE_IMAGE,"v0600");
	images_state[VERTICAL_0700]       = get_resource_image(RESOURCE_IMAGE,"v0700");
	images_state[VERTICAL_0800]       = get_resource_image(RESOURCE_IMAGE,"v0800");
	images_state[VERTICAL_0900]       = get_resource_image(RESOURCE_IMAGE,"v0900");
	images_state[VERTICAL_UP]         = get_resource_image(RESOURCE_IMAGE,"v0900p");
	images_state[VERTICAL_DOWN]       = get_resource_image(RESOURCE_IMAGE,"v0000p");
	images_state[VERTICAL_NO_ENGINE]  = get_resource_image(RESOURCE_IMAGE,"vxxxx");
	images_state[VERTICAL_NO_CONNECT] = get_resource_image(RESOURCE_IMAGE,"vbase");

	images_state[HORIZONTAL_0000]       = get_resource_image(RESOURCE_IMAGE,"h0000");
	images_state[HORIZONTAL_0100]       = get_resource_image(RESOURCE_IMAGE,"h0100");
	images_state[HORIZONTAL_0200]       = get_resource_image(RESOURCE_IMAGE,"h0200");
	images_state[HORIZONTAL_0300]       = get_resource_image(RESOURCE_IMAGE,"h0300");
	images_state[HORIZONTAL_0400]       = get_resource_image(RESOURCE_IMAGE,"h0400");
	images_state[HORIZONTAL_0500]       = get_resource_image(RESOURCE_IMAGE,"h0500");
	images_state[HORIZONTAL_0600]       = get_resource_image(RESOURCE_IMAGE,"h0600");
	images_state[HORIZONTAL_0700]       = get_resource_image(RESOURCE_IMAGE,"h0700");
	images_state[HORIZONTAL_0800]       = get_resource_image(RESOURCE_IMAGE,"h0800");
	images_state[HORIZONTAL_0900]       = get_resource_image(RESOURCE_IMAGE,"h0900");
	images_state[HORIZONTAL_1000]       = get_resource_image(RESOURCE_IMAGE,"h1000");
	images_state[HORIZONTAL_1100]       = get_resource_image(RESOURCE_IMAGE,"h1100");
	images_state[HORIZONTAL_1200]       = get_resource_image(RESOURCE_IMAGE,"h1200");
	images_state[HORIZONTAL_1300]       = get_resource_image(RESOURCE_IMAGE,"h1300");
	images_state[HORIZONTAL_1400]       = get_resource_image(RESOURCE_IMAGE,"h1400");
	images_state[HORIZONTAL_1500]       = get_resource_image(RESOURCE_IMAGE,"h1500");
	images_state[HORIZONTAL_1600]       = get_resource_image(RESOURCE_IMAGE,"h1600");
	images_state[HORIZONTAL_1700]       = get_resource_image(RESOURCE_IMAGE,"h1700");
	images_state[HORIZONTAL_1800]       = get_resource_image(RESOURCE_IMAGE,"h1800");
	images_state[HORIZONTAL_1900]       = get_resource_image(RESOURCE_IMAGE,"h1900");
	images_state[HORIZONTAL_2000]       = get_resource_image(RESOURCE_IMAGE,"h2000");
	images_state[HORIZONTAL_2100]       = get_resource_image(RESOURCE_IMAGE,"h2100");
	images_state[HORIZONTAL_2200]       = get_resource_image(RESOURCE_IMAGE,"h2200");
	images_state[HORIZONTAL_2300]       = get_resource_image(RESOURCE_IMAGE,"h2300");
	images_state[HORIZONTAL_2400]       = get_resource_image(RESOURCE_IMAGE,"h2400");
	images_state[HORIZONTAL_2500]       = get_resource_image(RESOURCE_IMAGE,"h2500");
	images_state[HORIZONTAL_2600]       = get_resource_image(RESOURCE_IMAGE,"h2600");
	images_state[HORIZONTAL_2700]       = get_resource_image(RESOURCE_IMAGE,"h2700");
	images_state[HORIZONTAL_2800]       = get_resource_image(RESOURCE_IMAGE,"h2800");
	images_state[HORIZONTAL_2900]       = get_resource_image(RESOURCE_IMAGE,"h2900");
	images_state[HORIZONTAL_3000]       = get_resource_image(RESOURCE_IMAGE,"h3000");
	images_state[HORIZONTAL_3100]       = get_resource_image(RESOURCE_IMAGE,"h3100");
	images_state[HORIZONTAL_3200]       = get_resource_image(RESOURCE_IMAGE,"h3200");
	images_state[HORIZONTAL_3300]       = get_resource_image(RESOURCE_IMAGE,"h3300");
	images_state[HORIZONTAL_3400]       = get_resource_image(RESOURCE_IMAGE,"h3400");
	images_state[HORIZONTAL_3500]       = get_resource_image(RESOURCE_IMAGE,"h3500");
	images_state[HORIZONTAL_3600]       = get_resource_image(RESOURCE_IMAGE,"h3600");
	images_state[HORIZONTAL_RIGHT]      = get_resource_image(RESOURCE_IMAGE,"h0000p");
	images_state[HORIZONTAL_LEFT]       = get_resource_image(RESOURCE_IMAGE,"h3600p");
	images_state[HORIZONTAL_NO_ENGINE]  = get_resource_image(RESOURCE_IMAGE,"hxxxx");
	images_state[HORIZONTAL_NO_CONNECT] = get_resource_image(RESOURCE_IMAGE,"hbase");

	images_state[PRESSURE_NORM]    = get_resource_image(RESOURCE_IMAGE,"pressure_norm");
	images_state[PRESSURE_NOTNORM] = get_resource_image(RESOURCE_IMAGE,"pressure_notnorm");

	images_state[VALVE_OPEN]      = get_resource_image(RESOURCE_IMAGE,"valve_open");
	images_state[VALVE_CLOSE]     = get_resource_image(RESOURCE_IMAGE,"valve_close");
	images_state[VALVE_OPEN_RUN]  = get_resource_image(RESOURCE_IMAGE,"valve_open_run");
	images_state[VALVE_CLOSE_RUN] = get_resource_image(RESOURCE_IMAGE,"valve_close_run");
	images_state[VALVE_ERROR]     = get_resource_image(RESOURCE_IMAGE,"valve_error");

	return SUCCESS;
}

static GdkPixbuf * get_image_vertical(int16_t angle)
{
	GdkPixbuf * buf = NULL;

	if(angle <= min_vertical){
		buf = images_state[VERTICAL_DOWN];
	}
	else{
		if(angle >= max_vertical){
			buf = images_state[VERTICAL_UP];
		}
		else{
			angle = angle / period_vertical;
			angle = angle + VERTICAL_0000;
			buf = images_state[angle];
		}
	}
	return buf;
}
static GdkPixbuf * get_image_horizontal(int16_t angle)
{
 	GdkPixbuf * buf = NULL;

	if(angle <= min_horizontal){
		buf = images_state[HORIZONTAL_RIGHT];
	}
	else{
		if(angle >= max_horizontal){
			buf = images_state[HORIZONTAL_RIGHT];
		}
		else{
			angle = angle / period_horizontal;
			angle = angle + HORIZONTAL_0000;
 			buf = images_state[angle];
 		}
	}
	return buf;
}

static GdkPixbuf * get_image_pressure(int pressure)
{
	GdkPixbuf * buf;
	switch(pressure){
		case PRESSURE_NORM:
			buf = images_state[PRESSURE_NORM];
	 		break;
		case PRESSURE_NOTNORM:
			buf = images_state[PRESSURE_NOTNORM];
			break;
		default:
			buf = images_state[PRESSURE_NOTNORM];
 			break;
	}
	return buf;
}

static GdkPixbuf * get_image_valve(unsigned int valve)
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
			buf = images_state[VALVE_CLOSE];
			break;
	}

	return buf;
}

#define MIN_VERTICAL_TIC      0
#define MAX_VERTICAL_TIC      30
#define MIN_VERTICAL_ANGLE    0
#define MAX_VERTICAL_ANGLE    90
static int16_t calculate_angle_tic_vertical(state_controller_s * state,config_controller_s * config)
{
	int16_t angle;
	uint16_t tic = state->tic_vertical;
	gdouble rate = config->rate_tic_vertical;

	if(tic > MAX_VERTICAL_TIC){
		tic = MAX_VERTICAL_TIC;
	}
	angle = (int16_t)tic;
	/*angle -= (MAX_VERTICAL_TIC >> 1);*/ /*Угол от -90 до 90*/
	rate = (gdouble)angle * rate;
	angle =(int16_t)rate;
	/*если коэффициент не корректный*/
	if(angle < MIN_VERTICAL_ANGLE){
		angle = MIN_VERTICAL_ANGLE;
	}
	if(angle > MAX_VERTICAL_ANGLE){
		angle = MAX_VERTICAL_ANGLE;
	}

 	return angle;
}
static int16_t calculate_angle_encoder_vertical(state_controller_s * state,config_controller_s * config)
{
	int16_t angle = 0;
	return angle;
}
#define MIN_HORIZONTAL_TIC      0
#define MAX_HORIZONTAL_TIC      60
#define MIN_HORIZONTAL_ANGLE    0
#define MAX_HORIZONTAL_ANGLE    360
static int16_t calculate_angle_tic_horizontal(state_controller_s * state,config_controller_s * config)
{
	int16_t angle;
	uint16_t tic = state->tic_horizontal;
	gdouble rate = config->rate_tic_horizontal;

	if(tic > MAX_HORIZONTAL_TIC){
		tic = MAX_HORIZONTAL_TIC;
	}
	rate = (gdouble)tic * rate;
	angle =(int16_t)rate;
	/*если коэффициент не корректный*/
	if(angle < MIN_HORIZONTAL_ANGLE){
		angle = MIN_HORIZONTAL_ANGLE;
	}
	if(angle > MAX_HORIZONTAL_ANGLE){
		angle = MAX_HORIZONTAL_ANGLE;
	}

	return angle;
}

static int16_t calculate_angle_encoder_horizontal(state_controller_s * state,config_controller_s * config)
{
	int16_t angle = 0;
	return angle;
}
static int16_t calculate_pressure(state_controller_s * state,config_controller_s * config)
{
	return PRESSURE_NORM;
}

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
				g_debug("read controller");
				g_mutex_lock(&(cc->mutex));
				copy_state(controller->state,&state);
				command = controller->control->command;
				g_mutex_unlock(&(cc->mutex));
				if(command.all != COMMAND_EMPTY){
					/*g_debug(" :> %ld ",command);*/
					rc = command_controller(link,command);
					if(rc == FAILURE){
						/*TODO сделать реконнект*/
						controller->object->status = STATUS_ERROR;
					}
					g_mutex_lock(&(cc->mutex));
					controller->control->command.all = COMMAND_EMPTY;
					g_mutex_unlock(&(cc->mutex));
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
static flag_t set_image(GtkImage * image,GdkPixbuf * buf_des,GdkPixbuf * buf_src)
{
	int width = gdk_pixbuf_get_width(buf_des);
	int height = gdk_pixbuf_get_height(buf_des);
	gdk_pixbuf_copy_area(buf_src,0,0,width,height,buf_des,0,0);

	gtk_image_set_from_pixbuf(image,buf_des);
	return SUCCESS;
}

static int show_vertical(show_state_s * show_state,show_control_s * show_controls
                        ,state_controller_s * controller_state,config_controller_s * controller_config)
{
	uint64_t flag = controller_config->flag;
	int16_t angle = 0;
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
	angle_image = get_image_vertical(angle);
	set_image(show_state->axis_vertical,show_state->buf_axis_vertical,angle_image);

	return SUCCESS;
}

static int show_horizontal(show_state_s * show_state,show_control_s * show_controls
                          ,state_controller_s * controller_state,config_controller_s * controller_config)
{
	uint64_t flag = controller_config->flag;
	int16_t angle = 0;
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

	angle_image = get_image_horizontal(angle);
	set_image(show_state->axis_horizontal,show_state->buf_axis_horizontal,angle_image);

	return SUCCESS;
}

#if 0
static int show_pipe(block_controller_s * bc)
{
	communication_controller_s * cc = bc->communication_controller;
	controller_s * controller = cc->current;
	GdkPixbuf * buf = bc->state->buf_pipe;
	GtkImage * image = bc->state->pipe;

	int pressure = calculate_pressure(controller);
	int valve = get_state_valve(controller->state);

	GdkPixbuf * pressure_image = get_image_pressure(pressure);
	GdkPixbuf * valve_image = get_image_valve(valve);
	int width = gdk_pixbuf_get_width(pressure_image);
	int height = gdk_pixbuf_get_height(pressure_image);

	/*TODO маштабирование */
	gdk_pixbuf_copy_area(pressure_image ,0,0,width,height,buf,0    ,0);
	gdk_pixbuf_copy_area(valve_image    ,0,0,width,height,buf,width,0);
	gtk_image_set_from_pixbuf(image,buf);
	return SUCCESS;
}

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
#if 0
	show_pipe(bc);
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

	frame = gtk_frame_new("Вертикальная Ось");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	buf = images_state[VERTICAL_NO_CONNECT];
	state->buf_axis_vertical = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование */
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_AXIS_VERTICAL,DEFAULT_SIZE_HEIGHT_AXIS_VERTICAL);*/
	state->axis_vertical = GTK_IMAGE(image);

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

	frame = gtk_frame_new("Горизонтальная Ось");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	buf = images_state[HORIZONTAL_NO_CONNECT];
	state->buf_axis_horizontal = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование*/
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_AXIS_HORIZONTAL,DEFAULT_SIZE_HEIGHT_AXIS_HORIZONTAL);*/
	state->axis_horizontal = GTK_IMAGE(image);

	gtk_container_add(GTK_CONTAINER(frame),image);

	gtk_widget_show(frame);
	gtk_widget_show(image);

	return frame;
}

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


static GtkWidget * create_block_state(block_controller_s * bc)
{
	GtkWidget * frame;
	GtkWidget * grid;
	GtkWidget * label_name;
	GtkWidget * block_vertical;
	GtkWidget * block_horizontal;
	GtkWidget * block_valve;
	/*GtkWidget * block_fire_sensor;*/
	/*GtkWidget * block_fire_alarm;*/

	frame = gtk_frame_new("Информация");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,FALSE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid),FALSE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid),FALSE);

	label_name = gtk_label_new("Нет подключения к контролеру!");
	layout_widget(label_name,GTK_ALIGN_CENTER,GTK_ALIGN_START,TRUE,TRUE);

	bc->name = GTK_LABEL(label_name);

	block_vertical = create_block_state_vertical(bc->state);
	block_horizontal = create_block_state_horizontal(bc->state);
	block_valve = create_block_state_valve(bc->state);
#if 0
	block_fire_sensor = create_block_fire_sensor(bc);
	block_fire_alarm = create_block_fire_alarm(bc);
#endif

	gtk_container_add(GTK_CONTAINER(frame),grid);

	gtk_grid_attach(GTK_GRID(grid),label_name       ,0,0,4,1);
	gtk_grid_attach(GTK_GRID(grid),block_vertical   ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_horizontal ,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_valve      ,0,2,2,1);
#if 0
	gtk_grid_attach(GTK_GRID(grid),block_fire_sensor,2,1,1,2);
	gtk_grid_attach(GTK_GRID(grid),block_fire_alarm ,3,1,1,2);
#endif
	gtk_widget_show(frame);
	gtk_widget_show(grid);
	gtk_widget_show(label_name);

	return frame;
}

/***** Функции отображения системы управления ********************************/


static flag_t push_command_queue(communication_controller_s * cc,controller_s * controller,command_u command)
{
	control_controller_s * control = controller->control;

	g_mutex_lock(&(cc->mutex));
	if(control->command.all == COMMAND_EMPTY){
		control->command = command;
	}
	g_mutex_unlock(&(cc->mutex));

	return SUCCESS;
}

/*static char STR_MODE_AUTO[]   = "Автоматический режим";*/
static char STR_MODE_MANUAL[] = "РУЧНОЙ РЕЖИМ";

static GtkWidget * create_block_control_mode(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * label;
	/*GtkWidget * but;*/

	bc->control->mode = MODE_MANUAL;
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,TRUE);

	label = gtk_label_new(STR_MODE_MANUAL);
	layout_widget(label,GTK_ALIGN_START,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_label_set_xalign(GTK_LABEL(label),0);

	/*but = gtk_button_new_with_label("")*/

	gtk_box_pack_start(GTK_BOX(box),label,TRUE,TRUE,0);
	gtk_widget_show(box);
	gtk_widget_show(label);
	return box;
}

static char STR_NAME_BUTTON_VALVE_OPEN[] =  "Открыть";
static char STR_NAME_BUTTON_VALVE_CLOSE[] = "Закрыть";
static void clicked_button_valve(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * controller = cc->current;
	state_controller_s state;
	flag_t flag;
	/*command_u command = {0};*/
	/*char * name;*/

	if( controller == NULL){
		g_info("Не выбран контролер");
		return ;
	}

	g_mutex_lock(&(cc->mutex));
	copy_state(&state,controller->state);
	g_mutex_unlock(&(cc->mutex));

	flag = get_state_valve(&state);
	--
#if 0
	if(bc->control->but_valve == STATE_VALVE_CLOSE){
		command.part.value = COMMAND_VALVE_OPEN;
		bc->control->but_valve = STATE_VALVE_OPEN;
		name = STR_NAME_BUTTON_VALVE_CLOSE;
	}
	else{
		command.part.value = COMMAND_VALVE_CLOSE;
		bc->control->but_valve = STATE_VALVE_CLOSE;
		name = STR_NAME_BUTTON_VALVE_OPEN;
	}
	push_command_queue(communication_controller,controller,command);
	gtk_button_set_label(b,name);
#endif
}

static gdouble min_valve = 0;
static gdouble max_valve = 4000;
static gdouble step_valve = 10;
static gboolean change_value_scale_valve(GtkRange * r,GtkScrollType s,gdouble v,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;
	command_u command = {0};
	double valve_d = v;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return FALSE;
	}

	push_command_queue(communication_controller,controller,command);
	gtk_range_set_value(r,valve_d);
	return TRUE;
}

static GtkWidget * create_block_control_valve(block_controller_s * bc)
{
	GtkWidget * grid;
	GtkWidget * label;
	GtkWidget * but;
	/*GtkWidget * scale;*/

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label = gtk_label_new("Заслонка");

	but = gtk_button_new_with_label(STR_NAME_BUTTON_VALVE_OPEN);
	layout_widget(but,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but,"clicked",G_CALLBACK(clicked_button_valve),bc);
	bc->control->but_valve = GTK_BUTTON(but);
#if 0
	scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,min_valve,max_valve,step_valve);
	layout_widget(scale,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_scale_set_digits(GTK_SCALE(scale),0); /*колличество знаков после запятой*/
	gtk_scale_set_value_pos(GTK_SCALE(scale),GTK_POS_RIGHT);
	g_signal_connect(scale,"change-value",G_CALLBACK(change_value_scale_valve),bc);
#endif
	gtk_grid_attach(GTK_GRID(grid),label,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),but  ,0,1,1,1);
	/*gtk_grid_attach(GTK_GRID(grid),scale,0,2,1,1);*/

	gtk_widget_show(grid);
	gtk_widget_show(label);
	gtk_widget_show(but);
	/*gtk_widget_show(scale);*/

	return grid;
}

static flag_t set_button_not_active(GtkButton * but)
{
	return SUCCESS;
}

static void button_press_event_button_up(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;

	command_u command = {0};
	command.part.value = COMMAND_LAFET_UP;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}

	push_command_queue(communication_controller,controller,command);
}
static void button_press_event_button_down(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;

	command_u command = {0};
	command.part.value = COMMAND_LAFET_DOWN;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	push_command_queue(communication_controller,controller,command);
}

static void button_press_event_button_right(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;

	command_u command = {0};
	command.part.value = COMMAND_LAFET_RIGHT;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	push_command_queue(communication_controller,controller,command);
}

static void button_press_event_button_left(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;

	command_u command = {0};
	command.part.value = COMMAND_LAFET_LEFT;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	push_command_queue(communication_controller,controller,command);
}

static void button_release_event_button_stop(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;

	command_u command = {0};
	command.part.value = COMMAND_LAFET_STOP;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	push_command_queue(communication_controller,controller,command);
}

static GtkWidget * create_block_control_lafet(block_controller_s * bc)
{
	GtkWidget * grid;
	GtkWidget * but_up;
	GtkWidget * but_down;
	GtkWidget * but_right;
	GtkWidget * but_left;

	grid = gtk_grid_new();

	but_up = gtk_button_new_with_label("ВВЕРХ");
	layout_widget(but_up,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);
	g_signal_connect(but_up,"button-press-event",G_CALLBACK(button_press_event_button_up),bc);
	g_signal_connect(but_up,"button-release-event",G_CALLBACK(button_release_event_button_stop),bc);
	bc->control->but_up = GTK_BUTTON(but_up);

	but_down = gtk_button_new_with_label("ВНИЗ");
	layout_widget(but_down,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);
	g_signal_connect(but_down,"button-press-event",G_CALLBACK(button_press_event_button_down),bc);
	g_signal_connect(but_down,"button-release-event",G_CALLBACK(button_release_event_button_stop),bc);
	bc->control->but_down = GTK_BUTTON(but_down);

	but_right = gtk_button_new_with_label("ВПРАВО");
	layout_widget(but_right,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);
	g_signal_connect(but_right,"button-press-event",G_CALLBACK(button_press_event_button_right),bc);
	g_signal_connect(but_right,"button-release-event",G_CALLBACK(button_release_event_button_stop),bc);
	bc->control->but_right = GTK_BUTTON(but_right);

	but_left = gtk_button_new_with_label("ВЛЕВО");
	layout_widget(but_left,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);
	g_signal_connect(but_left,"button-press-event",G_CALLBACK(button_press_event_button_left),bc);
	g_signal_connect(but_left,"button-release-event",G_CALLBACK(button_release_event_button_stop),bc);
	bc->control->but_left = GTK_BUTTON(but_left);

	gtk_grid_attach(GTK_GRID(grid),but_up   ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_down ,1,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_left ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_right,2,1,1,1);

	gtk_widget_show(grid);
	gtk_widget_show(but_up);
	gtk_widget_show(but_down);
	gtk_widget_show(but_right);
	gtk_widget_show(but_left);

 	return grid;
}

static gdouble min_spray = 0;
static gdouble max_spray = 100;
static gdouble step_spray = 1;
static gdouble min_rate = 0;
static gdouble max_rate = 100;
static gdouble step_rate = 1;
static GtkWidget * create_block_actuator(block_controller_s * bc)
{
	GtkWidget * grid;
	GtkWidget * label_spray;
	GtkWidget * scale_spray;
	GtkWidget * label_rate;
	GtkWidget * scale_rate;

	grid = gtk_grid_new();

	label_spray = gtk_label_new("Распыл");
	layout_widget(label_spray,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	scale_spray = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,min_spray,max_spray,step_spray);
	layout_widget(scale_spray,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label_rate = gtk_label_new("Литраж");
	layout_widget(label_rate,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	scale_rate = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,min_rate,max_rate,step_rate);
	layout_widget(scale_rate,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	gtk_grid_attach(GTK_GRID(grid),label_spray,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),scale_spray,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),label_rate ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),scale_rate ,1,1,1,1);

	gtk_widget_show(grid);
	gtk_widget_show(label_spray);
	gtk_widget_show(scale_spray);
	gtk_widget_show(label_rate);
	gtk_widget_show(scale_rate);

	return grid;
}

static void clicked_button_oscillation_vertical(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;

	command_u command = {0};
	command.part.value = COMMAND_OSCILLATION_VERTICAL;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	push_command_queue(communication_controller,controller,command);
}
static void clicked_button_oscillation_horizontal(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;

	command_u command = {0};
	command.part.value = COMMAND_OSCILLATION_HORIZONTAL;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	push_command_queue(communication_controller,controller,command);
}
static void clicked_button_oscillation_saw(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;

	command_u command = {0};
	command.part.value = COMMAND_OSCILLATION_SAW;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	push_command_queue(communication_controller,controller,command);
}
static void clicked_button_oscillation_step(GtkButton * b,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * communication_controller = bc->communication_controller;
	controller_s * controller = communication_controller->current;

	command_u command = {0};
	command.part.value = COMMAND_OSCILLATION_STEP;

	if( controller == NULL){
		g_info("Не выбран контролер");
		return;
	}
	push_command_queue(communication_controller,controller,command);
}

static GtkWidget * create_block_oscillation(block_controller_s * bc)
{
	GtkWidget * grid;
	GtkWidget * but_vertical;
	GtkWidget * but_horizontal;
	GtkWidget * but_saw;
	GtkWidget * but_step;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	but_vertical = gtk_radio_button_new_with_label(NULL,"Вертикальная");
	g_signal_connect(but_vertical,"clicked",G_CALLBACK(clicked_button_oscillation_vertical),bc);

	but_horizontal = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(but_vertical)
	                                                            ,"Горизонтальная");
	g_signal_connect(but_horizontal,"clicked",G_CALLBACK(clicked_button_oscillation_horizontal),bc);

	but_saw = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(but_vertical)
	                                                     ,"Пила");
	g_signal_connect(but_saw,"clicked",G_CALLBACK(clicked_button_oscillation_saw),bc);

	but_step = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(but_vertical)
	                                                      ,"Ступенька");
	g_signal_connect(but_step,"clicked",G_CALLBACK(clicked_button_oscillation_step),bc);

	gtk_grid_attach(GTK_GRID(grid),but_vertical  ,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_horizontal,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_saw       ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_step      ,1,1,1,1);

	gtk_widget_show(grid);
	gtk_widget_show(but_vertical);
	gtk_widget_show(but_horizontal);
	gtk_widget_show(but_saw);
	gtk_widget_show(but_step);

	return grid;
}

static GtkWidget * create_block_control_console(block_controller_s * bc)
{
	GtkWidget * box;
	GtkWidget * block_lafet;
	GtkWidget * block_valve;
	/*GtkWidget * block_actuator;*/
	/*GtkWidget * block_oscillation;*/

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
  layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_lafet = create_block_control_lafet(bc);
	block_valve = create_block_control_valve(bc);
#if 0
	block_actuator = create_block_actuator(bc);
	block_oscillation = create_block_oscillation(bc);
#endif

	gtk_box_pack_start(GTK_BOX(box),block_lafet,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_valve,TRUE,TRUE,0);
#if 0
	gtk_box_pack_start(GTK_BOX(box),block_actuator,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_oscillation,TRUE,TRUE,0);
#endif
	gtk_widget_show(box);

	return box;
}

static GtkWidget * create_block_control(block_controller_s * bc)
{
	GtkWidget * frame;
	GtkWidget * box;
	GtkWidget * block_control_mode;
	GtkWidget * block_control_console;


	frame = gtk_frame_new("Управление");
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_frame_set_label_align(GTK_FRAME(frame),0.5,1);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	block_control_mode = create_block_control_mode(bc);

	block_control_console = create_block_control_console(bc);

	gtk_container_add(GTK_CONTAINER(frame),box);

	gtk_box_pack_start(GTK_BOX(box),block_control_mode,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),block_control_console,TRUE,TRUE,0);

	gtk_widget_show(frame);
	gtk_widget_show(box);

	return frame;
}

static flag_t	changed_block_controller(block_controller_s * bc
                                      ,config_controller_s * controller_config
                                      ,state_controller_s * controller_state)
{
	show_control_s * control = bc->control;
	show_state_s * state = bc->state;
	uint64_t flag = controller_config->flag;

	if(!ENGINE_VERTICAL(flag)){
		set_button_not_active(control->but_up);
		set_button_not_active(control->but_down);
		state->buf_axis_vertical = images_state[VERTICAL_NO_ENGINE];
		gtk_image_set_from_pixbuf(state->axis_vertical,state->buf_axis_vertical);
	}

	if(!ENGINE_HORIZONTAL(flag)){
		set_button_not_active(control->but_left);
		set_button_not_active(control->but_right);
		state->buf_axis_vertical = images_state[HORIZONTAL_NO_ENGINE];
		gtk_image_set_from_pixbuf(state->axis_horizontal,state->buf_axis_horizontal);
	}

#if 0
	if(ACTUATOR_SPRAY(flag)){
	}
	if(ACTUATOR_RATE(flag)){
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
	if(PRESSURE(flag)){
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
	if(VALVE_DRY(flag)){
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

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	frame_state = create_block_state(&block_controller);

	frame_control = create_block_control(&block_controller);

	gtk_box_pack_start(GTK_BOX(box),frame_state,TRUE,TRUE,5);
	gtk_box_pack_start(GTK_BOX(box),frame_control,TRUE,TRUE,5);

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
	g_info(" %s ",controller->name);
#if 0
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
