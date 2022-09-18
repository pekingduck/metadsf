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

#include <stdint.h>
#include <stdio.h>
#include <iostream>

class fwriter
{
public:
    fwriter(FILE *f) : _f(f), _total(0) {}
    ~fwriter() {}
    void write(const void *ptr, size_t n)
    {
        _total += fwrite(ptr, 1, n, _f);
    }
    void print()
    {
        std::cout << _total << " bytes written" << std::endl;
    }
private:
    FILE *_f;
    size_t _total;
};

// turn 64-bit integer to raw
inline char *u64raw(uint64_t n, char *buf)
{
    for (int i = 0; i < 8; i++)
    {
        buf[i] = ((0xff << (i * 8)) & n) >> (i * 8);
    }
    return buf;
}

// turn 32-bit integer to raw
inline char *u32raw(uint32_t n, char *buf)
{
    for (int i = 0; i < 4; i++)
    {
        buf[i] = ((0xff << (i * 8)) & n) >> (i * 8);
    }
    return buf;
}

int main(int argc, char *argv[])
{
    // Create a minimal DSF file for testing

    if (argc != 2)
    {
        std::cerr << "file name not specified" << std::endl;
        exit(2);
    }

    FILE *f = fopen(argv[1], "w");
    if (!f)
    {
        std::cerr << "file name not specified" << std::endl;
        exit(2);
    }

    fwriter w(f);

    std::string s = "DSD ";
    w.write(s.c_str(), 4); // dsd chunk header

    char buf[8192];

    w.write(u64raw(28, buf), 8);   // dsd header size
    w.write(u64raw(8284, buf), 8); // file size
    w.write(u64raw(0, buf), 8); // pointer to id3v2 metadata

    s = "fmt ";
    w.write(s.c_str(), 4); // FMT chunk header
    w.write(u64raw(52, buf), 8); // FMT chunk size
    w.write(u32raw(1, buf), 4); // format version
    w.write(u32raw(0, buf), 4); // format id
    w.write(u32raw(2, buf), 4); // channel type
    w.write(u32raw(2, buf), 4); // channel number
    w.write(u32raw(2822400, buf), 4); // sampling freq.
    w.write(u32raw(1, buf), 4); // bits per sample
    w.write(u64raw(8192, buf), 8); // sampling count
    w.write(u32raw(4096, buf), 4); // block size per channel
    w.write(u32raw(0, buf), 4); // reserved

    s = "data"; // data chunk
    w.write(s.c_str(), 4);
    w.write(u64raw(8204, buf), 4); // data chunk size

    memset(&buf[0], 0, 8192);
    w.write(&buf[0], 8192); //sample data: 8192 samples = 2 x 4096 which
    //is the block size per channel

    w.print();

    fclose(f);
}
