Ctrl2cap Keyboard Filter
Copyright (C) 1996 Mark Russinovich

To install:

	- Copy ctrl2cap.sys to your winnt\system32\drivers directory
	- Type 'Regedit ctr2lcap.reg'
	- Reboot

To build:
	- Copy ddk\src\input\inc\ntddkbd.h to this directory
	- Build with standard DDK tools
