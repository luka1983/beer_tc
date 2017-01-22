MAKE    = make
MV		= mv
RM		= rm
MKDIR	= mkdir -p

SRC     = ./src
BIN		= ./bin
AVRDUDE	= avrdude

all:
	$(MAKE) -C $(SRC) all
	$(RM) $(SRC)/*.o $(SRC)/*.elf $(SRC)/*.map
	$(MKDIR) $(BIN)
	$(MV) $(SRC)/*.hex $(BIN)/

up:
	$(AVRDUDE) -c usbasp -p m328p -u -U flash:w:$(BIN)/beer_tc.hex

clean:
	$(RM) $(BIN)/*