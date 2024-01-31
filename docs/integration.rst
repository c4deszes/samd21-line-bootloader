Integrations
============

Concept
-------

Applications built on top of the bootloader must fulfill the following criterias:

* Used memory region must be appropriately offset from the bootloader
* Bootheader must be correctly set, including communication settings, addresses and CRCs
* RAM area must be appropriately placed, the application must be able to write the magic value to
  initiate bootloader entry

CMake
-----

The project can be included via CPM.CMake or as a submodule.
