#include "TextureLoader.h"

/*
Introduction

TGA or TARGA format is a format for describing bitmap images, it is capable of
representing bitmaps ranging from black and white, indexed colour, and RGB colour,
the format also supports various compression methods. This note describes the
minimal requirements for creating a TGA file for a 24 bit RGB uncompressed
colour image, this covers most applications where a developer might want to
create an image for another package which reads TGA files. A more complete
description of the format is given after this special case and the complete
specification is supplied at the end.

Mimimal requirements for writing a TGA file of a 24 or 32 bit image

A TGA file consists of 2 main parts, a header followed by the colour pixel
information. The minimal header consists of the size of the bitmap and the
amount of colour information. In cases where an integer is written to a file
the information is stored with the low order byte followed by the high order
byte, ie: little endian.

The code to create the header in C might be:
putc(0,fptr);
putc(0,fptr);
putc(2,fptr);
putc(0,fptr); putc(0,fptr);
putc(0,fptr); putc(0,fptr);
putc(0,fptr);
putc(0,fptr); putc(0,fptr);
putc(0,fptr); putc(0,fptr);
putc((width & 0x00FF),fptr);
putc((width & 0xFF00) / 256,fptr);
putc((height & 0x00FF),fptr);
putc((height & 0xFF00) / 256,fptr);
putc(24,fptr);
putc(0,fptr);
To determine the possible use of the other fields see the full format
description below. TGA can support a fourth alpha channel, just specify
32 instead of 24 for the depth, the alpha channel comes after the red byte.

In the special case of 24 bit colour described here the image data
consists of 3 bytes per pixel, for the red, green, and blue component
in the normal colour cube space. The image pixel data should follow
for width * height pixels, a single pixel might be written as

putc((int)(colour.b*255),fptr);
putc((int)(colour.g*255),fptr);
putc((int)(colour.r*255),fptr);
where the components of colour range from 0 to 1.
More complete description

The header (18 bytes) might be defined as follows for C/C++. You
should be careful with this since it has byte values which some
compilers will align to natural boundaries. This is checked by
printing out the sizeof(HEADER), if it greater than 18 then byte
alignment is taking effect and a fread() using the struct below
will not give the right results. In that case you either need to
read your compiler manual and turn off boundary alignment, of
read the components one at a time, sigh!

typedef struct {
char  idlength;
char  colourmaptype;
char  datatypecode;
short int colourmaporigin;
short int colourmaplength;
char  colourmapdepth;
short int x_origin;
short int y_origin;
short width;
short height;
char  bitsperpixel;
char  imagedescriptor;
} HEADER;
The idlength is the length of a string located located after the header.

All Targa formats are identified by a Data Type field, which
is a one byte binary integer located in byte three of the file. These data types are:

0  -  No image data included.
1  -  Uncompressed, color-mapped images.
2  -  Uncompressed, RGB images.
3  -  Uncompressed, black and white images.
9  -  Runlength encoded color-mapped images.
10  -  Runlength encoded RGB images.
11  -  Compressed, black and white images.
32  -  Compressed color-mapped data, using Huffman, Delta, and
runlength encoding.
33  -  Compressed color-mapped data, using Huffman, Delta, and
runlength encoding.  4-pass quadtree-type process.
The bitsperpixel specifies the size of each colour value.When 24 or 32 the
normal conventions apply. For 16 bits each colour component is stored
as 5 bits and the remaining bit is a binary alpha value. The colour
components are converted into single byte components by simply shifting
each component up by 3 bits (multiply by 8).

Source code

Here is some simple source code tgatest.c that reads a compressed or
uncompressed TGA file of either 16, 24, or 32 bit colour and writes
an uncompressed version. The primary purpose is to illustrate how the
RLE compression works and the implementation of 16 bit extraction.

Published specification

DATA TYPE 1: Color-mapped images
________________________________________________________________________________
| Offset | Length |                     Description                            |
|--------|--------|------------------------------------------------------------|
|    0   |     1  |  Number of Characters in Identification Field.             |
|        |        |                                                            |
|        |        |  This field is a one-byte unsigned integer, specifying     |
|        |        |  the length of the Image Identification Field.  Its range  |
|        |        |  is 0 to 255.  A value of 0 means that no Image            |
|        |        |  Identification Field is included.                         |
|--------|--------|------------------------------------------------------------|
|    1   |     1  |  Color Map Type.                                           |
|        |        |                                                            |
|        |        |  This field contains a binary 1 for Data Type 1 images.    |
|--------|--------|------------------------------------------------------------|
|    2   |     1  |  Image Type Code.                                          |
|        |        |                                                            |
|        |        |  This field will always contain a binary 1.                |
|        |        |  ( That's what makes it Data Type 1 ).                     |
|--------|--------|------------------------------------------------------------|
|    3   |     5  |  Color Map Specification.                                  |
|        |        |                                                            |
|    3   |     2  |  Color Map Origin.                                         |
|        |        |  Integer ( lo-hi ) index of first color map entry.         |
|    5   |     2  |  Color Map Length.                                         |
|        |        |  Integer ( lo-hi ) count of color map entries.             |
|    7   |     1  |  Color Map Entry Size.                                     |
|        |        |  Number of bits in each color map entry.  16 for           |
|        |        |  the Targa 16, 24 for the Targa 24, 32 for the Targa 32.   |
|--------|--------|------------------------------------------------------------|
|    8   |    10  |  Image Specification.                                      |
|        |        |                                                            |
|    8   |     2  |  X Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) X coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   10   |     2  |  Y Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) Y coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   12   |     2  |  Width of Image.                                           |
|        |        |  Integer ( lo-hi ) width of the image in pixels.           |
|   14   |     2  |  Height of Image.                                          |
|        |        |  Integer ( lo-hi ) height of the image in pixels.          |
|   16   |     1  |  Image Pixel Size.                                         |
|        |        |  Number of bits in a stored pixel index.                   |
|   17   |     1  |  Image Descriptor Byte.                                    |
|        |        |  Bits 3-0 - number of attribute bits associated with each  |
|        |        |             pixel.                                         |
|        |        |  Bit 4    - reserved.  Must be set to 0.                   |
|        |        |  Bit 5    - screen origin bit.                             |
|        |        |             0 = Origin in lower left-hand corner.          |
|        |        |             1 = Origin in upper left-hand corner.          |
|        |        |             Must be 0 for Truevision images.               |
|        |        |  Bits 7-6 - Data storage interleaving flag.                |
|        |        |             00 = non-interleaved.                          |
|        |        |             01 = two-way (even/odd) interleaving.          |
|        |        |             10 = four way interleaving.                    |
|        |        |             11 = reserved.                                 |
|        |        |  This entire byte should be set to 0.  Don't ask me.       |
|--------|--------|------------------------------------------------------------|
|   18   | varies |  Image Identification Field.                               |
|        |        |                                                            |
|        |        |  Contains a free-form identification field of the length   |
|        |        |  specified in byte 1 of the image record.  It's usually    |
|        |        |  omitted ( length in byte 1 = 0 ), but can be up to 255    |
|        |        |  characters.  If more identification information is        |
|        |        |  required, it can be stored after the image data.          |
|--------|--------|------------------------------------------------------------|
| varies | varies |  Color map data.                                           |
|        |        |                                                            |
|        |        |  The offset is determined by the size of the Image         |
|        |        |  Identification Field.  The length is determined by        |
|        |        |  the Color Map Specification, which describes the          |
|        |        |  size of each entry and the number of entries.             |
|        |        |  Each color map entry is 2, 3, or 4 bytes.                 |
|        |        |  Unused bits are assumed to specify attribute bits.        |
|        |        |  The 4 byte entry contains 1 byte for blue, 1 byte         |
|        |        |  for green, 1 byte for red, and 1 byte of attribute        |
|        |        |  information, in that order.                               |
|        |        |  The 3 byte entry contains 1 byte each of blue, green,     |
|        |        |  and red.                                                  |
|        |        |  The 2 byte entry is broken down as follows:               |
|        |        |  ARRRRRGG GGGBBBBB, where each letter represents a bit.    |
|        |        |  But, because of the lo-hi storage order, the first byte   |
|        |        |  coming from the file will actually be GGGBBBBB, and the   |
|        |        |  second will be ARRRRRGG. "A" represents an attribute bit. |
|--------|--------|------------------------------------------------------------|
| varies | varies |  Image Data Field.                                         |
|        |        |                                                            |
|        |        |  This field specifies (width) x (height) color map         |
|        |        |  indices.  Each index is stored as an integral number      |
|        |        |  of bytes (typically 1 or 2).   All fields are unsigned.   |
|        |        |  The low-order byte of a two-byte field is stored first.   |
--------------------------------------------------------------------------------
DATA TYPE 2: Unmapped RGB
________________________________________________________________________________
| Offset | Length |                     Description                            |
|--------|--------|------------------------------------------------------------|
|    0   |     1  |  Number of Characters in Identification Field.             |
|        |        |                                                            |
|        |        |  This field is a one-byte unsigned integer, specifying     |
|        |        |  the length of the Image Identification Field.  Its value  |
|        |        |  is 0 to 255.  A value of 0 means that no Image            |
|        |        |  Identification Field is included.                         |
|--------|--------|------------------------------------------------------------|
|    1   |     1  |  Color Map Type.                                           |
|        |        |                                                            |
|        |        |  This field contains either 0 or 1.  0 means no color map  |
|        |        |  is included.  1 means a color map is included, but since  |
|        |        |  this is an unmapped image it is usually ignored.  TIPS    |
|        |        |  ( a Targa paint system ) will set the border color        |
|        |        |  the first map color if it is present.                     |
|--------|--------|------------------------------------------------------------|
|    2   |     1  |  Image Type Code.                                          |
|        |        |                                                            |
|        |        |  This field will always contain a binary 2.                |
|        |        |  ( That's what makes it Data Type 2 ).                     |
|--------|--------|------------------------------------------------------------|
|    3   |     5  |  Color Map Specification.                                  |
|        |        |                                                            |
|        |        |  Ignored if Color Map Type is 0; otherwise, interpreted    |
|        |        |  as follows:                                               |
|    3   |     2  |  Color Map Origin.                                         |
|        |        |  Integer ( lo-hi ) index of first color map entry.         |
|    5   |     2  |  Color Map Length.                                         |
|        |        |  Integer ( lo-hi ) count of color map entries.             |
|    7   |     1  |  Color Map Entry Size.                                     |
|        |        |  Number of bits in color map entry.  16 for the Targa 16,  |
|        |        |  24 for the Targa 24, 32 for the Targa 32.                 |
|--------|--------|------------------------------------------------------------|
|    8   |    10  |  Image Specification.                                      |
|        |        |                                                            |
|    8   |     2  |  X Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) X coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   10   |     2  |  Y Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) Y coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   12   |     2  |  Width of Image.                                           |
|        |        |  Integer ( lo-hi ) width of the image in pixels.           |
|   14   |     2  |  Height of Image.                                          |
|        |        |  Integer ( lo-hi ) height of the image in pixels.          |
|   16   |     1  |  Image Pixel Size.                                         |
|        |        |  Number of bits in a pixel.  This is 16 for Targa 16,      |
|        |        |  24 for Targa 24, and .... well, you get the idea.         |
|   17   |     1  |  Image Descriptor Byte.                                    |
|        |        |  Bits 3-0 - number of attribute bits associated with each  |
|        |        |             pixel.  For the Targa 16, this would be 0 or   |
|        |        |             1.  For the Targa 24, it should be 0.  For     |
|        |        |             Targa 32, it should be 8.                      |
|        |        |  Bit 4    - reserved.  Must be set to 0.                   |
|        |        |  Bit 5    - screen origin bit.                             |
|        |        |             0 = Origin in lower left-hand corner.          |
|        |        |             1 = Origin in upper left-hand corner.          |
|        |        |             Must be 0 for Truevision images.               |
|        |        |  Bits 7-6 - Data storage interleaving flag.                |
|        |        |             00 = non-interleaved.                          |
|        |        |             01 = two-way (even/odd) interleaving.          |
|        |        |             10 = four way interleaving.                    |
|        |        |             11 = reserved.                                 |
|--------|--------|------------------------------------------------------------|
|   18   | varies |  Image Identification Field.                               |
|        |        |                                                            |
|        |        |  Contains a free-form identification field of the length   |
|        |        |  specified in byte 1 of the image record.  It's usually    |
|        |        |  omitted ( length in byte 1 = 0 ), but can be up to 255    |
|        |        |  characters.  If more identification information is        |
|        |        |  required, it can be stored after the image data.          |
|--------|--------|------------------------------------------------------------|
| varies | varies |  Color map data.                                           |
|        |        |                                                            |
|        |        |  If the Color Map Type is 0, this field doesn't exist.     |
|        |        |  Otherwise, just read past it to get to the image.         |
|        |        |  The Color Map Specification describes the size of each    |
|        |        |  entry, and the number of entries you'll have to skip.     |
|        |        |  Each color map entry is 2, 3, or 4 bytes.                 |
|--------|--------|------------------------------------------------------------|
| varies | varies |  Image Data Field.                                         |
|        |        |                                                            |
|        |        |  This field specifies (width) x (height) pixels.  Each     |
|        |        |  pixel specifies an RGB color value, which is stored as    |
|        |        |  an integral number of bytes.                              |
|        |        |  The 2 byte entry is broken down as follows:               |
|        |        |  ARRRRRGG GGGBBBBB, where each letter represents a bit.    |
|        |        |  But, because of the lo-hi storage order, the first byte   |
|        |        |  coming from the file will actually be GGGBBBBB, and the   |
|        |        |  second will be ARRRRRGG. "A" represents an attribute bit. |
|        |        |  The 3 byte entry contains 1 byte each of blue, green,     |
|        |        |  and red.                                                  |
|        |        |  The 4 byte entry contains 1 byte each of blue, green,     |
|        |        |  red, and attribute.  For faster speed (because of the     |
|        |        |  hardware of the Targa board itself), Targa 24 images are  |
|        |        |  sometimes stored as Targa 32 images.                      |
--------------------------------------------------------------------------------
DATA TYPE 9: Run Length Encoded, color-mapped images
--------------------------------------------------------------------------------
| Offset | Length |                     Description                            |
|--------|--------|------------------------------------------------------------|
|    0   |     1  |  Number of Characters in Identification Field.             |
|        |        |                                                            |
|        |        |  This field is a one-byte unsigned integer, specifying     |
|        |        |  the length of the Image Identification Field.  Its value  |
|        |        |  is 0 to 255.  A value of 0 means that no Image            |
|        |        |  Identification Field is included.                         |
|--------|--------|------------------------------------------------------------|
|    1   |     1  |  Color Map Type.                                           |
|        |        |                                                            |
|        |        |  This field is always 1 for color-mapped images.           |
|--------|--------|------------------------------------------------------------|
|    2   |     1  |  Image Type Code.                                          |
|        |        |                                                            |
|        |        |  A binary 9 for this data type.                            |
|--------|--------|------------------------------------------------------------|
|    3   |     5  |  Color Map Specification.                                  |
|        |        |                                                            |
|    3   |     2  |  Color Map Origin.                                         |
|        |        |  Integer ( lo-hi ) index of first color map entry.         |
|    5   |     2  |  Color Map Length.                                         |
|        |        |  Integer ( lo-hi ) count of color map entries.             |
|    7   |     1  |  Color Map Entry Size.                                     |
|        |        |  Number of bits in each color map entry.  16 for the       |
|        |        |  Targa 16, 24 for the Targa 24, 32 for the Targa 32.       |
|--------|--------|------------------------------------------------------------|
|    8   |    10  |  Image Specification.                                      |
|        |        |                                                            |
|    8   |     2  |  X Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) X coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   10   |     2  |  Y Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) Y coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   12   |     2  |  Width of Image.                                           |
|        |        |  Integer ( lo-hi ) width of the image in pixels.           |
|   14   |     2  |  Height of Image.                                          |
|        |        |  Integer ( lo-hi ) height of the image in pixels.          |
|   16   |     1  |  Image Pixel Size.                                         |
|        |        |  Number of bits in a pixel.  This is 16 for Targa 16,      |
|        |        |  24 for Targa 24, and .... well, you get the idea.         |
|   17   |     1  |  Image Descriptor Byte.                                    |
|        |        |  Bits 3-0 - number of attribute bits associated with each  |
|        |        |             pixel.  For the Targa 16, this would be 0 or   |
|        |        |             1.  For the Targa 24, it should be 0.  For the |
|        |        |             Targa 32, it should be 8.                      |
|        |        |  Bit 4    - reserved.  Must be set to 0.                   |
|        |        |  Bit 5    - screen origin bit.                             |
|        |        |             0 = Origin in lower left-hand corner.          |
|        |        |             1 = Origin in upper left-hand corner.          |
|        |        |             Must be 0 for Truevision images.               |
|        |        |  Bits 7-6 - Data storage interleaving flag.                |
|        |        |             00 = non-interleaved.                          |
|        |        |             01 = two-way (even/odd) interleaving.          |
|        |        |             10 = four way interleaving.                    |
|        |        |             11 = reserved.                                 |
|--------|--------|------------------------------------------------------------|
|   18   | varies |  Image Identification Field.                               |
|        |        |                                                            |
|        |        |  Contains a free-form identification field of the length   |
|        |        |  specified in byte 1 of the image record.  It's usually    |
|        |        |  omitted ( length in byte 1 = 0 ), but can be up to 255    |
|        |        |  characters.  If more identification information is        |
|        |        |  required, it can be stored after the image data.          |
|--------|--------|------------------------------------------------------------|
| varies | varies |  Color map data.                                           |
|        |        |                                                            |
|        |        |  The offset is determined by the size of the Image         |
|        |        |  Identification Field.  The length is determined by        |
|        |        |  the Color Map Specification, which describes the          |
|        |        |  size of each entry and the number of entries.             |
|        |        |  Each color map entry is 2, 3, or 4 bytes.                 |
|        |        |  Unused bits are assumed to specify attribute bits.        |
|        |        |  The 4 byte entry contains 1 byte for blue, 1 byte         |
|        |        |  for green, 1 byte for red, and 1 byte of attribute        |
|        |        |  information, in that order.                               |
|        |        |  The 3 byte entry contains 1 byte each of blue, green,     |
|        |        |  and red.                                                  |
|        |        |  The 2 byte entry is broken down as follows:               |
|        |        |  ARRRRRGG GGGBBBBB, where each letter represents a bit.    |
|        |        |  But, because of the lo-hi storage order, the first byte   |
|        |        |  coming from the file will actually be GGGBBBBB, and the   |
|        |        |  second will be ARRRRRGG. "A" represents an attribute bit. |
|--------|--------|------------------------------------------------------------|
| varies | varies |  Image Data Field.                                         |
|        |        |                                                            |
|        |        |  This field specifies (width) x (height) color map         |
|        |        |  indices.  The indices are stored in packets.  There       |
|        |        |  two types of packets:  Run-length packets, and Raw        |
|        |        |  packets.  Both types of packets consist of a 1-byte       |
|        |        |  header, identifying the type of packet and specifying a   |
|        |        |  count, followed by a variable-length body.                |
|        |        |  The high-order bit of the header is "1" for the           |
|        |        |  run length packet, and "0" for the raw packet.            |
|        |        |                                                            |
|        |        |  For the run-length packet, the header consists of:        |
|        |        |      __________________________________________________    |
|        |        |      | 1 bit |   7 bit repetition count minus 1.      |    |
|        |        |      |   ID  |   Since the maximum value of this      |    |
|        |        |      |       |   field is 127, the largest possible   |    |
|        |        |      |       |   run size would be 128.               |    |
|        |        |      |-------|----------------------------------------|    |
|        |        |      |   1   |  C     C     C     C     C     C    C  |    |
|        |        |      --------------------------------------------------    |
|        |        |                                                            |
|        |        |  For the raw packet, the header consists of:               |
|        |        |      __________________________________________________    |
|        |        |      | 1 bit |   7 bit number of pixels minus 1.      |    |
|        |        |      |   ID  |   Since the maximum value of this      |    |
|        |        |      |       |   field is 127, there can never be     |    |
|        |        |      |       |   more than 128 pixels per packet.     |    |
|        |        |      |-------|----------------------------------------|    |
|        |        |      |   0   |  N     N     N     N     N     N    N  |    |
|        |        |      --------------------------------------------------    |
|        |        |                                                            |
|        |        |  For the run length packet, the header is followed by      |
|        |        |  a single color index, which is assumed to be repeated     |
|        |        |  the number of times specified in the header.  The RLE     |
|        |        |  packet may cross scan lines ( begin on one line and end   |
|        |        |  on the next ).                                            |
|        |        |  For the raw packet, the header is followed by the number  |
|        |        |  of color indices specified in the header.  The raw        |
|        |        |  packet may cross scan lines ( begin on one line and end   |
|        |        |  on the next ).                                            |
--------------------------------------------------------------------------------
DATA TYPE 10: Run Length Encoded, RGB images
________________________________________________________________________________
| Offset | Length |                     Description                            |
|--------|--------|------------------------------------------------------------|
|    0   |     1  |  Number of Characters in Identification Field.             |
|        |        |                                                            |
|        |        |  This field is a one-byte unsigned integer, specifying     |
|        |        |  the length of the Image Identification Field.  Its range  |
|        |        |  is 0 to 255.  A value of 0 means that no Image            |
|        |        |  Identification Field is included.                         |
|--------|--------|------------------------------------------------------------|
|    1   |     1  |  Color Map Type.                                           |
|        |        |                                                            |
|        |        |  This field contains either 0 or 1.  0 means no color map  |
|        |        |  is included.  1 means a color map is included, but since  |
|        |        |  this is an unmapped image it is usually ignored.  TIPS    |
|        |        |  ( a Targa paint system ) will set the border color        |
|        |        |  the first map color if it is present.  Wowie zowie.       |
|--------|--------|------------------------------------------------------------|
|    2   |     1  |  Image Type Code.                                          |
|        |        |                                                            |
|        |        |  Binary 10 for this type of image.                         |
|--------|--------|------------------------------------------------------------|
|    3   |     5  |  Color Map Specification.                                  |
|        |        |                                                            |
|        |        |  Ignored if Color Map Type is 0; otherwise, interpreted    |
|        |        |  as follows:                                               |
|    3   |     2  |  Color Map Origin.                                         |
|        |        |  Integer ( lo-hi ) index of first color map entry.         |
|    5   |     2  |  Color Map Length.                                         |
|        |        |  Integer ( lo-hi ) count of color map entries.             |
|    7   |     1  |  Color Map Entry Size.                                     |
|        |        |  Number of bits in color map entry.  This value is 16 for  |
|        |        |  the Targa 16, 24 for the Targa 24, 32 for the Targa 32.   |
|--------|--------|------------------------------------------------------------|
|    8   |    10  |  Image Specification.                                      |
|        |        |                                                            |
|    8   |     2  |  X Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) X coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   10   |     2  |  Y Origin of Image.                                        |
|        |        |  Integer ( lo-hi ) Y coordinate of the lower left corner   |
|        |        |  of the image.                                             |
|   12   |     2  |  Width of Image.                                           |
|        |        |  Integer ( lo-hi ) width of the image in pixels.           |
|   14   |     2  |  Height of Image.                                          |
|        |        |  Integer ( lo-hi ) height of the image in pixels.          |
|   16   |     1  |  Image Pixel Size.                                         |
|        |        |  Number of bits in a pixel.  This is 16 for Targa 16,      |
|        |        |  24 for Targa 24, and .... well, you get the idea.         |
|   17   |     1  |  Image Descriptor Byte.                                    |
|        |        |  Bits 3-0 - number of attribute bits associated with each  |
|        |        |             pixel.  For the Targa 16, this would be 0 or   |
|        |        |             1.  For the Targa 24, it should be 0.  For the |
|        |        |             Targa 32, it should be 8.                      |
|        |        |  Bit 4    - reserved.  Must be set to 0.                   |
|        |        |  Bit 5    - screen origin bit.                             |
|        |        |             0 = Origin in lower left-hand corner.          |
|        |        |             1 = Origin in upper left-hand corner.          |
|        |        |             Must be 0 for Truevision images.               |
|        |        |  Bits 7-6 - Data storage interleaving flag.                |
|        |        |             00 = non-interleaved.                          |
|        |        |             01 = two-way (even/odd) interleaving.          |
|        |        |             10 = four way interleaving.                    |
|        |        |             11 = reserved.                                 |
|--------|--------|------------------------------------------------------------|
|   18   | varies |  Image Identification Field.                               |
|        |        |  Contains a free-form identification field of the length   |
|        |        |  specified in byte 1 of the image record.  It's usually    |
|        |        |  omitted ( length in byte 1 = 0 ), but can be up to 255    |
|        |        |  characters.  If more identification information is        |
|        |        |  required, it can be stored after the image data.          |
|--------|--------|------------------------------------------------------------|
| varies | varies |  Color map data.                                           |
|        |        |                                                            |
|        |        |  If the Color Map Type is 0, this field doesn't exist.     |
|        |        |  Otherwise, just read past it to get to the image.         |
|        |        |  The Color Map Specification, describes the size of each   |
|        |        |  entry, and the number of entries you'll have to skip.     |
|        |        |  Each color map entry is 2, 3, or 4 bytes.                 |
|--------|--------|------------------------------------------------------------|
| varies | varies |  Image Data Field.                                         |
|        |        |                                                            |
|        |        |  This field specifies (width) x (height) pixels.  The      |
|        |        |  RGB color information for the pixels is stored in         |
|        |        |  packets.  There are two types of packets:  Run-length     |
|        |        |  encoded packets, and raw packets.  Both have a 1-byte     |
|        |        |  header, identifying the type of packet and specifying a   |
|        |        |  count, followed by a variable-length body.                |
|        |        |  The high-order bit of the header is "1" for the           |
|        |        |  run length packet, and "0" for the raw packet.            |
|        |        |                                                            |
|        |        |  For the run-length packet, the header consists of:        |
|        |        |      __________________________________________________    |
|        |        |      | 1 bit |   7 bit repetition count minus 1.      |    |
|        |        |      |   ID  |   Since the maximum value of this      |    |
|        |        |      |       |   field is 127, the largest possible   |    |
|        |        |      |       |   run size would be 128.               |    |
|        |        |      |-------|----------------------------------------|    |
|        |        |      |   1   |  C     C     C     C     C     C    C  |    |
|        |        |      --------------------------------------------------    |
|        |        |                                                            |
|        |        |  For the raw packet, the header consists of:               |
|        |        |      __________________________________________________    |
|        |        |      | 1 bit |   7 bit number of pixels minus 1.      |    |
|        |        |      |   ID  |   Since the maximum value of this      |    |
|        |        |      |       |   field is 127, there can never be     |    |
|        |        |      |       |   more than 128 pixels per packet.     |    |
|        |        |      |-------|----------------------------------------|    |
|        |        |      |   0   |  N     N     N     N     N     N    N  |    |
|        |        |      --------------------------------------------------    |
|        |        |                                                            |
|        |        |  For the run length packet, the header is followed by      |
|        |        |  a single color value, which is assumed to be repeated     |
|        |        |  the number of times specified in the header.  The         |
|        |        |  packet may cross scan lines ( begin on one line and end   |
|        |        |  on the next ).                                            |
|        |        |  For the raw packet, the header is followed by             |
|        |        |  the number of color values specified in the header.       |
|        |        |  The color entries themselves are two bytes, three bytes,  |
|        |        |  or four bytes ( for Targa 16, 24, and 32 ), and are       |
|        |        |  broken down as follows:                                   |
|        |        |  The 2 byte entry -                                        |
|        |        |  ARRRRRGG GGGBBBBB, where each letter represents a bit.    |
|        |        |  But, because of the lo-hi storage order, the first byte   |
|        |        |  coming from the file will actually be GGGBBBBB, and the   |
|        |        |  second will be ARRRRRGG. "A" represents an attribute bit. |
|        |        |  The 3 byte entry contains 1 byte each of blue, green,     |
|        |        |  and red.                                                  |
|        |        |  The 4 byte entry contains 1 byte each of blue, green,     |
|        |        |  red, and attribute.  For faster speed (because of the     |
|        |        |  hardware of the Targa board itself), Targa 24 image are   |
|        |        |  sometimes stored as Targa 32 images.                      |
--------------------------------------------------------------------------------
*/

LAME_BEGIN

#pragma pack(push, 1)
typedef struct {
	char idlength;
	char colourmaptype;
	char datatypecode;
	short int colourmaporigin;
	short int colourmaplength;
	char colourmapdepth;
	short int x_origin;
	short int y_origin;
	short width;
	short height;
	char bitsperpixel;
	char imagedescriptor;
} TgaHeader;
#pragma pack(pop)

typedef struct {
	unsigned char r, g, b, a;
} TgaPixel;

Void __MergeBytes(TgaPixel* pixel, Uint8P p, Uint32 bytes) {

	if (bytes == 4) {
		pixel->r = p[2];
		pixel->g = p[1];
		pixel->b = p[0];
		pixel->a = p[3];
	}
	else if (bytes == 3) {
		pixel->r = p[2];
		pixel->g = p[1];
		pixel->b = p[0];
		pixel->a = 0;
	}
	else if (bytes == 2) {
		pixel->r = (p[1] & 0x7c) << 1;
		pixel->g = ((p[1] & 0x03) << 6) | ((p[0] & 0xe0) >> 2);
		pixel->b = (p[0] & 0x1f) << 3;
		pixel->a = (p[1] & 0x80);
	}
}

Bool TgaLoader::Load(StringC fileName) {

	File file;
	TgaHeader header;
	TgaPixel* pixels;
	Uint32 skipOver;
	Uint32 bytes2read;
	Uint32 i, j, n;
	Uint8 p[5];
    
    ResourceTextureInfoPtr texInfo = this->GetInfo();

	texInfo->BuildFileInfo(fileName);

	i = 0;
	j = 0;
	n = 0;
	skipOver = 0;

	file.Open(fileName, File::kRead | File::kBinary);
	file.Read(&header, sizeof(header));

	pixels = (TgaPixel*)malloc(header.width * header.height * sizeof(TgaPixel));

	if (header.datatypecode != 2 && header.datatypecode != 10) {
		PostErrorMessage("Can only handle image type 2 and 10", 1);
	}
	if (header.bitsperpixel != 16 && header.bitsperpixel != 24 && header.bitsperpixel != 32) {
		PostErrorMessage("Can only handle pixel depths of 16, 24, and 32", 1);
	}
	if (header.colourmaptype != 0 && header.colourmaptype != 1) {
		PostErrorMessage("Can only handle colour map types of 0 and 1", 1);
	}

	skipOver += header.idlength;
	skipOver += header.colourmaptype * header.colourmaplength;

	file.SetPosition(file.GetPosition() + skipOver);
	bytes2read = header.bitsperpixel / 8;

	while (n < (Uint32)header.width * header.height) {
		if (header.datatypecode == 2) {                     /* Uncompressed */
			if (fread(p, 1, bytes2read, (FILE*)file.GetHandle()) != bytes2read) {
				PostErrorMessage("Unexpected end of file at pixel %d\n", i);
			}
			__MergeBytes(&(pixels[n]), p, bytes2read);
			n++;
		}
		else if (header.datatypecode == 10) {             /* Compressed */
			if (fread(p, 1, bytes2read + 1, (FILE*)file.GetHandle()) != bytes2read + 1) {
				PostErrorMessage("Unexpected end of file at pixel %d\n", i);
			}
			j = p[0] & 0x7f;
			__MergeBytes(&(pixels[n]), &(p[1]), bytes2read);
			n++;
			if (p[0] & 0x80) {         /* RLE chunk */
				for (i = 0; i<j; i++) {
					__MergeBytes(&(pixels[n]), &(p[1]), bytes2read);
					n++;
				}
			}
			else {                   /* Normal chunk */
				for (i = 0; i<j; i++) {
					if (fread(p, 1, bytes2read, (FILE*)file.GetHandle()) != bytes2read) {
						PostErrorMessage("Unexpected end of file at pixel %d\n", i);
					}
					__MergeBytes(&(pixels[n]), p, bytes2read);
					n++;
				}
			}
		}
	}
	file.Close();

	texInfo->imageBits = header.bitsperpixel;
	texInfo->imageBytes = header.bitsperpixel / 8;
	texInfo->imageData = pixels;
	texInfo->imageHeight = header.height;
	texInfo->imageSize = header.height * header.width * header.bitsperpixel / 8;
	texInfo->imageWidth = header.width;

	return LAME_TRUE;
}

Bool TgaLoader::Save(StringC fileName) {

    ResourceTextureInfoPtr texInfo = this->GetInfo();
    
	TgaHeader header = {
		0, 0, 2, 0, 0, 0, 0, 0,
		static_cast<short>(texInfo->imageWidth),
		static_cast<short>(texInfo->imageHeight),
		static_cast<char>(texInfo->imageBits & 0xff), 8
	};

	File file;

	file.Open(fileName, File::kWrite | File::kBinary);
	file.Write(&header, sizeof(header));
	file.Write(texInfo->imageData,
		texInfo->imageWidth * texInfo->imageHeight * texInfo->imageBytes);
	file.Close();

	return LAME_TRUE;
}

LAME_END