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

struct _generic_s
{
	GString * pub;
	GdkPixbuf * default_icon;
};
typedef struct _generic_s generic_s;

/**************************************/

#define FIRST_NUMBER_GROUP     0
#define TYPE_UNKNOWN           0xFF
#define TYPE_GROUP             0x80

#define TYPE_OBJECT            0x00
#define TYPE_VIDEOCAMERA       0x02

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
#endif

