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

#include <iostream>
#include <algorithm>
#include <fstream>

#include <string.h>

#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>
#include <taglib/urllinkframe.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>

#include "dsffile.h"
#include "utils.h"
#include "metadsf.h"

//////////////////////////// IMPL //////////////////////////////
class MetaDSF::MetaDSFImpl
{
public:
    MetaDSFImpl(const char *path) : _changed(false),
        _file(path),
        _ID3v2_version(4),
        _encoding(TagLib::String::UTF8)
    {}
    ~MetaDSFImpl() {}

    /////////////// Utility Functions //////////////

    // delete all frames in the list
    void deleteFrames(const TagLib::ID3v2::FrameList &);

    // dup list
    void dupFrameList(const TagLib::ID3v2::FrameList &src,
                      TagLib::ID3v2::FrameList &dest);

    int deleteTags(const TagLib::String &);

    /////////////// Variables //////////////
    bool _changed; // whether there's any change to metadata
    // save() uses this to determine whether to write to disk
    DSFFile _file;
    int _ID3v2_version; // What version of ID3v2 to save (3 or 4)
    TagLib::String::Type _encoding; // Text encoding
};

///////////////////////////// METADSF //////////////////////////
MetaDSF::MetaDSF(const char *path)
{
    _i = new MetaDSFImpl(path);
}

MetaDSF::~MetaDSF()
{
    //save();
    if (_i)
    {
        delete _i;
    }
}

bool MetaDSF::save()
{
    // only save when changed were made
    if (_i->_changed)
    {
        _i->_changed = false;
        return _i->_file.save(_i->_ID3v2_version, true);
    }
    return true;
}

bool MetaDSF::isOK() const
{
    return (_i->_file.isOpen() && _i->_file.isValid());
}

void MetaDSF::printInfo(const char *prefix) const
{
    DSFProperties *p = static_cast<DSFProperties *>
                       (_i->_file.audioProperties());

    if (p)
    {
        std::cout << prefix;
        std::cout << "DSD version=" << p->version() << std::endl;
        std::cout << prefix;
        std::cout << "Sample rate=" << p->sampleRate() << "Hz" << std::endl;
        std::cout << prefix;
        std::cout << "No. of channels=" << p->channels() << std::endl;
        std::cout << prefix;
        std::cout << "Channel type=" << channelTypeDesc[p->channelType()] << std::endl;
        std::cout << prefix;
        std::cout << "Length=" << p->length() << "s" << std::endl;
        std::cout << prefix;
        std::cout << "No. of samples=" << p->sampleCount() << std::endl;
        std::cout << prefix;
        std::cout << "Bits per sample=" << p->bitsPerSample() << std::endl;
        std::cout << prefix;
        std::cout << "Metadata offset=" << p->ID3v2Offset() << std::endl;
        std::cout << prefix;
        std::cout << "File size=" << p->fileSize() << std::endl;
    }

    // if (_i->_file.ID3v2Tag()->isEmpty()) {
    //   std::cout << prefix;
    //   std::cout << "No ID3v2 metadata found" << std::endl;
    //   return;
    // }

    std::cout << prefix;
    std::cout << "ID3v2 version=2."
              << _i->_file.ID3v2Tag()->header()->majorVersion()
              << "."
              << _i->_file.ID3v2Tag()->header()->revisionNumber()
              << std::endl;
    std::cout << prefix;
    std::cout << "Tag size="
              << _i->_file.ID3v2Tag()->header()->completeTagSize()
              << " bytes" << std::endl;
}

void MetaDSF::printTags(const char *prefix) const
{
    if (_i->_file.ID3v2Tag()->isEmpty())
    {
        return;
    }

    TagLib::ID3v2::FrameList l = _i->_file.ID3v2Tag()->frameList();
    TagLib::ID3v2::FrameList::ConstIterator it;

    for (it = l.begin(); it != l.end(); it++)
    {
        TagLib::String val = (*it)->toString();
        std::cout << prefix;
        std::cout << (*it)->frameID() << "=" << val.to8Bit(true) << std::endl;
    }
}

int MetaDSF::setTag(const TagLib::String &key, const TagLib::String &val,
                    bool replace)
{
    std::string valt = val.to8Bit(false);
    TagLib::String valu = TagLib::String(valt, _i->_encoding);
    TagLib::StringList vals;

    vals.append(valu);
    return setTag(key, vals, replace);
}

int MetaDSF::setTag(const TagLib::String &key,
                    const TagLib::StringList &sl,
                    bool replace)
{
    int nReplaced = 0;
    if (replace)
        nReplaced = _i->deleteTags(key);

    TagLib::ID3v2::Frame *f = TagLib::ID3v2::Frame::createTextualFrame(key, sl);
    _i->_file.ID3v2Tag()->addFrame(f);
    _i->_changed = true;
    return nReplaced;
}

int MetaDSF::setTagTXXX(const TagLib::String &desc,
                        const TagLib::String &val,
                        bool replace)
{
    int nReplaced = 0;
    if (replace)
        nReplaced = _i->deleteTags("TXXX");

    TagLib::ID3v2::UserTextIdentificationFrame *f =
        new TagLib::ID3v2::UserTextIdentificationFrame(TagLib::String(desc),
                TagLib::String(val),
                _i->_encoding);
    _i->_file.ID3v2Tag()->addFrame(f);
    _i->_changed = true;
    return nReplaced;
}

int MetaDSF::setTagTMCL(const TagLib::PropertyMap &m, bool replace)
{
    int nReplaced = 0;
    if (replace)
        nReplaced = _i->deleteTags("TMCL");

    TagLib::ID3v2::TextIdentificationFrame *f =
        TagLib::ID3v2::TextIdentificationFrame::createTMCLFrame(m);
    _i->_file.ID3v2Tag()->addFrame(f);
    _i->_changed = true;
    return nReplaced;
}

int MetaDSF::setTagWXXX(const TagLib::String &url,
                        const TagLib::String &urlDesc,
                        bool replace)
{
    int nReplaced = 0;
    if (replace)
        nReplaced = _i->deleteTags("WXXX");

    TagLib::ID3v2::UserUrlLinkFrame *f =
        new TagLib::ID3v2::UserUrlLinkFrame(_i->_encoding);
    f->setDescription(urlDesc);
    f->setUrl(url);
    _i->_file.ID3v2Tag()->addFrame(f);
    _i->_changed = true;
    return nReplaced;
}

int MetaDSF::setTagTIPL(const TagLib::PropertyMap &m, bool replace)
{
    int nReplaced = 0;
    if (replace)
        nReplaced = _i->deleteTags("TIPL");

    TagLib::ID3v2::TextIdentificationFrame *f =
        TagLib::ID3v2::TextIdentificationFrame::createTIPLFrame(m);
    _i->_file.ID3v2Tag()->addFrame(f);
    _i->_changed = true;
    return nReplaced;
}


// void MetaDSF::setTitle(const TagLib::String &val) {
//   _i->deleteTags("TIT2");
//   _i->_file.ID3v2Tag()->setTitle(val);
// }

// void MetaDSF::setGenre(const TagLib::String &val) {
//   _i->deleteTags("TCON");
//   _i->_file.ID3v2Tag()->setGenre(val);
// }

// void MetaDSF::setYear(unsigned int year) {
//   _i->deleteTags("TDRC");
//   _i->_file.ID3v2Tag()->setYear(year);
// }

// void MetaDSF::setAlbum(const TagLib::String &val) {
//   _i->deleteTags("TALB");
//   _i->_file.ID3v2Tag()->setAlbum(val);
// }

// void MetaDSF::setTrack(unsigned int track) {
//   _i->deleteTags("TRCK");
//   _i->_file.ID3v2Tag()->setTrack(track);
// }

// void MetaDSF::setArtist(const TagLib::String &val) {
//   _i->deleteTags("TPE1");
//   _i->_file.ID3v2Tag()->setArtist(val);
// }

int MetaDSF::deleteTags(const TagLib::String &key)
{
    if (key.isEmpty())
    {
        std::cerr << "deleteTags(): key is empty" << std::endl;
        return 0;
    }
    return _i->deleteTags(key);
}

int MetaDSF::deleteAllTags()
{
    return _i->deleteTags("");
}


bool MetaDSF::attachPicture(const TagLib::String &path,
                            const TagLib::ID3v2::AttachedPictureFrame::Type t,
                            const TagLib::String &comment)
{
// Determine MIME type
    TagLib::String mimeType;
    TagLib::String ext = path.substr(path.rfind(".") + 1);

    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (extToMIMETypeMap.find(ext) == extToMIMETypeMap.end())
    {
        std::cerr << "Unknown image format " << ext << std::endl;
        return false;
    }

    mimeType = extToMIMETypeMap[ext];

    // Load file into memory
    TagLib::ByteVector v;
    if (loadFileIntoVector(path.toCString(), v) <= 0)
    {
        return false;
    }

    TagLib::ID3v2::AttachedPictureFrame *apic =
        new TagLib::ID3v2::AttachedPictureFrame();
    apic->setPicture(v);
    apic->setMimeType(mimeType);
    apic->setType(t);
    apic->setDescription(comment);
    _i->_file.ID3v2Tag()->addFrame(apic);
    _i->_changed = true;
    return true;
}

bool MetaDSF::attachPicture(const TagLib::String &path,
                            const TagLib::String &ptype)
{
    // Determine MIME type
    TagLib::String mimeType;
    TagLib::String ext = path.substr(path.rfind(".") + 1);

    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (extToMIMETypeMap.find(ext) == extToMIMETypeMap.end())
    {
        std::cerr << "Unknown image format " << ext << std::endl;
        return false;
    }

    mimeType = extToMIMETypeMap[ext];

    // Load file into memory
    TagLib::ByteVector v;
    if (loadFileIntoVector(path.toCString(), v) <= 0)
    {
        return false;
    }

    std::string pt = ptype.toCString();
    TagLib::ID3v2::AttachedPictureFrame *apic =
        new TagLib::ID3v2::AttachedPictureFrame();
    TagLib::ID3v2::AttachedPictureFrame::Type t =
        static_cast<TagLib::ID3v2::AttachedPictureFrame::Type>(std::stoi(pt, 0, 16));
    apic->setPicture(v);
    apic->setMimeType(mimeType);
    apic->setType(t);
    _i->_file.ID3v2Tag()->addFrame(apic);
    _i->_changed = true;
    return true;
}

int MetaDSF::deletePictures(const TagLib::String &ptype)
{
    TagLib::ID3v2::FrameList l = _i->_file.ID3v2Tag()->frameList("APIC");
    TagLib::ID3v2::FrameList dl; // a list of frames to be deleted
    TagLib::ID3v2::FrameList::ConstIterator it;
    std::string pt = ptype.toCString();
    // retrieve a list of frames that matches the given type
    for (it = l.begin(); it != l.end(); it++)
    {
        TagLib::ID3v2::AttachedPictureFrame *f =
            static_cast<TagLib::ID3v2::AttachedPictureFrame *>(*it);
        TagLib::ID3v2::AttachedPictureFrame::Type t =
            static_cast<TagLib::ID3v2::AttachedPictureFrame::Type>(std::stoi(pt,
                    0, 16));
        if (f->type() == t)
        {
            dl.append(f);
        }
    }

    _i->deleteFrames(dl);
    _i->_changed = true;
    return dl.size();
}

bool MetaDSF::exportPictures(const char *prefix) const
{
    std::map<std::string, unsigned int> counter;
    TagLib::ID3v2::FrameList l = _i->_file.ID3v2Tag()->frameList("APIC");
    TagLib::ID3v2::FrameList::ConstIterator it;
    for (it = l.begin(); it != l.end(); ++it)
    {
        TagLib::ID3v2::AttachedPictureFrame *f =
            static_cast<TagLib::ID3v2::AttachedPictureFrame *>(*it);
        std::string fname = prefix;
        std::string tname = picTypeDesc[f->type()].toCString();
        std::string ext = MIMETypeToExtMap[f->mimeType()].toCString();
        if (counter.find(tname) == counter.end())
            counter[tname] = 1;
        else
            counter[tname] += 1;
        fname += "_";
        fname += tname;
        fname += "_";
        fname += std::to_string(counter[tname]);
        fname += ".";
        fname += ext;
        //std::cout << "fname = " << fname << std::endl;
        writeFileFromVector(fname.c_str(), f->picture());
    }
    return true;
}

void MetaDSF::setID3v2Version(int v)
{
    if (v < 3 || v > 4)
    {
        _i->_ID3v2_version = 4;
    }
    else
    {
        _i->_ID3v2_version = v;
    }
}

void MetaDSF::setEncoding(const TagLib::String::Type type)
{
    _i->_encoding = type;
}

void MetaDSF::setEncoding(const TagLib::String &name)
{
    if (!isValidEncoding(name))
    {
        _i->_encoding = TagLib::String::UTF8;
    }
    else
    {
        _i->_encoding = encodingType[name];
    }
}

const TagLib::String::Type MetaDSF::getEncTypeByName(const TagLib::String &name)
{
    return encodingType[name];
}

const TagLib::String &MetaDSF::getFrameNameByID(const TagLib::String &id)
{
    return frameIDToNameMap[id];
}

bool MetaDSF::isValidEncoding(const TagLib::String &name)
{
    if (encodingType.find(name) == encodingType.end())
        return false;
    return true;
}

bool MetaDSF::isValidImage(const TagLib::String &file)
{
    TagLib::String ext = file.substr(file.rfind(".") + 1);
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (extToMIMETypeMap.find(ext) == extToMIMETypeMap.end())
        return false;
    return true;
}

bool MetaDSF::isValidFrameID(const TagLib::String &id)
{
    if (frameIDToNameMap.find(id) == frameIDToNameMap.end())
        return false;
    return true;
}

//// Private ////
void MetaDSF::MetaDSFImpl::deleteFrames(const TagLib::ID3v2::FrameList &l)
{
    TagLib::ID3v2::FrameList::ConstIterator it;

    for (it = l.begin(); it != l.end(); it++)
    {
        _file.ID3v2Tag()->removeFrame(*it);
    }
}

// Too bad can't use STL copy alogirthm....
// copy(src.begin(), src.end(), back_inserter(dest));
void MetaDSF::MetaDSFImpl::dupFrameList(const TagLib::ID3v2::FrameList &src,
                                        TagLib::ID3v2::FrameList &dest)
{
    TagLib::ID3v2::FrameList::ConstIterator it;

    for (it = src.begin(); it != src.end(); it++)
    {
        dest.append(*it);
    }
}

int MetaDSF::MetaDSFImpl::deleteTags(const TagLib::String &key)
{
    TagLib::ID3v2::FrameList l;
    if (key == "")
    {
        l = _file.ID3v2Tag()->frameList();
    }
    else
    {
        l = _file.ID3v2Tag()->frameList(key.toCString());
    }
    TagLib::ID3v2::FrameList ll;

    dupFrameList(l, ll);
    deleteFrames(ll);
    _changed = true;
    return ll.size();
}

/////////// static members ///////////
StringVector MetaDSF::channelTypeDesc =
{
    "Dummy", "Mono", "Stereo", "3 Channels", "Quad", "4 Channels",
    "5 Channels", "5.1 Channels"
};

std::vector<TagLib::ID3v2::AttachedPictureFrame::Type> MetaDSF::picType =
{
    TagLib::ID3v2::AttachedPictureFrame::Other,
    TagLib::ID3v2::AttachedPictureFrame::FileIcon,
    TagLib::ID3v2::AttachedPictureFrame::OtherFileIcon,
    TagLib::ID3v2::AttachedPictureFrame::FrontCover,
    TagLib::ID3v2::AttachedPictureFrame::BackCover,
    TagLib::ID3v2::AttachedPictureFrame::LeafletPage,
    TagLib::ID3v2::AttachedPictureFrame::Media,
    TagLib::ID3v2::AttachedPictureFrame::LeadArtist,
    TagLib::ID3v2::AttachedPictureFrame::Artist,
    TagLib::ID3v2::AttachedPictureFrame::Conductor,
    TagLib::ID3v2::AttachedPictureFrame::Band,
    TagLib::ID3v2::AttachedPictureFrame::Composer,
    TagLib::ID3v2::AttachedPictureFrame::Lyricist,
    TagLib::ID3v2::AttachedPictureFrame::RecordingLocation,
    TagLib::ID3v2::AttachedPictureFrame::DuringRecording,
    TagLib::ID3v2::AttachedPictureFrame::DuringPerformance,
    TagLib::ID3v2::AttachedPictureFrame::MovieScreenCapture,
    TagLib::ID3v2::AttachedPictureFrame::ColouredFish,
    TagLib::ID3v2::AttachedPictureFrame::Illustration,
    TagLib::ID3v2::AttachedPictureFrame::BandLogo,
    TagLib::ID3v2::AttachedPictureFrame::PublisherLogo
};

StringVector MetaDSF::picTypeDesc =
{
    "Other",
    "FileIcon",
    "OtherFileIcon",
    "FrontCover",
    "BackCover",
    "LeafletPage",
    "Media",
    "LeadArtist",
    "Artist",
    "Conductor",
    "Band",
    "Composer",
    "Lyricist",
    "RecordingLocation",
    "DuringRecording",
    "DuringPerformance",
    "MovieScreenCapture",
    "ColouredFish",
    "Illustration",
    "BandLogo",
    "PublisherLogo"
};

std::map<TagLib::String, TagLib::String::Type> MetaDSF::encodingType =
{
    { "UTF8", TagLib::String::UTF8 },
    { "LATIN1", TagLib::String::Latin1 },
    { "UTF16", TagLib::String::UTF16 },
    { "UTF16LE", TagLib::String::UTF16LE },
    { "UTF16BE", TagLib::String::UTF16BE }
};


StringMap MetaDSF::frameIDToNameMap =
{
    { "TALB","ALBUM" },
    { "TBPM","BPM" },
    { "TCOM","COMPOSER" },
    { "TCON","GENRE" },
    { "TCOP","COPYRIGHT" },
    { "TDEN","ENCODINGTIME" },
    { "TDLY","PLAYLISTDELAY" },
    { "TDOR","ORIGINALDATE" },
    { "TDRC","DATE" },
    { "TDRL","RELEASEDATE" },
    { "TDTG","TAGGINGDATE" },
    { "TENC","ENCODEDBY" },
    { "TEXT","LYRICIST" },
    { "TFLT","FILETYPE" },
    { "TIT1","CONTENTGROUP" },
    { "TIT2","TITLE" },
    { "TIT3","SUBTITLE" },
    { "TKEY","INITIALKEY" },
    { "TLAN","LANGUAGE" },
    { "TLEN","LENGTH" },
    { "TMED","MEDIA" },
    { "TMOO","MOOD" },
    { "TOAL","ORIGINALALBUM" },
    { "TOFN","ORIGINALFILENAME" },
    { "TOLY","ORIGINALLYRICIST" },
    { "TOPE","ORIGINALARTIST" },
    { "TOWN","OWNER" },
    { "TPE1","ARTIST" },
    { "TPE2","ALBUMARTIST" },
    { "TPE2","PERFORMER" },
    { "TPE3","CONDUCTOR" },
    { "TPE4","REMIXER" },
    { "TPOS","DISCNUMBER" },
    { "TPRO","PRODUCEDNOTICE" },
    { "TPUB","LABEL" },
    { "TRCK","TRACKNUMBER" },
    { "TRSN","RADIOSTATION" },
    { "TRSO","RADIOSTATIONOWNER" },
    { "TSOA","ALBUMSORT" },
    { "TSOP","ARTISTSORT" },
    { "TSOT","TITLESORT" },
    { "TSO2","ALBUMARTISTSORT" },
    { "TSRC","ISRC" },
    { "TSSE","ENCODING" },
    { "WCOP","COPYRIGHTURL" },
    { "WOAF","FILEWEBPAGE" },
    { "WOAR","ARTISTWEBPAGE" },
    { "WOAS","AUDIOSOURCEWEBPAGE" },
    { "WORS","RADIOSTATIONWEBPAGE" },
    { "WPAY","PAYMENTWEBPAGE" },
    { "WPUB","PUBLISHERWEBPAGE" },
    { "COMM","COMMENT" },
    { "TRDA","DATE" },
    { "TDAT","DATE" },
    { "TYER","DATE" },
    { "TIME","DATE" },
    { "TCMP","ITUNESCOMPILATIONFLAG" } // not in ID3v2 specs but wildly used
    //{ "TMCL","MUSICIANCREDITS" },
    //{ "TIPL","INVOLVEDPEOPLE" },
    //{ "WXXX","URL" }
};

StringMap MetaDSF::extToMIMETypeMap =
{
    { "jpg","image/jpeg" },
    { "jpe","image/jpeg" },
    { "jpeg","image/jpeg" },
    { "gif","image/gif" },
    { "tif","image/tiff" },
    { "tiff","image/tiff" }
};

StringMap MetaDSF::MIMETypeToExtMap =
{
    { "image/jpeg", "jpg" },
    { "image/gif", "gif" },
    { "image/tiff", "tiff" }
};
