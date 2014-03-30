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

#include <list>
#include <string>
#include <taglib/tstring.h>

#include "typedefs.h"

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

//namespace myopt {

class OptionObj {
public:
  TagLib::String encoding;
  TagLib::String version;
  TagLib::String setTagsFile;
  TagLib::String addTagsFile;
  TagLib::String separator;
  StringMap addTagMap;
  StringMap handyMap;
  StringVector fileList;
  StringVector addPicList;
  StringVector removeTagList;
  StringVector removePicList;
  bool showTags;
  bool showInfo;
  bool removeEverything;
  bool dryRun;
  bool removeAllPics;

 OptionObj() : showTags(false), showInfo(false), removeEverything(false),
    dryRun(false), removeAllPics(false) {}
  void printUsage();
  void print();
  bool parse(int argc, char *argv[]);  
};

//} // namespace

#endif
