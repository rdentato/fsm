#  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
#  SPDX-License-Identifier: MIT

SRC=../src
DIST=../dist

CFLAGS=-I ../src -O2 -Wall

all: uncomment numbers

uncomment: uncomment.o
numbers: numbers.o

%.png:%.dot
	dot -Tpng $< -o $*.png

clean:
	rm -f *.o uncomment numbers
