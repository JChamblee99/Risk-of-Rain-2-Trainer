IDIR = /usr/share/mingw-w64/include/
CC = x86_64-w64-mingw32-g++
CFLAGS = -I $(IDIR)

all: clean dllmain.dll inject.exe

inject.exe: inject.cpp dllmain.dll
	$(CC) $(CFLAGS) -o $@ $<

dllmain.dll: dllmain.cpp
	$(CC) $(CFLAGS) -shared -o $@ $<

clean:
	$(RM) *.dll *.exe
