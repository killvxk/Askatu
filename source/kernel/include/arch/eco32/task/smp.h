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

#include <common.h>

class SMP : public SMPBase {
};

inline bool SMPBase::initArch() {
	/* mmix does not support SMP */
	return false;
}

inline cpuid_t SMPBase::getCurId() {
	return 0;
}

inline void SMPBase::pauseOthers() {
	/* nothing to do */
}

inline void SMPBase::resumeOthers() {
	/* nothing to do */
}

inline void SMPBase::haltOthers() {
	/* nothing to do */
}

inline void SMPBase::ensureTLBFlushed() {
	/* nothing to do */
}

inline void SMPBase::sendIPI(A_UNUSED cpuid_t id,A_UNUSED uint8_t vector) {
	/* ignored */
}
