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

#include <tuple>
#include "metadsf.h"
#include "utils.h"
#include "options.h"

typedef std::tuple<const TagLib::String, 
		   TagLib::ID3v2::AttachedPictureFrame::Type, 
		   const TagLib::String> PicTuple;
typedef std::list<PicTuple> PicTupleList; 


bool doDelete(MetaDSF &, OptionObj &);
bool doAdd(MetaDSF &, OptionObj &);
bool validatePictures(OptionObj &, PicTupleList &);
bool importPictures(MetaDSF &, PicTupleList &);

int main(int argc, char *argv[])
{
  // Command line parsing using a wrapper around 
  // "The Lean Mean C++ Option Parser"
  // http://optionparser.sourceforge.net/
  OptionObj opt;
  if (!opt.parse(argc - 1, argv + 1)) {
    return 1;
  }

  // Validate encoding if supplied
  if (!opt.encoding.isEmpty() && !MetaDSF::isValidEncoding(opt.encoding)) {
    std::cerr << "Invalid encoding: " << opt.encoding << std::endl;
    return 1;
  }

  // Validate version if supplied
  if (!opt.version.isEmpty() && opt.version.toInt() != 3 && 
      opt.version.toInt() != 4) 
  {
    std::cerr << "Invalid ID3v2 version: " << opt.version << std::endl;
    return 1;
  }

  // Load tag data from files
  StringMap tmp;
  if (!opt.setTagsFile.isEmpty()) {
    if (!readPairsFromFile(opt.setTagsFile.toCString(), tmp))
      return 1;
    if (!opt.removeEverything)
      for (auto &p: tmp)
	opt.removeTagList.push_back(p.first);
    opt.addTagMap.insert(tmp.begin(), tmp.end()); 
  }
  
  if (!opt.addTagsFile.isEmpty())
    if (!readPairsFromFile(opt.addTagsFile.toCString(), opt.addTagMap))
      return 1;

  // Validate keys - only for addMap
  // for (auto &p : opt.addTagMap)
  //   if (!MetaDSF::isValidFrameID(p.first)) {
  //     std::cerr << "Tag '" << p.first;
  //     std::cerr <<"' invalid or unsupported" << std::endl;
  //     return 1;
  //   }

  // Validate the types of the pictures to be deleted
  for (auto &p : opt.removePicList) {
    long t;
    if (!stringToLong(p.toCString(), t) || !MetaDSF::isValidPicType(t)) {
      std::cerr << "Picture type '" << p << "' invalid'" << std::endl;
      return 1;
    }
  }

  // Validate pictures to be imported
  // Check if the files really exist,  are readable and of supported extensions
  //    (jpeg, tiff, gif...)
  // Check if types are correct
  PicTupleList picTupleList;
  if (!validatePictures(opt, picTupleList)) {
    return 1;
  }

  if (opt.dryRun) {
    opt.print();
  }

  for (auto &fileName : opt.fileList) {
    MetaDSF dsf(fileName.toCString());

    if (!opt.encoding.isEmpty())
      dsf.setEncoding(MetaDSF::getEncTypeByName(opt.encoding));
    if (!opt.version.isEmpty())
      dsf.setID3v2Version(opt.version.toInt());

    if (!dsf.isOK()) {
      std::cerr << fileName << ": error reading file." << std::endl;
      continue;
    }
    if (!doDelete(dsf, opt)) {
      return 1;
    }
    if (!doAdd(dsf, opt)) {
      return 1;
    }
    if (!importPictures(dsf, picTupleList)) {
      return 1;
    }
    if (!opt.dryRun)
      dsf.save();

    if (opt.exportPics) {
      std::string basename = 
	std::string(fileName.toCString()).substr(0, fileName.rfind("."));
      dsf.exportPictures(basename.c_str());
    }
    std::string prefix = "";
    if (opt.fileList.size() > 1) {
      prefix += fileName.toCString();
      prefix += ":";
    }

    if (opt.showInfo)
      dsf.printInfo(prefix.c_str());
   
    if (opt.showTags)
      dsf.printTags(prefix.c_str());
  } 
} // main()

bool doDelete(MetaDSF &dsf, OptionObj &opt) {
  if (opt.removeEverything) {
    dsf.deleteAllTags();
  } else {
    for (auto &i : opt.removeTagList) {
      dsf.deleteTags(i);
    }
    for (auto &i : opt.removePicList) {
      dsf.deletePictures(i);
    }
  }
  return true;
}

bool doAdd(MetaDSF &dsf, OptionObj &opt) {
  for (auto &i : opt.addTagMap) {
    TagLib::String name = MetaDSF::getFrameNameByID(i.first);
    
    //std::cerr << "<ADD> set " << i.first << "(" << name << ") to ";
    //std::cerr << i.second << std::endl;
    dsf.setTag(name, i.second);
  }
  return true;
}

bool validatePictures(OptionObj &opt, PicTupleList &tupleList) {
  for (auto &p : opt.addPicList) {
    StringVector tmp;
    TagLib::ID3v2::AttachedPictureFrame::Type pt = 
      TagLib::ID3v2::AttachedPictureFrame::FrontCover; 
    TagLib::String comment;

    // parse picture specs
    // --import-picture=PATH|[type|comment]
    split(p.toCString(), "|", tmp, true, 3);

    // check extension
    if (!MetaDSF::isValidImage(tmp[0])) {
      std::cerr << "Unknown image format: " << tmp[0] << std::endl;
      return false;
    }

    if (!isReadableFile(tmp[0].toCString())) {
      std::cerr << tmp[0] << " not accessible" << std::endl;
      return false;
    }
    if (tmp.size() >= 2) {
      if (tmp[1].size() != 0) {       // check if type is valid
	long t;
	if (!stringToLong(tmp[1].toCString(), t) || !MetaDSF::isValidPicType(t)) {
	  std::cerr << "Picture type '" << tmp[1] << "' invalid'" << std::endl;
	  return false;
	}
	pt = static_cast<TagLib::ID3v2::AttachedPictureFrame::Type>(t);
      }
    }
    if (tmp.size() == 3)
      comment = tmp[2];

    tupleList.push_back(PicTuple(tmp[0], pt, comment));
  }
  return true;
}

bool importPictures(MetaDSF &dsf, PicTupleList &pList) {
  for (auto &p : pList) {
    if (!dsf.attachPicture(std::get<0>(p), std::get<1>(p), std::get<2>(p)))
      return false;
  }
  return true;
}
