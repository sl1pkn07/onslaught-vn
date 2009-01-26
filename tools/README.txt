This package contains some basic tools needed to create working games. They are
backwards compatible with O/NScripter (except for recoder). I may decide later
on to add incompatible features, such as compression.

crypt
crypt can be used to unencrypt O/NScripter and ONSlaught script files. It uses
XOR-based encryption, which is symmetric. This means that the same code is used
for both encryption and decryption.
The original tool, nscmake, was crap. A 450 MHz took over twenty minutes to
process a 4 MiB script. crypt takes less than two seconds.
Run it with "crypt --help" for details on how to use it. xor84 encryption
works fine for most cases.

recoder
A simple encoding conversion tool. It can convert between the following
encodings: Shift JIS, UTF-8, UCS-2, and ISO-8859-1.
Run it with "recoder --help" for details on how to use it.
Notes on encodings:
UTF-8 conversion is limited to the range [U+0000;U+FFFF]. Code points above that
are converted to '?'. UTF-8 will work fine for most cases.
UCS-2 by definition can only encode [U+0000;U+FFFF]. UCS-2 has a slightly better
bytes-per-character ratio than UTF-8 for asian languages, so if your script
contains a lot of such characters, you might want to use it instead of UTF-8
ISO-8859-1 is simply the first 256 code points of Unicode encoded into a single
byte. It's slighly better data compactness-wise than UTF-8 for Western European
languages.
Shift JIS: An obsolete Japanese code page O/NScripter uses, only included here
for decoding purposes. It should not be used for new scripts.

nsaio
Compared to the other two tools, rather complex. It can extract, list, and
create both SAR and NSA archives. At the moment (2009-01-25) it supports LZSS
and NBZ (bzip2) de/compression. SPB is only supported for decompression.
If you find source code or an example of SPB decompression or you stumble upon
the sources of nsaarc.exe (a feat I have yet to accomplish) please send them to
me (helios.vmg@gmail.com).