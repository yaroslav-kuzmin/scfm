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

#include "pub.h"
#include "common.h"
#include "database.h"
#include "device.h"

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

struct _communication_controller_s
{
	int exit;

	GThread * tid;
	GMutex m_flag;
	GMutex m_state;
	GMutex m_control;

	GSList * list;
	controller_s * current;

	uint32_t timeout_current;
	uint32_t timeout_all;
	uint32_t timeout_config;
};
typedef struct _communication_controller_s communication_controller_s;

struct _block_controller_s
{
	int stop_show;
	uint32_t timeout_show;
	communication_controller_s * communication_controller;
	GtkLabel * name;

	GtkImage * axis_vertical;
	GdkPixbuf * buf_axis_vertical;
	GtkImage * axis_horizontal;
	GdkPixbuf * buf_axis_horizontal;
	GtkImage * pressure_valve;
};
typedef struct _block_controller_s block_controller_s;

/*****************************************************************************/
/* Функции работы с изображениями                                            */
/*****************************************************************************/
enum {
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
	AMOUNT_IMAGE_VERTICAL
};
static char * STR_IMAGE_VERTICAL[AMOUNT_IMAGE_VERTICAL] = {0};
static GdkPixbuf * IMAGES_VERTICAL[AMOUNT_IMAGE_VERTICAL] = {0};
static int16_t period_vertical = 10;
static int16_t min_vertical = 0;
static int16_t max_vertical = 90;

enum {
	HORIZONTAL_0000 = 0,
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
	AMOUNT_IMAGE_HORIZONTAL
};
static char * STR_IMAGE_HORIZONTAL[AMOUNT_IMAGE_HORIZONTAL] = {0};
static GdkPixbuf * IMAGES_HORIZONTAL[AMOUNT_IMAGE_HORIZONTAL] = {0};
static gdouble period_horizontal = 10;
static int16_t min_horizontal = 0;
static int16_t max_horizontal = 360;

static GdkPixbuf * get_pixbuf(const char * name)
{
	GError * err = NULL;
	GdkPixbuf * buf;
	buf = gdk_pixbuf_new_from_resource(name,&err);
	if(buf == NULL){
		g_warning("Not image : %s",err->message);
		g_error_free(err);
	}
	return buf;
}

static int init_image(block_controller_s * bc)
{
	/*TODO высвободить память из буфферов*/

	STR_IMAGE_VERTICAL[VERTICAL_0000] = "/image/v0000.png";
	STR_IMAGE_VERTICAL[VERTICAL_0100] = "/image/v0100.png";
	STR_IMAGE_VERTICAL[VERTICAL_0200] = "/image/v0200.png";
	STR_IMAGE_VERTICAL[VERTICAL_0300] = "/image/v0300.png";
	STR_IMAGE_VERTICAL[VERTICAL_0400] = "/image/v0400.png";
	STR_IMAGE_VERTICAL[VERTICAL_0500] = "/image/v0500.png";
	STR_IMAGE_VERTICAL[VERTICAL_0600] = "/image/v0600.png";
	STR_IMAGE_VERTICAL[VERTICAL_0700] = "/image/v0700.png";
	STR_IMAGE_VERTICAL[VERTICAL_0800] = "/image/v0800.png";
	STR_IMAGE_VERTICAL[VERTICAL_0900] = "/image/v0900.png";
	STR_IMAGE_VERTICAL[VERTICAL_UP]   = "/image/v0900p.png";
	STR_IMAGE_VERTICAL[VERTICAL_DOWN] = "/image/v0000p.png";

	IMAGES_VERTICAL[VERTICAL_0000] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0000]);
	IMAGES_VERTICAL[VERTICAL_0100] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0100]);
	IMAGES_VERTICAL[VERTICAL_0200] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0200]);
	IMAGES_VERTICAL[VERTICAL_0300] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0300]);
	IMAGES_VERTICAL[VERTICAL_0400] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0400]);
	IMAGES_VERTICAL[VERTICAL_0500] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0500]);
	IMAGES_VERTICAL[VERTICAL_0600] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0600]);
	IMAGES_VERTICAL[VERTICAL_0700] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0700]);
	IMAGES_VERTICAL[VERTICAL_0800] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0800]);
	IMAGES_VERTICAL[VERTICAL_0900] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_0900]);
	IMAGES_VERTICAL[VERTICAL_UP]   = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_UP]);
	IMAGES_VERTICAL[VERTICAL_DOWN] = get_pixbuf(STR_IMAGE_VERTICAL[VERTICAL_DOWN]);

	STR_IMAGE_HORIZONTAL[HORIZONTAL_0000]  = "/image/h0000.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_0100]  = "/image/h0100.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_0200]  = "/image/h0200.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_0300]  = "/image/h0300.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_0400]  = "/image/h0400.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_0500]  = "/image/h0500.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_0600]  = "/image/h0600.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_0700]  = "/image/h0700.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_0800]  = "/image/h0800.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_0900]  = "/image/h0900.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1000]  = "/image/h1000.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1100]  = "/image/h1100.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1200]  = "/image/h1200.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1300]  = "/image/h1300.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1400]  = "/image/h1400.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1500]  = "/image/h1500.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1600]  = "/image/h1600.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1700]  = "/image/h1700.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1800]  = "/image/h1800.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_1900]  = "/image/h1900.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2000]  = "/image/h2000.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2100]  = "/image/h2100.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2200]  = "/image/h2200.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2300]  = "/image/h2300.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2400]  = "/image/h2400.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2500]  = "/image/h2500.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2600]  = "/image/h2600.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2700]  = "/image/h2700.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2800]  = "/image/h2800.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_2900]  = "/image/h2900.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_3000]  = "/image/h3000.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_3100]  = "/image/h3100.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_3200]  = "/image/h3200.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_3300]  = "/image/h3300.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_3400]  = "/image/h3400.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_3500]  = "/image/h3500.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_3600]  = "/image/h3600.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_RIGHT] = "/image/h0000p.png";
	STR_IMAGE_HORIZONTAL[HORIZONTAL_LEFT]  = "/image/h3600p.png";

	IMAGES_HORIZONTAL[HORIZONTAL_0000]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0000]);
	IMAGES_HORIZONTAL[HORIZONTAL_0100]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0100]);
	IMAGES_HORIZONTAL[HORIZONTAL_0200]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0200]);
	IMAGES_HORIZONTAL[HORIZONTAL_0300]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0300]);
	IMAGES_HORIZONTAL[HORIZONTAL_0400]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0400]);
	IMAGES_HORIZONTAL[HORIZONTAL_0500]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0500]);
	IMAGES_HORIZONTAL[HORIZONTAL_0600]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0600]);
	IMAGES_HORIZONTAL[HORIZONTAL_0700]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0700]);
	IMAGES_HORIZONTAL[HORIZONTAL_0800]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0800]);
	IMAGES_HORIZONTAL[HORIZONTAL_0900]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_0900]);
	IMAGES_HORIZONTAL[HORIZONTAL_1000]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1000]);
	IMAGES_HORIZONTAL[HORIZONTAL_1100]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1100]);
	IMAGES_HORIZONTAL[HORIZONTAL_1200]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1200]);
	IMAGES_HORIZONTAL[HORIZONTAL_1300]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1300]);
	IMAGES_HORIZONTAL[HORIZONTAL_1400]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1400]);
	IMAGES_HORIZONTAL[HORIZONTAL_1500]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1500]);
	IMAGES_HORIZONTAL[HORIZONTAL_1600]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1600]);
	IMAGES_HORIZONTAL[HORIZONTAL_1700]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1700]);
	IMAGES_HORIZONTAL[HORIZONTAL_1800]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1800]);
	IMAGES_HORIZONTAL[HORIZONTAL_1900]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_1900]);
	IMAGES_HORIZONTAL[HORIZONTAL_2000]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2000]);
	IMAGES_HORIZONTAL[HORIZONTAL_2100]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2100]);
	IMAGES_HORIZONTAL[HORIZONTAL_2200]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2200]);
	IMAGES_HORIZONTAL[HORIZONTAL_2300]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2300]);
	IMAGES_HORIZONTAL[HORIZONTAL_2400]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2400]);
	IMAGES_HORIZONTAL[HORIZONTAL_2500]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2500]);
	IMAGES_HORIZONTAL[HORIZONTAL_2600]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2600]);
	IMAGES_HORIZONTAL[HORIZONTAL_2700]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2700]);
	IMAGES_HORIZONTAL[HORIZONTAL_2800]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2800]);
	IMAGES_HORIZONTAL[HORIZONTAL_2900]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_2900]);
	IMAGES_HORIZONTAL[HORIZONTAL_3000]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_3000]);
	IMAGES_HORIZONTAL[HORIZONTAL_3100]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_3100]);
	IMAGES_HORIZONTAL[HORIZONTAL_3200]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_3200]);
	IMAGES_HORIZONTAL[HORIZONTAL_3300]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_3300]);
	IMAGES_HORIZONTAL[HORIZONTAL_3400]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_3400]);
	IMAGES_HORIZONTAL[HORIZONTAL_3500]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_3500]);
	IMAGES_HORIZONTAL[HORIZONTAL_3600]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_3600]);
	IMAGES_HORIZONTAL[HORIZONTAL_RIGHT] = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_RIGHT]);
	IMAGES_HORIZONTAL[HORIZONTAL_LEFT]  = get_pixbuf(STR_IMAGE_HORIZONTAL[HORIZONTAL_LEFT]);

	return SUCCESS;
}

static GdkPixbuf * get_image_vertical(int16_t angle)
{
	GdkPixbuf * buf = NULL;

	if(angle <= min_vertical){
		buf = IMAGES_VERTICAL[VERTICAL_DOWN];
	}
	else{
		if(angle >= max_vertical){
			buf = IMAGES_VERTICAL[VERTICAL_UP];
		}
		else{
			angle = angle / period_vertical;
			buf = IMAGES_VERTICAL[angle];
		}
	}
	return buf;
}
static GdkPixbuf * get_image_horizontal(int16_t angle)
{
	GdkPixbuf * buf = NULL;

	if(angle <= min_horizontal){
		buf = IMAGES_HORIZONTAL[HORIZONTAL_RIGHT];
	}
	else{
		if(angle >= max_horizontal){
			buf = IMAGES_HORIZONTAL[HORIZONTAL_RIGHT];
		}
		else{
			angle = angle / period_horizontal;
			buf = IMAGES_HORIZONTAL[angle];
		}
	}
	return buf;
}
#define MIN_VERTICAL_TIC      0
#define MAX_VERTICAL_TIC      30
#define MIN_VERTICAL_ANGLE    0
#define MAX_VERTICAL_ANGLE    90
static int16_t calculate_angle_tic_vertical(controller_s * controller)
{
	config_controller_s * config = controller->config;
	state_controller_s * state = controller->state;
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
#define MIN_HORIZONTAL_TIC      0
#define MAX_HORIZONTAL_TIC      60
#define MIN_HORIZONTAL_ANGLE    0
#define MAX_HORIZONTAL_ANGLE    360
static int16_t calculate_angle_tic_horizontal(controller_s * controller)
{
	config_controller_s * config = controller->config;
	state_controller_s * state = controller->state;
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
		g_warning("Контролер подключен!");
		return SUCCESS;
	}
	rc = check_link_controller(link,&check,state);
	if(rc == FAILURE){
		g_warning("Нет подключения к контроллеру!");
		return rc;
	}
	rc = check_config_controller(&check,config);
	if(rc == FAILURE){
		/*TODO перезапись базы данных*/
		g_warning("Данные из контроллера не совпадают с данными из базы данных!");
		link_disconnect_controller(link);
		return rc;
	}
	return SUCCESS;
}

static int disconnect_controller(controller_s * controller)
{
	link_s * link = controller->link;
	if(link->connect == NULL){
		g_warning("Контролер не подключен!");
		return FAILURE;
	}
	return link_disconnect_controller(link);
}

/*uint32_t debug_id = 0;*/
/* функция  потока комуникации с контролерами */
static gpointer controllers_communication(gpointer ud)
{
	int rc;
	communication_controller_s * cc = (communication_controller_s *)ud;
	controller_s * controller;
	link_s * link;
	state_controller_s * state;
	control_controller_s * control;
	GQueue * queue;
	uint64_t command;
	/*GSList * list;*/

	for(;;){
		g_mutex_lock(&(cc->m_flag));
		controller = cc->current;
		g_mutex_unlock(&(cc->m_flag));
		if(controller != NULL){
			link = controller->link;
			state = controller->state;
			rc = link_state_controller(link,state);
			if(rc == FAILURE){
				/*TODO сделать реконнект*/
				/*g_debug("reconnect");*/
			}
			g_debug("read controller");
			control = controller->control;
			queue = control->command;
			g_mutex_lock(&(cc->m_control));
			command = POINTER_TO_INT(g_queue_pop_tail(queue));
			g_mutex_unlock(&(cc->m_control));
			if(command != COMMAND_EMPTY){
				/*g_debug(" :> %ld ",command);*/
				rc = command_controller(link,command);
				if(rc == FAILURE){
					/*TODO сделать реконнект*/
					/*g_debug("reconnect");*/
				}
			}
		}
#if 0
		/*проверка всех */
		list = ac->list;
		for(;list;){
			controller = (controller_s*)list->data;
			control = controller->control;
			g_mutex_lock(&(cc->flag));
			if(control->select == OK ){
				g_mutex_unlock(&(cc->m_flag));
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
		g_mutex_lock(&(cc->m_flag));
		if(cc->exit == OK){
			g_mutex_unlock(&(cc->m_flag));
			g_thread_exit(cc->tid);
		}
		g_mutex_unlock(&(cc->m_flag));
	}
	return NULL;
}

static int control_controllers_on(communication_controller_s * cc)
{
	int rc;
	GSList * list = cc->list;

	if(list == NULL){
		g_info("Нет контролеров");
		return FAILURE;
	}

	if(cc->tid != NULL){
		g_warning("Поток коммуникации уже запущен!");
		return FAILURE;
	}

	for(;list;){
		controller_s * controller = (controller_s*)list->data;
		rc = connect_controller(controller);
		if(rc == SUCCESS){
			g_info("Подключился к %s",controller->name);
		}
		else{
			g_info("Несмог подключится к %s",controller->name);
		}
		list = g_slist_next(list);
	}

	cc->exit = NOT_OK;
	g_mutex_init(&(cc->m_flag));
	g_mutex_init(&(cc->m_state));
	g_mutex_init(&(cc->m_control));
	cc->tid = g_thread_new("controller",controllers_communication,cc);

	return FAILURE;
}

static int control_controllers_off(communication_controller_s * cc)
{
	int rc;
	GSList * list = cc->list;

	if(cc->tid != NULL){/*поток запущен*/
		cc->exit = OK;
		g_thread_join(cc->tid);
		g_mutex_clear(&(cc->m_flag));
		g_mutex_clear(&(cc->m_state));
		g_mutex_clear(&(cc->m_control));
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
		}
		list = g_slist_next(list);
	}
	return FAILURE;
}

static communication_controller_s communication_controller;

int control_controllers(int mode)
{
	int rc = FAILURE;
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

/* Функции отрисовки по таймеру*/
static int show_vertical(block_controller_s * bc)
{
	communication_controller_s * cc = bc->communication_controller;
	controller_s * controller = cc->current;
	GdkPixbuf * buf = bc->buf_axis_vertical;
	GtkImage * image = bc->axis_vertical;
	int16_t angle = calculate_angle_tic_vertical(controller);
	GdkPixbuf * angle_image = 	get_image_vertical(angle);
	int width = gdk_pixbuf_get_width(buf);
	int height = gdk_pixbuf_get_height(buf);
	/*TODO маштабирование */
	gdk_pixbuf_copy_area(angle_image,0,0,width,height,buf,0,0);
	gtk_image_set_from_pixbuf(image,buf);
	return SUCCESS;
}

static int show_horizontal(block_controller_s * bc)
{
	communication_controller_s * cc = bc->communication_controller;
	controller_s * controller = cc->current;
	GdkPixbuf * buf = bc->buf_axis_horizontal;
	GtkImage * image = bc->axis_horizontal;
	int16_t angle = calculate_angle_tic_horizontal(controller);
	GdkPixbuf * angle_image = 	get_image_horizontal(angle);
	int width = gdk_pixbuf_get_width(buf);
	int height = gdk_pixbuf_get_height(buf);
	/*TODO маштабирование */
	gdk_pixbuf_copy_area(angle_image,0,0,width,height,buf,0,0);
	gtk_image_set_from_pixbuf(image,buf);
	return SUCCESS;
}

static int show_block_controler(gpointer data)
{
	/*GtkLabel * label;*/
	block_controller_s * bc = (block_controller_s *)data;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * c = cc->current;
	/*state_controller_s * state;*/
	/*uint64_t flag;*/

	if(bc->stop_show == OK ){
		return FALSE; /*завершить работу*/
	}
	if(c == NULL){
		/*контролер не выбран*/
		return FALSE;
	}
	show_vertical(bc);
	show_horizontal(bc);
	/*state = c->state;*/
	/*flag = c->config->flag;*/

	return TRUE; /* продолжаем работу */
}

/*****************************************************************************/
static void button_press_event_button_up(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * c = cc->current;
	control_controller_s * control;
	GQueue * queue;
	uint64_t command = COMMAND_UP;

	if( c == NULL){
		g_warning("Не выбран контролер");
		return;
	}
	control = c->control;
	queue = control->command;

	g_mutex_lock(&(cc->m_control));
	g_queue_push_head(queue,INT_TO_POINTER(command));
	g_mutex_unlock(&(cc->m_control));

	g_debug("press up");
}
/*TODO  одна функция отпускания клавиши команда одна */
static void button_release_event_button_up(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * c = cc->current;
	control_controller_s * control;
	GQueue * queue;
	uint64_t command = COMMAND_STOP;

	if( c == NULL){
		g_warning("Не выбран контролер");
		return;
	}
	control = c->control;
	queue = control->command;

	g_mutex_lock(&(cc->m_control));
	g_queue_push_head(queue,INT_TO_POINTER(command));
	g_mutex_unlock(&(cc->m_control));

	g_debug("release up");
}

static void button_press_event_button_down(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * c = cc->current;
	control_controller_s * control;
	GQueue * queue;
	uint64_t command = COMMAND_DOWN;

	if( c == NULL){
		g_warning("Не выбран контролер");
		return;
	}
	control = c->control;
	queue = control->command;

	g_mutex_lock(&(cc->m_control));
	g_queue_push_head(queue,INT_TO_POINTER(command));
	g_mutex_unlock(&(cc->m_control));

	g_debug("press down");
}
static void button_release_event_button_down(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * c = cc->current;
	control_controller_s * control;
	GQueue * queue;
	uint64_t command = COMMAND_STOP;

	if( c == NULL){
		g_warning("Не выбран контролер");
		return;
	}
	control = c->control;
	queue = control->command;

	g_mutex_lock(&(cc->m_control));
	g_queue_push_head(queue,INT_TO_POINTER(command));
	g_mutex_unlock(&(cc->m_control));

	g_debug("release down");
}

static void button_press_event_button_right(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * c = cc->current;
	control_controller_s * control;
	GQueue * queue;
	uint64_t command = COMMAND_RIGHT;

	if( c == NULL){
		g_warning("Не выбран контролер");
		return;
	}
	control = c->control;
	queue = control->command;

	g_mutex_lock(&(cc->m_control));
	g_queue_push_head(queue,INT_TO_POINTER(command));
	g_mutex_unlock(&(cc->m_control));

	g_debug("press rigth");
}
static void button_release_event_button_right(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * c = cc->current;
	control_controller_s * control;
	GQueue * queue;
	uint64_t command = COMMAND_STOP;

	if( c == NULL){
		g_warning("Не выбран контролер");
		return;
	}
	control = c->control;
	queue = control->command;

	g_mutex_lock(&(cc->m_control));
	g_queue_push_head(queue,INT_TO_POINTER(command));
	g_mutex_unlock(&(cc->m_control));

	g_debug("release rigth");
}

static void button_press_event_button_left(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * c = cc->current;
	control_controller_s * control;
	GQueue * queue;
	uint64_t command = COMMAND_LEFT;

	if( c == NULL){
		g_warning("Не выбран контролер");
		return;
	}
	control = c->control;
	queue = control->command;

	g_mutex_lock(&(cc->m_control));
	g_queue_push_head(queue,INT_TO_POINTER(command));
	g_mutex_unlock(&(cc->m_control));

	g_debug("press left");
}
static void button_release_event_button_left(GtkButton * b,GdkEvent * e,gpointer ud)
{
	block_controller_s * bc = (block_controller_s*)ud;
	communication_controller_s * cc = bc->communication_controller;
	controller_s * c = cc->current;
	control_controller_s * control;
	GQueue * queue;
	uint64_t command = COMMAND_STOP;

	if( c == NULL){
		g_warning("Не выбран контролер");
		return;
	}
	control = c->control;
	queue = control->command;

	g_mutex_lock(&(cc->m_control));
	g_queue_push_head(queue,INT_TO_POINTER(command));
	g_mutex_unlock(&(cc->m_control));

	g_debug("release left");
}

#define DEFAULT_SIZE_WIDTH_AXIS_VERTICAL    300
#define DEFAULT_SIZE_HEIGHT_AXIS_VERTICAL   300
static char STR_VERTICAL[] = "Вертикальная Ось";
static char STR_IMAGE_BASE_VERTICAL[] = "/image/vbase.png";
static GtkWidget * create_block_vertical(block_controller_s * block)
{
	GtkWidget * frame;
	GtkWidget * image;
	GdkPixbuf * buf;

	frame = gtk_frame_new(STR_VERTICAL);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	buf = get_pixbuf(STR_IMAGE_BASE_VERTICAL);
	block->buf_axis_vertical = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование */
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_AXIS_VERTICAL,DEFAULT_SIZE_HEIGHT_AXIS_VERTICAL);*/
	block->axis_vertical = GTK_IMAGE(image);

	gtk_container_add(GTK_CONTAINER(frame),image);

	gtk_widget_show(frame);
	gtk_widget_show(image);
	return frame;
}

#define DEFAULT_SIZE_WIDTH_AXIS_HORIZONTAL    300
#define DEFAULT_SIZE_HEIGHT_AXIS_HORIZONTAL   300
static char STR_HORIZONTAL[] = "Горизонтальная Ось";
static char STR_IMAGE_BASE_HORIZONTAL[] = "/image/hbase.png";
static GtkWidget * create_block_horizontal(block_controller_s * block)
{
	GtkWidget * frame;
	GtkWidget * image;
	GdkPixbuf * buf;

	frame = gtk_frame_new(STR_HORIZONTAL);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	buf = get_pixbuf(STR_IMAGE_BASE_HORIZONTAL);
	block->buf_axis_horizontal = buf;
	image = gtk_image_new_from_pixbuf(buf);
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	/*TODO маштабирование*/
	/*gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_AXIS_HORIZONTAL,DEFAULT_SIZE_HEIGHT_AXIS_HORIZONTAL);*/
	block->axis_horizontal = GTK_IMAGE(image);

	gtk_container_add(GTK_CONTAINER(frame),image);

	gtk_widget_show(frame);
	gtk_widget_show(image);

	return frame;
}

#define DEFAULT_SIZE_WIDTH_PRESSURE_VALVE    600
#define DEFAULT_SIZE_HEIGHT_PRESSURE_VALVE   100
static char STR_VALVE[] = "Магистраль";
static GtkWidget * create_block_valve(block_controller_s * block)
{
	GtkWidget * frame;
	GtkWidget * image;

	frame = gtk_frame_new(STR_VALVE);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	image = gtk_image_new();
	layout_widget(image,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_set_size_request(image,DEFAULT_SIZE_WIDTH_PRESSURE_VALVE,DEFAULT_SIZE_HEIGHT_PRESSURE_VALVE);

	block->pressure_valve = GTK_IMAGE(image);

	gtk_container_add(GTK_CONTAINER(frame),image);

	gtk_widget_show(frame);
	gtk_widget_show(image);

	return frame;
}

static char STR_FIRE_SENSOR[] = "Датчик Пламени";
static GtkWidget * create_block_fire_sensor(block_controller_s * block)
{
	GtkWidget * frame;
	frame = gtk_frame_new(STR_FIRE_SENSOR);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_show(frame);
	return frame;
}

static char STR_FIRE_ALARM[] = "Пожарная Сигнализация";
static GtkWidget * create_block_fire_alarm(block_controller_s * block)
{
	GtkWidget * frame;
	frame = gtk_frame_new(STR_FIRE_ALARM);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_show(frame);
	return frame;
}

static char STR_STATE[] = "Информация";
static GtkWidget * create_block_state(block_controller_s * block)
{
	GtkWidget * frame;
	GtkWidget * grid;
	GtkWidget * label_name;
	GtkWidget * block_vertical;
	GtkWidget * block_horizontal;
	GtkWidget * block_valve;
	GtkWidget * block_fire_sensor;
	GtkWidget * block_fire_alarm;

	frame = gtk_frame_new(STR_STATE);
	layout_widget(frame,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_START,TRUE,FALSE);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid),FALSE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);


	label_name = gtk_label_new("Нет подключения к контролеру!");
	layout_widget(label_name,GTK_ALIGN_CENTER,GTK_ALIGN_START,TRUE,TRUE);
	block->name = GTK_LABEL(label_name);

	block_vertical = create_block_vertical(block);
	block_horizontal = create_block_horizontal(block);
	block_valve = create_block_valve(block);
	block_fire_sensor = create_block_fire_sensor(block);
	block_fire_alarm = create_block_fire_alarm(block);

	gtk_container_add(GTK_CONTAINER(frame),grid);

	gtk_grid_attach(GTK_GRID(grid),label_name       ,0,0,4,1);
	gtk_grid_attach(GTK_GRID(grid),block_vertical   ,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_horizontal ,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_valve      ,0,2,2,1);
	gtk_grid_attach(GTK_GRID(grid),block_fire_sensor,2,1,1,2);
	gtk_grid_attach(GTK_GRID(grid),block_fire_alarm ,3,1,1,2);

	gtk_widget_show(frame);
	gtk_widget_show(grid);
	gtk_widget_show(label_name);

	return frame;
}

static char STR_CONTROL[] = "Управление";
static GtkWidget * create_block_control(block_controller_s * bc)
{
	GtkWidget * frame;
	GtkWidget * grid;
	GtkWidget * but_up;
	GtkWidget * but_down;
	GtkWidget * but_right;
	GtkWidget * but_left;

	frame = gtk_frame_new(STR_CONTROL);
	layout_widget(frame,GTK_ALIGN_START,GTK_ALIGN_END,FALSE,FALSE);

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
#if 1
	but_up = gtk_button_new_with_label("ВВЕРХ");
	layout_widget(but_up,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);
	g_signal_connect(but_up,"button-press-event",G_CALLBACK(button_press_event_button_up),bc);
	g_signal_connect(but_up,"button-release-event",G_CALLBACK(button_release_event_button_up),bc);

	but_down = gtk_button_new_with_label("ВНИЗ");
	layout_widget(but_down,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);
	g_signal_connect(but_down,"button-press-event",G_CALLBACK(button_press_event_button_down),bc);
	g_signal_connect(but_down,"button-release-event",G_CALLBACK(button_release_event_button_down),bc);

	but_right = gtk_button_new_with_label("ВПРАВО");
	layout_widget(but_right,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);
	g_signal_connect(but_right,"button-press-event",G_CALLBACK(button_press_event_button_right),bc);
	g_signal_connect(but_right,"button-release-event",G_CALLBACK(button_release_event_button_right),bc);

	but_left = gtk_button_new_with_label("ВЛЕВО");
	layout_widget(but_left,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,TRUE);
	g_signal_connect(but_left,"button-press-event",G_CALLBACK(button_press_event_button_left),bc);
	g_signal_connect(but_left,"button-release-event",G_CALLBACK(button_release_event_button_left),bc);

#endif
	gtk_container_add(GTK_CONTAINER(frame),grid);

	gtk_grid_attach(GTK_GRID(grid),but_up   ,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_down ,1,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_right,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_left ,2,1,1,1);

	gtk_widget_show(frame);
	gtk_widget_show(grid);

	gtk_widget_show(but_up);
	gtk_widget_show(but_down);
	gtk_widget_show(but_right);
	gtk_widget_show(but_left);
	return frame;
}

static block_controller_s block_controller;

int select_block_controller(controller_s * controller)
{
	communication_controller_s * cc = block_controller.communication_controller;
	GtkLabel * label;
	GThread * tid = cc->tid;


	if(controller == NULL){
		/*TODO если таймер запушен то остановить*/
		block_controller.stop_show = OK;
		cc->current = NULL;
		return SUCCESS;
	}

	if(tid == NULL){
		g_warning("Нет подключения!");
		cc->current = NULL;
		return FAILURE;
	}

	g_mutex_lock(&(cc->m_flag));
	cc->current = controller;
	g_mutex_unlock(&(cc->m_flag));

	{
	GQueue * queue = controller->control->command;
	g_mutex_lock(&(cc->m_control));
	g_queue_clear(queue);
	g_mutex_unlock(&(cc->m_control));
	}

	block_controller.stop_show = NOT_OK;
	g_timeout_add(block_controller.timeout_show,show_block_controler,&block_controller);

	label = block_controller.name;
	gtk_label_set_text(label,controller->name);

	return SUCCESS;
}

/* 1000 миллесекунд == 1 секунде */
#define DEFAULT_TIMEOUT_SHOW          200    /*5 кадров в секунду*/

GtkWidget * create_block_controller(void)
{
	GtkWidget * box;
	GtkWidget * frame_state;
	GtkWidget * frame_control;

	block_controller.stop_show = NOT_OK;
	block_controller.timeout_show = DEFAULT_TIMEOUT_SHOW;
	block_controller.communication_controller = &communication_controller;

	init_image(&block_controller);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_box_set_homogeneous(GTK_BOX(box),FALSE);

	frame_state = create_block_state(&block_controller);

	frame_control = create_block_control(&block_controller);

	gtk_box_pack_start(GTK_BOX(box),frame_state,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),frame_control,FALSE,TRUE,0);

	gtk_widget_show(box);

	return box;
}

/*****************************************************************************/
/* Выделение и высвобождения памяти для структур контролера                  */
/*****************************************************************************/

/*Выделение памяти и считывание из базы данных*/
controller_s * init_controller(uint32_t number)
{
	int rc;
	controller_s * controller = NULL;

	controller = g_slice_alloc0(sizeof(controller_s));
	controller->link = g_slice_alloc0(sizeof(link_s));
	controller->config = g_slice_alloc0(sizeof(config_controller_s));
	controller->state = g_slice_alloc0(sizeof(state_controller_s));
	controller->control = g_slice_alloc0(sizeof(control_controller_s));
	controller->control->command = g_queue_new();
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

	g_debug(" :> %d",controller->config->type);
	g_debug(" :> %#lx",controller->config->flag);
	g_debug(" :> %f",controller->config->rate_tic_vertical);
	g_debug(" :> %f",controller->config->rate_encoder_vertical);
	g_debug(" :> %f",controller->config->rate_amperage_vertical);
	g_debug(" :> %f",controller->config->rate_tic_horizontal);
	g_debug(" :> %f",controller->config->rate_encoder_horizontal);
	g_debug(" :> %f",controller->config->rate_amperage_horizontal);
	g_debug(" :> %f",controller->config->rate_valve_analog);

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
	GQueue * queue;

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
	queue = control->command;
	g_queue_free(queue);
	g_slice_free1(sizeof(control_controller_s),control);

	g_slice_free1(sizeof(controller_s),property);

	return SUCCESS;
}
/*****************************************************************************/
