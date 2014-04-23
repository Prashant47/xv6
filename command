ld -N -e main -Ttext 0 -o a tcc/tcc.o tcc/libtcc.o tcc/tccelf.o tcc/tccgen.o tcc/tccpp.o tcc/i386-asm.o tcc/i386-gen.o tcc/bcheck.o tcc/tccasm.o ulib.o usys.o printf.o umalloc.o libfile.o 2>output

