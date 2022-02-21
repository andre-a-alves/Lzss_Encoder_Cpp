# Simple LZSS Encoder

A simple Lempel–Ziv–Storer–Szymanski (LZSS) encoder written in C++ with a GTK GUI.

## About

This is a simple application that was written to learn C++, learn GTK, and implement a well-known compression method.
This program will take an input file and compress it using an LZSS encoding method.

This has been written, compiled, and tested in Linux.

## LZSS Implementation Particulars

Wikipedia contains information on the [basics of LZSS](https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Storer%E2%80%93Szymanski).
This program implements an LZSS sceme with a window of 4095 bytes and tokens of up to 15 bytes in size.

### Encoding Bits

Since LZSS requires that each byte or group of bytes is preceded by a single bit that says whether it is encoded or not, every chunk of eight bytes or groups of bytes are preceded by a byte that contains the encoding bits for the entire chunk.


### Distance/Size

When a byte is not encoded, a single byte represents the next byte to be written to the file.
If a byte is encoded, two bytes contain the distance and size information of the encoded data.
The first 12 bits of the pair of bytes contains the distance from the current position where the encoded string of bytes begins.
Therefore, the program can look back 4095 bytes.
The last 4 bits of the pair of bytes contains the size of the encoded string.
Therefore, the program can compress strings of up to 15 bytes.

| First Byte | Second Byte |
|----|----|
|DDDDDDDD|DDDDssss|

### Null
The presence of a __null__ (`'\0'`, `00000000`) in a data bit signals the end of the encoded data.

## License
This software is released under the [MIT License](LICENSE).