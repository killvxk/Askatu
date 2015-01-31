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

#include <impl/streams/ios_base.h>
#include <fstream>
#include <iostream>

namespace std {
	// a trick to "publish" cin, cout, ... as istream/ostream/... instead of their real type
	// ifstream/ofstream/... the reason is that otherwise we would have to cast them to
	// ostream/istream for some >>-operators. therefore we put here the real objects as _* with
	// their real type and publish a reference to them.

	ifstream _cin;
	ofstream _cout;
	ofstream _cerr;
	ofstream _clog;

	istream &cin = _cin;
	ostream &cout = _cout;
	ostream &cerr = _cerr;
	ostream &clog = _clog;

	int ios_base::Init::init_cnt = 0;
	ios_base::Init init;

	ios_base::Init::Init() {
		if(init_cnt++ == 0) {
			_cin.open((int)STDIN_FILENO);
			_cin.tie(&cout);
			_cout.open((int)STDOUT_FILENO);
			_cerr.open((int)STDERR_FILENO);
			_cerr.tie(&cout);
			_clog.open((int)STDERR_FILENO);
		}
	}
	ios_base::Init::~Init() {
		if(--init_cnt == 0) {
			cout.flush();
			cerr.flush();
			clog.flush();
		}
	}
}
