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

  const char *text =
    R"(Usage: metadsf [options] files...
--help
          Print usage and exit
--show-info
          Print file info
--show-tags
          Print tags
--encoding=...
          Set encoding. Valid encodings are: UTF8(default), LATIN1, UTF16,
UTF16LE, UTF16BE
--add-tag=<NAME>=<VALUE>
          Add tag
--add-tags-from-file=<FILE>
          Add tags from a file, one per line (TAG=VALUE)
--set-tag=NAME=...
          Set tag. Old value will be overwritten
--set-tags-from-file=<FILE>
          Set tags from a file, one per line (TAG=VALUE)
--remove-tags=a,b,c
          Remove ALL occurences of the specified tags
--remove-pictures=t1,t2,t3
          Remove pictures of the specified types
--remove-all-pictures
          Remove ALL pictures
--remove-everything
          Remove *ALL* ID3v2 tags including pictures
--id3v2-version
          Which ID3V2 version to save. Can be either 3 or 4
--version
          Display version info
--import-picture=file[|type|comment]
          Import picture into file.
--export-picture
          Export all pictures
--dry-run
          Run without saving

For more complete documentation please go to https://github.com/pekingduck/metadsf
)";
