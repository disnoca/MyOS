# MyOS

This is my personal OS project.

### Installing dependencies
```sudo apt install build-essential libc6-i386 mtools nasm xorriso qemu qemu-system-i386 bochs bochs-sdl bochsbios vgabios```

Cross-Compiler: https://github.com/lordmilko/i686-elf-tools

At the time of writing, the bochsbios package has a bug in Ubuntu that causes bochs not to work. In that case replace /usr/share/bochs/BIOS-bochs-latest with the one from https://bochs.sourceforge.io/cgi-bin/lxr/source/bios/

Don't forget to add the cross-compiler's bin folder to the PATH environment variable, as well as add execute permission to all the files in the cross-compiler folder and the shell scripts and Makefiles in the project.
