ld -N -e main -Ttext 0 -o tcc ./tcc-bin/*.o ulib.o usys.o printf.o umalloc.o libfile.o 2>output
cat output


