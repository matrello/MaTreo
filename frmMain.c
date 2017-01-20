/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	designed for Handspring Treo (PalmOS3.5H)
*/

#include <PalmOS.h>
#include <HsExt.h>
#include <GSMLibrary.h>
#include "anim.h"
#include "matro2.h"
#include "matro2_res.h"   
#include "sysstuff.h"

#define kHandspringPhoneApp      'HsPh'

Boolean frmMain_HandleEvent(EventPtr event)
{
	FormPtr form;
	Boolean handled = false;
	Matro *matro;
	TreoInfo *treoinfo;	

    FtrGet(appCreator, ftrMatro, (UInt32 *)&matro);
    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);

	switch (event->eType)
	{
		case frmOpenEvent:
			form = FrmGetActiveForm();
			FrmDrawForm(form);
			
			if (treoinfo->gsmenabled)
				HsStatusSetGadgetType (form, YOMEN, hsStatusGadgetSignal);
	
			// can I turn on the light? :-)
			AccendoLaLuce();
			
			handled = true;
			AnimInit();
						
			break;
		
		case penDownEvent:	
		
			matro->LaunchOtherApp=true;
			break;	

        case nilEvent:
    
             if (WinGetActiveWindow() == (WinHandle)FrmGetActiveForm()) 
             {
    			form = FrmGetActiveForm();
    		    DoAnim(form);
				
				if (matro->switchapp_ticksStart>0 && treoinfo->switchapp_sec)
				{
					if ((TimGetTicks()-matro->switchapp_ticksStart)>matro->switchapp_ticksEnd)
						matro->LaunchOtherApp=true;					
				}
             }
             break;
    
        case frmCloseEvent:
    
             AnimEnd();
             break;
    
    		default:
    			break;
	}

	return handled;
}

