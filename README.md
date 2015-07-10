# smallHex
A hex editor for PS Vita and Windows.

# why smallHex
Not a special reason. There are better hex editors, but I decided to do it in order to do something highly portable and dynamic, usable virtually by any device.
Also, I released in 2008 a version of smallHex for PSP: that was my first homebrew. Thanks to PS Vita hacking scene, I decided to rewrite it completely also for this system. In 7 years my programming skills improved, so why not write a 2.0 / remake version?

# how to compile
### Windows:
I'm using Visual Studio 2013 to build it, but it should works with any MSVC and MinGW compiler.
### PS Vita
Install [devkitPro](http://devkitpro.org/) + [psp2sdk](https://github.com/173210/psp2sdk) . Set psp2sdk as variable path, then simply launch 'make'.

# how to use
### smallHex
* Currently smallHex works in read-only mode.
* You can use the arrow of your keyboard, digital or analog sticks to move through bytes. PgUp/PgDown or LTrigger/RTrigger can be used to move through pages very fast.
* Press select to show file manager
* Press start + select to exit
### file manager
* Use digital/analog sticks and PgUp/PgDown or LTrigger/RTrigger to move through file list
* Press left to go to previous directory
* Press right or cross to enter to selected directory
* Press cross to open selected file
* Press select to change through devices

# features
* File explorer ~~with file/directory creation, deletion and edit~~
* Custom fonts for carriage-return and new-line.
* Rendering on-demand to save CPU cycles (and power)

# to-do
### smallHex
* Hex editing
* Char editing
* Byte finder
* Data viewer
* String list
* Settings menu
### file manager
* File creation
* File truncation
* File deletion, renaming, change access privileges, copy, move
* Directory creation, deletion, renaming, access privileges, copy, move
