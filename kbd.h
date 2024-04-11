void outb(unsigned short port, unsigned char val);
void init_pics(int pic1, int pic2);
unsigned char inb(unsigned short int port);
unsigned char translate(unsigned char scan_code);