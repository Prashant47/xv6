rm ./cc/unistd.o ./cc/string.o
ld -N -e main -Ttext 0 -o _picoc ./cc/*.o ulib.o usys.o printf.o umalloc.o libfile.o pow.o exp.o liblog.o 2>output
cp _picoc test1
cat output


