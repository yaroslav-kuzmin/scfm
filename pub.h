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

#ifndef PUB_H
#define PUB_H

#include <stdint.h>
#include <gtk/gtk.h>

#define SUCCESS           0
#define FAILURE          -1

#define OK                0
#define NOT_OK           -1

#define MODE_WORK_CATALOG             0755

#define INT_TO_POINTER(s)  (((gpointer)((uint64_t)(s))))
#define POINTER_TO_INT(s)  ((uint64_t)(s))
/**************************************/

extern char STR_NAME_PROGRAMM[];

/**************************************/
struct _generic_s
{
	GString * pub;
	GdkPixbuf * default_icon;
};
typedef struct _generic_s generic_s;

/**************************************/

enum{
	MODE_NOT_WORK = 0,
	MODE_CONTROL_ON,
	MODE_CONTROL_OFF,
	MODE_CONFIG
};

/**************************************/

#define FIRST_NUMBER_GROUP     0

#define TYPE_UNKNOWN           0x00
#define TYPE_KERNEL            0xFF
#define TYPE_GROUP             0x80
#define TYPE_VIDEOCAMERA       0x02
#define TYPE_CONTROLLERE       0x03

extern char STR_TYPE_GROUP[];
extern char STR_TYPE_VIDEOCAMERE[];
extern char STR_TYPE_CONTROLLER[];

/**************************************/
extern GdkRGBA color_black;
extern GdkRGBA color_green;
extern GdkRGBA color_red;
extern GdkRGBA color_white;
extern GdkRGBA color_lite_blue;
extern GdkRGBA color_lite_red;
extern GdkRGBA color_lite_green;

/**************************************/
#define FORAMT_NAME_TABLE_OBJECT    "[o%07d]"
enum{
	COLUMN_TABLE_OBJECT_NUMBER = 0,
	COLUMN_TABLE_OBJECT_NAME,
	COLUMN_TABLE_OBJECT_TYPE,
	COLUMN_TABLE_OBJECT_AMOUNT
};
struct _object_s
{
	uint32_t number;
	char * name;
	uint8_t type;

	void * property;
	GSList * list;
};
typedef struct _object_s object_s;
/**************************************/
/*Основное дерево */
enum{
	COLUMN_NAME_TREE = 0,  /*имя обеъкта*/
	COLUMN_POINT_TREE,     /*указатель на обект*/
	AMOUNT_COLUMN_TREE
};

/**************************************/

enum{
	COLUMN_TABLE_GROUP_NUMBER = 0,
	COLUMN_TABLE_GROUP_IMAGE,
	COLUMN_TABLE_GROUP_AMOUNT
};
struct _group_s
{
	char * image;
};
typedef struct _group_s group_s;
/**************************************/

enum{
	COLUMN_TABLE_VIDEOCAMERA_NUMBER = 0,
	COLUMN_TABLE_VIDEOCAMERA_PROTOCOL,
	COLUMN_TABLE_VIDEOCAMERA_ADDRESS,
	COLUMN_TABLE_VIDEOCAMERA_PORT,
	COLUMN_TABLE_VIDEOCAMERA_ACCESS,
	COLUMN_TABLE_VIDEOCAMERA_AMOUNT
};
struct _videocamera_s
{
	char * protocol;
	char * address;
	uint32_t port;
	char * access;
};
typedef struct _videocamera_s videocamera_s;

/**************************************/
enum{
	COLUMN_TABLE_CONTROLLER_NUMBER = 0,
	COLUMN_TABLE_CONTROLLER_TYPE_LINK,
	COLUMN_TABLE_CONTROLLER_ID,
	COLUMN_TABLE_CONTROLLER_ADDRESS,
	COLUMN_TABLE_CONTROLLER_PORT,
	COLUMN_TABLE_CONTROLLER_DEVICE,
	COLUMN_TABLE_CONTROLLER_BAUD,
	COLUMN_TABLE_CONTROLLER_PARITY,
	COLUMN_TABLE_CONTROLLER_DATA_BIT,
	COLUMN_TABLE_CONTROLLER_STOP_BIT,
	COLUMN_TABLE_CONTROLLER_TYPE,
	COLUMN_TABLE_CONTROLLER_FLAG,
	COLUMN_TABLE_CONTROLLER_TIC_VERTICAL,
	COLUMN_TABLE_CONTROLLER_ENCODER_VERTICAL,
	COLUMN_TABLE_CONTROLLER_AMPERAGE_VERTICAL,
	COLUMN_TABLE_CONTROLLER_TIC_HORIZONTAL,
	COLUMN_TABLE_CONTROLLER_ENCODER_HORIZONTAL,
	COLUMN_TABLE_CONTROLLER_AMPERAGE_HORIZONTAL,
	COLUMN_TABLE_CONTROLLER_VALVE_ANALOG,
	COLUMN_TABLE_CONTROLLER_AMOUNT
};

#define TYPE_LINK_UART        0x01
#define TYPE_LINK_TCP         0x02
extern char STR_EMPTY[];
struct _link_s
{
	void * connect;
	uint8_t id;
	int type;
	uint16_t * dest;
	/*TCP*/
	char * address;
	uint16_t port;
	/*UART*/
	char * device;
	uint32_t baud;
	int8_t parity;
	uint8_t data_bit;
	uint8_t stop_bit;
};
typedef struct _link_s link_s;

struct _config_controller_s
{
	uint32_t type;
	uint64_t flag;

	double rate_tic_vertical;
	double rate_encoder_vertical;
	double rate_amperage_vertical;
	double rate_tic_horizontal;
	double rate_encoder_horizontal;
	double rate_amperage_horizontal;
	double rate_valve_analog;
};
typedef struct _config_controller_s config_controller_s;

struct _state_controller_s
{
	uint16_t lafet;
	uint16_t tic_vertical;
	uint16_t tic_horizontal;
	uint16_t encoder_vertical;
	uint16_t encoder_horizontal;
	uint16_t pressure;
	uint16_t amperage_vertical;
	uint16_t amperage_horizontal;
   uint16_t valve;
   uint16_t tic_valve;
   uint16_t fire_sensor;
   uint16_t fire_alarm;
};
typedef struct _state_controller_s state_controller_s;

struct _control_controller_s
{
	GQueue * command;
};
typedef struct _control_controller_s control_controller_s;
#define MIN_ID     1
#define MAX_ID     247
struct _controller_s
{
	link_s * link;
	char * name;
	config_controller_s * config;
	state_controller_s * state;
	control_controller_s * control;
};
typedef struct _controller_s controller_s;
/**************************************/

enum{
	COMMAND_EMPTY=0,
	COMMAND_STOP,
	COMMAND_UP,
	COMMAND_DOWN,
	COMMAND_RIGHT,
	COMMAND_LEFT,
	AMOUNT_COMMAND
};
#endif

