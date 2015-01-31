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

#include <ask/ipc/screendevice.h>
#include <ask/proto/screen.h>
#include <sys/common.h>

#include "../vbe.h"
#include "vesascreen.h"

class VESAGUI;
class VESATUI;

class VESA {
	VESA() = delete;

	class Client : public ask::ScreenClient {
	public:
		explicit Client(int f) : ScreenClient(f), screen() {
		}

		VESAScreen *screen;
	};

	class ScreenDevice : public ask::ScreenDevice<Client> {
	public:
		explicit ScreenDevice(const std::vector<ask::Screen::Mode> &modes,const char *path,mode_t mode)
			: ask::ScreenDevice<Client>(modes,path,mode) {
		}

		virtual void setScreenMode(Client *c,const char *shm,ask::Screen::Mode *mode,int type,bool sw);
		virtual void setScreenCursor(Client *c,gpos_t x,gpos_t y,int cursor);
		virtual void updateScreen(Client *c,gpos_t x,gpos_t y,gsize_t width,gsize_t height);
	};

public:
	static void init();
	static int run(void *arg);

private:
	static VESAGUI *gui;
	static VESATUI *tui;
	static std::vector<ask::Screen::Mode> modes;
};
