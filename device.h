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

#ifndef DEVICE_H
#define DEVICE_H

flag_t controller_type_device(config_controller_s * config);
char * controller_name(config_controller_s * config);
flag_t controller_state_valve(state_controller_s * state);
flag_t controller_mode(state_controller_s * state);
flag_t controller_state_fire_alarm(state_controller_s * state);
flag_t controller_info(state_controller_s * state,flag_t * info);

flag_t link_controller_command(link_s * link,command_u command);
flag_t link_controller_state(link_s * link,state_controller_s * state);
flag_t link_controller_config(link_s * link,config_controller_s * config);
flag_t link_controller_connect(link_s * link);
flag_t link_controller_disconnect(link_s * link);
flag_t link_controller(link_s * link,config_controller_s * config,state_controller_s * state);
flag_t link_check_connect(link_s * link);

flag_t controller_check_config(config_controller_s * config_c,config_controller_s * config_d);
flag_t controller_copy_state(state_controller_s * des,state_controller_s * src);
flag_t controller_null_state(state_controller_s * state);

#endif

