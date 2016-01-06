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
#define BIT_CONSOLE_SINGLY             0x0000000000002000L
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
#define SET_CONSOLE_SINGLY(f)          (f | BIT_CONSOLE_SINGLY)
#define UNSET_CONSOLE_SINGLY(f)        (f & (ALL_BIT^BIT_CONSOLE_SINGLY))
#define CONSOLE_SINGLY(f)              (f & BIT_CONSOLE_SINGLY)
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

typedef struct _check_connect_s check_connect_s;
typedef struct _check_connect_s check_connect_s;
struct _block_setting_controller_s
{
	GtkEntryBuffer * address;
	GtkEntryBuffer * port;
	GtkEntryBuffer * id;
	GtkWidget * block_info;

	link_s * link;
	char * name;
	config_controller_s * config;
	state_controller_s * state;
};
typedef struct _block_setting_controller_s block_setting_controller_s;

struct _block_controller_s
{
	GtkLabel * label;
};
typedef struct _block_controller_s block_controller_s;

struct _all_controller_s
{
	GSList * list;
};
typedef struct _all_controller_s all_controller_s;
/*****************************************************************************/
/* локальные функции                                                         */
/*****************************************************************************/
static int check_link_controller(link_s * link,config_controller_s * config,state_controller_s * state)
{
	int rc;
	rc = link_connect_controller(link);
	if(rc == FAILURE){
		return rc;
	}
	rc = link_config_controller(link,config);
	if(rc == FAILURE){
		return rc;
	}
	rc = link_state_controller(link,state);
	if(rc == FAILURE){
		return rc;
	}
	return SUCCESS;
}

static int fill_block_info(block_setting_controller_s * bsc)
{
	return SUCCESS;
}

/*static STR_NAME[] = "Наименование : ";*/

static GtkWidget * create_block_info()
{
	GtkWidget * box;
	/*GtkWidget * label;*/

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	return box;
}

static char * check_address(const char * address)
{
	char * str;
	GInetAddress * inet_address = g_inet_address_new_from_string(address);
	if(inet_address == NULL){
		return NULL;
	}
	str = g_inet_address_to_string(inet_address);
	g_object_unref(inet_address);
	return str;
}

static uint16_t check_port(const char * port)
{
	uint16_t p = 0;
	p = g_ascii_strtoull(port,NULL,10);
	return p;
}

static uint8_t check_id(const char * id)
{
	uint8_t t = 0;
	t = g_ascii_strtoull(id,NULL,10);
	if(t < MIN_ID){
		t = 0;
	}
	else{
		if(t > MAX_ID){
			t = 0;
		}
	}
	return t;
}

static GtkWidget * create_block_entry(char * name,GtkEntryBuffer ** buf)
{
	GtkWidget * box;
	GtkWidget * label;
	GtkWidget * entry;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	layout_widget(box,GTK_ALIGN_FILL,GTK_ALIGN_CENTER,TRUE,FALSE);

	label = gtk_label_new(name);
	layout_widget(label,GTK_ALIGN_START,GTK_ALIGN_CENTER,FALSE,FALSE);

	entry = gtk_entry_new();
	layout_widget(entry,GTK_ALIGN_START,GTK_ALIGN_CENTER,TRUE,FALSE);
	*buf = gtk_entry_get_buffer(GTK_ENTRY(entry));

	gtk_box_pack_start(GTK_BOX(box),label,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),entry,TRUE,TRUE,0);

	gtk_widget_show(box);
	gtk_widget_show(label);
	gtk_widget_show(entry);

	return box;
}

static void clicked_button_check(GtkButton * button,gpointer ud)
{
	int rc;
	char * address = NULL;
	uint16_t port;
	uint8_t id;
	const char * str;
	link_s * link;
	config_controller_s * config;
	state_controller_s * state;
	block_setting_controller_s * bsc = (block_setting_controller_s*)ud;

	/*TODO при повторном нажатии утечка памяти*/
	bsc->link = NULL;
	/*TODO вывод сообщений*/
	str = gtk_entry_buffer_get_text(bsc->address);
	address = check_address(str);
	if(address == NULL){
		g_warning("Адрес контролера не корректный!");
		return ;
	}
	str = gtk_entry_buffer_get_text(bsc->port);
	port = check_port(str);
	if(port == 0){
		g_warning("Порт контролера не корректный!");
		return ;
	}
	str = gtk_entry_buffer_get_text(bsc->id);
	id = check_id(str);
	if(id == 0){
		g_warning("Индификатор контролера не корректный!");
		return ;
	}

	link = g_slice_alloc0(sizeof(link_s));
	config = g_slice_alloc0(sizeof(config_controller_s));
	state = g_slice_alloc0(sizeof(state_controller_s));

	/*TODO добавление других типов соединений*/
	link->type = TYPE_LINK_TCP;
	link->connect = NULL;
	link->address = address;
	link->port = port;
	link->id = id;

	rc = check_link_controller(link,config,state);
	if(rc == FAILURE){
		g_slice_free1(sizeof(link_s),link);
		g_slice_free1(sizeof(config_controller_s),config);
		g_slice_free1(sizeof(state_controller_s),state);
		return;
	}

	bsc->link = link;
	bsc->config = config;
	bsc->state = state;
	bsc->name = get_name_controller(config);
	/*TODO сообщенийние что проверка корректноа*/
	fill_block_info(bsc);

	link_disconnect_controller(link);
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
static block_setting_controller_s block_setting_controller;

void * new_property_controller(void)
{
	controller_s * controller;
	link_s * link = block_setting_controller.link;
	config_controller_s * config = block_setting_controller.config;
	state_controller_s * state = block_setting_controller.state;

	/*TODO вывод сообщений*/
	if(link == NULL){
		g_warning("Соединение не проверено!");
		return NULL;
	}

	controller = g_slice_alloc0(sizeof(controller_s));
	controller->name = block_setting_controller.name;
	controller->link = link;
	controller->config = config;
	controller->state = state;

	return controller;
}

static char STR_NAME_CONTROLLER[] = "Контроллер";
static char STR_NAME_ADDRESS[] = "Адрес";
static char STR_NAME_PORT[] = "Порт";
static char STR_NAME_ID[] = "Номер";
static char STR_NAME_CHECK[] = "Поиск";

GtkWidget * create_block_setting_controller(void)
{
	GtkWidget * grid;
	GtkWidget * label;
	GtkWidget * block_address;
	GtkWidget * block_port;
	GtkWidget * block_id;
	GtkWidget * block_info;
	GtkEntryBuffer * buf;
	GtkWidget * but_check;

	block_setting_controller.link = NULL;

	grid = gtk_grid_new();
	layout_widget(grid,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);

	label = gtk_label_new(STR_NAME_CONTROLLER);
	layout_widget(label,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,TRUE,FALSE);

	block_address = create_block_entry(STR_NAME_ADDRESS,&buf);
	block_setting_controller.address = buf;

	block_port = create_block_entry(STR_NAME_PORT,&buf);
	block_setting_controller.port = buf;

	block_id = create_block_entry(STR_NAME_ID,&buf);
	block_setting_controller.id = buf;

	but_check = gtk_button_new_with_label(STR_NAME_CHECK);
	layout_widget(but_check,GTK_ALIGN_CENTER,GTK_ALIGN_CENTER,FALSE,FALSE);
	g_signal_connect(but_check,"clicked",G_CALLBACK(clicked_button_check),&block_setting_controller);

	block_info = create_block_info();
	block_setting_controller.block_info = block_info;

	gtk_grid_attach(GTK_GRID(grid),label        ,0,0,2,1);
	gtk_grid_attach(GTK_GRID(grid),block_address,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_port   ,0,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),block_id     ,0,3,1,1);
	gtk_grid_attach(GTK_GRID(grid),but_check    ,0,4,2,1);
	gtk_grid_attach(GTK_GRID(grid),block_info   ,0,5,2,1);

	gtk_widget_show(grid);
	gtk_widget_show(label);
	gtk_widget_show(but_check);

	return grid;
}

/*****************************************************************************/
all_controller_s all_controller;
int init_all_controller(void)
{
	all_controller.list = g_slist_alloc();
	return SUCCESS;
}

int deinit_all_controller(void)
{
	g_slist_free(all_controller.list);
	return SUCCESS;
}

int connect_controller(controller_s * controller)
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

/*TODO считывание данных из базыданных*/
controller_s * init_controller(uint32_t number)
{
	int rc;
	controller_s * controller = NULL;

	controller = g_slice_alloc0(sizeof(controller_s));
	controller->link = g_slice_alloc0(sizeof(link_s));
	controller->config = g_slice_alloc0(sizeof(config_controller_s));
	controller->state = g_slice_alloc0(sizeof(state_controller_s));
	/*память для обектов выделяется при чтении из базыданых*/
	rc = read_database_controller(number,controller);
	if(rc != SUCCESS){
		g_slice_free1(sizeof(controller_s),controller);
		g_slice_free1(sizeof(link_s),controller->link);
		g_slice_free1(sizeof(config_controller_s),controller->config);
		g_slice_free1(sizeof(state_controller_s),controller->state);
		controller = NULL;
	}
	controller->name = get_name_controller(controller->config);
	all_controller.list = g_slist_append(all_controller.list,controller);
	return controller;
}

int del_property_controller(controller_s * property)
{
	char * str;
	link_s * link;
	config_controller_s * config;
	state_controller_s * state;

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
	g_slice_free1(sizeof(controller_s),property);

	return SUCCESS;
}

block_controller_s block_controller;

int fill_block_controller(controller_s * controller)
{
	GtkLabel * label = block_controller.label;
	gtk_label_set_text(label,controller->name);
	return SUCCESS;
}

GtkWidget * create_block_controller(void)
{
	GtkWidget * label;

	label = gtk_label_new(NULL);
	layout_widget(label,GTK_ALIGN_FILL,GTK_ALIGN_FILL,TRUE,TRUE);
	gtk_widget_show(label);
	block_controller.label = GTK_LABEL(label);
	return label;
}
/*****************************************************************************/
