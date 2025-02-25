CC = arm-none-eabi-gcc
CFLAGS = -Wall -mthumb -mcpu=cortex-m0plus
CPPFLAGS = -I ./includes
LDFLAGS = --specs=nano.specs -Wl,--gc-sections,-Map,main.map,-Tlink.ld

SRCS = startup.c main.c
OBJS = $(SRCS:.c=.o)

.PHONY: all compile compile-dbg run debug clean

all: compile run

compile: CFLAGS += -O2
compile: main.elf

compile-dbg: CFLAGS += -O0 -g3
compile-dbg: main.elf

main.elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run:
	openocd -f openocd.cfg -c "program main.elf verify reset exit"

debug:
	gdb-multiarch -ex "target extended-remote localhost:3333" main.elf

clean:
	rm -f *.o *.elf *.map
