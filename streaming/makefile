# Software Name
PROGRAM = streaming

# Compiler
CC = /root/git/buildroot-rg350-old-kernel/output/host/usr/bin/mipsel-rg350-linux-uclibc-gcc

# You can use Ofast too but it can be more prone to bugs, careful.
CFLAGS = -O2 -fdata-sections -ffunction-sections -fno-PIC -flto -Wall -Wextra -std=gnu99 -fPIE

CFLAGS +=  -I.

LIBS +=
LDFLAGS = -Wl,--start-group $(LIBS) -Wl,--end-group -Wl,--as-needed -Wl,--gc-sections -flto

ifeq ($(DEBUG), YES)
CFLAGS +=  -DDEBUG -g3
else
LDFLAGS	+=  -s
endif

CFILES = streaming.c lz4.c
SFILES =

OFILES = $(SFILES:.S=.o) $(CFILES:.c=.o)

$(PROGRAM):	$(OFILES)
			$(CC) $(CFLAGS) $(OFILES) -o $@ $(LDFLAGS)

all: $(PROGRAM)

%.o: %.c
	 $(CC) $(CFLAGS) -c $< -o $@

clean:
	 -rm -f $(OFILES) $(MAPFILE) $(PROGRAM)
