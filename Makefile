# uBarcode Makefile
INCLUDES=-I include -I libpng
INDENT_FLAGS=-br -ce -i4 -bl -bli0 -bls -c4 -cdw -ci4 -cs -nbfda -l100 -lp -prs -nlp -nut -nbfde -npsl -nss

OBJS = \
	bin/main.o \
	bin/plot.o \
	bin/util.o \
	bin/export.o \
	bin/png.o \
	bin/code.o \
	bin/code128.o \
	bin/code39.o \
	bin/codabar.o \
	bin/code93.o

all: host

internal: prepare
	@echo "  CC    src/main.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/main.c -o bin/main.o
	@echo "  CC    src/plot.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/plot.c -o bin/plot.o
	@echo "  CC    src/util.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/util.c -o bin/util.o
	@echo "  CC    src/export.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/export.c -o bin/export.o
	@echo "  CC    src/png.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/png.c -o bin/png.o
	@echo "  CC    src/code.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/code.c -o bin/code.o
	@echo "  CC    src/code128.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/code128.c -o bin/code128.o
	@echo "  CC    src/code39.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/code39.c -o bin/code39.o
	@echo "  CC    src/codabar.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/codabar.c -o bin/codabar.o
	@echo "  CC    src/code93.c"
	@$(CC) $(CFLAGS) $(INCLUDES) src/code93.c -o bin/code93.o
	@echo "  LD    bin/ubarcode"
	@$(LD) -o bin/ubarcode $(OBJS) $(LDFLAGS)

prepare:
	@mkdir -p bin

verbose:
	@make $(TARGET) EXTRA=-DVERBOSE_MODE

host:
	@make internal \
		CC=gcc \
		LD=gcc \
		CFLAGS='-c -Wall -Wextra -O2 -ffunction-sections -fdata-sections -Wstrict-prototypes' \
		LDFLAGS='-s -Wl,--gc-sections -Wl,--relax -lpng -lm -lz'

mipsel:
	@make internal \
		CC=mips-unknown-linux-gnu-gcc \
		LD=mips-unknown-linux-gnu-gcc \
		CFLAGS='-c $(MIPSEL_CFLAGS) -I $(ESLIB_INC) -O2 -EL' \
		LDFLAGS='$(MIPSEL_LDFLAGS) -L $(ESLIB_DIR) -les-mipsel-Os -EL'

mipseb:
	@make internal \
		CC=mips-unknown-linux-gnu-gcc \
		LD=mips-unknown-linux-gnu-gcc \
		CFLAGS='-c $(MIPSEB_CFLAGS) -I $(ESLIB_INC) -O2 -EB' \
		LDFLAGS='$(MIPSEB_LDFLAGS) -L $(ESLIB_DIR) -les-mipseb-Os -EB'

arm:
	@make internal \
		CC=arm-linux-gnueabi-gcc \
		LD=arm-linux-gnueabi-gcc \
		CFLAGS='-c $(ARM_CFLAGS) -I $(ESLIB_INC) -O2' \
		LDFLAGS='$(ARM_LDFLAGS) -L $(ESLIB_DIR) -les-arm'

install:
	@cp -v bin/ubarcode /usr/bin/ubarcode

uninstall:
	@rm -fv /usr/bin/ubarcode

post:
	@echo "  STRIP ubarcode"
	@sstrip bin/ubarcode
	@echo "  UPX   ubarcode"
	@upx bin/ubarcode
	@echo "  LCK   ubarcode"
	@perl -pi -e 's/UPX!/EsNf/g' bin/ubarcode
	@echo "  AEM   ubarcode"
	@nogdb bin/ubarcode

post2:
	@echo "  STRIP ubarcode"
	@sstrip bin/ubarcode
	@echo "  AEM   ubarcode"
	@nogdb bin/ubarcode

indent:
	@indent $(INDENT_FLAGS) ./*/*.h
	@indent $(INDENT_FLAGS) ./*/*.c
	@rm -rf ./*/*~

clean:
	@echo "  CLEAN ."
	@rm -rf bin

analysis:
	@scan-build make
	@cppcheck --force */*.h
	@cppcheck --force */*.c

gendoc:
	@doxygen aux/doxygen.conf
