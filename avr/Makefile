CC = avr-gcc
OBJS = COP421Reader.o serial.o crc.o

MCU = -mmcu=atmega644p
ELF = COP421Reader.elf
HEX = COP421Reader.hex
BIN = COP421Reader.bin

CFLAGS = ${MCU} -Wall -gdwarf-2 -std=gnu99 -DF_CPU=18432000UL -O3 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums

COP421Reader: ${OBJS}
	${CC} ${MCU} -Wl,-Map=COP421Reader.map ${OBJS} -o ${ELF}
	avr-objcopy -O ihex -R .eeprom -R .lock -R .signature ${ELF} ${HEX}
	avr-objcopy -I ihex ${HEX} -O binary ${BIN}

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm -f ${OBJS} ${ELF} ${HEX} ${BIN} COP421Reader.map
