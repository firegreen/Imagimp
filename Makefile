## LE MAKEFILE ULTIME ##

# Ce Makefile devrait marcher sous Linux, Windaube (MinGW) et OS X.
# Normalement il n'y a plus besoin d'y toucher :
# Créez juste vos fichiers dans src/imagimp et laissez-lui le sale boulot ;)
#
# Aussi, il n'y a pas lieu de changer la variable d'environnement
# LD_LIBRARY_PATH (ce que je trouve super intrusif).
# J'utilise l'option '-rpath' de 'ld' à la place, que je passe grace à
# l'option '-Wl' de GCC. Ca marche tout aussi bien.


SYS = $(shell gcc -dumpmachine)
ifneq (,$(findstring mingw,$(SYS)))
OS=windows
else
OS=unix
endif

.PHONY: all clean mrproper re dirs

CC = gcc
CFLAGS = -Wall -Iinclude -g -O2
LDLIBS = -fPIC -Llib -lglimagimp -lm
EXE = bin/imagimp$

ifeq ($(OS),windows)
	MKDIRP=mkdir
	DLL_PREFIX=
	DLL_EXTENSION=.dll
	LIBGL=-lopengl32 -lglu32 -lfreeglut
	CLEANCMD = if exist obj ( rmdir /Q /S obj lib )
	COPYCMD = xcopy /y /i
else
	LDLIBS+=-Wl,-rpath=lib -Wl,-rpath=../lib
	MKDIRP=mkdir -p
	DLL_PREFIX=lib
	DLL_EXTENSION=.so
	LIBGL=-lGL -lGLU -lglut
	CLEANCMD = rm -rf obj lib
	COPYCMD = cp
endif

LIBGLIMAGIMP = lib/$(DLL_PREFIX)glimagimp$(DLL_EXTENSION)


all: $(LIBGLIMAGIMP) $(EXE)

dirs: bin lib obj/glimagimp obj/imagimp
obj:
	$(MKDIRP) "$@"
obj/glimagimp: | obj
	$(MKDIRP) "$@"
obj/imagimp: | obj
	$(MKDIRP) "$@"
lib:
	$(MKDIRP) "$@"
bin:
	$(MKDIRP) "$@"


obj/imagimp/%.o : src/imagimp/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(addsuffix .o, \
			$(addprefix obj/imagimp/, \
				$(notdir \
					$(basename \
						$(wildcard src/imagimp/*.c)))))
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(LIBGL)

obj/glimagimp/%.o: src/glimagimp/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBGLIMAGIMP): obj/glimagimp/interface.o \
			     obj/glimagimp/outils.o
	$(CC) $(CFLAGS) -shared $^ -o $@ $(LIBGL) -lm
	$(COPYCMD) "$(@D)" "bin"
clean:
	$(CLEANCMD)
mrproper: clean all
re: mrproper
