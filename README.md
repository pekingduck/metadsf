metadsf
=======
`metadsf` is a command line tool that lets you batch-edit ID3v2 tags embedded in your DSF files.

``` sh
# long options
$ metadsf --set-tag=TPE1=Eagles --set-tag=TIT2="Hotel California" music.dsf
$ metadsf --show-tags music.dsf
TPE1=Eagles
TIT2=Hotel California

# short options
$ metadsf -sTPE1='Pink Floyd' -sTIT2=Money music.dsf
```

Installation
------------
Dependencies:
* **taglib 1.9.1** or newer
* A C++11-compliant compiler

```sh
# You may have to tell configure where your taglib is located
$ ./configure --prefix=/usr/local/
$ make
$ make install
```

Options
-------
Usage: metadsf [options] file1 file2 file3 ...

#### `--help` or `-h`
Display help info and exit.

#### `--version` or `-v`
Display the current version of `metadsf` and exit.

#### `--show-info` or `-i`
Display audio properties of the specified DSF files.
If more than one file are specified in the command line, each output line will be preceded by 
the corresponding file name.

#### `--show-tags` or `-t`
Display all ID3v2 tags in the specified DSF files.
If more than one file are specified in the command line, each output line will be preceded by 
the file name.

#### `--encoding` or `-e`
Set the text encoding of your input to various commands. Valid encodings are: "UTF8" (default), "LATIN1", "UTF16", "UTF16LE", "UTF16BE".

```sh
$ metadsf --set-tag=TALB="My Album" --encoding=UTF8 mymusic.dsf
$ metadsf --set-tag=TALB="My Album" -eUTF8
```

#### `--id3v2-version`
Save ID3v2 tags in either 2.3.0 or 2.4.0 version. Can be either 3 or 4.
```sh
# Save as ID3v2 2.3.0
$ metadsf -sTPE1=XXX --id3v2-version=3
```

#### `--add-tag` or `-a`
Add a tag. Existing tags with the same name are untouched.
```sh
$ metadsf --add-tag=TRCK=4 -aTPOS=1/2 test.dsf
```

#### `--add-tags-from-file`
Same as `--add-tag`, but instead of specify the key-value pairs in the command line, you can supply a text file containing the pairs (one per line).
```sh
$ cat tags.txt
TPE1=Rolling Stones
TPOS=1/1
$ metadsf --add-tags-from-file=tags.txt test.dsf
```

#### `--set-tag` or `-s`
This **replaces** all occurences of the tag with the specified value.
```sh
$ metadsf --show-tags test.dsf
TPE2=XYZ
TPE2=ABC
# replace and display tags
$ metadsf --set-tag=TPE=IJK --show-tags test.dsf
TPE2=IJK
```

#### `--set-tags-from-file`
Same as `--set-tag`, but instead of specify the key-value pairs in the command line, you can supply a text file containing the pairs (one per line).

#### `--import-picture` or  `-p`
Import picture into the DSF file
```sh
# long form
$ metadsf --import-picture='/path/to/your.jpg|3|Picture Comment' test.dsf
# short form
$ metadsf -p'/some/dir/file.jpg'
```

The string is a list of fields separated by '|'. The first field is the path to your picture file.
The second field is the picture type which is optional (the default is "3" which means "Front Cover")
The third field is picture comment which is also optional.
```sh
$ metadsf --import-picture='/path/to/your.jpg'
# no comment specified
$ metadsf --import-picture='/path/to/your.jpg|11'
# no picture type specified
$ metadsf --import-picture='/path/to/your.jpg||My Comment'
```

Currently GIF(`.gif`), TIFF (`.tif` or `.tiff`), PNG(`.png`) and JPEG(`.jpeg`, `.jpg`, `.jpe`) files are allowed.
 
See Appendix I for the complete list of picture types.

#### `--export-all-pictures`
Export embedded pictures to files. Exported files are named `<ORIGINAL_FILENAME>_<PICTURE_TYPE>_<NUMBER>.<EXT>`.

E.g. let's say you have 2 front covers and 1 back cover in JPEG format embedded inside `SomeDSDFile.dsf`:
```sh
$ metadsf --export-all-pictures SomeDSDFile.dsf
$ ls *.jpg
SomeDSDFile_FrontCover_1.jpg
SomeDSDFile_FrontCover_2.jpg
SomeDSDFile_BackCover_1.jpg
```

#### `--remove-tags` or `-r`
A comma-separated list of tag names to be removed.
```sh
$ metadsf --remove-tags=TPE1,TPE2
```

#### `--remove-pictures`
A comma-separated list of picture types to be removed.
```sh
# remove ALL front and back covers
$ metadsf --remove-pictures=3,4
```

#### `--remove-all-pictures`
Remove all embedded pictures in the file

#### `--remove-everything`
Remove **ALL** ID3v2 metadata including tags and pictures.

Examples
--------
Note that you can mix and match options together
```sh
# Clear everything and start all over again
$ metadsf --remove-everything -sTALB=Thriller --import-picture=thriller.jpg 01-Beat_It.dsf 02-Thriller.dsf

# Remove all pictures and a couple tags, and import new pictures
$ metadsf --remove-all-pictures -rTALB,WCOP --import-picture=somepic.jpg --import-picture=anotherpic.jpg music.dsf
```

TODO
----
1. Support for TXXX, WXXX, TMCP, TIPL tags


Appendix I
----------
Below is the complete list of picture types.
* 0: Other
* 1: 32x32 pixels 'file icon'
* 2: Other file icon
* 3: Cover (front)
* 4: Cover (back)
* 5: Leaflet page
* 6: Media (e.g. label side of CD)
* 7: Lead artist/lead performer/soloist
* 8: Artist/performer
* 9: Conductor
* 10: Band/Orchestra
* 11: Composer
* 12: Lyricist/text writer
* 13: Recording Location
* 14: During recording
* 15: During performance
* 16: Movie/video screen capture
* 17: A bright coloured fish
* 18: Illustration
* 19: Band/artist logotype
* 20: Publisher/Studio logotype

Appendix II
-----------
Below is the complete list of supported ID3v2 tags 
* TALB
* TBPM
* TCOM
* TCON
* TCOP
* TDEN
* TDLY
* TDOR
* TDRC
* TDRL
* TDTG
* TENC
* TEXT
* TFLT
* TIT1
* TIT2
* TIT3
* TKEY
* TLAN
* TLEN
* TMED
* TMOO
* TOAL
* TOFN
* TOLY
* TOPE
* TOWN
* TPE1
* TPE2
* TPE3
* TPE4
* TPOS
* TPRO
* TPUB
* TRCK
* TRSN
* TRSO
* TSOA
* TSOP
* TSOT
* TSO2
* TSRC
* TSSE
* WCOP
* WOAF
* WOAR
* WOAS
* WORS
* WPAY
* WPUB
* COMM
* TRDA
* TDAT
* TYER
* TIME
