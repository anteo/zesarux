/*
    ZEsarUX  ZX Second-Emulator And Released for UniX
    Copyright (C) 2013 Cesar Hernandez Bano

    This file is part of ZEsarUX.

    ZEsarUX is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>



#include "uartbridge.h"
#include "cpu.h"
#include "debug.h"
#include "utils.h"
#include "chardevice.h"
#include "screen.h"


//Nombre de la ruta al dispositivo uart bridge
//char uartbridge_name[PATH_MAX]="";
//temp
char uartbridge_name[PATH_MAX]="/dev/ttyUSB0";

//Si esta habilitado uart bridge
z80_bit uartbridge_enabled={0};

//file handler del dispositivo uart bridge. -1 si no esta abierto
int uartbridge_handler=-1;

enum chardevice_speed uartbridge_speed=CHDEV_SPEED_DEFAULT;

int uartbridge_available(void)
{
	//No dispositivo abierto
	if (uartbridge_enabled.v==0 || uartbridge_handler<0) return 0;
	else return 1;
}

void uartbridge_enable(void)
{

	debug_printf(VERBOSE_DEBUG,"Opening uart bridge");

	if (uartbridge_enabled.v) return;

	uartbridge_handler=chardevice_open(uartbridge_name,CHDEV_RDWR);

	if (uartbridge_handler>=0) {
		uartbridge_enabled.v=1;
        if (uartbridge_speed!=CHDEV_SPEED_DEFAULT) chardevice_setspeed(uartbridge_handler,uartbridge_speed);
	}
	else {
		debug_printf (VERBOSE_ERR,"Error opening uart bridge %s",uartbridge_name);
		uartbridge_enabled.v=0;
	}
}

void uartbridge_disable(void)
{
	if (uartbridge_enabled.v==0) return;

	debug_printf(VERBOSE_DEBUG,"Closing uart bridge");

	if (chardevice_close(uartbridge_handler)<0) {
		debug_printf (VERBOSE_ERR,"Error closing uart bridge");
	}

	uartbridge_enabled.v=0;
}


z80_byte uartbridge_readdata(void)
{
	z80_byte value=0;
	uartbridge_try_read(&value);
	return value;
}

void uartbridge_writedata(z80_byte value)
{
	uartbridge_try_write(value);
}

int uartbridge_try_read(z80_byte *value)
{
	if (!uartbridge_available()) return 0;
	if (chardevice_read(uartbridge_handler,value)!=1) return 0;
	generic_footertext_print_operating("UART");
	return 1;
}

int uartbridge_try_write(z80_byte value)
{
	if (!uartbridge_available() || chardevice_write(uartbridge_handler,value)!=1) return 0;
	generic_footertext_print_operating("UART");
	return 1;
}

void uartbridge_discard_input(void)
{
	if (uartbridge_available()) chardevice_discard_input(uartbridge_handler);
}

