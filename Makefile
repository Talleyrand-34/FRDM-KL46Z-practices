# Variables propias
CC      := arm-none-eabi-gcc
CFLAGS  := -I ./includes -Wall -mthumb -mcpu=cortex-m0plus
OPT     := -O2
DEBUG   := -O0 -g3
LDFLAGS := --specs=nano.specs -Wl,--gc-sections,-Map,main.map,-Tlink.ld

# Arquivos e destino
SRCS    := startup.c main.c
OBJS    := $(SRCS:.c=.o)
TARGET  := main.elf

# Regras phony
.PHONY: all compile compile-dbg flash run debug clean cleanall

# Compilación normal (usa regras implícitas para .c -> .o)
all: compile

compile: CFLAGS += $(OPT)
compile: $(TARGET)

compile-dbg: CFLAGS += $(DEBUG)
compile-dbg: $(TARGET)

# Única regra explícita de linkado
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Flashear no dispositivo
flash: $(TARGET)
	openocd -f openocd.cfg -c "program $< verify reset exit"

# Executar programa en OpenOCD
run: $(TARGET)
	openocd -f openocd.cfg -c "program $< verify reset exit"

# Debug con gdb
debug: $(TARGET)
	gdb -ex "target extended-remote localhost:3333" $<

# Limpeza parcial
clean:
	rm -f *.o *.map

# Limpeza total (tamén o binario final)
cleanall: clean
	rm -f $(TARGET)
