# 🥜 nutsh 🥜
An elementary Unix shell, for educational purposes.

https://github.com/SeanFxyz/nutsh

Written by Sean M Fitzgerald

sean_m_fitzgerald@tutanota.com

## Compiling
After acquiring the source code, enter the source code directory and simply run the `make` command.

## Installing
Run `make install` with root privileges to copy the `nutsh` binary to /usr/bin/

## Features

At present, nutsh will run any program in your `$PATH` as well as four builtin commands:
* help: displays builtin commands
* cd: changes the current directory
* pwd: prints the current directory
* exit: terminates the shell

Pipes from one program to another are also supported, but are not compatible with builtin commands.
