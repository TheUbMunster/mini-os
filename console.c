#define COM1    0x3f8

static int uart;    // is there a uart?

void microdelay(unsigned long us) {

}

static inline unsigned char inb(unsigned short port)
{
	unsigned char data;

	asm volatile("in %1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void outb(unsigned short port, unsigned char data)
{
	asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void uartinit(void)
{

	// Turn off the FIFO
	outb(COM1 + 2, 0);

	// 9600 baud, 8 data bits, 1 stop bit, parity off.
	outb(COM1 + 3, 0x80);    // Unlock divisor
	outb(COM1 + 0, 115200 / 115200);
	outb(COM1 + 1, 0);
	outb(COM1 + 3, 0x03);    // Lock divisor, 8 data bits.
	outb(COM1 + 4, 0);
	outb(COM1 + 1, 0x01);    // Enable receive interrupts.

	// If status is 0xFF, no serial port.
	if (inb(COM1 + 5) == 0xFF)
		return;

	uart = 1;

	// Acknowledge pre-existing interrupt conditions;
	// enable interrupts.
	inb(COM1 + 2);
	inb(COM1 + 0);
}

static void uartputc(int c)
{
	static int initialized = 0;
	if (!initialized)
	{
		uartinit();
		initialized = 1;
	}
	int i;

	if (!uart)
		return;

	for (i = 0; i < 128 && !(inb(COM1 + 5) & 0x20); i++)
		microdelay(10);

	outb(COM1 + 0, c);
}

static int consolex = 0, consoley = 0;
void clearscreencolor(char bg_color)
{
	bg_color = (((bg_color & 0xF0) == 0) ? bg_color << 4 : bg_color);
	int stc = 80 * 25 * 2;
	for (volatile char* vgaclr = (volatile char*)0xb8000; vgaclr - (volatile char*)0xb8000 < stc; )
	{
		*vgaclr++ = ' '; // in this order because little-endian
		*vgaclr++ = bg_color;
	}
	consolex = 0;
	consoley = 0;
}

void clearscreen()
{
	clearscreencolor(0x00);
}

void printuart(const char* str)
{
	int i, c;
	for (i = 0; (c = str[i]) != 0; i++)
	{
		if (c == '\n' && (i == 0 || str[i - 1] != '\r'))
			uartputc('\r'); //qemu serial out wants carrage return.
		uartputc(c);
	}
}

void seekconsole(char x, char y)
{
	if (x < 0)
		x = 0;
	else if (x >= 80)
		x = 79;
	if (y < 0)
		y = 0;
	else if (y >= 25)
		y = 24;
	consolex = x;
	consoley = y;
}

void printsquare(char bg_color)
{
	bg_color = (((bg_color & 0xF0) == 0) ? bg_color << 4 : bg_color);
	volatile char* vgal = (volatile char*)0xb8000 + ((consolex / 2) * 4) + (consoley * 2 * 80);
	*vgal++ = ' ';
	*vgal++ = bg_color;
	consolex++;
	*vgal++ = ' ';
	*vgal++ = bg_color;
	consolex++;
}

void printnsquare(char bg_color, char n) 
{
	for (int i = 0; i < n; i++)
	{
		printsquare(bg_color);
	}
}

// Print to the console
void printkcolor(const char* str, char bg_fg_color)
{
	//vga mode 03?
	//x max is 80, y max is 25
	volatile char* vgal = (volatile char*)0xb8000 + (consolex * 2) + (consoley * 2 * 80);
	int i, c;

	for (i = 0; (c = str[i]) != 0; i++)
	{
		//if (c == '\n' && (i == 0 || str[i - 1] != '\r'))
		//    uartputc('\r'); //qemu serial out wants carrage return.
		//uartputc(c);
		if (c != '\n')
		{
			if (consolex >= 80)
				continue; //don't print off the right edge (would just line wrap if we did, and I'm too lazy to implement horizontal scrolling)
			*vgal++ = c; // in this order because little-endian
			*vgal++ = bg_fg_color; //0x05
			consolex++;
		}
		else
		{
			consoley++;
			if (consoley >= 25)
			{
				//wrap it forward by one.
				for (volatile char* vgamov = (volatile char*)(0xb8000); vgamov < (volatile char*)(0xb8000 + (23 * 2 * 80) + (80 * 2)); vgamov++)
				{
					*vgamov = *(vgamov + (80 * 2));
				}
				//clear the last line (we're going to write to it now)
				for (volatile char* vgaclr = (volatile char*)(0xb8000 + (23 * 2 * 80) + (80 * 2)); vgaclr < (volatile char*)(0xb8000 + (24 * 2 * 80) + (80 * 2)); )
				{
					*vgaclr++ = ' '; // in this order because little-endian
					*vgaclr++ = 0x00;
				}
				consoley--;
			}
			else
			{
				vgal = (volatile char*)0xb8000 + (consoley * 2 * 80);
				//vgal += 2 * 80; //next row
			}
			consolex = 0;
		}
	}
}

void printk(const char* str)
{
	printkcolor(str, 0x0F);
}

void itoahex(unsigned int num, char buf[11])
{
	buf[0] = '0'; //0
	buf[1] = 'x'; //x
	int c = 0;
	for (int nib = 0, lead = 1; nib < 8; nib++)
	{
		int numnib = (num >> ((7 - nib) * 4)) & 0xF;
		if (lead && !numnib) //if checking for leading zeros and this is a leading zero
		{
			continue; //get more nibbles, don't increment the counter.
		}
		else
		{
			if (numnib <= 9)
				buf[c + 2] = (char)('0' + numnib);
			else
				buf[c + 2] = (char)('A' + (numnib - 10));
			lead = 0; //no more leading 0's
			c++;
		}
	}
	if (c == 0)
	{
		buf[c + 2] = '0'; //if num == 0, print at least one zero
		c++;
	}
	if (c != 7)
		buf[c + 2] = '\0';
}