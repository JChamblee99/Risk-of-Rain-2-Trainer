IDIR = /usr/share/mingw-w64/include/
CC = x86_64-w64-mingw32-gcc
CFLAGS = -I $(IDIR)

all: clean dllmain.dll inject.exe

inject.exe: inject.cpp dllmain.dll
	$(CC) $(CFLAGS) -o inject.exe $<

dllmain.dll: dllmain.cpp
	$(CC) $(CFLAGS) -shared -o dllmain.dll $<

clean:
	$(RM) *.dll *.exe
