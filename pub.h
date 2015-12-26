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

#define SUCCESS           0
#define FAILURE          -1

#define OK                0
#define NOT_OK           -1

#define MODE_WORK_CATALOG             0755

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
	MODE_CONTROL,
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
extern char STR_TYPE_LAFET[];

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
enum{
	COLUMN_NAME_TREE = 0,
	COLUMN_POINT_TREE,
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
	COLUMN_TABLE_LAFET_NUMBER = 0,
	COLUMN_TABLE_LAFET_FLAG,
	COLUMN_TABLE_LAFET_AMOUNT
};

#define ALL_BIT                      0xFFFFFFFFFFFFFFFFL
#define BIT_LIMIT_UP                 0x0000000000000001L
#define BIT_LIMIT_DOWN               0x0000000000000002L
#define BIT_TIC_VERTICAL             0x0000000000000004L
#define BIT_ENCODER_VERTICAL         0x0000000000000008L
#define BIT_AMPERAGE_VERTICAL        0x0000000000000010L
#define BIT_LIMIT_LEFT               0x0000000000000020L
#define BIT_LIMIT_RIGHT              0x0000000000000040L
#define BIT_TIC_HORIZONTAL           0x0000000000000080L
#define BIT_ENCODER_HORIZONTAL       0x0000000000000100L
#define BIT_AMPERAGE_HORIZONTAL      0x0000000000000200L
#define BIT_PRESSURE                 0x0000000000000400L
#define BIT_ENGINE_VERTICAL          0x0000000000000800L
#define BIT_ENGINE_HORIZONTAL        0x0000000000001000L
#define BIT_ACTUATOR_0               0x0000000000002000L
#define BIT_ACTUATOR_1               0x0000000000004000L
#define BIT_ACTUATOR_2               0x0000000000008000L
#define BIT_ACTUATOR_3               0x0000000000010000L
#define BIT_ACTUATOR_4               0x0000000000020000L
#define BIT_ACTUATOR_5               0x0000000000040000L

#define YES_LIMIT_UP(f)              (f | BIT_LIMIT_UP)
#define NOT_LIMIT_UP(f)              (f & (ALL_BIT^BIT_LIMIT_UP))
#define LIMIT_UP(f)                  (f & BIT_LIMIT_UP)
#define YES_LIMIT_DOWN(f)            (f | BIT_LIMIT_DOWN)
#define NOT_LIMIT_DOWN(f)            (f & (ALL_BIT^BIT_LIMIT_DOWN))
#define LIMIT_DOWN(f)                (f & BIT_LIMIT_DOWN)
#define YES_TIC_VERTICAL(f)          (f | BIT_TIC_VERTICAL)
#define NOT_TIC_VERTICAL(f)          (f & (ALL_BIT^BIT_TIC_VERTICAL))
#define TIC_VERTICAL(f)              (f & BIT_TIC_VERTICAL)
#define YES_ENCODER_VERTICAL(f)      (f | BIT_ENCODER_VERTICAL)
#define NOT_ENCODER_VERTICAL(f)      (f & (ALL_BIT^BIT_ENCODER_VERTICAL))
#define ENCODER_VERTICAL(f)          (f & BIT_ENCODER_VERTICAL)
#define YES_AMPERAGE_VERTICAL(f)     (f | BIT_AMPERAGE_VERTICAL)
#define NOT_AMPERAGE_VERTICAL(f)     (f & (ALL_BIT^BIT_AMPERAGE_VERTICAL))
#define AMPERAGE_VERTICAL(f)         (f & BIT_AMPERAGE_VERTICAL)
#define YES_LIMIT_LEFT(f)            (f | BIT_LIMIT_LEFT)
#define NOT_LIMIT_LEFT(f)            (f & (ALL_BIT^BIT_LIMIT_LEFT))
#define LIMIT_LEFT(f)                (f & BIT_LIMIT_LEFT)
#define YES_LIMIT_RIGHT(f)           (f | BIT_LIMIT_RIGHT)
#define NOT_LIMIT_RIGHT(f)           (f & (ALL_BIT^BIT_LIMIT_RIGHT))
#define LIMIT_RIGHT(f)               (f & BIT_LIMIT_RIGHT)
#define YES_TIC_HORIZONTAL(f)        (f | BIT_TIC_HORIZONTAL)
#define NOT_TIC_HORIZONTAL(f)        (f & (ALL_BIT^BIT_TIC_HORIZONTAL))
#define TIC_HORIZONTAL(f)            (f & BIT_TIC_HORIZONTAL)
#define YES_ENCODER_HORIZONTAL(f)    (f | BIT_ENCODER_HORIZONTAL)
#define NOT_ENCODER_HORIZONTAL(f)    (f & (ALL_BIT^BIT_ENCODER_HORIZONTAL))
#define ENCODER_HORIZONTAL(f)        (f & BIT_ENCODER_HORIZONTAL)
#define YES_AMPERAGE_HORIZONTAL(f)   (f | BIT_AMPERAGE_HORIZONTAL)
#define NOT_AMPERAGE_HORIZONTAL(f)   (f & (ALL_BIT^BIT_AMPERAGE_HORIZONTAL))
#define AMPERAGE_HORIZONTAL(f)       (f & BIT_AMPERAGE_HORIZONTAL)
#define YES_PRESSURE(f)              (f | BIT_PRESSURE)
#define NOT_PRESSURE(f)              (f & (ALL_BIT^BIT_PRESSURE))
#define PRESSURE(f)                  (f & BIT_PRESSURE)
#define YES_ENGINE_VERTICAL(f)       (f | BIT_ENGINE_VERTICAL)
#define NOT_ENGINE_VERTICAL(f)       (f & (ALL_BIT^BIT_ENGINE_VERTICAL))
#define ENGINE_VERTICAL(f)           (f & BIT_ENGINE_VERTICAL)
#define YES_ENGINE_HORIZONTAL(f)     (f | BIT_ENGINE_HORIZONTAL)
#define NOT_ENGINE_HORIZONTAL(f)     (f & (ALL_BIT^BIT_ENGINE_HORIZONTAL))
#define ENGINE_HORIZONTAL(f)         (f & BIT_ENGINE_HORIZONTAL)
#define YES_ACTUATOR_0(f)            (f | BIT_ACTUATOR_0)
#define NOT_ACTUATOR_0(f)            (f & (ALL_BIT^BIT_ACTUATOR_0))
#define ACTUATOR_0(f)                (f & BIT_ACTUATOR_0)
#define YES_ACTUATOR_1(f)            (f | BIT_ACTUATOR_1)
#define NOT_ACTUATOR_1(f)            (f & (ALL_BIT^BIT_ACTUATOR_1))
#define ACTUATOR_1(f)                (f & BIT_ACTUATOR_1)
#define YES_ACTUATOR_2(f)            (f | BIT_ACTUATOR_2)
#define NOT_ACTUATOR_2(f)            (f & (ALL_BIT^BIT_ACTUATOR_2))
#define ACTUATOR_2(f)                (f & BIT_ACTUATOR_2)
#define YES_ACTUATOR_3(f)            (f | BIT_ACTUATOR_3)
#define NOT_ACTUATOR_3(f)            (f & (ALL_BIT^BIT_ACTUATOR_3))
#define ACTUATOR_3(f)                (f & BIT_ACTUATOR_3)
#define YES_ACTUATOR_4(f)            (f | BIT_ACTUATOR_4)
#define NOT_ACTUATOR_4(f)            (f & (ALL_BIT^BIT_ACTUATOR_4))
#define ACTUATOR_4(f)                (f & BIT_ACTUATOR_4)
#define YES_ACTUATOR_5(f)            (f | BIT_ACTUATOR_5)
#define NOT_ACTUATOR_5(f)            (f & (ALL_BIT^BIT_ACTUATOR_5))
#define ACTUATOR_5(f)                (f & BIT_ACTUATOR_5)

struct _setting_controller_s
{
	uint64_t flag;
	uint32_t index_tic_vertical;
	uint32_t index_encoder_vertical;
	uint32_t index_amperage_vertical;
	uint32_t index_tic_horizantal;
	uint32_t index_encoder_horizontal;
	uint32_t index_amperage_horizontal;
	uint32_t index_pressure;
	uint32_t type_engine_vertical;
	uint32_t type_engine_horizontal;
};
typedef struct _setting_controller_s setting_controller_s;

struct _state_controller_s
{

};
typedef struct _state_controller_s state_controller_s;

struct _controller_s
{

};
typedef struct _controller_s controller_s;
/**************************************/
#endif

