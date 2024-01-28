Cyclic redundancy check
=======================

There are a total of 3 CRC values that are checked by the bootloader:

* Bootloader CRC, found at the end of the BL ROM area
* Bootheader CRC, found at the end of the header
* Application CRC, found in the header

References
----------

`SRecord CRC32 algorithm <https://github.com/BrianAker/srecord/blob/master/srecord/crc32.cc>`_

`CRC32 algorithms and LUTs <https://users.ece.cmu.edu/~koopman/crc/crc32.html>`_

`CRC32 algorithm in Python <https://gist.github.com/Lauszus/6c787a3bc26fea6e842dfb8296ebd630>`_

`ELF File editing tool <https://sourceware.org/git/?p=elfutils.git;a=summary>`_
