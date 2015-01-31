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

#include <exception>
#include <new>
#include <stddef.h>
#include <string>
#include <utility>
#include <vector>

namespace std {
	typedef off_t streamoff;
	typedef size_t streamsize;

	class ios_base {
	public:
		class failure: public std::exception {
		public:
			explicit failure(const string& msg)
				: _msg(msg.c_str()) {
			}
			virtual const char* what() const throw() {
				return _msg;
			}
		private:
			const char *_msg;
		};

		class Init {
		public:
			/**
			 * Constructs an object of class Init. If init_cnt is zero, the function stores the
			 * value one in init_cnt , then constructs and initializes the objects cin, cout,
			 * cerr, clog (27.3.1), wcin, wcout, wcerr, and wclog (27.3.2). In any case, the
			 * function then adds one to the value stored in init_cnt.
			 */
			Init();
			/**
			 * Destroys an object of class Init. The function subtracts one from the value stored
			 * in init_cnt and, if the resulting stored value is one, calls cout.flush(),
			 * cerr.flush(), clog.flush(), wcout.flush(), wcerr.flush(), wclog.flush().
			 */
			~Init();
		private:
			// counts the number of constructor and destructor calls for class Init,
			// initialized to zero.
			static int init_cnt;
		};

		typedef unsigned long fmtflags;
		static const fmtflags boolalpha;	// insert and extract bool type in alphabetic format
		static const fmtflags dec;			// integer input/output in decimal base
		static const fmtflags fixed;		// floating-point output in fixed-point notation
		static const fmtflags hex;			// integer input/output in hexadecimal base
		static const fmtflags internal;		// adds fill characters at a designated internal point
											// in certain generated output, or identical to right
											// if no such point is designated
		static const fmtflags left;			// adds fill characters on the right (final positions)
		static const fmtflags oct;			// integer input/output in octal base
		static const fmtflags right;		// adds fill characters on the left (initial positions)
		static const fmtflags scientific;	// generates floating-point output in scientific notation
		static const fmtflags showbase;		// adds prefix with the base of generated integer output
		static const fmtflags showpoint;	// adds a decimal-point unconditionally for floats
		static const fmtflags showpos;		// adds a + sign in non-negative generated numeric output
		static const fmtflags skipws;		// skips leading whitespace before certain input operations
		static const fmtflags unitbuf;		// flushes output after each output operation
		static const fmtflags uppercase;	// lowercase -> uppercase for certain letters

		static const fmtflags adjustfield;	// left | right | internal
		static const fmtflags basefield;	// dec | oct | hex
		static const fmtflags floatfield;	// scientific | fixed

		typedef unsigned char iostate;
		static const iostate badbit;		// indicates a loss of integrity in an input or output
											// sequence (such as an irrecoverable read error from
											// a file);
		static const iostate eofbit;		// indicates that an input operation reached EOF
		static const iostate failbit;		// indicates that an input operation failed to read
											// the expected characters, or that an output operation
											// failed to generate the desired characters.
		static const iostate goodbit;		// everything is fine :) (= 0)

		typedef unsigned char openmode;
		static const openmode app;			// seek to end before each write
		static const openmode ate;			// open and seek to end immediately after opening
		static const openmode binary;		// perform input/output in binary mode (not text-mode)
		static const openmode in;			// open for input
		static const openmode out;			// open for output
		static const openmode trunc;		// truncate an existing stream when opening
		static const openmode noblock;		// no block when reading or receiving a msg from devices
		static const openmode signals;		// stop if a syscall was interrupted by a signal

		typedef unsigned char seekdir;
		static const seekdir beg;			// seek relative to the beginning of the stream
		static const seekdir cur;			// seek relative to the current position
		static const seekdir end;			// seek relative to the current end of the sequence

		/**
		 * @return The format control information for both input and output.
		 */
		fmtflags flags() const {
			return _flags;
		}
		/**
		 * Sets the format flags to <fmtfl>.
		 *
		 * @param fmtfl the flags to set
		 * @return the previous flags
		 */
		fmtflags flags(fmtflags fmtfl) {
			fmtflags old = _flags;
			_flags = fmtfl;
			return old;
		}
		/**
		 * Sets <fmtfl> in flags()
		 *
		 * @param fmtfl the flag to set
		 * @return the previous flags
		 */
		fmtflags setf(fmtflags fmtfl) {
			fmtflags old = _flags;
			_flags |= fmtfl;
			return old;
		}
		/**
		 * Clears <mask> in flags(), sets <fmtfl> & <mask> in flags().
		 *
		 * @param fmtfl the flags to set
		 * @param mask the mask
		 * @return the previous flags
		 */
		fmtflags setf(fmtflags fmtfl,fmtflags mask) {
			fmtflags old = _flags;
			_flags &= ~mask;
			_flags |= fmtfl & mask;
			return old;
		}
		/**
		 * Clears mask in flags().
		 *
		 * @param mask the mask to clear
		 */
		void unsetf(fmtflags mask) {
			_flags &= ~mask;
		}

		/**
		 * @return the precision to generate on certain output conversions.
		 */
		streamsize precision() const {
			return _prec;
		}
		/**
		 * Sets the precision to <prec>.
		 *
		 * @param prec the precision to set
		 * @return the previous value of precision().
		 */
		streamsize precision(streamsize prec) {
			streamsize old = _prec;
			_prec = prec;
			return old;
		}
		/**
		 * @return the minimum field width (number of characters) to generate on certain output
		 * 	conversions.
		 */
		streamsize width() const {
			return _width;
		}
		/**
		 * Sets the width to <wide>.
		 *
		 * @param wide the width to set
		 * @return the previous value of width().
		 */
		streamsize width(streamsize wide) {
			streamsize old = _width;
			_width = wide;
			return old;
		}

		// 27.4.2.6 callbacks;
		enum event {
			erase_event, copyfmt_event
		};
		typedef void (* event_callback)(event,ios_base &,int index);
		/**
		 * Registers the pair (fn ,index ) such that during calls to imbue() (27.4.2.3),
		 * copyfmt(), or ~ios_base() (27.4.2.7), the function fn is called with argument
		 * index . Functions registered are called when an event occurs, in opposite order of
		 * registration. Functions registered while a callback function is active are not called
		 * until the next event.
		 * The function fn shall not throw exceptions.
		 */
		void register_callback(event_callback fn,int index) {
			_callbacks.push_back(make_pair(fn,index));
		}

		/**
		 * @return whether we should sync stdio
		 */
		static bool sync_with_stdio();
		/**
		 * If any input or output operation has occurred using the standard streams prior to the
		 * call, the effect is implementation-defined. Otherwise, called with a false argument,
		 * it allows the standard streams to operate independently of the standard C streams.
		 *
		 * @return true if the previous state of the standard iostream objects (27.3) was
		 * 	synchronized and otherwise returns false. The first time it is called, the function
		 * 	returns true.
		 */
		static bool sync_with_stdio(bool sync);

		/**
		 * Destroys an object of class ios_base. Calls each registered callback pair (fn , index )
		 * (27.4.2.6) as (*fn )(erase_event, *this, index ) at such time that any ios_base member
		 * function called from within fn has well defined results.
		 */
		virtual ~ios_base() {
			raise_event(erase_event);
		}

	protected:
		/**
		 * Calls all registers callbacks with <event>
		 */
		void raise_event(event ev);
		/**
		 * @return the base to use for input/output (depending on basefield)
		 */
		int get_base();
		/**
		 * Each ios_base member has an indeterminate value after construction. These members
		 * shall be initialized by calling ios::init. If an ios_base object is destroyed
		 * before these initializations have taken place, the behavior is undefined.
		 */
		ios_base();

	private:
		fmtflags _flags;
		streamsize _prec;
		streamsize _width;
		vector<pair<event_callback,int> > _callbacks;

	private:
		ios_base(const ios_base &);
		ios_base & operator =(const ios_base &);
	};
}
