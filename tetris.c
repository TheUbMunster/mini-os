#include "console.h"
#define DEBUG
#define CFLAG 0x80
static char grid[10][20]; //values are colors
static char mask[10][20]; //values are indeces &| CFLAG
static const char color_indeces[] = { 0x10, 0x20, 0x50, 0x40, 0x30, 0x60, 0x90 };
static char need_spawn = 1;

unsigned int rand(void)
{
	static unsigned int ind = 0;
	const int vals[] = { 0,1,2,3,4,5,6 };
	ind++;
	ind = (ind >= sizeof(vals) / sizeof(vals[0])) ? 0 : ind;
	return vals[ind];
}

void rotate(int direction) //pos=clockwise, neg=counterclockwise
{
	//lock
	static char rotbuf[5][5];
	//clear the rotbuf
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
			rotbuf[i][j] = 0;
	//find the center of the current piece.
	int centx = -1, centy = -1, searching = 1;
	for (int x = 0; x < 10 && searching; x++)
	{
		for (int y = 0; y < 20 && searching; y++)
		{
			if (mask[x][y] & CFLAG)
			{
				//we found it!
				centx = x;
				centy = y;
				searching = 1;
			}
		}
	}
	//if (centx == -1)
		//panic()
	//copy it in the rotbuf
	for (int x = centx - 2; x < 5; x++)
	{
		for (int y = centy - 2; y < 5; y++) 
		{
			if (x >= 0 && x < 10 && y >= 0 && y < 20)
			{
				rotbuf[x][y] = mask[x][y];
			}
		}
	}
	//rotate within the rotbuf
	for (int r = 1; r < 3; r++) //r at one because center doesn't change
	{
		for (int theta = 0; theta < r * 8; theta++)
		{
			//int tplus90 = (theta + (r * 2)) % (r * 8);
			//int tplus180 = (theta + (r * 4)) % (r * 8);
			//int tplus270 = (theta + (r * 6)) % (r * 8);
			//copy cw/ccw
			//a-b
			//b-c
			//c-d
			//d-a
			printk(rotbuf[0]);
		}
	}
	//adjust center of original if rotation causes it to be pushed by boundaries.
	//todo
	//check if the desired resulting location is free, if so, commit, otherwise, do nothing.
	//todo
	//unlock
}

void seeksquare(char x, char y)
{
	seekconsole((char)(30 + (x * 2)), (char)(3 + y));
}

void printgrid(void)
{
#ifdef DEBUG
	printuart("printed the grid\n");
#endif
	for (int j = 0; j < 20; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			seeksquare(i, j);
			printsquare(grid[i][j]);
		}
	}
}

void printinitial(void)
{
#ifdef DEBUG
	printuart("cleared the screen\n");
#endif
	clearscreencolor(dark_gray);
	for (int i = 0; i < 20; i++)
	{
		seekconsole(30, i + 3);
		printnsquare(black, 10);
	}
}

void spawn_piece(void)
{
	need_spawn = 0;
	int i = rand() % 7;
#ifdef DEBUG
	printuart("spawned piece no ");
	char buf[11];
	itoahex(i, buf);
	printuart(buf);
	printuart("\n");
#endif
	switch (i)
	{
	case 0: //L
		mask[4][0] = i;
		mask[4][1] = i | CFLAG;
		mask[4][2] = i;
		mask[5][2] = i;
		break;
	case 1: //mirror L
		mask[5][0] = i;
		mask[5][1] = i | CFLAG;
		mask[5][2] = i;
		mask[4][2] = i;
		break;
	case 2: //T
		mask[5][0] = i;
		mask[5][1] = i | CFLAG;
		mask[4][1] = i;
		mask[6][1] = i;
		break;
	case 3: //Box
		mask[4][0] = i;
		mask[4][1] = i;
		mask[5][0] = i;
		mask[5][1] = i;
		break;
	case 4: //Line
		mask[4][0] = i;
		mask[4][1] = i;
		mask[4][2] = i | CFLAG;
		mask[4][3] = i;
		break;
	case 5: //S
		mask[4][1] = i;
		mask[5][1] = i | CFLAG;
		mask[5][0] = i;
		mask[6][0] = i;
		break;
	case 6: //Z
		mask[4][0] = i;
		mask[5][1] = i | CFLAG;
		mask[5][0] = i;
		mask[6][1] = i;
		break;
	}
}

void tick(void)
{
	//disable keyboard interrupts
	static int initialized = 0;
	if (!initialized)
	{
		printinitial();
		initialized = 1;
	}

	if (need_spawn)
	{
		spawn_piece();
	}

	//if cementable (cement, break)
	//otherwise, move
	//is cementable in current position?
	int cementable = 0;
	for (int j = 0; j < 20; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			//we are currently a moving piece && 
			//(we are on the bottom row || 
			//(there is a piece below && the piece isn't us)).            
			cementable |= (mask[i][j] && (j == 19 || (grid[i][j + 1] && !mask[i][j + 1])));
		}
	}

	if (cementable)
	{
		//freeze it in place.
#ifdef DEBUG
		printuart("cementing a piece in place\n");
#endif
		for (int j = 0; j < 20; j++)
		{
			for (int i = 0; i < 10; i++)
			{
				if (mask[i][j])
				{
					grid[i][j] = color_indeces[mask[i][j] & (char)(~CFLAG)]; //add visual
					mask[i][j] = 0; //no longer moving.
				}
			}
		}
		need_spawn = 1; //we cemented, need a new piece!
	}
	else
	{
#ifdef DEBUG
		printuart("moving a piece one tick\n");
#endif
		//clear old color for this piece
		for (int j = 0; j < 20; j++)
		{
			for (int i = 0; i < 10; i++)
			{
				if (mask[i][j])
				{
#ifdef DEBUG
					char buf[11];
					printuart("clearing the color for loc: ");
					itoahex(i, buf);
					printuart(buf);
					itoahex(j, buf);
					printuart(buf);
					printuart("\n");
#endif
					grid[i][j] = black; //set to background color (erase, about to move)
			}
		}
}
		//move the mask
		for (int j = 20 - 1; j >= 0; j--)
		{
			for (int i = 0; i < 10; i++)
			{
				if (j > 0)
					mask[i][j] = mask[i][j - 1]; //forr move downwards by copying above.
				else
					mask[i][j] = 0; //top row is always zeroed
			}
		}
		//set color for new mask location
		for (int j = 0; j < 20; j++)
		{
			for (int i = 0; i < 10; i++)
			{
				if (mask[i][j])
				{
					grid[i][j] = color_indeces[mask[i][j] & (char)(~CFLAG)];
#ifdef DEBUG
					char buf[11];
					itoahex(mask[i][j], buf);
					printuart("(mask: ");
					printuart(buf);
					printuart(") ");
					printuart("setting the color: ");
					itoahex(grid[i][j], buf);
					printuart(buf);
					printuart(" for loc: ");
					itoahex(i, buf);
					printuart(buf);
					itoahex(j, buf);
					printuart(buf);
					printuart("\n");
#endif
				}
			}
		}
	}

	printgrid();
}