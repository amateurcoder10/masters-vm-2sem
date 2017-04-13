#include <dos.h>

void main()
{
	// Enter mode 13h
	asm mov ah,0x00
	asm mov al,0x13
	asm int 0x10

	// Now print the entire palette to the screen
	for(int i = 0; i < 256; i++)
	{
		for(int l = 0; l < 200; l++)
		{
			poke(0xA000, (l*320)+i, i);
		}
	}

}
