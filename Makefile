#IF DEBUG
DBGOPTIONS = $(DBGOPTIONS) -D_DEBUG
#ENDIF

all: all-linux windows-all

all-linux: stub test

test: binder.o test.c
	gcc -Wall test.c Obj/Linux/binder.o -o binder

binder.o: binder.c
	gcc -c binder.c -o Obj/Linux/binder.o -D_DEBUG

stub: stub.o binder.o
	gcc -Wall -o stub Obj/Linux/binder.o Obj/Linux/stub.o

stub.o:
	gcc -c stub.c -o Obj/Linux/stub.o

clean:
	rm Obj/Linux/stub.o Obj/Linux/binder.o binder binder.exe stub.exe stub

windows-all: windows-stub windows-test

windows-test: binder.o test.c
	x86_64-w64-mingw32-gcc -Wall test.c Obj/Windows/binder.o -o binder.exe

windows-binder.o: binder.c
	x86_64-w64-mingw32-gcc -c binder.c -o Obj/Windows/binder.o -D_DEBUG

windows-stub: windows-binder.o windows-stub.o
	x86_64-w64-mingw32-gcc -Wall -o stub.exe Obj/Windows/binder.o Obj/Windows/stub.o

windows-stub.o: stub.c
	x86_64-w64-mingw32-gcc -c stub.c -o Obj/Windows/stub.o
