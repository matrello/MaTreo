/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	portions by Aaron Ardiri
	
	designed for Handspring Treo (PalmOS3.5H)
*/

#include <PalmOS.h>
#include "matro2.h"
#include "matro2_res.h"
#include "device.h"
#include "MathLib.h"
#include "graphics.h"

#define BARSCOLOR_GREY		 15
#define BARSCOLOR_RED		204
#define BARSCOLOR_GREEN		 51
#define BARSCOLOR_BLUE		  0

typedef struct
{
  Boolean   scr160x160;
  WinHandle drawWindow;
  WinHandle mainWindow;
  MemHandle paletteBars;   
} GraphicsGlobals;

#ifdef PDACOLORCREATE
RGBColorType		BackColorRGB, colRGB;
IndexedColorType	NewColor=0, BackColor=0;
#endif

Boolean GraphicsInitialize()
{
  GraphicsGlobals *globals;
  Boolean         result = true;
  UInt16          err;

  // create the globals object, and register it
  globals = (GraphicsGlobals *)MemPtrNew(sizeof(GraphicsGlobals));
  MemSet(globals, sizeof(GraphicsGlobals), 0);
  FtrSet(appCreator, ftrGraphicsGlobals, (UInt32)globals);

  // determine screen size (only >= 3.5)
  globals->scr160x160 = true;
  if (DeviceSupportsVersion(romVersion3_5)) {

    UInt32 width, height;
    UInt32 cpuID;

    // determine the current display mode
    WinScreenMode(winScreenModeGetDefaults,&width,&height,NULL,NULL);
    globals->scr160x160 = ((width == 160) && (height == 160));

    // lets make sure we are running on a "m68k chip" :) - asm routines work
    FtrGet(sysFtrCreator, sysFtrNumProcessorID, &cpuID);
    globals->scr160x160 &= ((cpuID == sysFtrNumProcessor328) ||
                            (cpuID == sysFtrNumProcessorEZ));//  ||
  }

  	globals->mainWindow=WinGetDrawWindow();

#ifdef PDACREATE		
	globals->paletteBars=MemHandleNew(1600);
#endif
	  
  // create the offscreen window
  globals->drawWindow = 
    WinCreateOffscreenWindow(SCREEN_WIDTH, SCREEN_HEIGHT, screenFormat, &err);
  err |= (Boolean)(globals->drawWindow == NULL);

  // something went wrong, display error and exit
  if (err != errNone) {
    // MATRO DIALOG ERRORE QUA
    //ApplicationDisplayDialog(XmemForm);
    result = false;
  }

  return result;
}

WinHandle GraphicsGetDrawWindow()
{
  GraphicsGlobals *globals;
  
  // get a globals reference
  FtrGet(appCreator, ftrGraphicsGlobals, (UInt32 *)&globals);

  return globals->drawWindow;
}

void GraphicsTerminate()
{
  GraphicsGlobals *globals;

  // get a globals reference
  FtrGet(appCreator, ftrGraphicsGlobals, (UInt32 *)&globals);

  // clean up windows/memory
  if (globals->drawWindow != NULL) 
    WinDeleteWindow(globals->drawWindow, false);
  MemPtrFree(globals);

  // unregister global data
  FtrUnregister(appCreator, ftrGraphicsGlobals);
}

void GraphicsClear()
{
  const CustomPatternType erase = {0,0,0,0,0,0,0,0};
  const RectangleType     rect  = {{0,0},{SCREEN_WIDTH,SCREEN_HEIGHT}};
  GraphicsGlobals *globals;
  WinHandle       currWindow;

  WinPushDrawState();
  
  // get a globals reference
  FtrGet(appCreator, ftrGraphicsGlobals, (UInt32 *)&globals);

  // clear the buffer
  currWindow = WinSetDrawWindow(globals->drawWindow);
  WinSetPattern(&erase);
  WinFillRectangle(&rect, 0);
  WinSetDrawWindow(currWindow);
  
  WinPopDrawState();
  
}

void GraphicsRepaint()
{
  const RectangleType rect = {{0,0},{SCREEN_WIDTH,SCREEN_HEIGHT-1}};
  GraphicsGlobals *globals;
  
  // get a globals reference
  FtrGet(appCreator, ftrGraphicsGlobals, (UInt32 *)&globals);

  // copy the backbuffer to the screen
  WinCopyRectangle(globals->drawWindow, WinGetDisplayWindow(),
                   &rect, SCREEN_X, SCREEN_Y , winPaint );
}

void MatroPaintAliasedLinePixel(Int32 x, Int32 y, Int32 transvalue, Boolean isOffscreenWindow, MemPtr barspalette)
{
	// this routine took me lots of efforts :-)
	// handle with care :-))

#ifdef PDACOLOR

#ifdef PDACOLORCREATE
	GraphicsGlobals		*globals;
	MemPtr				myPalette;
	Int16				c1,c2,c3;
	Int32				trans, transcounter=0;
	
	FtrGet(appCreator, ftrGraphicsGlobals, (UInt32 *)&globals);		
	if (isOffscreenWindow)
    	WinSetDrawWindow(globals->mainWindow);
	myPalette=MemHandleLock(globals->paletteBars);
	colRGB.r=BARSCOLOR_RED;
	colRGB.g=BARSCOLOR_GREEN;
	colRGB.b=BARSCOLOR_BLUE;
	BackColor=255;
    do {    
		transcounter=0; BackColor++;
		do {          	
        	WinIndexToRGB(BackColor,&BackColorRGB);        
        	BackColorRGB.index=0;		
        	trans=100-((transcounter*100)/255);        	
        	if (trans>0)
        	{
        		c1=BackColorRGB.r; c2=colRGB.r;
        		c3=c2-c1;
        		c3=(c3*trans)/100;
        		c3=c1+c3;
        		if (c3<0) c3=0;
        		if (c3>255) c3=255;
        		BackColorRGB.r=(UInt8) c3;
        
        		c1=BackColorRGB.g; c2=colRGB.g;
        		c3=c2-c1;
        		c3=(c3*trans)/100;
        		c3=c1+c3;
        		if (c3<0) c3=0;
        		if (c3>255) c3=255;
        		BackColorRGB.g=(UInt8) c3;
        
        		c1=BackColorRGB.b; c2=colRGB.b;
        		c3=c2-c1;
        		c3=(c3*trans)/100;
        		c3=c1+c3;
        		if (c3<0) c3=0;
        		if (c3>255) c3=255;
        		BackColorRGB.b=(UInt8) c3;      	
        	}	        
        	NewColor=WinRGBToIndex(&BackColorRGB);
        	*(Int8 *)(myPalette+((transcounter/51)*256)+BackColor)=NewColor;

			transcounter+=51;
        } while (transcounter<256);

    } while (BackColor<255);

	if (isOffscreenWindow)
		WinSetDrawWindow(globals->drawWindow);

	MemHandleUnlock(globals->paletteBars);

#else
	IndexedColorType BackColor= WinGetPixel(x,y);
	WinSetForeColor( *(Int8 *)(barspalette+((transvalue/51)*256)+BackColor) );
	WinDrawPixel(x,y);
#endif
		
#else

#ifdef PDAGREYCREATE

	IndexedColorType	BackColor=0, NewColor=BARSCOLOR_GREY;
	GraphicsGlobals		*globals;
	MemPtr				myPalette;
	Int32				trans;

	FtrGet(appCreator, ftrGraphicsGlobals, (UInt32 *)&globals);		
	myPalette=MemHandleLock(globals->paletteBars);

    do {
		NewColor=BARSCOLOR_GREY;
		transvalue=0;
		do {    
			trans=(transvalue*100)/255;
        	if (trans>0)
        		NewColor+=((BackColor-NewColor)*trans)/100;	

        	*(Int8 *)(myPalette+((transvalue/51)*16)+BackColor)=NewColor;

			transvalue+=51;
        } while (transvalue<256);
		BackColor++;
    } while (BackColor<16);
		
	MemHandleUnlock(globals->paletteBars);
		
#else
	IndexedColorType BackColor= WinGetPixel(x,y);
	WinSetForeColor( *(Int8 *)(barspalette+((transvalue/51)*16)+BackColor) );
	WinDrawPixel(x,y);
#endif

#endif	

}

void MatroPaintAliasedLine(MatroLine *ln, Boolean isOffscreenWindow)
{
	// this routine took me lots of efforts :-)
	// handle with care :-))
	
    GraphicsGlobals *globals;
    Int32 x, y, inc, dx, dy, matro;
	MemPtr barspalette;
	MemHandle mBarsPalette;

#ifndef  PDACREATE
	mBarsPalette=DmGetResource('data', BARSPALETTE);
	barspalette=MemHandleLock(mBarsPalette);
#endif

#ifdef  PDACREATE
    MatroPaintAliasedLinePixel(0, 0, 0, isOffscreenWindow, barspalette);
	FtrGet(appCreator, ftrGraphicsGlobals, (UInt32 *)&globals);	
	barspalette=MemHandleLock(globals->paletteBars);
	// save "palettebarsgrey" 0x02d78 514
	MemHandleUnlock(globals->paletteBars);	
#endif
	
    dx = (ln->x2 - ln->x1); dy = (ln->y2 - ln->y1);

    if (fabs(dx) > fabs(dy)) {
        if (dx < 0) {
            dx = -dx; dy = -dy;
			matro=ln->x1; ln->x1=ln->x2; ln->x2=matro; 
			matro=ln->y1; ln->y1=ln->y2; ln->y2=matro; 
        }
        x = ln->x1 << 16; y = ln->y1 << 16;
        inc = (dy * 65536) / dx;

        while ((x >> 16) < ln->x2) {
            MatroPaintAliasedLinePixel(x >> 16, y >> 16, (y >> 8) & 0xFF, isOffscreenWindow, barspalette);
            MatroPaintAliasedLinePixel(x >> 16, (y >> 16) + 1, (~y >> 8) & 0xFF, isOffscreenWindow, barspalette);
            x += ((Int32) 1 << 16);
            y += inc;
        }
    } else {
        if (dy < 0) {
            dx = -dx; dy = -dy;
			matro=ln->x1; ln->x1=ln->x2; ln->x2=matro; 
			matro=ln->y1; ln->y1=ln->y2; ln->y2=matro; 
        }
        x = ln->x1 << 16; y = ln->y1 << 16;
        inc = (dx * 65536) / dy;

        while ((y >> 16) < ln->y2) {
            MatroPaintAliasedLinePixel(x >> 16, y >> 16, (x >> 8) & 0xFF, isOffscreenWindow, barspalette);
            MatroPaintAliasedLinePixel((x >> 16) + 1, (y >> 16), (~x >> 8) & 0xFF, isOffscreenWindow, barspalette);
            x += inc;
            y += ((Int32) 1 << 16);
        }
    }

#ifndef  PDACREATE
	MemHandleUnlock(mBarsPalette);
	DmReleaseResource(mBarsPalette);
#endif
	
    return;
}

