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

#include <ask/proto/ui.h>
#include <gui/graphics/rectangle.h>
#include <sys/common.h>
#include <sys/messages.h>

#define WINDOW_COUNT					32
#define WINID_UNUSED					WINDOW_COUNT

#define WIN_STYLE_DEFAULT				0
#define WIN_STYLE_POPUP					1
#define WIN_STYLE_DESKTOP				2

typedef uint32_t tColor;

class WinRect : public gui::Rectangle {
public:
	explicit WinRect() : gui::Rectangle(), id(WINID_UNUSED) {
	}
	explicit WinRect(const gui::Rectangle &r) : gui::Rectangle(r), id(WINID_UNUSED) {
	}

	gwinid_t id;
};

/* a window */
struct Window : public WinRect {
	gpos_t z;
	int owner;
	int evfd;
	uint style;
	gsize_t titleBarHeight;
	ask::FrameBuffer *fb;
	bool ready;
};

/**
 * Inits all window-stuff
 *
 * @param sid the device-id
 * @param ui the ui
 * @param width the desired screen width
 * @param height the desired screen height
 * @param bpp the desired bits per pixel
 * @param shmname the shared memory name
 * @return the mode id on success
 */
int win_init(int sid,ask::UI *ui,gsize_t width,gsize_t height,gcoldepth_t bpp,const char *shmname);

/**
 * @return the current mode
 */
const ask::Screen::Mode *win_getMode(void);

/**
 * Changes the mode to the given one.
 *
 * @param width the desired screen width
 * @param height the desired screen height
 * @param bpp the desired bits per pixel
 * @param shmname the shared memory name
 * @return 0 on success
 */
int win_setMode(gsize_t width,gsize_t height,gcoldepth_t bpp,const char *shmname);

/**
 * Sets the cursor at given position (writes to vesa)
 *
 * @param x the x-coordinate
 * @param y the y-coordinate
 * @param cursor the cursor to use
 */
void win_setCursor(gpos_t x,gpos_t y,uint cursor);

/**
 * Creates a new window from given create-message
 *
 * @param x the x-coordinate
 * @param y the y-coordinate
 * @param width the width
 * @param height the height
 * @param owner the window-owner (fd)
 * @param style style-attributes
 * @param titleBarHeight the height of the titlebar of that window
 * @param title the title of the window
 * @param winmng the window-manager name
 * @return the window-id or WINID_UNUSED if no slot is free
 */
gwinid_t win_create(gpos_t x,gpos_t y,gsize_t width,gsize_t height,int owner,uint style,
	gsize_t titleBarHeight,const char *title,const char *winmng);

/**
 * Attaches the given fd as event-channel to the window with id winid
 *
 * @param winid the window-id
 * @param fd the fd for the event-channel
 */
void win_attach(gwinid_t winid,int fd);

/**
 * Detaches the given fd from the windows that uses it.
 *
 * @param fd the fd for the event-channel
 */
void win_detachAll(int fd);

/**
 * Destroys all windows of the given thread
 *
 * @param cid the client-fd
 * @param mouseX the current x-coordinate of the mouse
 * @param mouseY the current y-coordinate of the mouse
 */
void win_destroyWinsOf(int cid,gpos_t mouseX,gpos_t mouseY);

/**
 * Destroys the given window
 *
 * @param id the window-id
 * @param mouseX the current x-coordinate of the mouse
 * @param mouseY the current y-coordinate of the mouse
 */
void win_destroy(gwinid_t id,gpos_t mouseX,gpos_t mouseY);

/**
 * @param id the window-id
 * @return the window with given id or NULL
 */
Window *win_get(gwinid_t id);

/**
 * @param id the window-id
 * @return whether the window with given id exists
 */
bool win_exists(gwinid_t id);

/**
 * Determines the window at given position
 *
 * @param x the x-coordinate
 * @param y the y-coordinate
 * @return the window or NULL
 */
Window *win_getAt(gpos_t x,gpos_t y);

/**
 * @return the currently active window; NULL if no one is active
 */
Window *win_getActive(void);

/**
 * Sets the active window to the given one. This will put <id> to the front and windows that
 * are above one step behind (z-coordinate)
 *
 * @param id the window-id
 * @param repaint whether the window should receive a repaint-event
 * @param mouseX the current x-coordinate of the mouse
 * @param mouseY the current y-coordinate of the mouse
 */
void win_setActive(gwinid_t id,bool repaint,gpos_t mouseX,gpos_t mouseY);

/**
 * Shows a preview for the given resize-operation
 *
 * @param x the x-coordinate
 * @param y the y-coordinate
 * @param width the new width
 * @param height the new height
 */
void win_previewResize(gpos_t x,gpos_t y,gsize_t width,gsize_t height);

/**
 * Shows a preview for the given move-operation
 *
 * @param window the window-id
 * @param x the x-coordinate
 * @param y the y-coordinate
 */
void win_previewMove(gwinid_t window,gpos_t x,gpos_t y);

/**
 * Resizes the window to the given size
 *
 * @param window the window-id
 * @param x the x-coordinate
 * @param y the y-coordinate
 * @param width the new width
 * @param height the new height
 * @param winmng the window-manager name
 */
void win_resize(gwinid_t window,gpos_t x,gpos_t y,gsize_t width,gsize_t height,const char *winmng);

/**
 * Moves the given window to given position and optionally changes the size
 *
 * @param window the window-id
 * @param x the x-coordinate
 * @param y the y-coordinate
 * @param width the new width
 * @param height the new height
 */
void win_moveTo(gwinid_t window,gpos_t x,gpos_t y,gsize_t width,gsize_t height);

/**
 * Sends update-events to the given window to update the given area
 *
 * @param window the window-id
 * @param x the x-coordinate in the window
 * @param y the y-coordinate in the window
 * @param width the width
 * @param height the height
 */
void win_update(gwinid_t window,gpos_t x,gpos_t y,gsize_t width,gsize_t height);

/**
 * Notifies the UI-manager that the given rectangle has changed.
 *
 * @param x the x-coordinate
 * @param y the y-coordinate
 * @param width the width
 * @param height the height
 */
void win_notifyUimng(gpos_t x,gpos_t y,gsize_t width,gsize_t height);
