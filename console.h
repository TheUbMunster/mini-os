#if !defined(__CONSOLE_H__)
#define __CONSOLE_H__

#define black           0x0 
#define blue            0x1 
#define green           0x2 
#define cyan            0x3 
#define red             0x4 
#define magenta         0x5 
#define brown           0x6 
#define gray            0x7 
#define dark_gray       0x8 
#define bright_blue     0x9 
#define bright_green    0xA 
#define bright_cyan     0xB 
#define bright_red      0xC 
#define bright_magenta  0xD 
#define yellow          0xE 
#define white           0xF 

void clearscreencolor(char bg_color);
void clearscreen(void);
void printuart(const char* str);
void seekconsole(char x, char y);
void printsquare(char bg_color);
void printnsquare(char bg_color, char n);
void printk(const char* str);
void printkcolor(const char * str, char bg_fg_color);
void itoahex(unsigned int num, char buf[11]);

#endif