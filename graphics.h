/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	portions by Aaron Ardiri
	
	designed for Handspring Treo (PalmOS3.5H)
*/

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

typedef struct MatroLine {
    Int32 x1, y1;
    Int32 x2, y2;
} MatroLine;

Boolean   GraphicsInitialize();
WinHandle GraphicsGetDrawWindow();
void      GraphicsClear();
void      GraphicsRepaint();
void      GraphicsTerminate();
void	  MatroPaintAliasedLine(MatroLine *ln, Boolean isOffscreenWindow);

#endif 
