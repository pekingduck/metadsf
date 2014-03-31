MetaDSF
=======
Metadsf is a command line tool that helps you batch-edit the ID3v2 tags embedded in your DSF files.

``` sh
$ metadsf --set-tag=TPE1=Eagles --set-tag=TIT2="Hotel California" music.dsf
$ metadsf --show-tags music.dsf
TPE1=Eagles
TIT2=Hotel California
```

Installation
------------
Dependencies:
* **taglib 1.9.1** or newer
* A C++11-compliant compiler

Just do a `make` in the source directory and copy `metadsf` to somewhere in your `PATH`.

Options
-------
`--help` or `-h`

Display help info

`--version` or `-v`

Display the current version of metadsf

`--show-info` or `-i`

Display audio properties of the specified DSF files.

`--show-tags` or `-t`

Display all ID3v2 tags in the specified DSF files. 

`--encoding` or `-e`

Set the text encoding. Valid encodings are: "UTF8", "LATIN1", "UTF16", "UTF16LE", "UTF16BE". Default is "UTF8"

`--id3v2-version`

Set the version of the

`--add-tag` or `-a`

Add a tag. Existing tags with the same name are untouched.

```sh
$ metadsf --add-tag=TRCK=4 test.dsf
```

`--add-tags-from-file`

Same as `--add-tag`, but instead of specify the key-value pairs in the command line, you can supply a text file containing the pairs (one per line).

```sh
$ cat tags.txt
TPE=Rolling Stones
TPOS=1/1
$ metadsf --add-tags-from-file=tags.txt test.dsf
```

`--set-tag` or `-s`

This **replaces** all occurences of the tag with the specified value.

```sh
$ metadsf --show-tags test.dsf
TPE2=XYZ
TPE2=ABC
# update then display tags
$ metadsf --set-tag=TPE=IJK --show-tags test.dsf
TPE2=IJK
```

`--set-tags-from-file`

Same as `--set-tag`, but instead of specify the key-value pairs in the command line, you can supply a text file containing the pairs (one per line).

`--import-picture` or  `-p`

```sh
$ metadsf --import-picture='/path/to/your.jpg|3|Picture Comment' test.dsf
```

The string is a list of fields separated by '|'. The first field is the path to your picture file.
The second field is the picture type which is optional (the default is "3" which is "Front Cover")
The third field is picture comment which is also optional.

```sh
$ metadsf --import-picture='/path/to/your.jpg'
# no comment specified
$ metadsf --import-picture='/path/to/your.jpg|11'
# no picture type specified
$ metadsf --import-picture='/path/to/your.jpg||My Comment'
```

Currently GIF (`.gif`), TIFF (`tif` or `tiff`), PNG(`.png`) and JPEG(`.jpeg`, `.jpg`, `.jpe`) are allowed.
 
See appendix for a complete list of picture types.

`--export-all-pictures`

Export embedded pictures to files. Let's say you have 2 front covers and 1 back cover in JPEG format:
```sh
$ metadsf --export-all-pictures SomeDSDFile.dsf
$ ls *.jpg
SomeDSDFile_FrontCover_1.jpg
SomeDSDFile_FrontCover_2.jpg
SomeDSDFile_BackCover_1.jpg
```

`--remove-tags` or `-r`

A comma-separated list of tag names to be removed.
```sh
$ metadsf --remove-tags=TPE1,TPE2
```

`--remove-pictures`

A comma-separated of picture types to be removed.
```sh
# remove ALL front and back covers
$ metadsf --remove-pictures=3,4
```

`--remove-all-pictures`

Remove all embedded pictures in the file

`--remove-everything`

Remove all ID3v2 tags and pictures.

TODO
----
configure-style installation


Usage
              0: Other
              1: 32x32 pixels 'file icon'
              2: Other file icon
              3: Cover (front)
              4: Cover (back)
              5: Leaflet page
              6: Media (e.g. label side of CD)
              7: Lead artist/lead performer/soloist
              8: Artist/performer
              9: Conductor
              10: Band/Orchestra
              11: Composer
              12: Lyricist/text writer
              13: Recording Location
              14: During recording
              15: During performance
              16: Movie/video screen capture
              17: A bright coloured fish
              18: Illustration
              19: Band/artist logotype
              20: Publisher/Studio logotype
