###############################################################################
#                                                                             #
# сулс - система управления лафетными стволами                                #
#  Авторское Право (C) <2015> <Кузьмин Ярослав>                               #
#                                                                             #
#  Эта программа является свободным программным обеспечением:                 #
#  вы можете распространять и/или изменять это в соответствии с               #
#  условиями в GNU General Public License, опубликованной                     #
#  Фондом свободного программного обеспечения, как версии 3 лицензии,         #
#  или (по вашему выбору) любой более поздней версии.                         #
#                                                                             #
#  Эта программа распространяется в надежде, что она будет полезной,          #
#  но БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ; даже без подразумеваемой гарантии              #
#  Или ПРИГОДНОСТИ ДЛЯ КОНКРЕТНЫХ ЦЕЛЕЙ. См GNU General Public License        #
#  для более подробной информации.                                            #
#                                                                             #
#  Вы должны были получить копию GNU General Public License                   #
#  вместе с этой программой. Если нет, см <http://www.gnu.org/licenses/>      #
#                                                                             #
#  Адрес для контактов: kuzmin.yaroslav@gmail.com                             #
#                                                                             #
###############################################################################
#                                                                             #
# scfm - system control fire monitors                                         #
#  Copyright (C) <2015> <Kuzmin Yaroslav>                                     #
#                                                                             #
#  This program is free software: you can redistribute it and/or modify       #
#  it under the terms of the GNU General Public License as published by       #
#  the Free Software Foundation, either version 3 of the License, or          #
#  (at your option) any later version.                                        #
#                                                                             #
#  This program is distributed in the hope that it will be useful,            #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of             #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              #
#  GNU General Public License for more details.                               #
#                                                                             #
#  You should have received a copy of the GNU General Public License          #
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.      #
#                                                                             #
#  Email contact: kuzmin.yaroslav@gmail.com                                   #
#                                                                             #
###############################################################################

OS_LINUX=Linux
OS_WINDOWS=Windows
OS:=$(shell uname)
ifneq '$(OS)' '$(OS_LINUX)'
OS=$(OS_WINDOWS)
endif

OBJ_CATALOG=.obj/
DEPEND_CATALOG=.depend/

MODBUS_CATALOG=libmodbus/
LIB_MODBUS=$(MODBUS_CATALOG)libmodbus.a
LIB_MODBUS_OPTION=-lmodbus

EXEC=scfm.exe
SOURCE=$(wildcard *.c)
OBJS=$(patsubst %.c,$(OBJ_CATALOG)%.o,$(SOURCE))
DEPEND=$(patsubst %.c,$(DEPEND_CATALOG)%.d,$(SOURCE))

CXX=gcc
CFLAGS=-g2 -std=c99 -Wall -I. -I$(MODBUS_CATALOG) -DG_LOG_DOMAIN=\"scfm\" `pkg-config --cflags gtk+-3.0`
LDFLAGS=-g2 -L$(MODBUS_CATALOG) 
ifeq '$(OS)' '$(OS_WINDOWS)'
LDFLAGS+=-mwindows
endif
LIB=`pkg-config --libs gtk+-3.0` -lavformat -lavcodec -lswscale -lavutil -lsqlite3  
ifeq '$(OS)' '$(OS_WINDOWS)'
LIB+=-lws2_32	
endif	

HEADER_VERSION=version.h
GIT_REPOSITOR=.git
GIT_VERSION=./git_version.sh

COMPILE_RESOURCE=glib-compile-resources 
CATALOG_RESOURCE=resource/

CATALOG_IMAGE=$(CATALOG_RESOURCE)image
RESOURCE_IMAGE=$(CATALOG_RESOURCE)image.xml
RESOURCE_IMAGE_BUNDLE=$(CATALOG_RESOURCE)image.gresource

CATALOG_STYLE=$(CATALOG_RESOURCE)style
RESOURCE_STYLE=$(CATALOG_RESOURCE)style.xml
RESOURCE_STYLE_BUNDLE=$(CATALOG_RESOURCE)style.gresource

CATALOG_UI=$(CATALOG_RESOURCE)ui
RESOURCE_UI=$(CATALOG_RESOURCE)ui.xml
RESOURCE_UI_BUNDLE=$(CATALOG_RESOURCE)ui.gresource

ifeq '$(OS)' '$(OS_WINDOWS)'
RC=rcedit
ICON=$(CATALOG_STYLE)/assets/scfm.ico
endif

$(EXEC):$(OBJS) $(LIB_MODBUS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIB)
ifeq '$(OS)' '$(OS_WINDOWS)'
	$(RC) $(EXEC) --set-icon $(ICON)
endif		

$(LIB_MODBUS):$(MODBUS_CATALOG)
	make -C $<

$(OBJ_CATALOG)%.o:%.c
	$(CXX) $(CFLAGS) -c $< -o $@

$(DEPEND_CATALOG)%.d:%.c
	$(CXX) -MM -I. -I$(MODBUS_CATALOG) $< | sed -e '1s/^/.obj\//' > $@

include $(DEPEND)

$(HEADER_VERSION):$(GIT_REPOSITOR)
	$(GIT_VERSION)

STRIP=strip	

.PHONY:releas
releas:$(EXEC)
	$(STRIP) -s $(EXEC)		

.PHONY:resource
resource:
	$(COMPILE_RESOURCE) --target=$(RESOURCE_IMAGE_BUNDLE) --sourcedir=$(CATALOG_IMAGE) --generate $(RESOURCE_IMAGE)
	$(COMPILE_RESOURCE) --target=$(RESOURCE_STYLE_BUNDLE) --sourcedir=$(CATALOG_STYLE) --generate $(RESOURCE_STYLE)
	$(COMPILE_RESOURCE) --target=$(RESOURCE_UI_BUNDLE) --sourcedir=$(CATALOG_UI) --generate $(RESOURCE_UI)

.PHONY:clean
clean:
	-rm -f $(EXEC) *~ $(OBJ_CATALOG)*.o $(DEPEND_CATALOG)*.d $(RESOURCE_BASE_SOURCE) $(RESOURCE_BASE_OBJ) 

