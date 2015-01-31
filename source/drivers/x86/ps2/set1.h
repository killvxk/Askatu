/**
 * $Id$
 * Copyright (C) 2008 - 2014 Nils Asmussen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#pragma once

#include <sys/common.h>

class ScancodeSet1 {
	ScancodeSet1() = delete;

	struct Entry {
		uchar def;
		uchar ext;
	};

public:
	/**
	 * Converts the given scancode to a keycode
	 *
	 * @param flags will be set to the flags
	 * @param keycode will be set to the keycode
	 * @param scanCode the received scancode
	 * @param leds the LED status
	 * @return true if it was a keycode
	 */
	static bool getKeycode(uchar *flags,uchar *keycode,uchar scanCode,uint8_t leds);

private:
	static uchar set;
	static Entry sc2kc[];
};
