Cyclic redundancy check
=======================

There are a total of 3 CRC values that are checked by the bootloader:

* Bootloader CRC, found at the end of the BL ROM area
* Bootheader CRC, found at the end of the header
* Application CRC, found in the header

Algorithm
---------

Below is the pseudocode for CRC32 calculation given the polynom ``0xEDB88320`` and an initial value
of ``0xFFFFFFFF``.

.. code-block:: text

    function (data: bytearray) => integer {
        calculated = 0xffffffff
        for byte in data:
            calculated ^= byte
            for _ in range(8):
                calculated = (calculated >> 1) ^ 0xedb88320 if calculated & 1 else calculated >> 1
        return calculated ^ 0xffffffff
    }

References
----------

`SRecord CRC32 algorithm <https://github.com/BrianAker/srecord/blob/master/srecord/crc32.cc>`_

`CRC32 algorithms and LUTs <https://users.ece.cmu.edu/~koopman/crc/crc32.html>`_

`CRC32 algorithm in Python <https://gist.github.com/Lauszus/6c787a3bc26fea6e842dfb8296ebd630>`_

`ELF File editing tool <https://sourceware.org/git/?p=elfutils.git;a=summary>`_
