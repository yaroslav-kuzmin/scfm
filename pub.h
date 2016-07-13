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


/*****************************************************************************/
#define TEST_INTERFACE    FALSE

/*****************************************************************************/
#define SUCCESS           0
#define FAILURE          -1

#define OK                0
#define NOT_OK           -1

#define MODE_WORK_CATALOG             0755

#define INT_TO_POINTER(s)  (((gpointer)((uint64_t)(s))))
#define POINTER_TO_INT(s)  ((uint64_t)(s))

typedef unsigned int   flag_t;
#define MILLISECOND_PER_SECOND       1000


#define ERROR_PROGRAM        		g_critical("Ошибка программы %s:%d",__FILE__,__LINE__)
/**************************************/

extern char STR_NAME_PROGRAMM[];

/* Размеры интерфейса */
#define DEFAULT_WIDTH_WINDOW                     1680
#define DEFAULT_HEIGHT_WINDOW                    1050

#define DEFAULT_WIDTH_MENU                       DEFAULT_WIDTH_WINDOW
#define DEFAULT_HEIGHT_MENU                      30

#define DEFAULT_WIDTH_JOB                        DEFAULT_WIDTH_WINDOW
#define DEFAULT_HEIGHT_JOB                       820   /*870*/
#define DEFAULT_BORDER_JOB                       2
#define DEFAULT_WIDTH_JOB_TREE                   160
#define DEFAULT_WIDTH_JOB_OBJECT                 (DEFAULT_WIDTH_JOB - DEFAULT_WIDTH_JOB_TREE - (DEFAULT_BORDER_JOB * 4))
#define DEFAULT_WIDTH_CONTROLLER_CONTROL         190

#define DEFAULT_WIDTH_LOG                        DEFAULT_WIDTH_WINDOW
#define DEFAULT_HEIGHT_LOG                       200      /* 150*/
#define DEFAULT_BORDER_LOG                       2

/**************************************/
enum
{
	RESOURCE_STYLE = 1,
	RESOURCE_IMAGE,
	RESOURCE_UI,
	AMOUNT_RESOURCE
};

enum
{
	MODULE_MAIN_WINDOW = 1,
	AMOUNT_MODULE
};
extern char STR_MODULE_MAIN_WINDOW[];
/**************************************/

enum
{
	MODE_NOT_WORK = 0,
	MODE_CONTROL_ON,
	MODE_CONTROL_OFF,
	MODE_CONFIGURATION
};

/*Состояние обектов и состояние конторллера*/
enum
{
	STATUS_OFF = 0,
	STATUS_ON_NORM,
	STATUS_ON_LINK_ON,
	STATUS_ON_LINK_OFF,
	STATUS_ON_CRASH,
	STATUS_ON_WARNING,
 	STATUS_ON_ERROR_LINK,
	AMOUNT_STATUS
};

extern char MESSAGE_STATUS_OFF[];
extern char MESSAGE_STATUS_ON_NORM[];
extern char MESSAGE_STATUS_ON_CRASH[];
extern char MESSAGE_STATUS_ON_WARNING[];
extern char MESSAGE_STATUS_ON_ERROR_LINK[];
extern char MESSAGE_STATUS_ON_LINK_ON[];
extern char MESSAGE_STATUS_ON_LINK_OFF[];

/* расширеная информация о состоянии*/
extern char STATUS_ERROR_NOT_CORRECT_CONFIG[];
extern char STATUS_ERROR_DISCONNECT[];

/**************************************/

#define FIRST_NUMBER_GROUP     0

#define TYPE_UNKNOWN           0x00
#define TYPE_KERNEL            0xFF
#define TYPE_GROUP             0x80
#define TYPE_VIDEOCAMERA       0x02
#define TYPE_CONTROLLER        0x03

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
extern GdkRGBA color_background;

/*****************************************************************************/
#define FORAMT_NAME_TABLE_OBJECT    "[o%07d]"
enum
{
	COLUMN_TABLE_OBJECT_NUMBER = 0,
	COLUMN_TABLE_OBJECT_NAME,
	COLUMN_TABLE_OBJECT_TYPE,
	COLUMN_TABLE_OBJECT_AMOUNT
};

typedef struct _object_s object_s;
struct _object_s
{
	uint32_t number;
	char * name;
	uint8_t type;
	flag_t status;
	object_s * parent;

	void * property;
	GSList * list;
};
/*****************************************************************************/

enum
{
	COLUMN_TABLE_GROUP_NUMBER = 0,
	COLUMN_TABLE_GROUP_IMAGE,
	COLUMN_TABLE_GROUP_AMOUNT
};
typedef struct _group_s group_s;
struct _group_s
{
	object_s * object;

	char * image;
};
/*****************************************************************************/

enum
{
	COLUMN_TABLE_VIDEOCAMERA_NUMBER = 0,
	COLUMN_TABLE_VIDEOCAMERA_PROTOCOL,
	COLUMN_TABLE_VIDEOCAMERA_ADDRESS,
	COLUMN_TABLE_VIDEOCAMERA_PORT,
	COLUMN_TABLE_VIDEOCAMERA_ACCESS,
	COLUMN_TABLE_VIDEOCAMERA_AMOUNT
};
typedef struct _videocamera_s videocamera_s;
struct _videocamera_s
{
	object_s * object;

	char * protocol;
	char * address;
	uint32_t port;
	char * access;
};

/*****************************************************************************/
/*   КОНТРОЛЛЕР  */

#define ALL_BIT                        0xFFFFFFFFFFFFFFFFL
#define BIT_ENGINE_VERTICAL            0x0000000000000001L
#define BIT_ENGINE_HORIZONTAL          0x0000000000000002L
#define BIT_ACTUATOR_SPRAY             0x0000000000000004L
#define BIT_ACTUATOR_RATE              0x0000000000000008L
#define BIT_ACTUATOR_VEIL              0x0000000000000010L
#define BIT_LIMIT_VERTICAL             0x0000000000000020L
#define BIT_TIC_VERTICAL               0x0000000000000040L
#define BIT_ENCODER_VERTICAL           0x0000000000000080L
#define BIT_AMPERAGE_VERTICAL          0x0000000000000100L
#define BIT_LIMIT_HORIZONTAL           0x0000000000000200L
#define BIT_TIC_HORIZONTAL             0x0000000000000400L
#define BIT_ENCODER_HORIZONTAL         0x0000000000000800L
#define BIT_AMPERAGE_HORIZONTAL        0x0000000000001000L
#define BIT_PRESSURE                   0x0000000000002000L
#define BIT_CONSOLE                    0x0000000000004000L
#define BIT_POST                       0x0000000000008000L
#define BIT_SENSOR_FIRE_DRY            0x0000000000010000L
#define BIT_SENSOR_FIRE_485            0x0000000000020000L
#define BIT_SENSOR_FIRE_ETHERNET       0x0000000000040000L
#define BIT_SENSOR_DRY_485             0x0000000000080000L
#define BIT_SENSOR_DRY_ETHERNER        0x0000000000100000L
#define BIT_VALVE_DRY                  0x0000000000200000L
#define BIT_VALVE_ANALOG               0x0000000000400000L
#define BIT_VALVE_LIMIT                0x0000000000800000L
#define BIT_VALVE_FEEDBACK             0x0000000001000000L
#define BIT_CAM_ANALOG_DC              0x0000000002000000L
#define BIT_CAM_DIGITAL_DC             0x0000000004000000L
#define BIT_CAM_DIGITAL_POE            0x0000000008000000L
#define BIT_FIRE_ALARM_01              0x0000000010000000L
#define BIT_FIRE_ALARM_02              0x0000000020000000L
#define BIT_FIRE_ALARM_03              0x0000000040000000L
#define BIT_FIRE_ALARM_04              0x0000000080000000L
#define BIT_FIRE_ALARM_05              0x0000000100000000L
#define BIT_FIRE_ALARM_06              0x0000000200000000L
#define BIT_FIRE_ALARM_07              0x0000000400000000L
#define BIT_FIRE_ALARM_08              0x0000000800000000L
#define BIT_FIRE_ALARM_09              0x0000001000000000L
#define BIT_FIRE_ALARM_10              0x0000002000000000L
#define BIT_FIRE_ALARM_11              0x0000004000000000L
#define BIT_FIRE_ALARM_12              0x0000008000000000L
#define BIT_FIRE_ALARM_13              0x0000010000000000L
#define BIT_FIRE_ALARM_14              0x0000020000000000L
#define BIT_FIRE_ALARM_15              0x0000040000000000L
#define BIT_FIRE_ALARM_16              0x0000080000000000L
#define BIT_DEVICE_01_STATE_0          0x0000100000000000L
#define BIT_DEVICE_01_STATE_1          0x0000200000000000L
#define BIT_DEVICE_02_STATE_0          0x0000400000000000L
#define BIT_DEVICE_02_STATE_1          0x0000800000000000L
#define BIT_DEVICE_03_STATE_0          0x0001000000000000L
#define BIT_DEVICE_03_STATE_1          0x0002000000000000L
#define BIT_DEVICE_04_STATE_0          0x0004000000000000L
#define BIT_DEVICE_04_STATE_1          0x0008000000000000L
#define BIT_DEVICE_05_STATE_0          0x0010000000000000L
#define BIT_DEVICE_05_STATE_1          0x0020000000000000L
#define BIT_DEVICE_06_STATE_0          0x0040000000000000L
#define BIT_DEVICE_06_STATE_1          0x0080000000000000L
#define BIT_DEVICE_07_STATE_0          0x0100000000000000L
#define BIT_DEVICE_07_STATE_1          0x0200000000000000L
#define BIT_DEVICE_08_STATE_0          0x0400000000000000L
#define BIT_DEVICE_08_STATE_1          0x0800000000000000L
#define BIT_RESERVE_0                  0x1000000000000000L
#define BIT_RESERVE_1                  0x2000000000000000L
#define BIT_RESERVE_2                  0x4000000000000000L
#define BIT_RESERVE_3                  0x8000000000000000L

#define SET_ENGINE_VERTICAL(f)         (f | BIT_ENGINE_VERTICAL)
#define UNSET_ENGINE_VERTICAL(f)       (f & (ALL_BIT^BIT_ENGINE_VERTICAL))
#define ENGINE_VERTICAL(f)             (f & BIT_ENGINE_VERTICAL)
#define SET_ENGINE_HORIZONTAL(f)       (f | BIT_ENGINE_HORIZONTAL)
#define UNSET_ENGINE_HORIZONTAL(f)     (f & (ALL_BIT^BIT_ENGINE_HORIZONTAL))
#define ENGINE_HORIZONTAL(f)           (f & BIT_ENGINE_HORIZONTAL)
#define SET_ACTUATOR_SPRAY(f)          (f | BIT_ACTUATOR_SPRAY)
#define UNSET_ACTUATOR_SPRAY(f)        (f & (ALL_BIT^BIT_ACTUATOR_SPRAY))
#define ACTUATOR_SPRAY(f)              (f & BIT_ACTUATOR_SPRAY)
#define SET_ACTUATOR_RATE(f)           (f | BIT_ACTUATOR_RATE)
#define UNSET_ACTUATOR_RATE(f)         (f & (ALL_BIT^BIT_ACTUATOR_RATE))
#define ACTUATOR_RATE(f)               (f & BIT_ACTUATOR_RATE)
#define SET_ACTUATOR_VEIL(f)           (f | BIT_ACTUATOR_VEIL)
#define UNSET_ACTUATOR_VEIL(f)         (f & (ALL_BIT^BIT_ACTUATOR_VEIL))
#define ACTUATOR_VEIL(f)               (f & BIT_ACTUATOR_VEIL)
#define SET_LIMIT_VERTICAL(f)          (f | BIT_LIMIT_VERTICAL)
#define UNSET_LIMIT_VERTICAL(f)        (f & (ALL_BIT^BIT_LIMIT_VERTICAL))
#define LIMIT_VERTICAL(f)              (f & BIT_LIMIT_VERTICAL)
#define SET_TIC_VERTICAL(f)            (f | BIT_TIC_VERTICAL)
#define UNSET_TIC_VERTICAL(f)          (f & (ALL_BIT^BIT_TIC_VERTICAL))
#define TIC_VERTICAL(f)                (f & BIT_TIC_VERTICAL)
#define SET_ENCODER_VERTICAL(f)        (f | BIT_ENCODER_VERTICAL)
#define UNSET_ENCODER_VERTICAL(f)      (f & (ALL_BIT^BIT_ENCODER_VERTICAL))
#define ENCODER_VERTICAL(f)            (f & BIT_ENCODER_VERTICAL)
#define SET_AMPERAGE_VERTICAL(f)       (f | BIT_AMPERAGE_VERTICAL)
#define UNSET_AMPERAGE_VERTICAL(f)     (f & (ALL_BIT^BIT_AMPERAGE_VERTICAL))
#define AMPERAGE_VERTICAL(f)           (f & BIT_AMPERAGE_VERTICAL)
#define SET_LIMIT_HORIZONTAL(f)        (f | BIT_LIMIT_HORIZONTAL)
#define UNSET_LIMIT_HORIZONTAL(f)      (f & (ALL_BIT^BIT_LIMIT_HORIZONTAL))
#define LIMIT_HORIZONTAL(f)            (f & BIT_LIMIT_HORIZONTAL)
#define SET_TIC_HORIZONTAL(f)          (f | BIT_TIC_HORIZONTAL)
#define UNSET_TIC_HORIZONTAL(f)        (f & (ALL_BIT^BIT_TIC_HORIZONTAL))
#define TIC_HORIZONTAL(f)              (f & BIT_TIC_HORIZONTAL)
#define SET_ENCODER_HORIZONTAL(f)      (f | BIT_ENCODER_HORIZONTAL)
#define UNSET_ENCODER_HORIZONTAL(f)    (f & (ALL_BIT^BIT_ENCODER_HORIZONTAL))
#define ENCODER_HORIZONTAL(f)          (f & BIT_ENCODER_HORIZONTAL)
#define SET_AMPERAGE_HORIZONTAL(f)     (f | BIT_AMPERAGE_HORIZONTAL)
#define UNSET_AMPERAGE_HORIZONTAL(f)   (f & (ALL_BIT^BIT_AMPERAGE_HORIZONTAL))
#define AMPERAGE_HORIZONTAL(f)         (f & BIT_AMPERAGE_HORIZONTAL)
#define SET_PRESSURE(f)                (f | BIT_PRESSURE)
#define UNSET_PRESSURE(f)              (f & (ALL_BIT^BIT_PRESSURE))
#define PRESSURE(f)                    (f & BIT_PRESSURE)
#define SET_CONSOLE(f)                 (f | BIT_CONSOLE)
#define UNSET_CONSOLE(f)               (f & (ALL_BIT^BIT_CONSOLE))
#define CONSOLE(f)                     (f & BIT_CONSOLE)
#define SET_POST(f)                    (f | BIT_POST)
#define UNSET_POST(f)                  (f & (ALL_BIT^BIT_POST))
#define POST(f)                        (f & BIT_POST)
#define SET_SENSOR_FIRE_DRY(f)         (f | BIT_SENSOR_FIRE_DRY)
#define UNSET_SENSOR_FIRE_DRY(f)       (f & (ALL_BIT^BIT_SENSOR_FIRE_DRY))
#define SENSOR_FIRE_DRY(f)             (f & BIT_SENSOR_FIRE_DRY)
#define SET_SENSOR_FIRE_485(f)         (f | BIT_SENSOR_FIRE_485)
#define UNSET_SENSOR_FIRE_485(f)       (f & (ALL_BIT^BIT_SENSOR_FIRE_485))
#define SENSOR_FIRE_485(f)             (f & BIT_SENSOR_FIRE_485)
#define SET_SENSOR_FIRE_ETHERNET(f)    (f | BIT_SENSOR_FIRE_ETHERNET)
#define UNSET_SENSOR_FIRE_ETHERNET(f)  (f & (ALL_BIT^BIT_SENSOR_FIRE_ETHERNET))
#define SENSOR_FIRE_ETHERNET(f)        (f & BIT_SENSOR_FIRE_ETHERNET)
#define SET_SENSOR_DRY_485(f)          (f | BIT_SENSOR_DRY_485)
#define UNSET_SENSOR_DRY_485(f)        (f & (ALL_BIT^BIT_SENSOR_DRY_485))
#define SENSOR_DRY_485(f)              (f & BIT_SENSOR_DRY_485)
#define SET_SENSOR_DRY_ETHERNET(f)     (f | BIT_SENSOR_DRY_ETHERNER)
#define UNSET_SENSOR_DRY_ETHERNET(f)   (f & (ALL_BIT^BIT_SENSOR_DRY_ETHERNER))
#define SENSOR_DRY_ETHERNET(f)         (f & BIT_SENSOR_DRY_ETHERNER)
#define SET_VALVE_DRY(f)               (f | BIT_VALVE_DRY)
#define UNSET_VALVE_DRY(f)             (f & (ALL_BIT^BIT_VALVE_DRY))
#define VALVE_DRY(f)                   (f & BIT_VALVE_DRY)
#define SET_VALVE_ANALOG(f)            (f | BIT_VALVE_ANALOG)
#define UNSET_VALVE_ANALOG(f)          (f & (ALL_BIT^BIT_VALVE_ANALOG))
#define VALVE_ANALOG(f)                (f & BIT_VALVE_ANALOG)
#define SET_VALVE_LIMIT(f)             (f | BIT_VALVE_LIMIT)
#define UNSET_VALVE_LIMIT(f)           (f & (ALL_BIT^BIT_VALVE_LIMIT))
#define VALVE_LIMIT(f)                 (f & BIT_VALVE_LIMIT)
#define SET_VALVE_FEEDBACK(f)          (f | BIT_VALVE_FEEDBACK)
#define UNSET_VALVE_FEEDBACK(f)        (f & (ALL_BIT^BIT_VALVE_FEEDBACK))
#define VALVE_FEEDBACK(f)              (f & BIT_VALVE_FEEDBACK)
#define SET_CAM_ANALOG_DC(f)           (f | BIT_CAM_ANALOG_DC)
#define UNSET_CAM_ANALOG_DC(f)         (f & (ALL_BIT^BIT_CAM_ANALOG_DC))
#define CAM_ANALOG_DC(f)               (f & BIT_CAM_ANALOG_DC)
#define SET_CAM_DIGITAL_DC(f)          (f | BIT_CAM_DIGITAL_DC)
#define UNSET_CAM_DIGITAL_DC(f)        (f & (ALL_BIT^BIT_CAM_DIGITAL_DC))
#define CAM_DIGITAL_DC(f)              (f & BIT_CAM_DIGITAL_DC)
#define SET_CAM_DIGITAL_POE(f)         (f | BIT_CAM_DIGITAL_POE)
#define UNSET_CAM_DIGITAL_POE(f)       (f & (ALL_BIT^BIT_CAM_DIGITAL_POE))
#define CAM_DIGITAL_POE(f)             (f & BIT_CAM_DIGITAL_POE)
#define SET_FIRE_ALARM_01(f)           (f | BIT_FIRE_ALARM_01)
#define UNSET_FIRE_ALARM_01(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_01))
#define FIRE_ALARM_01(f)               (f & BIT_FIRE_ALARM_01)
#define SET_FIRE_ALARM_02(f)           (f | BIT_FIRE_ALARM_02)
#define UNSET_FIRE_ALARM_02(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_02))
#define FIRE_ALARM_02(f)               (f & BIT_FIRE_ALARM_02)
#define SET_FIRE_ALARM_03(f)           (f | BIT_FIRE_ALARM_03)
#define UNSET_FIRE_ALARM_03(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_03))
#define FIRE_ALARM_03(f)               (f & BIT_FIRE_ALARM_03)
#define SET_FIRE_ALARM_04(f)           (f | BIT_FIRE_ALARM_04)
#define UNSET_FIRE_ALARM_04(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_04))
#define FIRE_ALARM_04(f)               (f & BIT_FIRE_ALARM_04)
#define SET_FIRE_ALARM_05(f)           (f | BIT_FIRE_ALARM_05)
#define UNSET_FIRE_ALARM_05(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_05))
#define FIRE_ALARM_05(f)               (f & BIT_FIRE_ALARM_05)
#define SET_FIRE_ALARM_06(f)           (f | BIT_FIRE_ALARM_06)
#define UNSET_FIRE_ALARM_06(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_06))
#define FIRE_ALARM_06(f)               (f & BIT_FIRE_ALARM_06)
#define SET_FIRE_ALARM_07(f)           (f | BIT_FIRE_ALARM_07)
#define UNSET_FIRE_ALARM_07(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_07))
#define FIRE_ALARM_07(f)               (f & BIT_FIRE_ALARM_07)
#define SET_FIRE_ALARM_08(f)           (f | BIT_FIRE_ALARM_08)
#define UNSET_FIRE_ALARM_08(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_08))
#define FIRE_ALARM_08(f)               (f & BIT_FIRE_ALARM_08)
#define SET_FIRE_ALARM_09(f)           (f | BIT_FIRE_ALARM_09)
#define UNSET_FIRE_ALARM_09(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_09))
#define FIRE_ALARM_09(f)               (f & BIT_FIRE_ALARM_09)
#define SET_FIRE_ALARM_10(f)           (f | BIT_FIRE_ALARM_10)
#define UNSET_FIRE_ALARM_10(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_10))
#define FIRE_ALARM_10(f)               (f & BIT_FIRE_ALARM_10)
#define SET_FIRE_ALARM_11(f)           (f | BIT_FIRE_ALARM_11)
#define UNSET_FIRE_ALARM_11(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_11))
#define FIRE_ALARM_11(f)               (f & BIT_FIRE_ALARM_11)
#define SET_FIRE_ALARM_12(f)           (f | BIT_FIRE_ALARM_12)
#define UNSET_FIRE_ALARM_12(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_12))
#define FIRE_ALARM_12(f)               (f & BIT_FIRE_ALARM_12)
#define SET_FIRE_ALARM_13(f)           (f | BIT_FIRE_ALARM_13)
#define UNSET_FIRE_ALARM_13(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_13))
#define FIRE_ALARM_13(f)               (f & BIT_FIRE_ALARM_13)
#define SET_FIRE_ALARM_14(f)           (f | BIT_FIRE_ALARM_14)
#define UNSET_FIRE_ALARM_14(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_14))
#define FIRE_ALARM_14(f)               (f & BIT_FIRE_ALARM_14)
#define SET_FIRE_ALARM_15(f)           (f | BIT_FIRE_ALARM_15)
#define UNSET_FIRE_ALARM_15(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_15))
#define FIRE_ALARM_15(f)               (f & BIT_FIRE_ALARM_15)
#define SET_FIRE_ALARM_16(f)           (f | BIT_FIRE_ALARM_16)
#define UNSET_FIRE_ALARM_16(f)         (f & (ALL_BIT^BIT_FIRE_ALARM_16))
#define FIRE_ALARM_16(f)               (f & BIT_FIRE_ALARM_16)
#define SET_DEVICE_01_STATE_0(f)       (f | BIT_DEVICE_01_STATE_0)
#define UNSET_DEVICE_01_STATE_0(f)     (f & (ALL_BIT^BIT_DEVICE_01_STATE_0))
#define DEVICE_01_STATE_0(f)           (f & BIT_DEVICE_01_STATE_0)
#define SET_DEVICE_01_STATE_1(f)       (f | BIT_DEVICE_01_STATE_1)
#define UNSET_DEVICE_01_STATE_1(f)     (f & (ALL_BIT^BIT_DEVICE_01_STATE_1))
#define DEVICE_01_STATE_1(f)           (f & BIT_DEVICE_01_STATE_1)
#define SET_DEVICE_02_STATE_0(f)       (f | BIT_DEVICE_02_STATE_0)
#define UNSET_DEVICE_02_STATE_0(f)     (f & (ALL_BIT^BIT_DEVICE_02_STATE_0))
#define DEVICE_02_STATE_0(f)           (f & BIT_DEVICE_02_STATE_0)
#define SET_DEVICE_02_STATE_1(f)       (f | BIT_DEVICE_02_STATE_1)
#define UNSET_DEVICE_02_STATE_1(f)     (f & (ALL_BIT^BIT_DEVICE_02_STATE_1))
#define DEVICE_02_STATE_1(f)           (f & BIT_DEVICE_02_STATE_1)
#define SET_DEVICE_03_STATE_0(f)       (f | BIT_DEVICE_03_STATE_0)
#define UNSET_DEVICE_03_STATE_0(f)     (f & (ALL_BIT^BIT_DEVICE_03_STATE_0))
#define DEVICE_03_STATE_0(f)           (f & BIT_DEVICE_03_STATE_0)
#define SET_DEVICE_03_STATE_1(f)       (f | BIT_DEVICE_03_STATE_1)
#define UNSET_DEVICE_03_STATE_1(f)     (f & (ALL_BIT^BIT_DEVICE_03_STATE_1))
#define DEVICE_03_STATE_1(f)           (f & BIT_DEVICE_03_STATE_1)
#define SET_DEVICE_04_STATE_0(f)       (f | BIT_DEVICE_04_STATE_0)
#define UNSET_DEVICE_04_STATE_0(f)     (f & (ALL_BIT^BIT_DEVICE_04_STATE_0))
#define DEVICE_04_STATE_0(f)           (f & BIT_DEVICE_04_STATE_0)
#define SET_DEVICE_04_STATE_1(f)       (f | BIT_DEVICE_04_STATE_1)
#define UNSET_DEVICE_04_STATE_1(f)     (f & (ALL_BIT^BIT_DEVICE_04_STATE_1))
#define DEVICE_04_STATE_1(f)           (f & BIT_DEVICE_04_STATE_1)
#define SET_DEVICE_05_STATE_0(f)       (f | BIT_DEVICE_05_STATE_0)
#define UNSET_DEVICE_05_STATE_0(f)     (f & (ALL_BIT^BIT_DEVICE_05_STATE_0))
#define DEVICE_05_STATE_0(f)           (f & BIT_DEVICE_05_STATE_0)
#define SET_DEVICE_05_STATE_1(f)       (f | BIT_DEVICE_05_STATE_1)
#define UNSET_DEVICE_05_STATE_1(f)     (f & (ALL_BIT^BIT_DEVICE_05_STATE_1))
#define DEVICE_05_STATE_1(f)           (f & BIT_DEVICE_05_STATE_1)
#define SET_DEVICE_06_STATE_0(f)       (f | BIT_DEVICE_06_STATE_0)
#define UNSET_DEVICE_06_STATE_0(f)     (f & (ALL_BIT^BIT_DEVICE_06_STATE_0))
#define DEVICE_06_STATE_0(f)           (f & BIT_DEVICE_06_STATE_0)
#define SET_DEVICE_06_STATE_1(f)       (f | BIT_DEVICE_06_STATE_1)
#define UNSET_DEVICE_06_STATE_1(f)     (f & (ALL_BIT^BIT_DEVICE_06_STATE_1))
#define DEVICE_06_STATE_1(f)           (f & BIT_DEVICE_06_STATE_1)
#define SET_DEVICE_07_STATE_0(f)       (f | BIT_DEVICE_07_STATE_0)
#define UNSET_DEVICE_07_STATE_0(f)     (f & (ALL_BIT^BIT_DEVICE_07_STATE_0))
#define DEVICE_07_STATE_0(f)           (f & BIT_DEVICE_07_STATE_0)
#define SET_DEVICE_07_STATE_1(f)       (f | BIT_DEVICE_07_STATE_1)
#define UNSET_DEVICE_07_STATE_1(f)     (f & (ALL_BIT^BIT_DEVICE_07_STATE_1))
#define DEVICE_07_STATE_1(f)           (f & BIT_DEVICE_07_STATE_1)
#define SET_DEVICE_08_STATE_0(f)       (f | BIT_DEVICE_08_STATE_0)
#define UNSET_DEVICE_08_STATE_0(f)     (f & (ALL_BIT^BIT_DEVICE_08_STATE_0))
#define DEVICE_08_STATE_0(f)           (f & BIT_DEVICE_08_STATE_0)
#define SET_DEVICE_08_STATE_1(f)       (f | BIT_DEVICE_08_STATE_1)
#define UNSET_DEVICE_08_STATE_1(f)     (f & (ALL_BIT^BIT_DEVICE_08_STATE_1))
#define DEVICE_08_STATE_1(f)           (f & BIT_DEVICE_08_STATE_1)
#define SET_RESERVE_0(f)               (f | BIT_RESERVE_0)
#define UNSET_RESERVE_0(f)             (f & (ALL_BIT^BIT_RESERVE_0))
#define RESERVE_0(f)                   (f & BIT_RESERVE_0)
#define SET_RESERVE_1(f)               (f | BIT_RESERVE_1)
#define UNSET_RESERVE_1(f)             (f & (ALL_BIT^BIT_RESERVE_1))
#define RESERVE_1(f)                   (f & BIT_RESERVE_1)
#define SET_RESERVE_2(f)               (f | BIT_RESERVE_2)
#define UNSET_RESERVE_2(f)             (f & (ALL_BIT^BIT_RESERVE_2))
#define RESERVE_2(f)                   (f & BIT_RESERVE_2)
#define SET_RESERVE_3(f)               (f | BIT_RESERVE_3)
#define UNSET_RESERVE_3(f)             (f & (ALL_BIT^BIT_RESERVE_3))
#define RESERVE_3(f)                   (f & BIT_RESERVE_3)

#define AMOUNT_FIRE_SENSOR      16

enum
{
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
	COLUMN_TABLE_CONTROLLER_PRESSURE,
	COLUMN_TABLE_CONTROLLER_VALVE_ANALOG,
	COLUMN_TABLE_CONTROLLER_AMOUNT
};

#define TYPE_LINK_UART        0x01
#define TYPE_LINK_TCP         0x02
extern char STR_EMPTY[];
typedef struct _link_s link_s;
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


typedef struct _config_controller_s config_controller_s;
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
	double rate_pressure;
	double rate_valve_analog;
};

/* Состояние контроллера */
enum
{
	STATE_ERROR = 0,
	STATE_MODE_AUTO,
	STATE_MODE_MANUAL,
	STATE_MODE_TEST,
	STATE_MODE_ERROR,
	STATE_OSCILLATION_VERTICAL,
	STATE_OSCILLATION_HORIZONTAL,
	STATE_OSCILLATION_SAW,
	STATE_OSCILLATION_STEP,
	STATE_OSCILLATION_STOP,
	STATE_OSCILLATION_ERROR,
	STATE_INFO_NORM,
	STATE_INFO_LIMIT_VERTICAL,
	STATE_INFO_LIMIT_HORIZONTAL,
	STATE_INFO_CRASH_VERTICAL,
	STATE_INFO_CRASH_HORIZONTAL,
	STATE_INFO_CRASH_SPARY,
	STATE_INFO_CRASH_RATE,
	STATE_INFO_CRASH_VALVE,
	STATE_INFO_ERROR,
	STATE_VALVE_OPEN,
	STATE_VALVE_OPEN_RUN,
	STATE_VALVE_CLOSE,
	STATE_VALVE_CLOSE_RUN,
	STATE_VALVE_ERROR,
	STATE_FIRE_ALARM_ON,
	STATE_FIRE_ALARM_OFF,
	STATE_FIRE_ALARM_ERROR,
	AMOUNT_STATE_CONTROLLER
};

typedef struct _state_controller_s state_controller_s;
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
	uint16_t work;
	uint16_t valve;
	uint16_t tic_valve;
	uint16_t fire_sensor;
	uint16_t fire_alarm;
	uint16_t device;

};

/*комманды контроллера*/
enum
{
	COMMAND_EMPTY=0,
	COMMAND_LAFET_STOP,
	COMMAND_LAFET_UP,
	COMMAND_LAFET_DOWN,
	COMMAND_LAFET_RIGHT,
	COMMAND_LAFET_LEFT,
	COMMAND_LAFET_SPEED_VERTICAL,
	COMMAND_LAFET_SPEED_HORIZONTAL,
	COMMAND_ACTUATOT_STOP,
	COMMAND_SPRAY_LESS,
	COMMAND_SPRAY_MORE,
	COMMAND_RATE_LESS,
	COMMAND_RATE_MORE,
	COMMAND_VEIL_LESS,
	COMMAND_VEIL_MORE,
	COMMAND_OSCILLATION_RUN,
	COMMAND_OSCILLATION_STOP,
	COMMAND_OSCILLATION_VERTICAL,
	COMMAND_OSCILLATION_HORIZONTAL,
	COMMAND_OSCILLATION_SAW,
	COMMAND_OSCILLATION_STEP,
	COMMAND_VALVE_STOP,
	COMMAND_VALVE_OPEN,
	COMMAND_VALVE_CLOSE,
	COMMAND_VALVE_POSITION,
	COMMAND_ALL_DEVICE_STOP,
	COMMAND_MODE_STOP,
	COMMAND_MODE_AUTO,
	COMMAND_MODE_MANUAL,
	COMMAND_MODE_CONFIG,
	AMOUNT_COMMAND
};

typedef struct _command_part_s command_part_s;
struct _command_part_s
{
	uint16_t value;
	uint16_t parametr;
	uint16_t reserve_0;
	uint16_t reserve_1;
};
typedef union _command_u command_u;
union _command_u
{
	uint64_t all;
	command_part_s part;
};
/*TODO изменить название*/
typedef struct _control_controller_s control_controller_s;
struct _control_controller_s
{
	GMutex * mutex;
	uint32_t timeout;
	uint32_t counter;
};

enum
{
	TYPE_DEVICE_LSD = 0,
	TYPE_DEVICE_ROBOT,
	AMOUNT_TYPE_DEVICE
};
#define MIN_ID     1
#define MAX_ID     247
typedef struct _controller_s controller_s;
struct _controller_s
{
	object_s * object;
	char * name;
	flag_t type;

	flag_t status_link;
	char * str_status;

	link_s * link;
	config_controller_s * config;
	flag_t status_config;
	state_controller_s * state;
	state_controller_s * state_past;
	control_controller_s * control;
	command_u	command;
};
/**************************************/
#endif
/*****************************************************************************/
