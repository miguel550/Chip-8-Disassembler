#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
using namespace std;

void disassemble_chip8op(uint8_t *code_buffer, uint32_t pc){
	uint8_t *code = &code_buffer[pc];
	uint8_t first_inst_lastbit = (code[0] >> 4);
	//printf("%04X %02X %02X ", pc, code[0], code[1]);
	switch(first_inst_lastbit){
		case 0x00:{
			if(code[1] & 0x0E) printf("RTS");
			else printf("CLS");
		} break;
		case 0x01:{
			printf("%-10s $%x%2X", "JUMP", code[0] & 0x0F, code[1]);
		} break;
		case 0x02: {
			printf("%-10s $%x%2X", "CALL", code[0] & 0x0F, code[1]);
		}break;
		case 0x03:{
			uint8_t reg = (code[0] & 0x0F);
			printf("%-10s V%01X, #$%02X", "SKIPEQ", reg, code[1]);
		} break;
		case 0x04:{
			uint8_t reg = (code[0] & 0x0F);
			printf("%-10s V%01X, #$%02X", "SKIPNE", reg, code[1]);
		} break;
		case 0x05:{
			uint8_t reg_from = (code[0] & 0x0F);
			uint8_t reg_to  = (code[1] & 0x0F);
			printf("%-10s V%01X, V%01X", "SKIPEQ", reg_from, reg_to);
		} break;
		case 0x06:{
			uint8_t reg = (code[0] & 0x0F);
			printf("%-10s V%01X, #$%02X", "MVI", reg, code[1]);
		} break;
		case 0x07:{
			printf("%-10s V%01X, #$%02x", "ADI", code[0] & 0xf, code[1]);
		} break;
		case 0x08:{
			string noms[9] = {"MOV", "OR", "AND", "XOR", "CADD", "CSUB", "CSHR", "CSUBB", "CSHL"};
			uint8_t reg_from = (code[0] & 0x0F);
			uint8_t reg_to  = (code[1] & 0xF0) >> 0x04;
			uint8_t firstnbit = code[1] & 0x0F;
			switch(firstnbit){
				case 0x00:
				case 0x01:
				case 0x02:
				case 0x03:
				case 0x04:
				case 0x05: {
					printf("%-10s V%01X, V%01X", noms[firstnbit].c_str(), reg_from, reg_to);
				} break;
				case 0x06:{
					printf("%-10s V%01X", noms[firstnbit].c_str(), reg_from);
				} break;
				case 0x07: {
					printf("%-10s V%01X, V%01X", noms[firstnbit].c_str(), reg_from, reg_to);
				}break;
				case 0x0E:{
					printf("%-10s V%01X", noms[8].c_str(), reg_from);
				} break;
			}
		} break;
		case 0x09:{
			uint8_t reg_from = (code[0] & 0x0F);
			uint8_t reg_to  = (code[1] & 0x0F);
			printf("%-10s V%01X, V%01X", "SKIPNE", reg_from, reg_to);
		} break;
		case 0x0A:{
			uint8_t addresshi = (code[0] & 0x0F);
            printf("%-10s I, #$%01x%02x", "MVI", addresshi, code[1]);
		} break;
		case 0x0B:{
			printf("%-10s $%x%2X(V0)", "JUMP", code[0] & 0x0F, code[1]);
		} break;
		case 0x0C:{
			printf("%-10s V%01X, #$%02X", "RNDMSK", code[0] & 0x0F, code[1]);
		} break;
		case 0x0D:{
			uint8_t reg_x = (code[0] & 0x0F);
			uint8_t reg_y  = (code[1] & 0xF0) >> 0x04;
			uint8_t val_n = (code[1] & 0x0F);
			printf("%-10s V%01X, V%01X, #$%01X", "SPRITE", reg_x, reg_y, val_n);
		} break;
		case 0x0E:{
			uint8_t reg_x  = (code[0] & 0x0F);
			if(code[1] & 0xA1)
				printf("%-10s V%01X", "SKIPNOKEY", reg_x);
			else
				printf("%-10s V%01X", "SKIPKEY", reg_x);
		} break;
		case 0x0F: {
			uint8_t reg_x  = (code[0] & 0x0F);
			switch(code[1]){
				case 0x07:{
					printf("%-10s V%01X, DELAY", "MOV", reg_x);
				}break;
				case 0x0A:{
					printf("%-10s V%01X", "WAITKEY", reg_x);
				}break;
				case 0x15:{
					printf("%-10s DELAY, V%01X", "MOV", reg_x);
				}break;
				case 0x18:{
					printf("%-10s SOUND, V%01X", "MOV", reg_x);
				}break;
				case 0x1E:{
					printf("%-10s I, V%01X", "ADD", reg_x);
				}break;
				case 0x29:{
					printf("%-10s V%01X", "SPRITECHAR", reg_x);
				}break;
				case 0x33:{
					printf("%-10s V%01X", "MOVBCD", reg_x);
				}break;
				case 0x55:{
					printf("%-10s (I), V0-V%01X", "MOVM", reg_x);
				}break;
				case 0x65:{
					printf("%-10s V0-V%01X, (I)", "MOVM", reg_x);
				}break;
			}
		}break;
	}
}

int main(int argc, char** argv) {
	if(argc > 1){
 		FILE *rom = fopen(argv[1], "rb");
  		if (not rom) {
      		printf("Error: Couldn't open %s\n", argv[1]);
      		return 1;
  		}
  
  		//Get the file size
  		fseek(rom, 0L, SEEK_END);
  		uint32_t rom_sz = ftell(rom);
  		fseek(rom, 0L, SEEK_SET);

  		//CHIP-8 convention puts programs in memory at 0x200
  		// They will all have hardcoded addresses expecting that
  		//
  		//Read the file into memory at 0x200 and close it.
  		uint8_t *buffer = (uint8_t*) malloc(rom_sz + 0x200);
  		fread(buffer+0x200, rom_sz, 1, rom);
  		fclose(rom);

  		uint32_t pc = 0x200;
  		//printf("PC  INT VAL\n");
  		while (pc < (rom_sz + 0x200)) {
      		disassemble_chip8op(buffer, pc);
      		pc += 0x02;
      		printf(";");
      		printf ("\n");
  		}
	}
	return 0;
}
