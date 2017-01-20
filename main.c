/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	designed for Handspring Treo (PalmOS3.5H)
*/
 
#include <PalmOS.h>
#include <HsExt.h>
#include <GSMLibrary.h>
#include "sysstuff.h"
#include "mathlib.h"
#include "device.h"
#include "graphics.h"
#include "matro2.h"
#include "matro2_res.h"

#define kHandspringPhoneApp      'HsPh'

Boolean ApplicationHandleMenu(UInt16 menuID)
{
	Boolean handled = false;
	UInt16  ret;
	
	switch (menuID)
	{
		case mnuOptions:
			FrmPopupForm(frmOptions);
			break;
		case mnuAbout:
			ret=FrmAlert(altAbout);
			if (ret==1)
				FrmAlert(altGreets);
			if (ret==2)
				FrmAlert(altThanks);
			handled=true;
			break;
		default:
			break;
	}

	return handled;
}

Boolean LoadMathLibrary()
{
	Err error;

    error = SysLibFind(MathLibName, &MathLibRef);
    if (error)
        error = SysLibLoad(LibType, MathLibCreator, &MathLibRef);
		
	if (error)
	{
		FrmCustomAlert(altAlert, 
		"Could not find mathlib library. If it's not already installed, download it from Palmgear.com." 
			, NULL, NULL);
	}
	else
	{
	    error = MathLibOpen(MathLibRef, MathLibVersion);

    	if (error)
    		FrmCustomAlert(altAlert, 
			"Could not open mathlib library, it's locked by other apps or it was left open."
				, NULL, NULL);
	}
	
	return !error; 
}
 
static Boolean ApplicationHandleEvent(EventPtr event)
{
	UInt16 formID, objYomen;
	FormPtr form;
	Boolean handled = false;
	Matro *matro;
	Err error=0;

	FtrGet(appCreator, ftrMatro, (UInt32 *)&matro);
	
	switch (event->eType)
	{
		case keyDownEvent:
			if (event->data.keyDown.chr==hsChrJogScan)
				matro->LaunchOtherAppJog=true;
			break;
		case menuEvent:
			handled = ApplicationHandleMenu(event->data.menu.itemID);
			break;
		case frmLoadEvent:
			formID = event->data.frmLoad.formID;
			form = FrmInitForm(formID);
			FrmSetActiveForm(form);		

			switch (formID)
			{
				case frmOptions:
					FrmSetEventHandler(form, (FormEventHandlerPtr) frmOptions_HandleEvent);
					break;
				case frmMain:
					FrmSetEventHandler(form, (FormEventHandlerPtr) frmMain_HandleEvent);
					break;

				default:
					break;
			}
			handled = true;
			break;
			
        case winEnterEvent:
             {
               if (event->data.winEnter.enterWindow == 
                    (WinHandle)FrmGetFormPtr(frmMain))
			   {
                 matro->evtTimeOut = 1;
               }
             }
             break;
        
        case winExitEvent:
             {
               if (event->data.winEnter.enterWindow == 
                    (WinHandle)FrmGetFormPtr(frmMain))
			   {
                 matro->evtTimeOut = evtWaitForever;
               }
             }
             break;
			
		default:
			break;
	}

	return handled;
}

static void EventLoop(void)
{
	Err error;
	Int32 dummy, appType=0;
	EventType event;
    FormType *frm;
	Matro *matro;
	TreoInfo *treoinfo;	
	DmSearchStateType searchState; 

    FtrGet(appCreator, ftrMatro, (UInt32 *)&matro);
    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);
    matro->timerPointA = TimGetTicks();

	do
	{
		EvtGetEvent(&event, matro->evtTimeOut);
	    frm = FrmGetActiveForm();

        if ((WinGetActiveWindow() == (WinHandle)frm) &&
            (frm == FrmGetFormPtr(frmMain)) && (event.eType == nilEvent))
          matro->timerPointA = TimGetTicks();
		
		if (!SysHandleEvent(&event))
		{
			if (!MenuHandleEvent(0, &event, &error))
			{
				if (!ApplicationHandleEvent(&event))
					FrmDispatchEvent(&event);	
			}
		}

        if (matro->LaunchOtherApp && treoinfo->switchapp_tap && treoinfo->switchapp_dbID)
		{
			GetDefaultSwitchTo(&treoinfo->switchapp_cardNo, &treoinfo->switchapp_dbID, &treoinfo->switchapp_creator);

       	    if (SysUIAppSwitch(treoinfo->switchapp_cardNo, treoinfo->switchapp_dbID, sysAppLaunchCmdNormalLaunch, 0)!=0)
            {
            	EventType newEvent;
                newEvent.eType = keyDownEvent;
                newEvent.data.keyDown.chr = launchChr;
                newEvent.data.keyDown.modifiers = commandKeyMask;
                EvtAddEventToQueue (&newEvent);
            }
            matro->LaunchOtherApp=false;		
		}
		else
			matro->LaunchOtherApp=false;		

        if (matro->LaunchOtherAppJog && treoinfo->switchapp_jog && treoinfo->switchapp_dbIDJog)
		{
			GetDefaultSwitchTo(&treoinfo->switchapp_cardNoJog, &treoinfo->switchapp_dbIDJog, &treoinfo->switchapp_creatorJog);

       	    if (SysUIAppSwitch(treoinfo->switchapp_cardNoJog, treoinfo->switchapp_dbIDJog, sysAppLaunchCmdNormalLaunch, 0)!=0)
            {
            	EventType newEvent;
                newEvent.eType = keyDownEvent;
                newEvent.data.keyDown.chr = launchChr;
                newEvent.data.keyDown.modifiers = commandKeyMask;
                EvtAddEventToQueue (&newEvent);
            }
			else
				matro->LaunchOtherAppJog=false;		
		}
		else
			matro->LaunchOtherAppJog=false;		
				
        if ((WinGetActiveWindow() == (WinHandle)frm) &&
            (frm == FrmGetFormPtr(frmMain)))
        {
          matro->timerPointB = TimGetTicks();
    
          // double buffering: calculate the delay required
          matro->timerDiff = (matro->timerPointB - matro->timerPointA);
          matro->evtTimeOut = (matro->timerDiff > matro->ticksPerFrame) ?
            1 : (matro->ticksPerFrame - matro->timerDiff);
    
          // double buffering: manually add nilEvent if needed
		  // (only when pen held down)
          if ((matro->evtTimeOut <= 1) && (event.eType == penMoveEvent))
          {
            EventType event;
    
            MemSet(&event, sizeof(EventType), 0);
            event.eType = nilEvent;
            EvtAddEventToQueue(&event);
          }
        }
	}
	while (event.eType != appStopEvent);
}

UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
	GSMEventRecord* eventP = (GSMEventRecord*)cmdPBP;
    DmSearchStateType searchState; 
    LocalID dbID;
	Err error = 0;
	UInt32 result=0;
	Boolean libLoaded = false;
	UInt16 libRef = 0, usecount, cardNo;
    Matro *matro;
	
	Boolean DeviceChkOK=false, MathLibraryOK=false, MatreoOK=false, DeviceOK=false, GraphicsOK=false;

	switch (cmd) 
 	{
		case sysAppLaunchCmdNormalLaunch:

            matro = (Matro *)MemPtrNew(sizeof(Matro));
            MemSet(matro, sizeof(Matro), 0);
            matro->evtTimeOut    = evtWaitForever;
            matro->ticksPerFrame = (Int16)(SysTicksPerSecond() / ANIM_FPS);
            FtrSet(appCreator, ftrMatro, (UInt32)matro);

			DeviceChkOK=DeviceCheckCompatability();
			MathLibraryOK=LoadMathLibrary();
			
			if (DeviceChkOK && MathLibraryOK)
				MatreoOK=MaTreoInitialize(true, &matro->gsmLibRef, &matro->gsmLibLoaded);

			if (MatreoOK)
            	DeviceOK=DeviceInitialize();	

			if (DeviceOK)
				GraphicsOK=GraphicsInitialize();
				
			if (DeviceChkOK && MathLibraryOK && MatreoOK && DeviceOK && GraphicsOK)
			{
	            FrmGotoForm(frmMain);			
	            EventLoop();			
                FrmCloseAllForms();
   			}
			
			if (GraphicsOK)
                GraphicsTerminate();
				
			if (DeviceOK)	
                DeviceTerminate();
				
			if (MatreoOK)
    			MaTreoTerminate(true);
			
			if (MathLibraryOK)
			{
                error = MathLibClose(MathLibRef, &usecount);
                ErrFatalDisplayIf(error, "MaTreo: Can't close MathLib.");
                if (usecount == 0)
    	            SysLibRemove(MathLibRef);
			}
			
            FtrGet(appCreator, ftrMatro, (UInt32 *)&matro);
            MemPtrFree(matro);
	
            break;

		case gsmAppLaunchCmdEvent:

			// following code workarounds a probable bug in Handspring's
			// kButton handling which caused kHeadset event not to be
			// propagated to Phonebook+ app
			if (eventP->event==kButton) {
	            DmGetNextDatabaseByTypeCreator(true, &searchState, sysFileTApplication, 
	            	kHandspringPhoneApp, true, &cardNo, &dbID);          
            	if (dbID) {
	    	        error = SysAppLaunch(cardNo, dbID, launchFlags, 
						gsmAppLaunchCmdEvent, cmdPBP, &result);		
			    	eventP->acknowledge = false;
					return error;
				}	
			}
		
			if (!(launchFlags & sysAppLaunchFlagSubCall)) 
				error = !MaTreoInitialize(false, NULL, NULL);

			error = MaTreoHandleEvent(cmdPBP, launchFlags);

			if (!(launchFlags & sysAppLaunchFlagSubCall)) 
				MaTreoTerminate(false);

			break;

		default:
			break;

	}
	
	return error;
}

