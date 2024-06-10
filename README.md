## Copyright TheUbMunster 2024, all rights reserved.

My attempt at a simple operating system with interrupts, a syscall system, a filesystem, a built in text editor and C compiler that hosts simple games (e.g., tetris).
I have a lot of ideas for where I can go with this, and it's likely that a lot of my other projects might be implemented in some form in this operating system.

![image of tetris from the vga output](https://raw.githubusercontent.com/TheUbMunster/mini-os/00b6e80486aa328db633d2fd5be5b165415d2f2c/readme%20resources/tetris%20screenshot.png?token=GHSAT0AAAAAACMBMJ4RKERKC2HJLQO2HQPIZTG5HNQ)   
*An early version of the tetris algorithm*

Inspired by the [x86 version of xv6](https://github.com/mit-pdos/xv6-public), and my college operating systems course.

## Todo
* Interrupt timer for delta-time based tetris inputs (as opposed to tuning busy waiting on a per-hardware basis).
* Better keyboard input (interrupt based).
* System/user modes.
* Filesystem.
* Better build system.
* [long mode/64-bit mode](https://wiki.osdev.org/Setting_Up_Long_Mode)