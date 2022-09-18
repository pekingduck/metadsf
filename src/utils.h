/***************************************************************************
    copyright            : (C) 2014 by Peking Duck Labs
    email                : pekingducklabs@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 ***************************************************************************/

#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>
#include <map>
#include <taglib/tbytevector.h>
#include "typedefs.h"

bool readPairsFromFile(const char *, StringMap &);

void split(const std::string &s,
           const std::string &delim,
           StringVector &v,
           const bool keep_empty,
           unsigned int upTo = 0);

size_t loadFileIntoVector(const char *path, TagLib::ByteVector &v);

bool isReadableFile(const char *path);

bool stringToLong(const std::string &, long &l);

bool writeFileFromVector(const char *path, const TagLib::ByteVector &v);

#endif
