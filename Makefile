CC = gcc
CFLAGS = -g -shared -fPIC

ifndef VERBOSE
	SILENT = @
else
	SILENT = 
endif

INCLUDE = stdlibx/include
TESTS = $(wildcard tests/*.c)
SOURCES = $(wildcard stdlibx/src/*.c)

all: out_dir stdlibx tests

out_dir:
	$(SILENT) mkdir -p bin

stdlibx: $(SOURCES)
	$(SILENT) $(CC) $(CFLAGS) $^ -I$(INCLUDE) -o bin/stdlibx.dll

tests: $(TESTS)
	$(SILENT) for test in $^; do											\
		out_name=$$(basename $$test .c); 									\
		$(CC) $$test -I$(INCLUDE) -Lbin -lstdlibx -o bin/$$out_name.exe; 	\
	done
