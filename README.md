# HIRUNDO
## A Synthesizable MIPS processor
Described in C and can be Synthesized from HLS tool

## Dependencies
- libelf
- Haskell-Platform

## Installation
### Getting Files
Firstly download and extract the archive or clone the repository.

```sh
# clone the repository
$ git clone https://github.com/Hara-Laboratory/Hirundo.git
$ git submodule init
$ git submodule update
```

### Install Elf2Mem
Build `elf2mem` and copy it to a directory, which is exists in `$PATH`.
```sh
$ cd elf2mem
$ make
$ cp elf2mem /path/to/the/dir # usualy ~/bin or /usr/local/bin
$ cd ..
```

### Install Subleq-Toolchain
Build `subleq-toolchain` and install it.
```sh
$ cd subleq-toolchain
$ cabal install
$ cd ..
```
