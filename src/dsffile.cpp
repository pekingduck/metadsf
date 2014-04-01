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

#include <taglib/id3v2tag.h>
#include <taglib/id3v2header.h>
#include <taglib/tpropertymap.h>

#include <bitset>

#include "dsffile.h"
#include "dsfheader.h"

using namespace TagLib;

class DSF::File::FilePrivate
{
public:
  FilePrivate(ID3v2::FrameFactory *frameFactory = ID3v2::FrameFactory::instance()) :
    ID3v2FrameFactory(frameFactory),
    ID3v2Location(0),
    ID3v2OriginalSize(0),
    fileSize(0),
    tag(0),
    hasID3v2(false),
    properties(0)
  {}

  ~FilePrivate()
  {
    if (properties) delete properties;
    if (tag) delete tag;
  }

  const ID3v2::FrameFactory *ID3v2FrameFactory;

  uint64_t ID3v2Location; // For DSD it's always > 0 if tag is present
  uint64_t ID3v2OriginalSize;
  uint64_t fileSize;
  TagLib::ID3v2::Tag *tag;

  // These indicate whether the file *on disk* has these tags, not if
  // this data structure does.  This is used in computing offsets.

  bool hasID3v2;

  Properties *properties;

  static inline ByteVector& uint64ToVector(uint64_t num, ByteVector &v) {
    char raw[8];
    
    for (int i = 0; i < 8; i++) {
      raw[i] = ((0xff << (i * 8)) & num) >> (i * 8);
    }
    v.setData(raw, 8);
    return v;
  }

  // 
  // ID3v2::Tag::render() fills up space previous occupied by 
  // deleted frames with 0's, presumably to avoid rewriting 
  // all audio data (in an mp3 file, the ID3v2 tag comes before the audio 
  // data). 
  // However in a DSD file the ID3v2 chunk is located at the end.
  // 
  // This function attempts to shrink the ID3v2 Tag by replacing
  // the old ID3v2::Tag object with a new one to free up that space.
  //
  void shrinkTag();
};

void DSF::File::FilePrivate::shrinkTag() {
  ID3v2::FrameList olist = tag->frameList();
  ID3v2::FrameList nlist;
  ID3v2::FrameList::ConstIterator it;

  for (it = olist.begin(); it != olist.end(); it++) {
    nlist.append(*it);
  }

  ID3v2::Tag *ntag = new ID3v2::Tag();
  for (it = nlist.begin(); it != nlist.end(); it++) {
    tag->removeFrame(*it, false);  // Don't delete, just transfer the ownership
    ntag->addFrame(*it);
  }
  
  delete tag;
  tag = ntag;
}


////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

DSF::File::File(FileName file, bool readProperties,
                 Properties::ReadStyle propertiesStyle) : TagLib::File(file)
{
  d = new FilePrivate;

  if(isOpen())
    read(readProperties, propertiesStyle);
}

DSF::File::File(FileName file, ID3v2::FrameFactory *frameFactory,
                 bool readProperties, Properties::ReadStyle propertiesStyle) :
  TagLib::File(file)
{
  d = new FilePrivate(frameFactory);

  if(isOpen())
    read(readProperties, propertiesStyle);
}

DSF::File::File(IOStream *stream, ID3v2::FrameFactory *frameFactory,
                 bool readProperties, Properties::ReadStyle propertiesStyle) :
  TagLib::File(stream)
{
  d = new FilePrivate(frameFactory);

  if(isOpen())
    read(readProperties, propertiesStyle);
}

DSF::File::~File()
{
  delete d;
}

TagLib::Tag *DSF::File::tag() const
{
  return d->tag;
}

PropertyMap DSF::File::properties() const
{
  if(d->hasID3v2)
    return d->tag->properties();
  return PropertyMap();
}

void DSF::File::removeUnsupportedProperties(const StringList &properties)
{
  if(d->hasID3v2)
    d->tag->removeUnsupportedProperties(properties);
}

PropertyMap DSF::File::setProperties(const PropertyMap &properties)
{
  return d->tag->setProperties(properties);
}

DSF::Properties *DSF::File::audioProperties() const
{
  return d->properties;
}

bool DSF::File::save() 
{
  return save(4);
}

bool DSF::File::save(int id3v2Version, bool shrink)
{
  if(readOnly()) {
    std::cerr << "DSF::File::save() -- File is read only." << std::endl;
    return false;
  }

  bool success = true;

  if(ID3v2Tag() && !ID3v2Tag()->isEmpty()) {
    if (shrink) // remove padding 0's
      d->shrinkTag();

    ByteVector id3v2_v = ID3v2Tag()->render(id3v2Version);
    uint64_t fileSize = d->fileSize + id3v2_v.size() - d->ID3v2OriginalSize;
    ByteVector fileSize_v;

    // Write new file size to DSD header 
    DSF::File::FilePrivate::uint64ToVector(fileSize, fileSize_v);
    insert(fileSize_v, 12, DSF::Header::LONG_INT_SIZE);

    // The file didn't have an ID3v2 metadata block
    // Make the offset point to the end of file
    if (d->ID3v2Location == 0) {
      d->ID3v2Location = d->fileSize;

      ByteVector offset_v;
      DSF::File::FilePrivate::uint64ToVector(d->fileSize, offset_v);
      insert(offset_v, 20, DSF::Header::LONG_INT_SIZE);
    }

    // Write ID3v2 to the end of the file
    insert(id3v2_v, d->ID3v2Location, d->ID3v2OriginalSize);
    
    // Reset header info
    d->fileSize = fileSize;
    d->ID3v2OriginalSize = id3v2_v.size();
    d->hasID3v2 = true;
  } else {
    //
    // All frames have been deleted. Remove ID3v2 block
    //

    ByteVector nulls_v(DSF::Header::LONG_INT_SIZE, 0); // a bunch of zeroes
    ByteVector fileSize_v;

    DSF::File::FilePrivate::uint64ToVector(d->ID3v2Location, fileSize_v);
    insert(fileSize_v, 12, DSF::Header::LONG_INT_SIZE); // new file size
    insert(nulls_v, 20, DSF::Header::LONG_INT_SIZE); // set metadata offset to 0
    removeBlock(d->ID3v2Location, d->ID3v2OriginalSize);

    // Reset header info
    d->ID3v2OriginalSize = 0;
    d->fileSize = d->ID3v2Location;
    d->ID3v2Location = 0;
    d->hasID3v2 = false;
  }

  // Reinitialize properties because DSD header may have been changed
  delete d->properties;
  d->properties = new Properties(this, Properties::Average);

  return success;
}

ID3v2::Tag *DSF::File::ID3v2Tag() const
{
  return d->tag;
}

void DSF::File::setID3v2FrameFactory(const ID3v2::FrameFactory *factory)
{
  d->ID3v2FrameFactory = factory;
}

bool DSF::File::hasID3v2Tag() const
{
  return d->hasID3v2;
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

bool DSF::File::secondSynchByte(char byte)
{
  std::bitset<8> b(byte);

  // check to see if the byte matches 111xxxxx
  return b.test(7) && b.test(6) && b.test(5);
}

void DSF::File::read(bool readProperties, Properties::ReadStyle propertiesStyle)
{
  if(readProperties)
    d->properties = new Properties(this, propertiesStyle);

  d->ID3v2Location = d->properties->ID3v2Offset();
  d->fileSize = d->properties->fileSize();

  if(d->ID3v2Location > 0) {
    d->tag = new ID3v2::Tag(this, d->ID3v2Location, d->ID3v2FrameFactory);
    d->ID3v2OriginalSize = d->tag->header()->completeTagSize();

    if(d->tag->header()->tagSize() > 0)
      d->hasID3v2 = true;
  } else {
    // No ID3v2 Tag found, create an empty object.
    d->tag = new ID3v2::Tag();
  }
}
