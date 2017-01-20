/*
 * Circle drawing algorithm.
 */

#include <PalmOS.h>

#include "circle.h"

/* Define this for draws directly to screen memory. */
#undef DIRECT_ACCESS


#ifdef DIRECT_ACCESS

static void
Set4Pixels_Standard(WinHandle winH, int dx, int dy, int xc, int yc)
{
    WinPtr win = WinGetWindowPointer(winH);
    BytePtr line, p;
    int x, y;

    y = yc - dy;
    line = win->displayAddr + y * (win->displayWidth / 8);

    x = xc - dx;
    p = line + x / 8;
    *p |= 1 << (7 - x % 8);

    x = xc + dx;
    p = line + x / 8;
    *p |= 1 << (7 - x % 8);

    y = yc + dy;
    line = win->displayAddr + y * (win->displayWidth / 8);

    x = xc - dx;
    p = line + x / 8;
    *p |= 1 << (7 - x % 8);

    x = xc + dx;
    p = line + x / 8;
    *p |= 1 << (7 - x % 8);
}

#else

static void
Set4Pixels_Standard(WinHandle winH, int dx, int dy, int xc, int yc)
{
#define SetPixel(x,y) WinDrawLine((x), (y), (x), (y))

    SetPixel(xc+dx, yc+dy);
    SetPixel(xc+dx, yc-dy);
    SetPixel(xc-dx, yc+dy);
    SetPixel(xc-dx, yc-dy);

#undef SetPixel
}    

#endif

static void
Set4Pixels_Pixel(WinHandle winH, int dx, int dy, int xc, int yc)
{
    WinDrawPixel(xc + dx, yc + dy);
    WinDrawPixel(xc + dx, yc - dy);
    WinDrawPixel(xc - dx, yc + dy);
    WinDrawPixel(xc - dx, yc - dy);
}

void
WinDrawEllipse(int xc, int yc, int a0, int b0)
{
    WinHandle win = WinGetDrawWindow();
    static void (*Set4Pixels)(WinHandle, int, int, int, int) = 0;

    int x = 0;
    int y = b0;

    long a = a0;
    long b = b0;

    long Asquared = a * a;
    long TwoAsquared = 2 * Asquared;
    long Bsquared = b * b;
    long TwoBsquared = 2 * Bsquared;

    long d;
    long dx,dy;

    if (!Set4Pixels) {
	UInt32 version;

	if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version) == 0
	    && version >= sysMakeROMVersion(3,5,0,0,0))
	    Set4Pixels = Set4Pixels_Pixel;
	else
	    Set4Pixels = Set4Pixels_Standard;
    }

    d = Bsquared - Asquared * b + Asquared/4L;
    dx = 0;
    dy = TwoAsquared * b;

    while (dx < dy) {
	Set4Pixels(win, x, y, xc, yc);

	if (d > 0L) {
	    --y;
	    dy -= TwoAsquared;
	    d -= dy;
	}

	++x;
	dx += TwoBsquared;
	d += Bsquared + dx;
    }

    d += (3L * (Asquared - Bsquared)/2L - (dx+dy)) / 2L;

    while (y >= 0) {
	Set4Pixels(win, x, y, xc, yc);

	if (d < 0L) {
	    ++x;
	    dx += TwoBsquared;
	    d += dx;
	}

	--y;
	dy -= TwoAsquared;
	d += Asquared - dy;
    }
}

void
WinDrawCircle(int xc, int yc, int r)
{
    WinDrawEllipse(xc, yc, r, r);
}
