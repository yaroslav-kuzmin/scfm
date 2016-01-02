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

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

/*****************************************************************************/
/*    Локальные функции                                                      */
/*****************************************************************************/

#define REG_D300    0
#define REG_D301    1
#define REG_D302    2
#define REG_D303    3
#define REG_D304    4
#define REG_D305    5
#define REG_D306    6
#define REG_D307    7


static int set_config_controller(uint16_t * reg,config_controller_s * config)
{
	/*uint16_t reg;*/
	uint32_t type;
	/*uint64_t flag;*/

	type = reg[REG_D300];
	type <<= 16;
	type += reg[REG_D301];

	config->type = type;

	/*reg = reg[REG_302];*/

	return SUCCESS;
}

static int connect_tcp(link_s * link)
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
		g_warning("Несмог подключится к : %d",id);
		modbus_free(ctx);
		return FAILURE;
	}
	link->connect = ctx;
	link->dest = g_slice_alloc0(MODBUS_TCP_MAX_ADU_LENGTH);
	return SUCCESS;
}
static int connect_uart(link_s * link)
{
	return FAILURE;
}
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/

int link_disconnect_controller(link_s * link)
{
	uint16_t * dest = link->dest;
	modbus_t * ctx = (modbus_t*)link->connect;
	if(ctx != NULL){
    	modbus_close(ctx);
		modbus_free(ctx);
		g_slice_free1(MODBUS_TCP_MAX_ADU_LENGTH,dest);
	}
	link->connect = NULL;

	return SUCCESS;
}

int check_config_controller(config_controller_s * config_c,config_controller_s * config_d)
{
	return SUCCESS;
}
/*создать имя */
char * get_name_controller(config_controller_s * config)
{
	return NULL;
}
/*считать состояние*/
int link_state_controller(link_s * link,state_controller_s * state)
{
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
#define AMOUNT_CONFIG_REGISTR    8

/*считать конфигурацию*/
int link_config_controller(link_s * link,config_controller_s * config)
{
	int rc;
	uint16_t * dest = link->dest;
	modbus_t * ctx = (modbus_t*)link->connect;

	if(ctx == NULL){
		return FAILURE;
	}
	rc = modbus_read_registers(ctx,reg_D300,AMOUNT_CONFIG_REGISTR,dest);
	if(rc == -1){
		link_disconnect_controller(link);
		return FAILURE;
	}
	set_config_controller(dest,config);

	return SUCCESS;
}

int link_connect_controller(link_s * link)
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
