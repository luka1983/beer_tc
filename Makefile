MAKE    = make
MV		= mv
RM		= rm

SRC     = ./src
BIN		= ./bin
AVRDUDE	= avrdude

all:
	$(MAKE) -C $(SRC) all
	$(RM) $(SRC)/*.o $(SRC)/*.elf $(SRC)/*.map
	$(MV) $(SRC)/*.hex $(BIN)/

up:
	$(AVRDUDE) -c usbasp -p m328p -u -U flash:w:$(BIN)/beer_tc.hex

clean:
	$(RM) $(BIN)/*