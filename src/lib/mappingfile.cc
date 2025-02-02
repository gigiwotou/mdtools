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

#include <mdtools/mappingfile.hh>

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>

#include <boost/io/ios_state.hpp>

#include <mdcomp/bigendian_io.hh>

using std::cerr;
using std::cout;
using std::endl;
using std::hex;
using std::ios;
using std::istream;
using std::map;
using std::ostream;
using std::setfill;
using std::setw;
using std::uppercase;
using std::vector;

void mapping_file::read(istream& in, int const ver) {
    in.seekg(0, ios::beg);

    vector<size_t> off;
    auto           term = static_cast<int16_t>(BigEndian::Read2(in));
    while (term == 0) {
        off.push_back(term);
        term = static_cast<int16_t>(BigEndian::Read2(in));
    }
    off.push_back(term);
    while (in.tellg() < term) {
        auto const newterm = static_cast<int16_t>(BigEndian::Read2(in));
        if (newterm > 0 && newterm < term) {
            term = newterm;
        }
        off.push_back(newterm);
    }

    for (auto const pos : off) {
        in.clear();
        in.seekg(pos);
        frame_mapping sd;
        sd.read(in, ver);
        frames.push_back(sd);
    }
}

void mapping_file::write(
    ostream& out, int const ver, bool const nullfirst) const {
    map<frame_mapping, size_t>            mappos;
    map<size_t, frame_mapping>            posmap;
    size_t                                sz = 2 * frames.size();
    vector<frame_mapping>::const_iterator it;
    it = frames.begin();

    if (nullfirst && it != frames.end() && it->empty()) {
        mappos.emplace(*it, 0);
        posmap.emplace(0, *it);
    }
    for (; it != frames.end(); ++it) {
        auto const it2 = mappos.find(*it);
        if (it2 != mappos.end()) {
            BigEndian::Write2(out, it2->second);
        } else {
            mappos.emplace(*it, sz);
            posmap.emplace(sz, *it);
            BigEndian::Write2(out, sz);
            sz += it->size(ver);
        }
    }
    for (auto const& elem : posmap) {
        if (elem.first == size_t(out.tellp())) {
            (elem.second).write(out, ver);
        } else if (elem.first != 0U) {
            cerr << "Missed write at " << out.tellp() << endl;
            (elem.second).print();
        }
    }
}

void mapping_file::print() const {
    cout << "=================================================================="
            "=============="
         << endl;
    for (size_t i = 0; i < frames.size(); i++) {
        cout << "Mappings for frame $";
        boost::io::ios_all_saver flags(cout);
        cout << uppercase << hex << setfill('0') << setw(4) << i;
        cout << ":" << endl;
        flags.restore();
        frames[i].print();
    }
}

void mapping_file::split(mapping_file const& src, dplc_file& dplc) {
    for (auto const& elem : src.frames) {
        frame_mapping nn;
        frame_dplc    interm;
        frame_dplc    dd;
        nn.split(elem, interm);
        dd.consolidate(interm);
        frames.push_back(nn);
        dplc.insert(dd);
    }
}

void mapping_file::merge(mapping_file const& src, dplc_file const& dplc) {
    for (size_t i = 0; i < src.frames.size(); i++) {
        frame_mapping nn;
        nn.merge(src.frames[i], dplc.get_dplc(i));
        frames.push_back(nn);
    }
}

void mapping_file::optimize(
    mapping_file const& src, dplc_file const& indplc, dplc_file& outdplc) {
    for (size_t i = 0; i < src.frames.size(); i++) {
        frame_mapping        endmap;
        frame_dplc           enddplc;
        frame_mapping const& intmap  = src.frames[i];
        frame_dplc const&    intdplc = indplc.get_dplc(i);
        if ((!intdplc.empty()) && (!intmap.empty())) {
            frame_mapping mm;
            frame_dplc    dd;
            mm.merge(intmap, intdplc);
            endmap.split(mm, dd);
            enddplc.consolidate(dd);
        } else if (!intdplc.empty()) {
            enddplc.consolidate(intdplc);
        } else {
            endmap = intmap;
        }

        frames.push_back(endmap);
        outdplc.insert(enddplc);
    }
}

void mapping_file::change_pal(int const srcpal, int const dstpal) {
    for (auto& elem : frames) {
        elem.change_pal(srcpal, dstpal);
    }
}

size_t mapping_file::size(int const ver) const {
    size_t sz = 2 * frames.size();
    for (auto const& elem : frames) {
        sz += elem.size(ver);
    }
    return sz;
}
