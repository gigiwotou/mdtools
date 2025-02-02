/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) Flamewing 2011-2015 <flamewing.sonic@gmail.com>
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

#ifndef __LIB_SINGLEMAPPING_H
#define __LIB_SINGLEMAPPING_H

#include <iosfwd>
#include <map>
#include <mdtools/singledplc.hh>

class single_mapping {
private:
    uint16_t flags, tile;
    int16_t  xx, yy;
    uint8_t  sx, sy;
    enum MapSizes : size_t { sizeS1 = 5, sizeS2 = 8, sizeS3 = 6 };

public:
    static size_t size(int const ver) {
        switch (ver) {
        case 1:
            return sizeS1;
        case 2:
            return sizeS2;
        default:
            return sizeS3;
        }
    }
    void read(std::istream& in, int ver);
    void write(std::ostream& out, int ver) const;
    void print() const;
    void split(
        single_mapping const& src, single_dplc& dplc,
        std::map<size_t, size_t>& vram_map);
    void merge(single_mapping const& src, std::map<size_t, size_t>& vram_map);
    void change_pal(int srcpal, int dstpal);

    uint16_t get_flags() const { return flags; }
    uint16_t get_tile() const { return tile; }
    int16_t  get_xx() const { return xx; }
    int16_t  get_yy() const { return yy; }
    uint8_t  get_sx() const { return sx; }
    uint8_t  get_sy() const { return sy; }

    void set_flags(uint16_t const t) { flags = t; }
    void set_tile(uint16_t const t) { tile = t; }
    void set_xx(int16_t t) { xx = t; }
    void set_yy(int16_t t) { yy = t; }
    void set_sx(int8_t const t) { sx = t; }
    void set_sy(int8_t const t) { sy = t; }
    bool operator<(single_mapping const& rhs) const {
        if (tile < rhs.tile) {
            return true;
        }
        if (tile > rhs.tile) {
            return false;
        }
        if (sx < rhs.sx) {
            return true;
        }
        if (sx > rhs.sx) {
            return false;
        }
        if (sy < rhs.sy) {
            return true;
        }
        if (sy > rhs.sy) {
            return false;
        }
        if (flags < rhs.flags) {
            return true;
        }
        if (flags > rhs.flags) {
            return false;
        }
        if (xx < rhs.xx) {
            return true;
        }
        if (xx > rhs.xx) {
            return false;
        }
        if (yy < rhs.yy) {
            return true;
        }
        /*if (yy > rhs.yy)*/
        return false;
    }
    bool operator==(single_mapping const& rhs) const {
        return !(*this < rhs || rhs < *this);
    }
};

#endif // __LIB_SINGLEMAPPING_H
