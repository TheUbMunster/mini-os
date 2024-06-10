#include "tetris.h"
#include "kbd.h"
#include "console.h"
//#include "ints.h"

static inline void lcr3(unsigned int val)
{   
    asm volatile("movl %0,%%cr3" : : "r" (val));
}

static inline void halt(void)
{
    asm volatile("hlt" : : );
}

#define PT_TABLE_COUNT 32
#if PT_TABLE_COUNT * 4 > 4096
    #error "TOO MANY PAGE TABLES, THEY DON'T ALL FIT IN THE PAGE TABLE DIRECTORY!"
#endif
char __attribute__ ((aligned (4096))) ptd_table[4096];
char __attribute__ ((aligned (4096))) pt_tables[4096 * PT_TABLE_COUNT];

int main(void)
{
    int i;

    // Create your page table here
    for (i = 0; i < PT_TABLE_COUNT; i++)
    {
        unsigned int* ptdentry = (unsigned int*)(ptd_table + (i * 4));
        *ptdentry = 0b11 | (int)(unsigned int*)(pt_tables + 4096 * i); //present + r/w

        for (int j = 0; j < 1024; j++)
        {
            unsigned int* ptentry = (unsigned int*)(pt_tables + (i * 4096) + (j * 4));
            *ptentry = ((j + (i * 1024)) << 12) | 0b11; //0b11 = present + r/w
        }
    }

    //assign the new page table.
    lcr3((unsigned int)ptd_table);
    init_pics(0x20, 0x28);

    //set up keyboard interrupts
    //ioapicenable(IRQ_KBD, 0);

    //print main tetris area.
    /*for (int i = 0; i < 75; i++)
        tick();*/

    //for (char sc = 0; sc != 1;)
    //{
    //    if (inb(0x60) != sc) //PORT FROM WHICH WE READ
    //    {
    //        sc = inb(0x60);
    //        if (sc > 0)
    //        {
    //            //terminal_putinput(c); //print on screen
    //            char c[2] = { translate(sc), 0 };
    //            printk(c);
    //        }
    //    }
    //}

    halt(); 
    return 0; 
}