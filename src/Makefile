# CPU clock in MHz
FCPU		= 16000000UL

PRG			= beer_tc
OBJS		= beer_tc.o tcontrol.o serial.o commands.o

MCU_TARGET	= atmega328p

OPTIMIZE	= -O2
DEFS		= -DF_CPU=$(FCPU)

# You should not have to change anything below here.
CC			= avr-gcc

# Override is only needed by avr-lib build system.
override CFLAGS		= -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS	= -Wl,-Map,$(PRG).map

OBJCOPY		= avr-objcopy

all: $(PRG).elf text
$(PRG).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# dependency:
beer_tc.o: beer_tc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c beer_tc.c

tcontrol.o: tcontrol.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c tcontrol.c

serial.o: serial.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c serial.c

commands.o: commands.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c commands.c

clean:
	rm -rf *.o $(PRG).elf $(PRG).hex *.bak 
	rm -rf *.map

# Rules for building the .text rom images
text: hex
hex:  $(PRG).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
