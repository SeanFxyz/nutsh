# 🥜 nutsh 🥜
An elementary Unix shell, for educational purposes.

https://github.com/SeanFxyz/nutsh

Written by Sean M Fitzgerald

sean_m_fitzgerald@tutanota.com

## Prerequisites
You must have a C compiler accessible via the `cc` command, and the POSIX standard libraries.

## Compiling
You can get the code by downloading a zip file from the GitHub page or by cloning the repository:
```
$ git clone git@github.com:SeanFxyz/nutsh.git
```

After acquiring the source code, enter the source code directory and simply run the `make` command:
```
$ cd nutsh
$ make
```
You can also run `make debug` to compile a version with debug symbols enabled for use with `gdb` or similar debugging tools.

## Installing
You can install nutsh to your system by running `make install` with root privileges to copy the `nutsh` binary to `/usr/bin/`

If you don't have root privileges, you can also copy the binary to `~/.local/bin/`, which is likely to be in your path, and can easily be added if it isn't already.

## Features

### Commands
At present, nutsh will run any program in your `$PATH` as well as four builtin commands:
* help: displays builtin commands
* cd: changes the current directory
* pwd: prints the current directory
* exit: terminates the shell

### Pipes

Pipes from one program to another are supported, but are not compatible with builtin commands.

To pipe from one program to another, type a command that outputs to stdout, like `ls`, then a pipe `|` character with **with spaces on either side**, then another command that reads from stdin, like grep, all on the same line, like this:
```
🥜$ ls Movies/ | grep -i nut
Nutcracker.mp4
The Nut Job.mp4
The Nutty Professor.mp4
🥜$
```
You should be able to chain as many commands as you like this way:
```
🥜$ ls Movies/ | grep -i nut | cowsay -n
 _________________________
/ Nutcracker.mp4          \
| The Nut Job.mp4         |
\ The Nutty Professor.mp4 /
 -------------------------
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
🥜$
```
