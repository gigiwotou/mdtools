/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) Flamewing 2014 <flamewing.sonic@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SSTRACK_H
#define __SSTRACK_H

#include "pattern_name_table.h"

#include <vector>

typedef std::pair<Pattern_Name,unsigned> RLEPattern;

class SSTrackFrame : public PlaneH128V28 {
private:
	static std::vector<Pattern_Name> SymLUT_6bit;
	static std::vector<Pattern_Name> SymLUT_10bit;
	static std::vector<RLEPattern>   DicLUT_6bit;
	static std::vector<RLEPattern>   DicLUT_7bit;
public:
	SSTrackFrame() noexcept = default;
	SSTrackFrame(std::istream &in, bool xflip) noexcept;
};

#endif // __SSTRACK_H
