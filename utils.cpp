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

#include <fstream>
#include "utils.h"

//namespace utils {

void split(const std::string &s, 
	   const std::string &delim, 
	   StringVector &result,
	   const bool keep_empty,
	   unsigned int upTo)
{
  if (delim.empty()) {
    result.push_back(s);
    return;
  }
  std::string::const_iterator substart = s.begin(), subend;
  while (true) {
    subend = search(substart, s.end(), delim.begin(), delim.end());
    std::string temp(substart, subend);
    if (keep_empty || !temp.empty()) {
      result.push_back(temp);
      --upTo;
    }
    if (subend == s.end()) {
      break;
    }
    substart = subend + delim.size();
    if (upTo == 1) {
      std::string rest(substart, s.end());
      result.push_back(rest);
      break;
    }
  }
}

bool readPairsFromFile(const char *path, StringMap &m)
{
  std::ifstream infile(path);
  // check if open was successful

  if (!infile.good()) {
    std::cerr << "Failed to open " << path << std::endl;
    return false;
  }
    
  std::string line;
  int i = 0;
  while (std::getline(infile, line)) {
    ++i;
    StringVector v;
    split(line, "=", v, false, 2);
    if (v.size() != 2) {
      std::cerr << path << ": line " << i << " is malformed" << std::endl;
      return false;
    }
    if (v[0].size() == 0) {
      std::cerr << path << ": line " << i << " tag name missing";
      return false;
    }
    if (v[1].size() == 0) {
      std::cerr << path << ": line " << i << " tag value missing" << std::endl;
      return false;
    }
    //std::cout << i << ": key=[" << key << "], val=[" << val << "]" << std::endl;
    m[v[0]] = v[1];
  }
  return true;
}

size_t loadFileIntoVector(const char *path, TagLib::ByteVector &v) 
{
  std::ifstream is(path, std::ifstream::binary);

  if (!is.good()) {
    return 0;
  }

  // get the file size and create an appropriate buffer
  is.seekg(0, is.end); 
  size_t len = is.tellg();
  is.seekg(0, is.beg);

  char *buf = new char[len];

  // read into buffer
  is.read(buf, len);
  v.setData(buf, len);

  is.close();
  delete[] buf;

  return len;
}

bool isReadableFile(const char *path)
{
  std::ifstream is(path, std::ifstream::binary);

  if (!is.good())
    return false;
  return true;
}

bool stringToLong(const std::string &s, long &l)
{
  try {
    l = std::stol(s);
    return true;
  } catch (std::invalid_argument) {
    return false;
  }
}

//} // namespace
