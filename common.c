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
#include <gtk/gtk.h>

#include <pub.h>

/*****************************************************************************/
/*    Общие переменые                                                        */
/*****************************************************************************/

const char STR_NAME_PROGRAMM[] = "Система Управления Лафетными Стволами";
const char STR_COPYRIGHT[] = "(C) <2015> <Кузьмин Ярослав>";
const char STR_COMMENT[] =
"Программа позволяет управлять большой группой\n"
" лафетных стволов управляемых контролером <X>\n"
" подключеных по протоколу modbus\n"
" Может выступать в роли сервера или клиента.";
const char STR_LICENSE[] =
"  Эта программа является свободным программным обеспечением:           \n"
"  вы можете распространять и/или изменять это в соответствии с         \n"
"  условиями в GNU General Public License, опубликованной               \n"
"  Фондом свободного программного обеспечения, как версии 3 лицензии,   \n"
"  или (по вашему выбору) любой более поздней версии.                   \n"
"                                                                       \n"
"  Эта программа распространяется в надежде, что она будет полезной,    \n"
"  но БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ; даже без подразумеваемой гарантии        \n"
"  Или ПРИГОДНОСТИ ДЛЯ КОНКРЕТНЫХ ЦЕЛЕЙ. См GNU General Public License  \n"
"  для более подробной информации.                                      \n"
"                                                                       \n"
"  Вы должны были получить копию GNU General Public License             \n"
"  вместе с этой программой. Если нет, см <http://www.gnu.org/licenses/>\n"
"                                                                         ";
const char STR_EMAIL[] = "kuzmin.yaroslav@gmail.com";
const char STR_EMAIL_LABEL[] = "kuzmin.yaroslav@gmail.com";
const char * STR_AUTHORS[] = {"Кузьмин Ярослав",NULL};

GString * pub = NULL;
/*****************************************************************************/
/*****************************************************************************/
/*    Общие функции                                                          */
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/*  конфиурирование системы                                                  */
/*****************************************************************************/
static char STR_CONFIG_FILE[] = G_DIR_SEPARATOR_S ".config" G_DIR_SEPARATOR_S "scfm" G_DIR_SEPARATOR_S "ini";
#ifdef G_OS_WIN32
static char STR_HOME_PATH[] = "HOMEPATH";
#endif
#ifdef G_OS_UNIX
static char STR_HOME_PATH[] = "HOME";
#endif

static int init_config(void)
{
	
	return SUCCESS;
}
static int deinit_config(void)
{

}
/*****************************************************************************/
/* система логирования                                                       */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int init_common(void)
{
	pub = g_string_new(NULL);

	init_config();
	return SUCCESS;
}

int deinit_common(void)
{
	g_string_free(pub,TRUE);
	deinit_config();

	return SUCCESS;
}
/*****************************************************************************/
