#include "ioport.h"
#include "setup_serial.h"

void setup_serial() {
	out8(0x3f8 + 3, 0x83);//0b10000011); // Enabling division coefficient in 0 and 1 ports
	out8(0x3f8 + 0, 0x18); // Just a valid value for the coefficient
	out8(0x3f8 + 1, 0x00);	
	
	out8(0x3f8 + 3, 3);
	out8(0x3f8 + 1, 0x00);//0b00000000); // Interrupt Enable Register
}

void print(char* string) {
	int i = 0;
	while(string[i] != 0) {
		if((in8(0x3f + 5)) & (1 << 5)) {
			out8(0x3f8 + 0, string[i]);
			++i;
		}
	}
	while(!((in8(0x3f8 + 5)) & (1 << 5))) {
		continue;
	}
//	out8(0x3f8 + 0, '\n');
}

void println(char* string) {
    print(string);
    print("\n");
}

void print_llu(uint64_t x) {
    if(x == 0) {
        print("0");
        return;
    }

    char str[30];
    char res[30];
    int i = 0;
    while(x > 0) {
        str[i] = x % 10 + '0';
        x /= 10;
        ++i;
    }
    for(int j = 0; j < i; j++) {
        res[j] = str[i - j - 1];
    }
    res[i] = 0;
    print(res);
}

void print_llu_x(uint64_t x) {
    print("0x");

    if(x == 0) {
        print("00");
        return;
    }

    char str[30];
    char res[30];
    int i = 0;
    while(x > 0) {
        if(x % 16 < 10) {
            str[i] = x % 16 + '0';
        } else {
            str[i] = x % 16 + 'a' - 10;
        }
        x /= 16;
        ++i;
    }
    for(int j = 0; j < i; j++) {
        res[j] = str[i - j - 1];
    }
    res[i] = 0;
    print(res);
}
