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

#ifndef _METADSF_H_
#define _METADSF_H_

#include "typedefs.h"

#include <taglib/attachedpictureframe.h>


class MetaDSF {
 public:
  // initializes the above static members

  MetaDSF(const char *);
  ~MetaDSF();
  
  // Check if object initializes OK
  bool isOK() const;

  // Save changes to disk
  bool save();

  // Delete a picture. Return the number of pictures deleted
  int deletePictures(const TagLib::String &ptype);

  // Attach a picture of type ptype (default is "03" which is front cover)
  bool attachPicture(const TagLib::String &file, 
		     const TagLib::String &ptype = "03");

  bool attachPicture(const TagLib::String &file,
		     const TagLib::ID3v2::AttachedPictureFrame::Type,
		     const TagLib::String &comment = "");

  bool exportPictures(const char *prefix) const;

  // Set simple textual data
  int setTag(const TagLib::String &key, 
	     const TagLib::String &val, bool replace = false);

  // Same as above, but takes value as a String list
  int setTag(const TagLib::String &key, 
	     const TagLib::StringList &vals, bool replace = false);

  // TXXX - user-defined text
  int setTagTXXX(const TagLib::String &desc, 
		 const TagLib::String &vals, bool replace = false);

  // WXXX - user-defined URL
  int setTagWXXX(const TagLib::String &url, 
		 const TagLib::String &urlDesc, bool replace = false);

  // TIPL - people involved.
  int setTagTIPL(const TagLib::PropertyMap &, bool replace = false);
    
  // TMCL - music credit list.
  int setTagTMCL(const TagLib::PropertyMap &, bool replace = false);

  // Handy functions
  // (All delete existing data first)
  /* void setArtist(const TagLib::String &); */
  /* void setGenre(const TagLib::String &); */
  /* void setTitle(const TagLib::String &); */
  /* void setAlbum(const TagLib::String &); */
  /* void setYear(unsigned int); */
  /* void setTrack(unsigned int); */


  // Delete tags by key. Return the number of tags deleted (same key can
  // appear more than once)
  int deleteTags(const TagLib::String &key);

  // Delete all tags (frames). Return the number of frames deleted
  int deleteAllTags();

  // Dump file info to cout
  void printInfo(const char *prefix = "") const;

  // Dump tags to cout
  void printTags(const char *prefix = "") const;

  // Set ID3v2 version. Can be either 3 or 4.
  void setID3v2Version(int);

  // Set encoding
  void setEncoding(const TagLib::String &name);
  void setEncoding(const TagLib::String::Type type);

  static const TagLib::String &getChannelTypeDesc(unsigned int pos);
  static const TagLib::String &getPicTypeDesc(unsigned int pos);
  static const TagLib::String &getFrameNameByID(const TagLib::String &id);
  static const TagLib::String::Type getEncTypeByName(const TagLib::String &name);
  static bool isValidEncoding(const TagLib::String &name);
  static bool isValidImage(const TagLib::String &file);
  static bool isValidFrameID(const TagLib::String &id);
  static bool isValidPicType(uint i) {
    return (i < picType.size());
  }
 private:
  MetaDSF(const MetaDSF &);
  MetaDSF &operator=(const MetaDSF &);

  static StringVector channelTypeDesc;
  static StringVector picTypeDesc;
  static std::vector<TagLib::ID3v2::AttachedPictureFrame::Type> picType;
  static std::map<TagLib::String, TagLib::String::Type> encodingType;
  static StringMap frameIDToNameMap;
  static StringMap extToMIMETypeMap;
  static StringMap MIMETypeToExtMap;

  class MetaDSFImpl;
  MetaDSFImpl *_i;
};

#endif
