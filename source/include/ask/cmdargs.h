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
#include <stdarg.h>
#include <stddef.h>
#include <vector>

namespace ask {
	/**
	 * A usage-example:
	 *
	 * string s;
	 * int d = 0;
	 * uint k = 0,flag = 0;
	 * cmdargs a(argc,argv,0);
	 * try {
	 *   a.parse("=s a1=d a2=k flag",&s,&a1,&a2,&flag);
	 *   if(a.is_help())
	 *     usage(argv[0]);
	 * }
	 * catch(const cmdargs_error& e) {
	 *   cerr << "Invalid arguments: " << e.what() << endl;
	 *   usage(argv[0]);
	 * }
	 */

	/**
	 * The exception that is thrown if an error occurred (= invalid arguments)
	 */
	class cmdargs_error : public std::exception {
	public:
		explicit cmdargs_error(const std::string& arg);
		virtual ~cmdargs_error() throw ();
		virtual const char* what() const throw ();
	private:
		std::string _msg;
	};

	/**
	 * The argument-parser
	 */
	class cmdargs {
	public:
		typedef unsigned char flag_type;

	public:
		/**
		 * Disallow '-' or '--' for arguments
		 */
		static const flag_type NO_DASHES	= 1;
		/**
		 * Require '=' for arguments
		 */
		static const flag_type REQ_EQ		= 2;
		/**
		 * Disallow '=' for arguments
		 */
		static const flag_type NO_EQ		= 4;
		/**
		 * Throw exception if free arguments are found
		 */
		static const flag_type NO_FREE		= 8;
		/**
		 * Max. 1 free argument
		 */
		static const flag_type MAX1_FREE	= 16;

	public:
		/**
		 * Creates a cmdargs-object for given arguments.
		 *
		 * @param argc the number of args
		 * @param argv the arguments
		 * @param flags the flags
		 */
		cmdargs(int argc,char * const *argv,flag_type flags = 0)
			: _argc(argc), _argv(argv), _flags(flags), _ishelp(false),
			  _args(std::vector<std::string*>()), _free(std::vector<std::string*>()) {
		}
		/**
		 * Destructor
		 */
		~cmdargs();

		/**
		 * Parses the command-line-arguments according to given format. It sets the given variable
		 * parameters depending on the format. The format works like the following:
		 * The first chars are the name of the argument, after that follows optionally a '=' which
		 * tells the parser that the argument has a value. Behind it the type of the value follows.
		 * The type may be:
		 * 	's':			a string, expects a std::string* as argument
		 * 	'd' or 'i':		a signed integer, expects a int* as argument
		 * 	'u':			a unsigned integer to base 10, expects a uint* as argument
		 * 	'x' or 'X':		a unsigned integer to base 16, expects a uint* as argument
		 * 	'c':			a character, expects a char* as argument
		 * 	'k':			a unsigned integer with optional suffix K,M or G. expects a uint* as arg
		 * After the type may follow a '*' to tell the parser that the argument is required. If the
		 * argument has no value a bool* is expected as argument. The arguments have to be separated
		 * by spaces.
		 *
		 * An example is:
		 * int flag;
		 * string s;
		 * int d;
		 * args->parse("flag a=s arg2=d*",&flag,&s,&d);
		 *
		 * There are some special things to know:
		 * 	- If an required argument is missing, a argument-value is missing, free arguments are
		 *    given but not allowed, or similar, a cmdargs_error is thrown.
		 * 	- The name of an argument may be empty. This means implicitly that its an required
		 *    argument which has just a value. These arguments are expected in the given order. So
		 *    if you specify an empty argument as the first one, it has to be the first in the
		 *    program-arguments (after the program-name of course).
		 * Additionally you can manipulate the behaviour by flags when creating the instance.
		 *
		 * @param fmt the format
		 * @throws cmdargs_error if parsing goes wrong
		 */
		void parse(const char *fmt,...);

		/**
		 * @return whether its a help-request (--help, -h, -?)
		 */
		bool is_help() const {
			return _ishelp;
		}

		/**
		 * @return a vector with all arguments (including the progname)
		 */
		const std::vector<std::string*> &get_args() const {
			return _args;
		}

		/**
		 * @return a vector with all free arguments
		 */
		const std::vector<std::string*> &get_free() const {
			return _free;
		}

	private:
		// no cloning
		cmdargs(const cmdargs& c);
		cmdargs& operator =(const cmdargs& c);

		/**
		 * Searches for the argument with given name in the args. Returns the argument-value
		 * and sets <pos> appropriatly to the argument-position in _args.
		 *
		 * @param name the argument-name
		 * @param hasVal whether the arg has a value
		 * @param pos will be set to the position in _args
		 * @return the value
		 */
		std::string find(const std::string& name,bool hasVal,std::vector<std::string*>::iterator& pos);
		/**
		 * Sets the value to the location pointed to by <ptr> in the format specified by <type>.
		 *
		 * @param arg the argument-value
		 * @param pos the position in _args
		 * @param hasVal whether the arg has a value
		 * @param type the arg-type
		 * @param ptr the location where to write to
		 */
		void setval(const std::string& arg,std::vector<std::string*>::iterator pos,bool hasVal,char type,void *ptr);
		/**
		 * Converts the given argument to an integer and treats 'K','M' and 'G' as binary-prefixes.
		 * I.e. K = 1024, M = 1024 * 1024, G = 1024 * 1024 * 1024
		 *
		 * @param arg the arg-value
		 * @return the integer
		 */
		unsigned int readk(const std::string& arg);

	private:
		int _argc;
		char * const *_argv;
		flag_type _flags;
		bool _ishelp;
		std::vector<std::string*> _args;
		std::vector<std::string*> _free;
	};
}
