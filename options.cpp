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
#include <taglib/tstringlist.h>
#include <memory>
#include "optionparser.h"
#include "options.h"
#include "utils.h"

//namespace myopt {

enum OptionIndex { 
  UNKNOWN, 
  HELP, 
  SHOW_INFO, 
  SHOW_TAGS,
  ENCODING, 
  VERSION,
  ID3V2_VERSION, 
  ADD_TAG, 
  ADD_TAGS_FROM_FILE,
  SET_TAG,
  SET_TAGS_FROM_FILE,
  REMOVE_TAGS, 
  REMOVE_EVERYTHING,
  REMOVE_ALL_PICTURES,
  IMPORT_PICTURE,
  REMOVE_PICTURES,
  EXPORT_PICTURES,
  DRY_RUN
};

const option::Descriptor usage[] = {
  { UNKNOWN, 0, "", "", option::Arg::None, "Usage: metadsf [options] files..." },
  { HELP, 0, "h", "help", option::Arg::None, "--help\n          Print usage and exit" },
  { SHOW_INFO, 0, "i", "show-info", option::Arg::None, "--show-info\n          Print file info" },
  { SHOW_TAGS, 0, "t", "show-tags", option::Arg::None, "--show-tags\n          Print tags" },
  { ENCODING, 0, "e", "encoding", option::Arg::Optional, "--encoding=...\n          Set encoding. Valid encodings are: UTF8(default), LATIN1, UTF16, UTF16LE, UTF16BE" },
  { ADD_TAG, 0, "a", "add-tag", option::Arg::Optional, "--add-tag=<NAME>=<VALUE>\n          Add tag" },
  { ADD_TAGS_FROM_FILE, 0, "", "add-tags-from-file", option::Arg::Optional, "--add-tags-from-file=<FILE>\n          Add tags from a file, one per line (TAG=VALUE)" },
  { SET_TAG, 0, "s", "set-tag", option::Arg::Optional, "--set-tag=NAME=...\n          Set tag. Old value will be overwritten" },
  { SET_TAGS_FROM_FILE, 0, "", "set-tags-from-file", option::Arg::Optional, "--set-tags-from-file=<FILE>\n          Set tags from a file, one per line (TAG=VALUE)" },
  { REMOVE_TAGS, 0, "r", "remove-tags", option::Arg::Optional, "--remove-tags=a,b,c\n          Remove ALL occurences of the specified tags" },
  { REMOVE_PICTURES, 0, "", "remove-pictures", option::Arg::Optional, "--remove-pictures=t1,t2,t3\n          Remove pictures of the specified types" },
  { REMOVE_ALL_PICTURES, 0, "", "remove-all-pictures", option::Arg::Optional, "--remove-all-pictures\n          Remove ALL pictures" },
  { REMOVE_EVERYTHING, 0, "", "remove-everything", option::Arg::Optional, "--remove-everything\n          Remove *ALL* ID3v2 tags including pictures" },
  { ID3V2_VERSION, 0, "", "id3v2-version", option::Arg::Optional, "--id3v2-version\n          Which ID3V2 version to save. Can be either 3 or 4" },
  { VERSION, 0, "v", "version", option::Arg::None, "--version\n          Display version info" },
  { IMPORT_PICTURE, 0, "p", "import-picture", option::Arg::Optional, "--import-picture=file[|type|comment]\n          Import picture into file."},
  { DRY_RUN, 0, "d", "dry-run", option::Arg::None, "--dry-run\n          Run without saving" },
  { EXPORT_PICTURES, 0, "", "export-all-pictures", option::Arg::Optional, "--export-all-pictures\n          Export pictures" }, 
  { 0, 0, 0, 0, 0, 0 }
};


// These handy functions are defined at the end

void getOptionPairsToMap(option::Option *o, OptionIndex index, StringMap &m);
int getUniqueReqdArg(option::Option *op, OptionIndex index, TagLib::String &val);
void getOptionsToVector(option::Option *o, OptionIndex index, StringVector &v,
			const char *sep = nullptr);

inline void printOptMultiError(const char *tag)
{
  std::cerr << "Multiple --" << tag << " supplied" << std::endl;
}

inline void printOptArgMissingError(const char *txt) 
{
  std::cerr << "No " << txt << " specified" << std::endl;
}

inline void printOptInvalidArgError(const char *txt) {
  std::cerr << "Invalid " << txt << std::endl;
}

void printMap(StringMap &m) {
  for (auto &i : m)
    std::cout << "  [" << i.first << "] [" << i.second << "]" << std::endl;
}

void printVector(StringVector &v) 
{
  for (auto &i : v)
    std::cout << "  " << i << std::endl;
}

#ifdef XXXX
int main(int argc, char *argv[]) {
  StringMap m;
  readTagsFromFile("test.csv", m);
  for (auto i = begin(m); i != end(m); ++i) {
    std::cout << "[" << i->first << "] [" << i->second << "]" << std::endl;
  }
}
#endif

void OptionObj::print() 
{
  std::cout << "Encoding: " << encoding << std::endl;
  std::cout << "Version: " << version << std::endl;
  std::cout << "Set Tags File: " << setTagsFile << std::endl;
  std::cout << "Add Tags File: " << addTagsFile << std::endl;
  std::cout << "Separator: " << separator << std::endl;
  std::cout << "Remove everything? " << removeEverything << std::endl;
  std::cout << "Remove all pictures? " << removeAllPics << std::endl;
  std::cout << "Show tags? " << showTags << std::endl;
  std::cout << "Show info? " << showInfo << std::endl;
  std::cout << "Export pics? " << exportPics << std::endl;
  std::cout << "Dry run? " << dryRun << std::endl;

  std::cout << "File List: " << std::endl;
  printVector(fileList);
  std::cout << "Remove Tag List: " << std::endl;
  printVector(removeTagList);
  std::cout << "Add Pic List: " << std::endl;
  printVector(addPicList);
  std::cout << "Delete Pic List: " << std::endl;
  printVector(removePicList);
  std::cout << "addTagMap: " << std::endl;
  printMap(addTagMap);
  //std::cout << "handyMap: " << std::endl;
  //printMap(handyMap);
}  
  
  
bool OptionObj::parse(int argc, char *argv[])
{
  option::Stats stats(usage, argc, argv);
  option::Option *options = new option::Option[stats.options_max];
  option::Option *buffer = new option::Option[stats.buffer_max];

  std::unique_ptr<option::Option[]> pOptions(options);
  std::unique_ptr<option::Option[]> pBuffer(buffer);

  option::Parser op(usage, argc, argv, options, buffer);

  if (op.error()) {
    return false;
  }

  if (options[HELP] || options[UNKNOWN] || argc == 1) {
    //option::printUsage(std::cout, usage);
    printUsage();
    return false;
  }

  for (int i = 0; i < op.nonOptionsCount(); ++i) {
    fileList.push_back(op.nonOption(i));
  }

  if (fileList.size() == 0) {
    std::cerr << "No files specified" << std::endl;
    return false;
  }

  if (options[SHOW_INFO].count() >= 1) {
    showInfo = true;
  }
  if (options[SHOW_TAGS].count() >= 1) {
    showTags = true;
  }
  if (options[EXPORT_PICTURES].count() >= 1) {
    exportPics = true;
  }
  if (options[DRY_RUN].count() >= 1) {
    dryRun = true;
  }

  // Encoding
  int c = getUniqueReqdArg(options, ENCODING, encoding);
  if (c > 1) {
    printOptMultiError("encoding");
    return false;
  } else if (c == -1) {
    printOptArgMissingError("encoding");
    return false;
  }
 
  // Separator
  // c = getUniqueReqdArg(options, SEPARATOR, separator);
  // if (c > 1) {
  //   std::cerr << "Multiple --separator supplied" << std::endl;
  //   return false;
  // } else if (c == -1) {
  //   std::cerr << "No separator specified" << std::endl;
  //   return false;
  // }

  // ID3v2 Version
  c = getUniqueReqdArg(options, ID3V2_VERSION, version);
  if (c > 1) {
    std::cerr << "Multiple --id3v2-version supplied" << std::endl;
    return false;
  } else if (c == -1) {
    std::cerr << "No ID3v2 version specified" << std::endl;
    return false;
  }

  // --remove-tag and
  // --remove-everything
  //StringMap removeMap;
  if (options[REMOVE_EVERYTHING].count() >= 1) {
    removeEverything = true;
  } else {
    getOptionsToVector(options, REMOVE_TAGS, removeTagList, ",");
    getOptionsToVector(options, REMOVE_PICTURES, removePicList, ",");
    if (options[REMOVE_ALL_PICTURES].count() >= 1) {
      removeAllPics = true;
      removeTagList.push_back("APIC");
    }
  }

  // --set-tags-from-file 
  //StringMap tmp;
  c = getUniqueReqdArg(options, SET_TAGS_FROM_FILE, setTagsFile);
  if (c > 1) {
    std::cerr << "Multiple --set-tags-from-file supplied" << std::endl;
    return false;
  } else if (c == -1) {
    std::cerr << "--set-tags-from-file: no file specified" << std::endl;
    return false;
  }

  // --set-tag
  //StringMap addMap;
  getOptionPairsToMap(options, SET_TAG, addTagMap);
  //addMap.insert(tmp.begin(), tmp.end());

  // --add-tags-from-file
  c = getUniqueReqdArg(options, ADD_TAGS_FROM_FILE, addTagsFile);
  if (c > 1) {
    std::cerr << "Multiple --add-tags-from-file supplied" << std::endl;
    return false;
  } else if (c == -1) {
    std::cerr << "--add-tags-from-file: no file specified" << std::endl;
    return false;
  }

  // --add-tag
  getOptionPairsToMap(options, ADD_TAG, addTagMap);

  // Processing handy commands such as --set-artist/title/.... etc
  // std::vector<std::pair<std::string, OptionIndex> > handyCmds = {
  //   { "artist", SET_ARTIST },
  //   { "title", SET_TITLE },
  //   { "genre", SET_GENRE },
  //   { "track", SET_TRACK },
  //   { "album", SET_ALBUM },
  //   { "year", SET_YEAR }
  // };
  // for (auto &cmd : handyCmds) { 
  //   TagLib::String val;
  //   c = getUniqueReqdArg(options, cmd.second, val);
  //   if (val.size() > 0) {
  //     handyMap[cmd.first] = val;
  //   }
  // }

  // --import-picture
  getOptionsToVector(options, IMPORT_PICTURE, addPicList);

  return true;
} // parse()


void getOptionsToVector(option::Option *o, OptionIndex index, StringVector &v,
			const char *sep)
{
  for (option::Option* opt = o[index]; opt; opt = opt->next())
    if (opt->arg != nullptr) {
      if (sep == 0) {
	v.push_back(opt->arg);
      } else {
	StringVector tmp;
	split(opt->arg, sep, tmp, false);
	for (auto &i : tmp) 
	  v.push_back(i);
      }
    }
}

void getOptionPairsToMap(option::Option *o, OptionIndex index, StringMap &m)
{
  for (option::Option* opt = o[index]; opt; opt = opt->next()) {
    if (opt->arg != nullptr) {
      std::string arg = opt->arg;
      StringVector v;
      split(arg, "=", v, true, 2);
      if (v.size() == 1)
	m[v[0]] = "";
      else if (v.size() == 2)
	m[v[0]] = v[1];
    }
  }
}

// check, if the option is supplied, that it appears only once and comes with
// an argument
int getUniqueReqdArg(option::Option *op, OptionIndex index, TagLib::String &val)
{
  if (op[index].count() > 1) {
    // multiple occurences
    return op[index].count();
  } else if (op[index].count() == 1){
    if (op[index].arg != nullptr && strlen(op[index].arg) > 0) {
      // OK
      val = op[index].arg;
      return 1;
    } 
    // option specific
    return -1;
  }
  return 0;
}

void OptionObj::printUsage()
{
#include "usage.h"
    std::cerr << text << std::endl;
}
//} // namespace
