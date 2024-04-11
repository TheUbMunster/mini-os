#include "kbd.h"

#define PIC1 0x20
#define PIC2 0xA0
#define ICW1 0x11
#define ICW4 0x01

void outb(unsigned short port, unsigned char val)
{
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

__inline unsigned char inb(unsigned short int port)
{
	unsigned char _v;

	__asm__ __volatile__("inb %w1,%0":"=a" (_v) : "Nd" (port));
	return _v;
}

void init_pics(int pic1, int pic2)
{
	/* send ICW1 */
	outb(PIC1, ICW1);
	outb(PIC2, ICW1);

	/* send ICW2 */
	outb(PIC1 + 1, pic1);
	outb(PIC2 + 1, pic2);

	/* send ICW3 */
	outb(PIC1 + 1, 4);
	outb(PIC2 + 1, 2);

	/* send ICW4 */
	outb(PIC1 + 1, ICW4);
	outb(PIC2 + 1, ICW4);

	/* disable all IRQs */
	outb(PIC1 + 1, 0xFF);
}

unsigned char translate(unsigned char scan_code)
{
	// https://web.archive.org/web/20120129081728/http://www.nondot.org/sabre/os/files/HCI/keyboard.txt
	static const char t[] =
	{
		'.', //index 0 undefined?
		'.', //ESC
		'1', //num row
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'0',
		'-', //- and _
		'=', //= and +
		'.', //BACKSPACE
		'.', //TAB
		'Q',
		'W',
		'E',
		'R',
		'T',
		'Y',
		'U',
		'I',
		'O',
		'P',
		'[', //[ and {
		']', //] and }
		'\n', //ENTER
		'.', //Left ctrl
		'A',
		'S',
		'D',
		'F',
		'G',
		'H',
		'J',
		'K',
		'L',
		';',
		'\'',
		'`',
		'.', //left shift
		'.', //index 43 undefined?
		'Z',
		'X',
		'C',
		'V',
		'B',
		'N',
		'M',
		',',
		'.',
		'/',
		'.', //right shift
		'*', //keypad
		'.', //left alt
		' ',
		'.', //capslock
	};
	return t[scan_code];
}