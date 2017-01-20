/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	designed for Handspring Treo (PalmOS3.5H)
*/

// note: DoAnim() is main entry call here

#include <PalmOS.h>
#include <HsExt.h>
#include "matro2.h"
#include "matro2_res.h"
#include "device.h"
#include "graphics.h"
#include "circle.h"
#include "mathlib.h"
#include "sysstuff.h"
#include "GSMLibrary.h"
#include "anim.h"

typedef struct
{
  UInt32 X;

  UInt16		counter;
  Boolean		clockInitOk;
  
  DateTimeType	Time;
  Int16			lastSecond;
  Int16			lastMinute;
  Boolean		changedSecond;
  Boolean		changedMinute;
  UInt32		lastTicks;
  
  Int16			lastBattery;
  Int16			signalDelay;
  Int16			signalInitOK;
} AnimGlobals;


#define CLOCKCIRCLE_X	0
#define CLOCKCIRCLE_Y	0
#define CLOCK_X			50
#define CLOCK_Y			50
#define BATTERY_H		92
#define BATTERY_W		42
#define BATTERY_X		5
#define BATTERY_Y		17
#define BATTERY_CX		5+2
#define BATTERY_CY		17+45
#define BATTERY_RAGGIO	33

#define LBCLOCKSP       165
#define LBSIGNAL_W		18
#define LBSIGNAL_DIST	3

#define CLOCK_DEGXSEC   6
#define CLOCK_DEGXMIN   6
#define CLOCK_DEGXHR    30
#define CLOCK_RAGGIOSEC 43
#define CLOCK_RAGGIOMIN 40
#define CLOCK_RAGGIOHR  22

#define PI				3.1415926535897932384626433832795 // :-)))

void ClockAnim(AnimGlobals *anim)
{
	float diffTicks, diffDots, degSecond, degMinute, degHour;
	Int16 hour12;
	Int16 xSecond, ySecond, xMinute, yMinute, xHour, yHour;
	IndexedColorType prevColor=WinSetForeColor(0);

	MatroLine line;
	
	if (!anim->clockInitOk)
		return;
			
	// calculate clock hands in degrees
    diffTicks=(TimGetTicks()-anim->lastTicks);			
    diffDots=(diffTicks*CLOCK_DEGXSEC)/((float)SysTicksPerSecond());
    degSecond=(anim->lastSecond*CLOCK_DEGXSEC)+diffDots;

    diffDots=(anim->lastSecond*CLOCK_DEGXMIN)/((float)60);
    degMinute=(anim->Time.minute*CLOCK_DEGXMIN)+diffDots;

	hour12=(anim->Time.hour>12?anim->Time.hour-12:anim->Time.hour);
    diffDots=(anim->Time.minute*CLOCK_DEGXHR)/((float)60);
    degHour=(hour12*CLOCK_DEGXHR)+diffDots;

	// calculate clock hands pixels positions
	xSecond=(sin((degSecond * (PI / 180)))*CLOCK_RAGGIOSEC);
	ySecond=-(cos((degSecond * (PI / 180)))*CLOCK_RAGGIOSEC);
	xMinute=(sin((degMinute * (PI / 180)))*CLOCK_RAGGIOMIN);
	yMinute=-(cos((degMinute * (PI / 180)))*CLOCK_RAGGIOMIN);
	xHour=(sin((degHour * (PI / 180)))*CLOCK_RAGGIOHR);
	yHour=-(cos((degHour * (PI / 180)))*CLOCK_RAGGIOHR);
	
	// paint clock hands
	line.x1=CLOCK_X; line.y1=CLOCK_Y;
	line.x2=CLOCK_X+xSecond; line.y2=CLOCK_Y+ySecond;
    MatroPaintAliasedLine(&line, true);

	line.x1=CLOCK_X; line.y1=CLOCK_Y;
	line.x2=CLOCK_X+xMinute; line.y2=CLOCK_Y+yMinute;
    MatroPaintAliasedLine(&line, true);

	line.x1=CLOCK_X; line.y1=CLOCK_Y;
	line.x2=CLOCK_X+xHour; line.y2=CLOCK_Y+yHour;
    MatroPaintAliasedLine(&line, true);

	WinSetForeColor(prevColor);
}

void DebugRefresh(FormPtr form, AnimGlobals *anim)
{
	// this routine, usually not active,
	// displays GSM events and main related 
	// parameters in the MaTreo main dialog
	
	UInt16 objIndex, i, evLabel;
	char strTick[20]; 
	TreoInfo *treoinfo;
	Err error=0;
	
	MemSet(strTick,20,0);

    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);

	if (treoinfo->gsmNewEvent)
	{
		treoinfo->gsmNewEvent=false;
		anim->counter++;

	    StrPrintF(&strTick[0], "%x %x", anim->counter, treoinfo->gsmEVpar[0]);
    
        objIndex=FrmGetObjectIndex(form, evcnt);
        FrmHideObject(form, objIndex);
        FrmCopyLabel(form, evcnt, &strTick[0]);
        FrmShowObject(form, objIndex);
		
		for (i=0; i<3; i++)
		{
			switch(treoinfo->gsmEV[i])
			{
            case kCardInsertion:
               StrCopy(strTick, "kCardInsertion");
            break; case kRegistration:
               StrCopy(strTick, "kRegistration");
            break; case kError:
               StrCopy(strTick, "kError");
            break; case kButton:
               StrCopy(strTick, "kButton");
            break; case kPower:
               StrCopy(strTick, "kPower");
            break; case kPassword:
               StrCopy(strTick, "kPassword");
            break; case kProgress:
               StrCopy(strTick, "kProgress");
            break; case kIndication:
               StrCopy(strTick, "kIndication");
            break; case kConnectInd:
               StrCopy(strTick, "kConnectInd");
            break; case kConnectConf:
               StrCopy(strTick, "kConnectConf");
            break; case kSubscriber:
               StrCopy(strTick, "kSubscriber");
            break; case kDisconnectInd:
               StrCopy(strTick, "kDisconnectInd");
            break; case kDisconnectConf:
               StrCopy(strTick, "kDisconnectConf");
            break; case kBusy:
               StrCopy(strTick, "kBusy");
            break; case kUpdate:
               StrCopy(strTick, "kUpdate");
            break; case kConference:
               StrCopy(strTick, "kConference");
            break; case kVoiceMail:
               StrCopy(strTick, "kVoiceMail");
            break; case kMessageInd:
               StrCopy(strTick, "kMessageInd");
            break; case kSegmentInd:
               StrCopy(strTick, "kSegmentInd");
            break; case kMessageStat:
               StrCopy(strTick, "kMessageStat");
            break; case kMessageDel:
               StrCopy(strTick, "kMessageDel");
            break; case kMessageMoved:
               StrCopy(strTick, "kMessageMoved");
            break; case kSATNotification:
               StrCopy(strTick, "kSATNotification");
            break; case kUSSDInd:
               StrCopy(strTick, "kUSSDInd");
            break; case kPhoneEquipmentMode:
               StrCopy(strTick, "kPhoneEquipmentMode");
			break;
			default:
               StrCopy(strTick, "(matro :)");
			break;
			}	

            objIndex=FrmGetObjectIndex(form, evcnt+1+i);
            FrmHideObject(form, objIndex);
            FrmCopyLabel(form, evcnt+1+i, &strTick[0]);
            FrmShowObject(form, objIndex);

		}
	}
}

void BatteryRefresh(FormPtr form, AnimGlobals *anim, TreoInfo *treoinfo)
{
	UInt8 batt=0;
	Err error=0;
	RectangleType srcRect;
	
	BitmapType *bitBFull, *nbitBFull, *bitBEmpty, *nbitBEmpty;
	WinHandle wbitBFull, wbitBEmpty, wold;
	MemHandle resBFull, resBEmpty;	
	IndexedColorType prevColor;

	Int16 hour12, xBatt, yBatt, objIndex;

	MatroLine line;
		
	if (anim->changedSecond)
	{

		if (treoinfo->gsmenabled)
		{
        	if ((anim->signalDelay--)==0)
        	{
        		anim->signalDelay=3;
    			if (!anim->signalInitOK)
    			{
    		        objIndex=FrmGetObjectIndex(form, lbCLOCK);
    				FrmShowObject(form, objIndex);
    				anim->signalInitOK=true;
    				prevColor=WinSetForeColor(15);
    				HsStatusSetGadgetType (form, YOMEN, hsStatusGadgetSignal);
    	    		HsStatusUpdateGadgets();
    				WinSetForeColor(prevColor);
    			}
    			else
    	    		HsStatusUpdateGadgets();
        	}
		}

		//error=HsCardPower (1, hsCardPowerCmdBatteryPercent, &batt );
		SysBatteryInfo (false, NULL, NULL, NULL, NULL, NULL, &batt);
		
		// add these lines for testing battery indicator behaviours
		//		batt=62;
		//		anim->lastBattery=99;
		
		if (anim->lastBattery!=batt)
		{
			anim->lastBattery=batt;
			batt=(((100-batt)*138)/100)+22;

            resBFull = DmGetResource (bitmapRsc, BATTERY);
            bitBFull = (BitmapPtr) MemHandleLock (resBFull);
			WinDrawBitmap(bitBFull,BATTERY_X,BATTERY_Y);
            MemHandleUnlock (resBFull);
            DmReleaseResource (resBFull);
	
			// calculate indicator pixels position
	        xBatt=(sin((batt * (PI / 180)))*BATTERY_RAGGIO);
        	yBatt=-(cos((batt * (PI / 180)))*BATTERY_RAGGIO);
        	
        	line.x1=BATTERY_CX; line.y1=BATTERY_CY;
        	line.x2=BATTERY_CX+xBatt; line.y2=BATTERY_CY+yBatt;
            MatroPaintAliasedLine(&line, false);

			
/*			
			// old code with old battery indicator,
			// never published: here just for future reference...

			// carico le resbitmapz "batteryfull" e "batteryempty"
            resBFull = DmGetResource (bitmapRsc, BATTERYFULL);
            bitBFull = (BitmapPtr) MemHandleLock (resBFull);
            resBEmpty = DmGetResource (bitmapRsc, BATTERYEMPTY);
            bitBEmpty = (BitmapPtr) MemHandleLock (resBEmpty);
			
			// creo membitmapz nuove e relative memwindowz
			nbitBFull=BmpCreate(BATTERY_W,BATTERY_H,8,BmpGetColortable(bitBFull),&error);
			wbitBFull = WinCreateBitmapWindow (nbitBFull, &error);
			nbitBEmpty=BmpCreate(BATTERY_W,BATTERY_H,8,BmpGetColortable(bitBEmpty),&error);
			wbitBEmpty = WinCreateBitmapWindow (nbitBEmpty, &error);
			
			// copio le resbitmapz nelle membitmapz
			wold=WinSetDrawWindow(wbitBFull);
			WinDrawBitmap(bitBFull,0,0);
			wold=WinSetDrawWindow(wbitBEmpty);
			WinDrawBitmap(bitBEmpty,0,0);

			// copio porzione di batteryfull sul display...
			srcRect.topLeft.x=0;
			srcRect.topLeft.y=BATTERY_H-batt;
			srcRect.extent.x =BATTERY_W;
			srcRect.extent.y =batt;
			WinSetDrawWindow(wold);
			WinCopyRectangle (wbitBFull, WinGetDrawWindow(), &srcRect, BATTERY_X, BATTERY_Y+(BATTERY_H-batt), winPaint); 		

			// copio porzione di batteryempty sul display...
			srcRect.topLeft.x=0;
			srcRect.topLeft.y=0;
			srcRect.extent.x =BATTERY_W;
			srcRect.extent.y =BATTERY_H-batt;
			WinSetDrawWindow(wold);
			WinCopyRectangle (wbitBEmpty, WinGetDrawWindow(), &srcRect, BATTERY_X, BATTERY_Y, winPaint); 		
			
			// free de tutto er ciborio			
		    WinDeleteWindow (wbitBFull, false);
		    WinDeleteWindow (wbitBEmpty, false);
			BmpDelete(nbitBFull);
			BmpDelete(nbitBEmpty);
            MemHandleUnlock (resBFull);
            MemHandleUnlock (resBEmpty);
            DmReleaseResource (resBFull);
            DmReleaseResource (resBEmpty);
	*/
		}	
	}
}

void TimeRefresh(FormPtr form, AnimGlobals *anim)
{
    int h;
	FontID prevFont;
    UInt32 now;
    UInt16 objIndex;
	Int16 lbCLOCKwd;
	char time[timeStringLength+dowDateStringLength+4];
	char date[dowDateStringLength+1];
	
    now=TimGetSeconds();
    TimSecondsToDateTime(now,&(anim->Time));
	MemSet(time,timeStringLength+dowDateStringLength+4,0);

    anim->changedSecond=(anim->Time.second!=anim->lastSecond);
    anim->changedMinute=(anim->Time.minute!=anim->lastMinute);
	anim->lastSecond=anim->Time.second;
	anim->lastMinute=anim->Time.minute;

	if (anim->changedSecond)
	{
        anim->lastTicks=TimGetTicks();
		anim->clockInitOk=true;
	}

	if (anim->changedMinute)
	{
		TimeToAscii(anim->Time.hour, anim->Time.minute, PrefGetPreference(prefTimeFormat), &time[0]);
		DateToDOWDMFormat(anim->Time.month, anim->Time.day, anim->Time.year, PrefGetPreference(prefLongDateFormat), &date[0]);
		StrCat(time, "   ");
		StrCat(time,date); 		
		prevFont=FntSetFont(boldFont);
		lbCLOCKwd=FntCharsWidth(time,StrLen(time));
        objIndex=FrmGetObjectIndex(form, lbCLOCK);
        FrmHideObject(form, objIndex);
        FrmCopyLabel(form, lbCLOCK, &time[0]);
		FrmSetObjectPosition (form, objIndex, 
			((LBCLOCKSP-lbCLOCKwd)/2), 146);
        FrmShowObject(form, objIndex);
		FntSetFont(prevFont);

	}
}

void ProviderRefresh(FormPtr form, AnimGlobals *anim)
{
	TreoInfo *treoinfo;

    int h;
	FontID prevFont;
    UInt16 objIndex, objYomen;
	Int16 lbCLOCKwd, myLBSIGNAL_W=LBSIGNAL_W, myLBSIGNAL_DIST=LBSIGNAL_DIST;

    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);

	if (treoinfo->isRefreshed)
	{
		char carriershown[64];

		// custom carrier string (hardcoded for now :)
		StrCopy(carriershown, treoinfo->carrier);
		if (StrCompare(carriershown, "OMNI")==0)
			StrCopy(carriershown, "OMNITEL");
		
		treoinfo->isRefreshed=false;
		prevFont=FntSetFont(largeBoldFont);
		lbCLOCKwd=FntCharsWidth(carriershown,StrLen(carriershown));
        objIndex=FrmGetObjectIndex(form, lbCARRIER);
		
		if (treoinfo->PowerState==kPowerOff)
		{
			myLBSIGNAL_W=0; myLBSIGNAL_DIST=0;
		}

        objYomen=FrmGetObjectIndex(form, YOMEN);
        FrmHideObject(form, objYomen);
  		FrmSetObjectPosition (form, objYomen, 
   			((LBCLOCKSP-lbCLOCKwd)/2)+lbCLOCKwd+myLBSIGNAL_DIST, 5);		
        FrmShowObject(form, objYomen);
		
        FrmHideObject(form, objIndex);
        FrmCopyLabel(form, lbCARRIER, carriershown);
		FrmSetObjectPosition (form, objIndex, 
			((LBCLOCKSP-lbCLOCKwd)/2), 4);
        FrmShowObject(form, objIndex);
		FntSetFont(prevFont);

		// custom carrier string (hardcoded for now :)
		StrCopy(carriershown, treoinfo->roaming);
		if (StrCompare(carriershown, "OMNI")==0)
			StrCopy(carriershown, "OMNITEL");
		
		prevFont=FntSetFont(stdFont);
		lbCLOCKwd=FntCharsWidth(carriershown,StrLen(carriershown));
        objIndex=FrmGetObjectIndex(form, lbROAMING);
        FrmHideObject(form, objIndex);
        FrmCopyLabel(form, lbROAMING, carriershown);
		FrmSetObjectPosition (form, objIndex, 
			((LBCLOCKSP-lbCLOCKwd)/2), 19);
        FrmShowObject(form, objIndex);
		FntSetFont(prevFont);
	}
}

void AnimInit()
{
  AnimGlobals *anim;

  anim = (AnimGlobals *)MemPtrNew(sizeof(AnimGlobals));
  MemSet(anim, sizeof(AnimGlobals), 0);
  FtrSet(appCreator, ftrAnimGlobals, (UInt32)anim);

  anim->lastTicks=TimGetTicks();
  anim->lastSecond=-1;
  anim->lastMinute=-1;
  anim->lastBattery=-1;  
}

void AnimEnd()
{
  AnimGlobals *anim;

  FtrGet(appCreator, ftrAnimGlobals, (UInt32 *)&anim);
  MemPtrFree(anim);
  FtrUnregister(appCreator, ftrAnimGlobals);
}

void DoAnim(FormPtr form)
{
    UInt8 *ptrScreen;
	MemHandle myres;	
	BitmapType *bitmap;
    AnimGlobals *anim;
	TreoInfo *treoinfo;
	WinHandle mainDrawWindow;

    myres = DmGetResource (bitmapRsc, CLOCKCIRCLE);
    (MemPtr) bitmap = MemHandleLock (myres);

    FtrGet(appCreator, ftrAnimGlobals, (UInt32 *)&anim);
    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);

	// handle doublebuffering	
	GraphicsClear();
	mainDrawWindow=WinGetDrawWindow();
    ptrScreen = (UInt8 *)DeviceWindowGetPointer(GraphicsGetDrawWindow());
	WinSetDrawWindow(GraphicsGetDrawWindow());

    //test: fill doublebuffering area
    //WinDrawRectangle(&rect, 0);

    WinPaintBitmap(bitmap, CLOCKCIRCLE_X, CLOCKCIRCLE_Y);
    MemHandleUnlock (myres);
    DmReleaseResource (myres);

	// just this for now :-)
	ClockAnim(anim);
	
	WinSetDrawWindow(mainDrawWindow);
	
    GraphicsRepaint();
	
	// refresh other main form datas not subjected
	// to doublebuffering
#ifdef SHOWDEBUG
    DebugRefresh(form, anim);
#endif

	TimeRefresh(form, anim);
	ProviderRefresh(form, anim);
	BatteryRefresh(form, anim, treoinfo);
		
}


