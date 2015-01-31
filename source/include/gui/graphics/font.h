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

#include <gui/graphics/size.h>
#include <sys/common.h>
#include <algorithm>
#include <string>

namespace gui {
	class Font {
		static const gsize_t charWidth = 8;
		static const gsize_t charHeight = 16;

	public:
		Size getSize() const {
			return Size(charWidth,charHeight);
		}
		gsize_t getStringWidth(const std::string& str) const {
			return str.length() * charWidth;
		}
		gsize_t limitStringTo(A_UNUSED const std::string& str,gsize_t width,
		                             A_UNUSED gsize_t start = 0) const {
			return std::min<gsize_t>(str.length(),width / charWidth);
		}
		bool isPixelSet(char c,gpos_t x,gpos_t y) const {
			return _font[(uchar)c * charHeight + y] & (1 << (charWidth - x - 1));
		}

	private:
		static uint8_t _font[];
	};
}
