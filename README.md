# HIRUNDO
## A Synthesizable MIPS processor
Described in C and can be Synthesized from HLS tool. The codes are tested on LegUp and Vivado HLS tools.

## Tool Required (anyone of the following)
- LegUp HLS (from University of Toronto, open source)
- Vivado HLS (from Xilinx)

## Generating ASIP
### Software simulation
Run an application or set of applications on the emulator. Before that modify the config.h file on the config subdirectory to enable the profiler. Remove the comment on #define PROFILE. It is give a rough estimation of most frequently used instructions. Depending on the profile results add instructions on the 'SUBLEQ' machine.

### Verification of SUBLEQ Routine
Remove the comment from the #define PRINT from the config.h and run the simulation to check the number of instructions and the dump the content of register file. 

### Using the Debugger
Each 'SUBLEQ' routine can be debugged by using the built in debugger. To enable the debugger, remove the comment from the each #define DEBUG_operation. In case of correct execution debugger will not report anything. For wrong calculation, input and output values will be printed.


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
