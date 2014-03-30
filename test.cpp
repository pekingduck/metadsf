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

#include <string>
#include <stdio.h>
#include <taglib/tstring.h>
#include <taglib/dsffile.h>
#include <taglib/id3v2tag.h>

using namespace TagLib;
using namespace std;

//#define ASSERT_EQUAL

int main(int argc, char *argv[]) {
  const char * file = "test.dsf";
  String xxx = ByteVector(254, 'X');
  DSF::File f(file);
  cout << "Before..hasID3Tag(): " << f.hasID3v2Tag() << endl;
  
  f.tag()->setTitle(xxx);
  f.tag()->setArtist("Artist A");
  f.save(4, true);
  cout << "After..hasID3Tag(): " << f.hasID3v2Tag() << endl;
  
  DSF::File f2(file);
  cout << "version: " << f2.ID3v2Tag()->header()->majorVersion() << endl;
  cout << "artist: " <<  f2.tag()->artist() << endl;
  cout << "title: " << f2.tag()->title() << endl;
}
