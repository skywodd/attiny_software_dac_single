DEVICE = attiny85
AVRDUDE = avrdude -c usbtiny -p $(DEVICE)

OUTPUT_NAME = softADC
OBJECTS = soft_dac.c main.c

COMPILE = avr-gcc -Wall -Os -I. -mmcu=$(DEVICE) -DF_CPU=16000000 -DDEBUG_LEVEL=0 -ffunction-sections -Wl,-gc-sections
SIZE = avr-size --mcu=$(DEVICE)


all: $(OBJECTS) clean
	$(COMPILE) -o $(OUTPUT_NAME).elf $(OBJECTS)
	avr-objcopy -j .text -j .data -O ihex $(OUTPUT_NAME).elf $(OUTPUT_NAME).hex
	$(SIZE) $(OUTPUT_NAME).hex

flash:
	$(AVRDUDE) -U flash:w:$(OUTPUT_NAME).hex

# ATtiny85 Fuses : 
# PLL Clock; Start-up time PWRDWN/RESET: 1K CK/14 CK + 4 ms; [CKSEL=0001 SUT=00]
# Brown-out detection level at VCC=4.3 V; [BODLEVEL=100]
# Preserve EEPROM memory through the Chip Erase cycle; [EESAVE=0]
# Serial program downloading (SPI) enabled; [SPIEN=0]
fuse:
	$(AVRDUDE) -U lfuse:w:0xc1:m -U hfuse:w:0xd4:m -U efuse:w:0xff:m

clean:
	rm -f *.o *.elf *.hex
