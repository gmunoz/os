#ifndef _KPRINTF_C
#define _KPRINTF_C

extern void putchar(int c);

#include <kprintf.h>
#include <kglob.h>

typedef char * va_list;

#define va_start(ap, v)   ((void) (ap = (va_list) &v + sizeof(v)))
#define va_arg(ap, type)  (*((type *)ap)++)
#define va_end(ap)        ((void) (ap = 0))

void kprintf(const char *format, ...)
{
	va_list ap;  /* Points to each unnamed arg in turn */
	char *p, *sval;
	int ival;

	va_start(ap, format);  /* Make `ap' point to 1st unnamed arg */
	for (p = (char *)format; *p; p++) {
		if (*p != '%') {
			putchar(*p);
			continue;
		}

		*++p;

		/* Determine if there was some number of padding specified */
		int padding = 0;
		if (*p >= 48 && *p <= 57) {
			int i = 0;
			char number[WORD_SIZE * 8];
			while (i < WORD_SIZE * 8 && (*p >= 48 && *p <= 57)) {
				number[i] = *p;
				*++p;
				i++;
			}
			number[i] = 0;
			padding = atoi(number);
		}

		/* Print out the appropriate variable */
		int length = 0;
		switch (*p) {
			case 'b':
				ival = va_arg(ap, unsigned int);
				putchar('0');
				putchar('b');
				length += 2;
				length += printn(ival, 2);
				break;
			case 'c':
				ival = va_arg(ap, int);
				putchar(ival);
				length++;
				break;
			case 'd':
				ival = va_arg(ap, int);
				length += printn(ival, 10);
				break;
			case 'f':
				break;
			case 'o':
				ival = va_arg(ap, unsigned int);
				putchar('0');
				length += 1;
				length += printn(ival, 8);
				break;
			case 'p':
				ival = va_arg(ap, unsigned int);
				putchar('0');
				putchar('x');
				length += 2;
				length += printn(ival, 16);
				break;
			case 's':
				for (sval = va_arg(ap, char *); *sval; sval++) {
					putchar(*sval);
					length++;
				}
				break;
			case 'u':
				ival = va_arg(ap, unsigned int);
				length += printn(ival, 10);
				break;
			case 'x':
				ival = va_arg(ap, unsigned int);
				putchar('0');
				putchar('x');
				length += 2;
				length += printn(ival, 16);
				break;
			default:
				putchar(*p);
				break;
		}

		/* Calculate how much padding is left (if any), and print that many
		 * empty (' ') characters. */
		padding -= length;
		int i;
		for (i = 0; i < padding; i++)
			putchar(' ');
	}
	va_end(ap);
}

int printn(unsigned int num, int base)
{
	int result = 0;
	unsigned int div;
	if ( (div = num / base) )
		result += printn(div, base);

	putchar(getascii_video(num % base));
	result++;

	return result;
}

char getascii_video(char c)
{
	char valor = '0' + c;
	if ( valor > '9' )
		valor += 7;

	return valor;
}

int atoi(const char *nptr)
{
	int i, n;
	n = 0;
	for (i = 0, n = 0; nptr[i] >= '0' && nptr[i] <= '9'; i++)
		n = 10 * n + (nptr[i] - '0');
	return n;
}

#endif
