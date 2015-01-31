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

#include <ask/proto/winmng.h>
#include <gui/event/subscriber.h>
#include <gui/graphics/pos.h>
#include <gui/graphics/size.h>
#include <gui/theme.h>
#include <sys/common.h>
#include <sys/messages.h>
#include <sys/sync.h>
#include <sys/thread.h>
#include <exception>
#include <functor.h>
#include <list>
#include <memory>
#include <mutex>
#include <signal.h>
#include <vector>

namespace gui {
	class Window;
	class Color;

	/**
	 * The exception that will be thrown if something goes wrong in Application. For example,
	 * if receiving a message from the window-manager failed.
	 */
	class app_error : public std::exception {
	public:
		app_error(const std::string& str) throw ()
			: exception(), _str(str) {
		}
		~app_error() throw () {
		}
		virtual const char *what() const throw () {
			return _str.c_str();
		}
	private:
		std::string _str;
	};

	/**
	 * The class Application is a singleton, because there is only one instance per application. It
	 * manages the communication with vesa and the window-manager, knows all windows and runs the
	 * event-loop.
	 */
	class Application {
		friend class Window;
		friend class GraphicsBuffer;
		friend class Color;

		struct TimeoutFunctor {
			TimeoutFunctor() : tsc(), functor() {
			}
			TimeoutFunctor(uint64_t _tsc,std::Functor<void> *_functor) : tsc(_tsc), functor(_functor) {
			}

			uint64_t tsc;
			std::Functor<void> *functor;
		};

	public:
		typedef Sender<gwinid_t,const std::string&> createdev_type;
		typedef Sender<gwinid_t> activatedev_type;
		typedef Sender<gwinid_t> destroyedev_type;

		/**
		 * Creates an instance of this class
		 *
		 * @param winmng the path to win-manager device (the env-var TERM is used if it is NULL)
		 * @return the created instance
		 */
		static Application *create(const char *winmng = NULL) {
			return _inst = new Application(winmng);
		}
		/**
		 * Destroys the instance of this class
		 */
		static void destroy() {
			delete _inst;
			_inst = NULL;
		}

		/**
		 * @return the instance of the class
		 */
		static Application *getInstance() {
			return _inst;
		}

	private:
		static Application *_inst;

	public:
		/**
		 * @return the size of the screen
		 */
		Size getScreenSize() const {
			return Size(_screenMode.width,_screenMode.height);
		}
		/**
		 * @return the color-depth
		 */
		uchar getColorDepth() const {
			return _screenMode.bitsPerPixel;
		}

		/**
		 * @return the current screen mode
		 */
		const ask::Screen::Mode *getMode() const {
			return &_screenMode;
		}

		/**
		 * Collects all supported screen modes.
		 *
		 * @return the modes
		 */
		std::vector<ask::Screen::Mode> getModes() {
			return _winMng.getModes();
		}

		/**
		 * Sets the given mode
		 *
		 * @param mode the mode
		 */
		void setMode(const ask::Screen::Mode &mode) {
			_winMng.setMode(mode.width,mode.height,mode.bitsPerPixel);
		}

		/**
		 * @return the path to the window-manager
		 */
		const char *getWinMng() const {
			return _winMngName;
		}

		/**
		 * @param id the window-id
		 * @return the window with given id or nullptr
		 */
		Window *getWindowById(gwinid_t id);

		/**
		 * @return the default-theme
		 */
		const Theme *getDefaultTheme() {
			return &_defTheme;
		}

		/**
		 * Requests that this window should be the active one
		 */
		void requestActiveWindow(gwinid_t wid) {
			_winMng.setActive(wid);
		}

		/**
		 * The event senders
		 */
		createdev_type &created() {
			return _created;
		}
		activatedev_type &activated() {
			return _activated;
		}
		destroyedev_type &destroyed() {
			return _destroyed;
		}

		/**
		 * Adds the given window to the window-list. Will announce the window at the window-manager
		 *
		 * @param win the window
		 */
		void addWindow(std::shared_ptr<Window> win);
		/**
		 * Removes the given window from the window-list. Will remove it from the window-manager
		 *
		 * @param win the window
		 */
		void removeWindow(std::shared_ptr<Window> win);

		/**
		 * Puts the given functor into the event-queue and calls it later, i.e. when the run-loop
		 * looks into the queue again. This is intended to execute GUI-code from a different
		 * thread (of course it is executed in the GUI thread later) or to execute a piece of code
		 * after an event passed through the whole chain. The latter is required if you want to
		 * remove controls as a reaction on an event.
		 *
		 * @param functor the functor to call
		 */
		void executeLater(std::Functor<void> *functor) {
			executeIn(0,functor);
		}

		/**
		 * Executes the given functor after <msecs> milliseconds. That is, it uses alarm() to get
		 * a signal later which will then execute the function.
		 *
		 * @param msecs the number of milliseconds to wait
		 * @param functor the functor to call
		 */
		void executeIn(uint msecs,std::Functor<void> *functor);

		/**
		 * Starts the message-loop
		 */
		int run();

		/**
		 * Stops the message-loop
		 */
		void exit();

	private:
		/**
		 * Constructor. Protected because its a singleton
		 */
		Application(const char *winmng);
		/**
		 * Destructor. Closes the connection to vesa and the window-manager
		 */
		virtual ~Application();
		/**
		 * Handles the given event
		 *
		 * @param ev the event
		 */
		virtual void handleEvent(const ask::WinMngEvents::Event &ev);
		/**
		 * Calls all functors in the queue
		 */
		void handleQueue();

	private:
		// prevent copying
		Application(const Application &a);
		Application &operator=(const Application &a);

		void notifyCreate(gwinid_t id,const std::string& title);
		void notifyActive(gwinid_t id);
		void notifyDestroy(gwinid_t id);

		/**
		 * @return the information received from vesa
		 */
		const ask::Screen::Mode *getScreenMode() const {
			return &_screenMode;
		}
		/**
		 * Requests an update for the window with given id and the given rectangle.
		 * Ensures that the update does only affect the given window
		 *
		 * @param id the window-id
		 * @param pos the position (global)
		 * @param size the size
		 */
		void requestWinUpdate(gwinid_t id,const Pos &pos,const Size &size);
		/**
		 * Performs a move-operation with the given window. It will use getMoveX() and getMoveY()
		 * as destination. If finish is true, the window will really be moved, otherwise the preview
		 * rectangle will be displayed/moved.
		 *
		 * @param win the window
		 * @param finish whether to finish this operation
		 */
		void moveWindow(Window *win,bool finish);
		/**
		 * Performs a resize-operation with the given window. It will use getMoveX(), getMoveY(),
		 * getResizeWidth() and getResizeHeight() as destination. This way, a move and resize may
		 * be performed at once (e.g. when resizing on the left side). If finish is true, the
		 * window will really be resized, otherwise the preview rectangle will be displayed/resized.
		 *
		 * @param win the window
		 * @param finish whether to finish this operation
		 */
		void resizeWindow(Window *win,bool finish);

		// only used in Application itself
		void passToWindow(gwinid_t win,const Pos &pos,short movedX,short movedY,
				short movedZ,uchar buttons);
		void closePopups(gwinid_t id,const Pos &pos);

	private:
		static const char *getWinMng(const char *winmng);

		const char *_winMngName;
		ask::WinMng _winMng;
		ask::WinMngEvents _winEv;
		bool _run;
		uchar _mouseBtns;
		ask::Screen::Mode _screenMode;
		std::vector<std::shared_ptr<Window>> _windows;
		createdev_type _created;
		activatedev_type _activated;
		destroyedev_type _destroyed;
		std::list<TimeoutFunctor> _timequeue;
		std::mutex _queueMutex;
		bool _listening;
		Theme _defTheme;
	};
}
