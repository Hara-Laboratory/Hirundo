#!/bin/bash

CC=gcc
#CFLAGS=ggdb -Og -Wall
CFLAGS=( -ggdb -O2 -Wall )
OBJS=( ./src/faultInjector.o \
	./src/listMem.o \
	./src/listTrace.o \
	#./src/list.c 
	)

EXPERIMENT_DIR=experiment
SRC=src/emulator.c
EXE_PREFIX=${EXPERIMENT_DIR}/emulator
# BENCHMARKS=( adpcm bf bs bubble crc fibcall gsm intmm insertsort jfdctint mpeg2 vec_add )
BENCHMARKS=( insertsort vec_add )
STUCKATS=( 0 1 )

function do_benchmark () {
	benchmark=$1
	stuckat=$2

	exe_name=${EXE_PREFIX}-$benchmark-$stuckat
	./enable-features.sh ${SRC} ${exe_name}.c benchmark-${benchmark} stuck-at-${stuckat}
	${CC} "${CFLAGS[@]}" -o "${exe_name}" "${exe_name}.c" "${OBJS[@]}"
	output_name=${EXPERIMENT_DIR}/output-${benchmark}-${stuckat}
	${exe_name} ${output_name}.dat | tee ${output_name}.log
}

make || exit 1
[ -d "${EXPERIMENT_DIR}" ] || mkdir ${EXPERIMENT_DIR}

for benchmark in "${BENCHMARKS[@]}" ; do
for stuckat in "${STUCKATS[@]}" ; do
	do_benchmark $benchmark $stuckat &
done
done

wait
