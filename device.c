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
#include "modbus.h"

#include <string.h>

/*****************************************************************************/
/*                                                                           */
/*  Проверка подключения конторллера                                         */
/*                                                                           */
/*****************************************************************************/
flag_t device_check_connect(link_s * link)
{
	if(link->connect == NULL){
		return STATUS_ON_LINK_OFF;
	}
	return STATUS_ON_NORM;
}

/*****************************************************************************/
/*                                                                           */
/*   Отключение от конторллера                                               */
/*                                                                           */
/*****************************************************************************/

flag_t device_disconnect(link_s * link)
{
	uint16_t * dest = link->dest;
	modbus_t * ctx = (modbus_t*)link->connect;
	if(ctx != NULL){
   	modbus_close(ctx);
		modbus_free(ctx);
		if(link->type == TYPE_LINK_TCP){
			g_slice_free1(MODBUS_TCP_MAX_ADU_LENGTH,dest);
		}
		if(link->type == TYPE_LINK_UART){
			g_slice_free1(MODBUS_RTU_MAX_ADU_LENGTH,dest);
		}
	}
	link->connect = NULL;

	return SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/*   Подключения к контролеру и проверка подключения                         */
/*                                                                           */
/*****************************************************************************/


#define REG_D300            0
#define AMOUNT_BIT_D300     16

#define REG_D301            1
#define AMOUNT_BIT_D301     16

#define REG_D302            2
#define AMOUNT_BIT_D302     16

#define REG_D303            3
#define AMOUNT_BIT_D303     5

#define REG_D304            4
#define AMOUNT_BIT_D304     4

#define REG_D305            5
#define AMOUNT_BIT_D305     3

#define REG_D306            6
#define AMOUNT_BIT_D306     16

#define REG_D307            7
#define AMOUNT_BIT_D307     16

#define AMOUNT_BIT_CONFIG   (AMOUNT_BIT_D300 + AMOUNT_BIT_D301 + AMOUNT_BIT_D302\
                            +AMOUNT_BIT_D303 + AMOUNT_BIT_D304 + AMOUNT_BIT_D305\
                            +AMOUNT_BIT_D306 + AMOUNT_BIT_D307)
#if !(AMOUNT_BIT_CONFIG > 64)
#error Увеличить колличество бит
#endif

static flag_t set_config_controller(uint16_t * reg,config_controller_s * config)
{
	uint64_t temp;
	uint32_t type;
	uint64_t flag;

	type = reg[REG_D300];
	type <<= AMOUNT_BIT_D300;
	type += reg[REG_D301];

	flag = reg[REG_D302];

	temp = reg[REG_D303];
	temp <<= AMOUNT_BIT_D302;
	flag += temp;

	temp = reg[REG_D304];
	temp <<= (AMOUNT_BIT_D302 + AMOUNT_BIT_D303);
	flag += temp;

	temp = reg[REG_D305];
	temp <<= (AMOUNT_BIT_D302 + AMOUNT_BIT_D303 +AMOUNT_BIT_D304);
	flag += temp;

	temp = reg[REG_D306];
	temp <<= (AMOUNT_BIT_D302 + AMOUNT_BIT_D303 + AMOUNT_BIT_D304 + AMOUNT_BIT_D305);
	flag += temp;

	temp = reg[REG_D307];
	temp <<= (AMOUNT_BIT_D302 + AMOUNT_BIT_D303 + AMOUNT_BIT_D304 + AMOUNT_BIT_D305 + AMOUNT_BIT_D306);
	flag += temp;

	config->type = type;
	config->flag = flag;

	return SUCCESS;
}

#define REG_D100    0
#define REG_D101    1
#define REG_D102    2
#define REG_D103    3
#define REG_D104    4
#define REG_D105    5
#define REG_D106    6
#define REG_D107    7
#define REG_D108    8
#define REG_D109    9
#define REG_D110   10
#define REG_D111   11
#define REG_D112   12
#define REG_D113   13
#define REG_D114   14
#define REG_D115   15
#define REG_D116   16
#define REG_D117   17
#define REG_D118   18

static flag_t set_state_controller(uint16_t * dest,state_controller_s *state)
{
	state->lafet               = dest[REG_D100];
	state->tic_vertical        = dest[REG_D101];
	state->tic_horizontal      = dest[REG_D102];
	state->encoder_vertical    = dest[REG_D103];
	state->encoder_horizontal  = dest[REG_D104];
	state->pressure            = dest[REG_D105];
	state->amperage_vertical   = dest[REG_D106];
	state->amperage_horizontal = dest[REG_D107];
	state->work                = dest[REG_D108];
  state->valve               = dest[REG_D110];
  state->tic_valve           = dest[REG_D111];
  state->fire_sensor         = dest[REG_D114];
  state->fire_alarm          = dest[REG_D116];
  state->device              = dest[REG_D118];

	return SUCCESS;
}

flag_t controller_copy_state(state_controller_s * des,state_controller_s * src)
{
	memmove(des,src,sizeof(state_controller_s));
	return SUCCESS;
}

flag_t controller_null_state(state_controller_s * state)
{
	memset(state,0,sizeof(state_controller_s));
	return SUCCESS;
}
static flag_t connect_tcp(link_s * link)
{
	int rc;
	char * address = link->address;
	uint16_t port = link->port;
	uint8_t id = link->id;
	modbus_t *ctx;

	ctx = modbus_new_tcp(address,port);
	if (ctx == NULL) {
		return FAILURE;
	}

	modbus_set_error_recovery(ctx,(MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL));

	modbus_set_slave(ctx,id);

	rc = modbus_connect(ctx);
	if(rc == -1){
		modbus_free(ctx);
		return FAILURE;
	}

	modbus_set_debug(ctx,FALSE);

	link->connect = ctx;
	link->dest = g_slice_alloc0(MODBUS_TCP_MAX_ADU_LENGTH);
	return SUCCESS;
}

static flag_t connect_uart(link_s * link)
{
	int rc;
	char * device = link->device;
	uint32_t baud = link->baud;
	int8_t parity = link->parity;
	uint8_t data_bit = link->data_bit;
	uint8_t stop_bit = link->stop_bit;
	uint8_t id = link->id;
	modbus_t *ctx;

	ctx = modbus_new_rtu(device,baud,parity,data_bit,stop_bit);
	if (ctx == NULL) {
		return FAILURE;
	}

	modbus_set_error_recovery(ctx,(MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL));

	modbus_set_slave(ctx,id);

	rc = modbus_connect(ctx);
	if(rc == -1){
		modbus_free(ctx);
		link->connect = NULL;
		return FAILURE;
	}

	modbus_set_debug(ctx,FALSE);

	link->connect = ctx;
	link->dest = g_slice_alloc0(MODBUS_RTU_MAX_ADU_LENGTH);
	return SUCCESS;
}

flag_t controller_check_config(config_controller_s * config_c,config_controller_s * config_d)
{
	if(config_c->type != config_d->type){
		return FAILURE;
	}
	if(config_c->flag != config_d->flag){
		return FAILURE;
	}
	return SUCCESS;
}

static uint16_t reg_D100 = 0x1064;
/*static uint16_t reg_D101 = 0x1065;*/
/*static uint16_t reg_D102 = 0x1066;*/
/*static uint16_t reg_D103 = 0x1067;*/
/*static uint16_t reg_D104 = 0x1068;*/
/*static uint16_t reg_D105 = 0x1069;*/
/*static uint16_t reg_D106 = 0x106A;*/
/*static uint16_t reg_D107 = 0x106B;*/
/*static uint16_t reg_D108 = 0x106C;*/
/*static uint16_t reg_D109 = 0x106D;*/
/*static uint16_t reg_D110 = 0x106E;*/
/*static uint16_t reg_D111 = 0x106F;*/
/*static uint16_t reg_D112 = 0x1070;*/
/*static uint16_t reg_D113 = 0x1071;*/
/*static uint16_t reg_D114 = 0x1072;*/
/*static uint16_t reg_D115 = 0x1073;*/
/*static uint16_t reg_D116 = 0x1074;*/
/*static uint16_t reg_D117 = 0x1075;*/
/*static uint16_t reg_D118 = 0x1076;*/
#define AMOUNT_STATE_REGISTER    19

/*считать состояние*/
flag_t device_read_state(link_s * link,state_controller_s * state)
{
 	int rc;
	uint16_t * dest = link->dest;
	modbus_t * ctx = (modbus_t*)link->connect;

	if(ctx == NULL){
		return FAILURE;
	}

	modbus_set_slave(ctx,link->id);

	/*g_debug("ID %d ",link->id);*/
	rc = modbus_read_registers(ctx,reg_D100,AMOUNT_STATE_REGISTER,dest);
	if(rc == -1){
		g_debug("ID %d : error read state",link->id);
		return FAILURE;
	}
	/*g_debug("read state : success");*/
	/*TODO запись чтение в разных потоках */
	if(dest == NULL){
		return FAILURE;
	}

	set_state_controller(dest,state);
	return SUCCESS;
}

static uint16_t reg_D300 = 0x112C;
/*static uint16_t reg_D301 = 0x112D;*/
/*static uint16_t reg_D302 = 0x112E;*/
/*static uint16_t reg_D303 = 0x112F;*/
/*static uint16_t reg_D304 = 0x1130;*/
/*static uint16_t reg_D305 = 0x1131;*/
/*static uint16_t reg_D306 = 0x1132;*/
/*static uint16_t reg_D307 = 0x1133;*/
#define AMOUNT_CONFIG_REGISTER    8

/*считать конфигурацию*/
flag_t device_read_config(link_s * link,config_controller_s * config)
{
	int rc;
	uint16_t * dest = link->dest;
	modbus_t * ctx = (modbus_t*)link->connect;

	if(ctx == NULL){
		return FAILURE;
	}

	modbus_set_slave(ctx,link->id);

	rc = modbus_read_registers(ctx,reg_D300,AMOUNT_CONFIG_REGISTER,dest);
	if(rc == -1){
		g_debug("ID %d : error read config",link->id);
		return FAILURE;
	}
	/*TODO запись чтение в разных потоках */
	if(dest == NULL){
		return FAILURE;
	}
	set_config_controller(dest,config);

	return SUCCESS;
}

flag_t device_connect(link_s * link)
{
	int rc;
	switch(link->type){
		case TYPE_LINK_TCP:
			rc = connect_tcp(link);
			break;
		case TYPE_LINK_UART:
			rc = connect_uart(link);
			break;
		default:
			rc = FAILURE;
			break;
	}
	return rc;
}

/*****************************************************************************/
/*                                                                           */
/*    Команды контролера                                                     */
/*                                                                           */
/*****************************************************************************/

static uint16_t reg_D200 = 0x10C8;
static uint16_t reg_D201 = 0x10C9;
static uint16_t reg_D202 = 0x10CA;
static uint16_t reg_D203 = 0x10CB;
static uint16_t reg_D204 = 0x10CC;
static uint16_t reg_D205 = 0x10CD;
/*static uint16_t reg_D206 = 0x10CE; РЕЗЕРВ*/
static uint16_t reg_D207 = 0x10CF;
static uint16_t reg_D208 = 0x10D0;

static uint16_t VALUE_LAFET_STOP  = 0x0000;
static uint16_t VALUE_LAFET_UP    = 0x0001;
static uint16_t VALUE_LAFET_DOWN  = 0x0002;
static uint16_t VALUE_LAFET_LEFT  = 0x0004;
static uint16_t VALUE_LAFET_RIGHT = 0x0008;
static uint16_t VALUE_ACTUATOR_STOP = 0x0000;
static uint16_t VALUE_SPRAY_LESS    = 0x0010;
static uint16_t VALUE_SPRAY_MORE    = 0x0020;
static uint16_t VALUE_RATE_MORE     = 0x0040;
static uint16_t VALUE_RATE_LESS     = 0x0080;
static uint16_t VALUE_VEIL_LESS     = 0x0100;
static uint16_t VALUE_VEIL_MORE     = 0x0200;
static uint16_t VALUE_OSCILLATION_STOP       = 0x0000;
static uint16_t VALUE_OSCILLATION_HORIZONTAL = 0x0001;
static uint16_t VALUE_OSCILLATION_VERTICAL   = 0x0002;
static uint16_t VALUE_OSCILLATION_SAW        = 0x0004;
static uint16_t VALUE_OSCILLATION_STEP       = 0x0008;
static uint16_t VALUE_VALVE_STOP       = 0x0000;
static uint16_t VALUE_VALVE_OPEN       = 0x0001;
static uint16_t VALUE_VALVE_CLOSE      = 0x0002;
static uint16_t VALUE_ALL_DEVICE_STOP      = 0x0000;
/*
static uint16_t VALUE_DEVICE_1_STATE_0     = 0x0001;
static uint16_t VALUE_DEVICE_1_STATE_1     = 0x0002;
static uint16_t VALUE_DEVICE_2_STATE_0     = 0x0004;
static uint16_t VALUE_DEVICE_2_STATE_1     = 0x0008;
static uint16_t VALUE_DEVICE_3_STATE_0     = 0x0010;
static uint16_t VALUE_DEVICE_3_STATE_1     = 0x0020;
static uint16_t VALUE_DEVICE_4_STATE_0     = 0x0040;
static uint16_t VALUE_DEVICE_4_STATE_1     = 0x0080;
static uint16_t VALUE_DEVICE_5_STATE_0     = 0x0100;
static uint16_t VALUE_DEVICE_5_STATE_1     = 0x0200;
static uint16_t VALUE_DEVICE_6_STATE_0     = 0x0400;
static uint16_t VALUE_DEVICE_6_STATE_1     = 0x0800;
static uint16_t VALUE_DEVICE_7_STATE_0     = 0x1000;
static uint16_t VALUE_DEVICE_7_STATE_1     = 0x2000;
static uint16_t VALUE_DEVICE_8_STATE_0     = 0x4000;
static uint16_t VALUE_DEVICE_8_STATE_1     = 0x8000;
*/
static uint16_t VALUE_MODE_STOP  = 0x0000;
static uint16_t VALUE_MODE_AUTO   = 0x0001;
static uint16_t VALUE_MODE_MANUAL = 0x0002;
static uint16_t VALUE_MODE_CONFIG = 0x0004;

static flag_t set_value_command(command_u command,uint16_t * reg,uint16_t * value)
{
	switch(command.part.value){
		case COMMAND_LAFET_UP:
			*reg = reg_D200;
			*value = VALUE_LAFET_UP;
			break;
		case COMMAND_LAFET_DOWN:
			*reg = reg_D200;
			*value = VALUE_LAFET_DOWN;
			break;
		case COMMAND_LAFET_RIGHT:
			*reg = reg_D200;
			*value = VALUE_LAFET_RIGHT;
			break;
		case COMMAND_LAFET_LEFT:
			*reg = reg_D200;
			*value = VALUE_LAFET_LEFT;
			break;
		case COMMAND_LAFET_SPEED_VERTICAL:
			*reg = reg_D201;
			*value = command.part.parametr;
			break;
		case COMMAND_LAFET_SPEED_HORIZONTAL:
			*reg = reg_D202;
			*value = command.part.parametr;
			break;
		case COMMAND_ACTUATOT_STOP:
			*reg = reg_D200;
			*value = VALUE_ACTUATOR_STOP;
			break;
		case COMMAND_SPRAY_LESS:
			*reg = reg_D200;
			*value = VALUE_SPRAY_LESS;
			break;
		case COMMAND_SPRAY_MORE:
			*reg = reg_D200;
			*value = VALUE_SPRAY_MORE;
			break;
		case COMMAND_RATE_LESS:
			*reg = reg_D200;
			*value = VALUE_RATE_LESS;
			break;
		case COMMAND_RATE_MORE:
			*reg = reg_D200;
			*value = VALUE_RATE_MORE;
			break;
		case COMMAND_VEIL_LESS:
			*reg = reg_D200;
			*value = VALUE_VEIL_LESS;
			break;
		case COMMAND_VEIL_MORE:
			*reg = reg_D200;
			*value = VALUE_VEIL_MORE;
			break;
		case COMMAND_OSCILLATION_STOP:
			*reg = reg_D203;
			*value = VALUE_OSCILLATION_STOP;
			break;
		case COMMAND_OSCILLATION_VERTICAL:
			*reg = reg_D203;
			*value = VALUE_OSCILLATION_VERTICAL;
			break;
		case COMMAND_OSCILLATION_HORIZONTAL:
			*reg = reg_D203;
			*value = VALUE_OSCILLATION_HORIZONTAL;
			break;
		case COMMAND_OSCILLATION_SAW:
			*reg = reg_D203;
			*value = VALUE_OSCILLATION_SAW;
			break;
		case COMMAND_OSCILLATION_STEP:
			*reg = reg_D203;
			*value = VALUE_OSCILLATION_STEP;
			break;
		case COMMAND_VALVE_STOP:
			*reg = reg_D204;
			*value = VALUE_VALVE_STOP;
			break;
		case COMMAND_VALVE_OPEN:
			*reg = reg_D204;
			*value = VALUE_VALVE_OPEN;
			break;
		case COMMAND_VALVE_CLOSE:
			*reg = reg_D204;
			*value = VALUE_VALVE_CLOSE;
			break;
		case COMMAND_VALVE_POSITION:
			*reg = reg_D205;
			*value = command.part.parametr;
			break;
		case COMMAND_ALL_DEVICE_STOP:
			*reg = reg_D207;
			*value = VALUE_ALL_DEVICE_STOP;
			break;
		case COMMAND_MODE_STOP:
			*reg = reg_D208;
			*value = VALUE_MODE_STOP;
			break;
		case COMMAND_MODE_AUTO:
			*reg = reg_D208;
			*value = VALUE_MODE_AUTO;
			break;
		case COMMAND_MODE_MANUAL:
			*reg = reg_D208;
			*value = VALUE_MODE_MANUAL;
			break;
		case COMMAND_MODE_CONFIG:
			*reg = reg_D208;
			*value = VALUE_MODE_CONFIG;
			break;
		default:
			/*TODO возможно возврашать ошибку */
			*reg = reg_D200;
			*value = VALUE_LAFET_STOP;
			break;
	}
	return SUCCESS;
}

flag_t device_write_command(link_s * link,command_u command)
{
	int rc;
	modbus_t * ctx = (modbus_t*)link->connect;
	uint16_t reg = 0;
	uint16_t value = 0;

	if(ctx == NULL){
		return FAILURE;
	}

	set_value_command(command,&reg,&value);

	modbus_set_slave(ctx,link->id);
	rc = modbus_write_register(ctx,reg,value);
	if(rc == -1){
		return FAILURE;
	}
	/*g_debug("write : register %#x : value %#x",reg,value);*/
	return SUCCESS;
}


/*****************************************************************************/
/*                                                                           */
/*   Возврашаемые состояния                                                  */
/*                                                                           */
/*****************************************************************************/

static uint16_t min_type_lsd = 6;
flag_t controller_type_device(config_controller_s * config)
{
	flag_t type_device;
	uint32_t type = config->type;
	uint16_t type_lsd;
	type_lsd = type >> AMOUNT_BIT_D300;

	if(type_lsd <= min_type_lsd){
		type_device = TYPE_DEVICE_LSD;
	}
	else{
		type_device = TYPE_DEVICE_ROBOT;
	}

	return type_device;
}
/*TODO  принимает и указатель в который записывет имя*/
char * controller_name(config_controller_s * config)
{
	uint16_t name_device;
	uint16_t liter_device;
	uint32_t type = config->type;

	name_device = type >> AMOUNT_BIT_D300;
	/*Регистр D301 литраж установки */
	liter_device = type;

	/*Регистр D300 названия установки*/
	switch(name_device){
		case 1:
			g_string_printf(pub,"ЛСД-С%dУ",liter_device);
			break;
		case 2:
			g_string_printf(pub,"ЛСД-С%dУ-Ех",liter_device);
			break;
		case 3:
			g_string_printf(pub,"ЛСД-С%dУ-ИК",liter_device);
			break;
		case 4:
			g_string_printf(pub,"ЛСД-С%dУ-Ех-ИК",liter_device);
			break;
		case 5:
			g_string_printf(pub,"ЛСД-С%dУ-ТВ",liter_device);
			break;
		case 6:
			g_string_printf(pub,"ЛСД-С%dУ-Ех-ТВ",liter_device);
			break;
		case 7:
			g_string_printf(pub,"ПР-ЛСД-С%dУ-ИК-ТВ",liter_device);
			break;
		case 8:
			g_string_printf(pub,"ПР-ЛСД-С%dУ-Ех-ИК-ТВ",liter_device);
			break;
		default:
			g_string_printf(pub,"ЛСД-%d",liter_device);
			break;
	}
	return g_strdup(pub->str);
}

flag_t controller_state_valve(state_controller_s * state)
{
	/*valve значение регистра  D110 */
	/*
	бит 0 - датчик состояния "ОТКРЫТ"
	бит 1 - датчик состояния "ЗАКРЫТ"
	бит 2 - состояние привода "ОТКРЫВАЕТ"
	бит 3 - состояние привода "ЗАКРЫВАЕТ" */
#define BIT_VALVE_OPEN             0x0001
#define BIT_VALVE_CLOSE            0x0002
#define BIT_VALVE_OPEN_RUN         0x0004
#define BIT_VALVE_CLOSE_RUN        0x0008
#define VALVE_OPEN(b)        (b & BIT_VALVE_OPEN)
#define VALVE_CLOSE(b)       (b & BIT_VALVE_CLOSE)
#define VALVE_OPEN_RUN(b)    (b & BIT_VALVE_OPEN_RUN)
#define VALVE_CLOSE_RUN(b)   (b & BIT_VALVE_CLOSE_RUN)
	uint16_t bit_valve = state->valve;

	if(VALVE_OPEN(bit_valve)){
		return STATE_VALVE_OPEN;
	}
	if(VALVE_CLOSE(bit_valve)){
		return STATE_VALVE_CLOSE;
	}
	if(VALVE_OPEN_RUN(bit_valve)){
		return STATE_VALVE_OPEN_RUN;
	}
	if(VALVE_CLOSE_RUN(bit_valve)){
		return STATE_VALVE_CLOSE_RUN;
	}
	return STATE_VALVE_ERROR;
}

/*lafet значение регистра D100*/
/*
бит 0 - датчик положения "ВВЕРХ"
бит 1 - датчик положения "ВНИЗ"
бит 2 - датчик положения "ВЛЕВО"
бит 3 - датчик положения "ВПРАВО"
бит 4 - резерв
бит 5 - резерв
бит 6 - резерв
бит 7 - резерв
бит 8 - состояние привода вертик. оси "ВВЕРХ"
бит 9 - состояние привода вертик. оси "ВНИЗ"
бит 10 - состояние привода горизонт. оси "ВЛЕВО"
бит 11 - состояние привода горизонт. оси "ВПРАВО"
бит 12 - состояние привода актуатора 1 "УЖЕ"
бит 13 - состояние привода актуатора 1 "ШИРЕ"
бит 14 - состояние привода актуатора 2 "МЕНЬШЕ"
бит 15 - состояние привода актуатора 2 "БОЛЬШЕ"
*/
#define BIT_LIMIT_UP      0x0001
#define BIT_LIMIT_BOTTOM  0x0002
#define BIT_LIMIT_LEFT    0x0004
#define BIT_LIMIT_RIGHT   0x0008
#define BIT_LAFET_UP      0x0100
#define BIT_LAFET_BOTTOM  0x0200
#define BIT_LAFET_LEFT    0x0400
#define BIT_LAFET_RIGHT   0x0800
#define BIT_SPRAY_LESS    0x1000
#define BIT_SPRAY_MORE    0x2000
#define BIT_RATE_LESS     0x4000
#define BIT_RATE_MORE     0x8000

#define LIMIT_UP(b)       (b & BIT_LIMIT_UP)
#define LIMIT_BOTTOM(b)   (b & BIT_LIMIT_BOTTOM)
#define LIMIT_LEFT(b)     (b & BIT_LIMIT_LEFT)
#define LIMIT_RIGHT(b)    (b & BIT_LIMIT_RIGHT)
#define LAFET_UP(b)       (b & BIT_LAFET_UP)
#define LAFET_BOTTOM(b)   (b & BIT_LAFET_BOTTOM)
#define LAFET_LEFT(b)     (b & BIT_LAFET_LEFT)
#define LAFET_RIGHT(b)    (b & BIT_LAFET_RIGHT)
#define SPRAY_LESS(b)     (b & BIT_SPRAY_LESS)
#define SPRAY_MORE(b)     (b & BIT_SPRAY_MORE)
#define RATE_LESS(b)      (b & BIT_RATE_LESS)
#define RATE_MORE(b)      (b & BIT_RATE_MORE)

/* work значение регистра  D108 */
/*
бит 0 - ошибка вертикальной оси
бит 1 - ошибка горизонтальной оси
бит 2 - ошибка привода актуатора 1
бит 3 - ошибка привода актуатора 2
бит 4 - ошибка привода заслонки
бит 5 - резерв
бит 6 - резерв
бит 7 - резерв
бит 8 - автоматический режим работы
бит 9 - ручной режим работы (управление местное)
бит 10 - ручной режим работы (управление оператора)
бит 11 - режим программирования
бит 12 - режим  горизонтальной осцилляции
бит 13 - режим вертикальной осцилляции
бит 14 - режим осцилляции SAW
бит 15 - режим осцилляции STEP
*/

#define BIT_ERROR_VERTICAL            0x0001
#define BIT_ERROR_HORIZONTAL          0x0002
#define BIT_ERROR_ACTUATOR_SPRAY      0x0004
#define BIT_ERROR_ACTUATOR_RATE       0x0008
#define BIT_ERROR_VALVE               0x0010
#define BIT_MODE_AUTO                 0x0100
#define BIT_MODE_MANUAL_0             0x0200
#define BIT_MODE_MANUAL_1             0x0400
#define BIT_MODE_TEST                 0x0800
#define BIT_OSCILLATION_HORIZONTAL    0x1000
#define BIT_OSCILLATION_VERTICAL      0x2000
#define BIT_OSCILLATION_SAW           0x4000
#define BIT_OSCILLATION_STEP          0x8000

#define ERROR_VERTICAL(b)          (b & BIT_ERROR_VERTICAL)
#define ERROR_HORIZONTAL(b)        (b & BIT_ERROR_HORIZONTAL)
#define ERROR_ACTUATOR_SPRAY(b)    (b & BIT_ERROR_ACTUATOR_SPRAY)
#define ERROR_ACTUATOR_RATE(b)     (b & BIT_ERROR_ACTUATOR_RATE)
#define ERROR_VALVE(b)             (b & BIT_ERROR_VALVE)
#define MODE_AUTO(b)               (b & BIT_MODE_AUTO)
#define MODE_MANUAL_0(b)           (b & BIT_MODE_MANUAL_0)
#define MODE_MANUAL_1(b)           (b & BIT_MODE_MANUAL_1)
#define MODE_TEST(b)               (b & BIT_MODE_TEST)
#define OSCILLATION_HORIZONTAL(b)  (b & BIT_OSCILLATION_HORIZONTAL)
#define OSCILLATION_VERTICAL(b)    (b & BIT_OSCILLATION_VERTICAL)
#define OSCILLATION_SAW(b)         (b & BIT_OSCILLATION_SAW)
#define OSCILLATION_STEP(b)        (b & BIT_OSCILLATION_STEP)

flag_t controller_mode(state_controller_s * state)
{
	uint16_t work = state->work;
	if(MODE_AUTO(work)){
		return STATE_MODE_AUTO;
	}
	if(MODE_MANUAL_0(work)){
		if(MODE_TEST(work)){
			return STATE_MODE_TEST;
		}
		return STATE_MODE_MANUAL;
	}
	if(MODE_MANUAL_1(work)){
		if(MODE_TEST(work)){
			return STATE_MODE_TEST;
		}
		return STATE_MODE_MANUAL;
	}
	return STATE_MODE_ERROR;
}


flag_t controller_state_info(state_controller_s * state)
{
	uint16_t work = state->work;
	uint16_t lafet = state->lafet;
/*
	flag_t mode = controller_mode(state);

	if(mode == STATE_MODE_ERROR){
		state->info[STATE_INFO_NORM] = NOT_OK;
		state->info[STATE_INFO_ERROR] = OK;
		return STATE_INFO_ERROR;
	}
*/
	state->info[STATE_INFO_NORM] = OK;
	state->info[STATE_INFO_ERROR] = NOT_OK;

	if(!LIMIT_UP(lafet)){
		state->info[STATE_INFO_LIMIT_VERTICAL] = OK;
		state->info[STATE_INFO_NORM] = NOT_OK;
	}
	else{
		if(!LIMIT_BOTTOM(lafet)){
			state->info[STATE_INFO_LIMIT_VERTICAL] = OK;
			state->info[STATE_INFO_NORM] = NOT_OK;
		}
		else{
			state->info[STATE_INFO_LIMIT_VERTICAL] = NOT_OK;
		}
	}

	if(!LIMIT_LEFT(lafet)){
		state->info[STATE_INFO_LIMIT_HORIZONTAL] = OK;
		state->info[STATE_INFO_NORM] = NOT_OK;
	}
	else{
		if(!LIMIT_RIGHT(lafet)){
			state->info[STATE_INFO_LIMIT_HORIZONTAL] = OK;
			state->info[STATE_INFO_NORM] = NOT_OK;
		}
		else{
			state->info[STATE_INFO_LIMIT_HORIZONTAL] = NOT_OK;
		}
	}

	/*g_info("D108 : %#x",work);*/
	if(ERROR_VERTICAL(work)){
		state->info[STATE_INFO_CRASH_VERTICAL] = OK;
		state->info[STATE_INFO_NORM] = NOT_OK;
		/*g_info("0 vertical %d",state->info[STATE_INFO_CRASH_VERTICAL]);*/
	}
	else{
		state->info[STATE_INFO_CRASH_VERTICAL] = NOT_OK;
		/*g_info("1 vertical %d",state->info[STATE_INFO_CRASH_VERTICAL]);*/
	}

	if(ERROR_HORIZONTAL(work)){
		state->info[STATE_INFO_CRASH_HORIZONTAL] = OK;
		state->info[STATE_INFO_NORM] = NOT_OK;
	}
	else{
		state->info[STATE_INFO_CRASH_HORIZONTAL] = NOT_OK;
	}

	if(ERROR_ACTUATOR_SPRAY(work)){
		state->info[STATE_INFO_CRASH_SPARY] = OK;
		state->info[STATE_INFO_NORM] = NOT_OK;
	}
	else{
		state->info[STATE_INFO_CRASH_SPARY] = NOT_OK;
	}

	if(ERROR_ACTUATOR_RATE(work)){
		state->info[STATE_INFO_CRASH_RATE] = OK;
		state->info[STATE_INFO_NORM] = NOT_OK;
	}
	else{
		state->info[STATE_INFO_CRASH_RATE] = NOT_OK;
	}

	if(ERROR_VALVE(work)){
		state->info[STATE_INFO_CRASH_VALVE] = OK;
		state->info[STATE_INFO_NORM] = NOT_OK;
	}
	else{
		state->info[STATE_INFO_CRASH_VALVE] = NOT_OK;
	}

	return STATE_INFO_NORM;
}

flag_t controller_state_fire_alarm(state_controller_s * state)
{
/*
	flag_t mode = controller_mode(state);

	if(mode == STATE_MODE_ERROR){
		return STATE_FIRE_ALARM_ERROR;
	}
*/
	if(state->fire_alarm){
		return STATE_FIRE_ALARM_ON;
	}
	return STATE_FIRE_ALARM_OFF;
}

flag_t controller_state_oscillation(state_controller_s * state)
{
	flag_t mode = controller_mode(state);

	if(mode == STATE_MODE_ERROR){
		return STATE_OSCILLATION_ERROR;
	}

	if(OSCILLATION_VERTICAL(state->work)){
		return STATE_OSCILLATION_VERTICAL;
	}
	if(OSCILLATION_HORIZONTAL(state->work)){
		return STATE_OSCILLATION_HORIZONTAL;
	}
	if(OSCILLATION_SAW(state->work)){
		return STATE_OSCILLATION_SAW;
	}
	if(OSCILLATION_STEP(state->work)){
		return STATE_OSCILLATION_STEP;
	}
	return STATE_OSCILLATION_STOP;

}
/*****************************************************************************/
