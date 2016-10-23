#include "printf.h"
#include "com.h"
#include <stdint.h>



static int tryToRead(char const **s, const char *p) { // true if p is prefix of s
	const char *cs = *s;
	for (const char *cp = p; *cp; ++cs, ++cp) {
		if (*cs != *cp) {
			return 0;
		}
	}
	*s = cs;
	return 1;
}

static char *__buffer;
static char *__endBuffer;

static void writeByte2stdout(char c) {
	writeByte(c);
}
static void writeByte2buffer(char c) {
	if (__buffer >= __endBuffer) {
		return;
	}
	*__buffer = c;
	++__buffer;
}


static void writeUnsigned(uint64_t x, int base, void (*write)(char)) {
	if (x == 0) {
		(*write)('0');	
		return;
	}
	char s[65] = {};
	int sz = 0;
	static const char digits[17] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
								   'a', 'b', 'c', 'd', 'e', 'f'};
	for (; x; x /= base) {
		s[sz++] = digits[x % (base)];
	}
	for(int i = sz - 1; i >= 0; --i) {
		(*write)(s[i]);
	}
}

static void writeSigned(int64_t x, void (*write)(char)) {
	// writeStr("hoho:");
	if (x == 0) {
		(*write)('0');
		return;
	}
	if (x < 0) {
		(*write)('-');
		x = -x;
	}
	char s[65] = {};
	int sz = 0;
	for (; x; x /= 10) {
		s[sz++] = '0' + x % 10;
	}
	for(int i = sz - 1; i >= 0; --i) {
		(*write)(s[i]);
	}	
}


int printfWrapper(const char *format, va_list args, void (*write)(char)) {
	
	for (const char *c = format; *c;) {

		while (*c && *c != '%') {
			(*write)(*c);
			++c;
		}

		if (*c == 0) {
			break;
		}
		++c; // read %

		uint64_t number;

		int read = 1;

		if (tryToRead(&c, "hh")) {
			number = va_arg(args, int); // unsigned char

		} else if (tryToRead(&c, "h")) {
			number = va_arg(args, int); // unsigned short

		} else if (tryToRead(&c, "ll")) {
			number = va_arg(args, unsigned long long);

		} else if (tryToRead(&c, "l")) {
			number = va_arg(args, unsigned long);
		} else {
			read = 0;
		}
		
		if (tryToRead(&c, "d") || tryToRead(&c, "i")) {
			if (!read) 
				number = va_arg(args, int);
			writeSigned(number, write);

		} else if (tryToRead(&c, "u")) {
			writeUnsigned(number, 10, write);

		} else if (tryToRead(&c, "o")) {
			writeUnsigned(number, 8, write);

		} else if (tryToRead(&c, "x")) {
			writeUnsigned(number, 16, write);

		} else if (tryToRead(&c, "c")) {
			(*write)(va_arg(args, int)); // char

		} else if (tryToRead(&c, "s")) {
			for (char *ic = va_arg(args, char *); *ic; ++ic) {
				(*write)(*ic);
			}
		} else {
			return 0;
		}
	}
	return 1;
}




int vprintf(const char *format, va_list args) {
	return printfWrapper(format, args, writeByte2stdout);
}

int vsnprintf (char * s, size_t n, const char * format, va_list args) {
	__buffer = s;
	__endBuffer = s + n;
	printfWrapper(format, args, writeByte2buffer);
	return __buffer - s;
}


int printf(const char *format, ... ) {
	va_list args;
	va_start(args, format);
	int result = vprintf(format, args);
	va_end(args);
	return result;
}

int snprintf (char * s, size_t n, const char * format, ...) {
	va_list args;
	va_start(args, format);
	int result = vsnprintf(s, n, format, args);
	va_end(args);
	return result;
}