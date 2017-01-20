/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	designed for Handspring Treo (PalmOS3.5H)
*/

#include <PalmOS.h>
#include <SystemMgr.h>
#include <GSMLibrary.h>
#include "sysstuff.h"
#include "matro2.h"
#include "matro2_res.h"   

Int16 optFromHours, optFromMinutes;
Int16 optToHours, optToMinutes;

static char from[timeStringLength+2];
static char to[timeStringLength+2];

#define OPTIONS_BACKLIGHT_TO_Y 116
#define SWITCHAPP_LOAD			 0
#define SWITCHAPP_UNLOADOK		 1
#define SWITCHAPP_UNLOADCANCEL	 2 

void SetCheckBox(int id,short b)
{
   FormPtr   form;
   ControlPtr check;

   form = FrmGetActiveForm();
   check = (ControlPtr)FrmGetObjectPtr(form, FrmGetObjectIndex(form,id));
   CtlSetValue(check, b);
}

Int16 GetCheckBox(int id)
{
   FormPtr   form;
   ControlPtr check;

   form = FrmGetActiveForm();
   check = (ControlPtr)FrmGetObjectPtr(form, FrmGetObjectIndex(form,id));
   return CtlGetValue(check);
}

void ShowEdit(UInt16 start)
{
  FormPtr form;
  form= FrmGetActiveForm ();
  FrmShowObject(form, FrmGetObjectIndex(form,start));
}

void HideEdit(UInt16 start)
{
  FormPtr form;
  form= FrmGetActiveForm ();
  FrmHideObject(form, FrmGetObjectIndex(form,start));
}

void SetFieldText(UInt16 id_in, char *text_in_t, UInt8 length)
{
  char *new_field_t;
  FieldPtr field_p;
  MemHandle old_handle;
  MemHandle new_handle;
  int field_index;

  field_index = FrmGetObjectIndex(FrmGetActiveForm(), id_in);
  field_p = FrmGetObjectPtr(FrmGetActiveForm(), field_index);

  if (field_p == NULL)
    return;

  old_handle = (MemHandle) FldGetTextHandle(field_p);
  new_handle = MemHandleNew(length) ;
  new_field_t = MemHandleLock(new_handle);
  StrCopy(new_field_t, text_in_t);

  FldSetTextHandle(field_p, (MemHandle) new_handle);
  MemHandleUnlock(new_handle);

  if (old_handle == NULL)
    return;
  MemHandleFree(old_handle);
}

void GetFieldText(UInt16 id,char *target) 
{
  FieldPtr field_p;
  int field_index;
  MemHandle old_handle;
  char *new_field_t;

  field_index = FrmGetObjectIndex(FrmGetActiveForm(), id);
  field_p = FrmGetObjectPtr(FrmGetActiveForm(), field_index);
  old_handle = (MemHandle) FldGetTextHandle(field_p);
  new_field_t = MemHandleLock(old_handle);
  StrCopy(target,new_field_t);
  MemHandleUnlock(old_handle);
}

void BacklightSelectorRefresh()
{
    UInt16 objto, objlbto;
	Int16 fromwd;
	
	ControlPtr ctl;
	FormType *formP = FrmGetActiveForm();
    MemSet(from, sizeof(from), 0);
    MemSet(to, sizeof(to), 0);
    TimeToAscii(optFromHours, optFromMinutes, PrefGetPreference(prefTimeFormat), &from[0]);
    TimeToAscii(optToHours, optToMinutes, PrefGetPreference(prefTimeFormat), &to[0]);
	fromwd=FntCharsWidth(from,StrLen(from));
	
    objto=FrmGetObjectIndex(formP, OPTIONS_BACKLIGHT_TO);
	objlbto=FrmGetObjectIndex(formP, OPTIONS_BACKLIGHT_LBTO);  
	FrmHideObject(formP, objlbto);
	FrmHideObject(formP, objto);
	ctl = (ControlPtr) FrmGetObjectPtr(formP, FrmGetObjectIndex(formP, OPTIONS_BACKLIGHT_FROM));
	CtlSetLabel(ctl, from);
    FrmSetObjectPosition (formP, objlbto, 53+fromwd, OPTIONS_BACKLIGHT_TO_Y);
    FrmSetObjectPosition (formP, objto, 65+fromwd, OPTIONS_BACKLIGHT_TO_Y);
	ctl = (ControlPtr) FrmGetObjectPtr(formP, objto);
	CtlSetLabel(ctl, to);
    FrmShowObject(formP, objlbto);
    FrmShowObject(formP, objto);

}

SwitchAppSelectorHandler(TreoInfo *treoinfo, UInt8 mode)
{
    static SysDBListItemType	*dbListIDsP;
    static MemHandle            dbListIDsH;
	static MemPtr 				dbListArray;
	Char						**dbListArrayItem;
	FormPtr						form;
	ListPtr						switchlist, switchlistJog;
	UInt16						switchtext, switchtextJog;
    UInt16                		dbCount = 0, counter, selected=-1, selectedJog=-1;
    Boolean                    	status;

    form = FrmGetActiveForm();
    
	if (mode!=SWITCHAPP_LOAD)
	{
		if (mode==SWITCHAPP_UNLOADOK)
		{
		    switchlist = FrmGetObjectPtr(form, FrmGetObjectIndex(form, OPTIONS_APPLIST)); 
		    switchlistJog = FrmGetObjectPtr(form, FrmGetObjectIndex(form, OPTIONS_APPLISTJOG)); 
			treoinfo->switchapp_dbID=dbListIDsP[LstGetSelection(switchlist)].dbID;
			treoinfo->switchapp_cardNo=dbListIDsP[LstGetSelection(switchlist)].cardNo;
			treoinfo->switchapp_creator=dbListIDsP[LstGetSelection(switchlist)].creator;
			treoinfo->switchapp_dbIDJog=dbListIDsP[LstGetSelection(switchlistJog)].dbID;
			treoinfo->switchapp_cardNoJog=dbListIDsP[LstGetSelection(switchlistJog)].cardNo;
			treoinfo->switchapp_creatorJog=dbListIDsP[LstGetSelection(switchlistJog)].creator;
		}
		
		MemHandleUnlock(dbListIDsH);
		MemPtrFree(dbListArray);
		return;
	}
	
    status = SysCreateDataBaseList('appl', 0, &dbCount, &dbListIDsH, true);

    if (status == true && dbCount > 0)
    {
	    switchtext = FrmGetObjectIndex(form, OPTIONS_SWITCHAPP);
	    switchtextJog = FrmGetObjectIndex(form, OPTIONS_SWITCHAPPJOG);
	    switchlist = FrmGetObjectPtr(form, FrmGetObjectIndex(form, OPTIONS_APPLIST));
		switchlistJog =  FrmGetObjectPtr(form, FrmGetObjectIndex(form, OPTIONS_APPLISTJOG));
		dbListIDsP = (SysDBListItemType*)MemHandleLock (dbListIDsH);
		dbListArray=MemPtrNew(dbCount*4);
		dbListArrayItem=(char **)dbListArray;
		for (counter=0; counter<dbCount; counter++)
		{
			*dbListArrayItem=dbListIDsP[counter].name;
			dbListArrayItem++;			
			if (dbListIDsP[counter].dbID==treoinfo->switchapp_dbID)
				selected=counter;
			if (dbListIDsP[counter].dbID==treoinfo->switchapp_dbIDJog)
				selectedJog=counter;
		}
		
	    LstSetListChoices(switchlist, dbListArray, dbCount);
	    LstSetListChoices(switchlistJog, dbListArray, dbCount);

		LstSetSelection(switchlist, selected);
		LstSetSelection(switchlistJog, selectedJog);
		CtlSetLabel((ControlPtr) FrmGetObjectPtr(form, switchtext),dbListIDsP[selected].name);
		CtlSetLabel((ControlPtr) FrmGetObjectPtr(form, switchtextJog),dbListIDsP[selectedJog].name);

    }	
	
}

Boolean frmOptions_HandleEvent(EventPtr event)
{
	FormPtr form;
	Boolean handled = false;
	char *CodeByMatro;

	TreoInfo *treoinfo;	
    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);
	
	switch (event->eType)
	{
		case ctlSelectEvent:
			switch (event->data.ctlSelect.controlID)
			{
				case OPTIONS_BACKLIGHT_TO:
					handled = frmOptions_OPTIONS_BACKLIGHT_TO_OnSelect(event);
					break;
				case OPTIONS_BACKLIGHT_FROM:
					handled = frmOptions_OPTIONS_BACKLIGHT_FROM_OnSelect(event);
					break;
				case OPTIONS_CANCEL:
					handled = frmOptions_OPTIONS_CANCEL_OnSelect(event);
					break;
				case OPTIONS_OK:
					handled = frmOptions_OPTIONS_OK_OnSelect(event);
					break;
			}
			break;
		case frmOpenEvent:
			form = FrmGetActiveForm();
			FrmDrawForm(form);
			handled = true;

			optFromHours=treoinfo->backlightFrom_hours;
			optFromMinutes=treoinfo->backlightFrom_minutes;
			optToHours=treoinfo->backlightTo_hours;
			optToMinutes=treoinfo->backlightTo_minutes;

			HideEdit(OPTIONS_TITLE);
			SetFieldText(OPTIONS_TITLE,treoinfo->title,StrLen(treoinfo->title)+1);
			ShowEdit(OPTIONS_TITLE);
			SetCheckBox(OPTIONS_BACKLIGHT, treoinfo->backlightOn);
			SetCheckBox(OPTIONS_WIRELESS, treoinfo->gsmenabledreq);
			SetCheckBox(OPTIONS_SWITCHAPP_TAP, treoinfo->switchapp_tap);
			SetCheckBox(OPTIONS_SWITCHAPP_JOG, treoinfo->switchapp_jog);
			SetCheckBox(OPTIONS_SWITCHAPP_SEC, treoinfo->switchapp_sec);

			HideEdit(OPTIONS_SWITCHAPP_SECNUM);
            CodeByMatro = (char *)MemPtrNew(3);
            MemSet(CodeByMatro, 3, 0);
			StrIToA(CodeByMatro, treoinfo->switchapp_secnum);
			SetFieldText(OPTIONS_SWITCHAPP_SECNUM,CodeByMatro,StrLen(CodeByMatro)+1);
			MemPtrFree(CodeByMatro);
			ShowEdit(OPTIONS_SWITCHAPP_SECNUM);

			SwitchAppSelectorHandler(treoinfo, SWITCHAPP_LOAD);
			
			BacklightSelectorRefresh();

			FrmSetFocus(form, FrmGetObjectIndex(form,OPTIONS_TITLE));
			
			break;
		default:
			break;
	}

	return handled;
}

static Boolean frmOptions_OPTIONS_OK_OnSelect(EventPtr event)
{
	char *CodeByMatro;

	TreoInfo *treoinfo;	
    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);

	if ( (treoinfo->RegistrationStatus!=registrationRoaming) &&
	     (StrCompare(treoinfo->title, treoinfo->roaming)==0) )
		GetFieldText(OPTIONS_TITLE, treoinfo->roaming);

	GetFieldText(OPTIONS_TITLE, treoinfo->title);
    treoinfo->backlightOn=GetCheckBox(OPTIONS_BACKLIGHT);
    treoinfo->backlightFrom_hours=optFromHours;
    treoinfo->backlightFrom_minutes=optFromMinutes;
    treoinfo->backlightTo_hours=optToHours;
    treoinfo->backlightTo_minutes=optToMinutes;
    treoinfo->gsmenabledreq=GetCheckBox(OPTIONS_WIRELESS);
	treoinfo->switchapp_tap=GetCheckBox(OPTIONS_SWITCHAPP_TAP);
	treoinfo->switchapp_jog=GetCheckBox(OPTIONS_SWITCHAPP_JOG);
	treoinfo->switchapp_sec=GetCheckBox(OPTIONS_SWITCHAPP_SEC);

    CodeByMatro = (char *)MemPtrNew(3);
    MemSet(CodeByMatro, 3, 0);
	GetFieldText(OPTIONS_SWITCHAPP_SECNUM, CodeByMatro);
	treoinfo->switchapp_secnum=StrAToI(CodeByMatro);	
	MemPtrFree(CodeByMatro);

    SwitchAppSelectorHandler(treoinfo, SWITCHAPP_UNLOADOK);
	
	if(treoinfo->gsmenabledreq!=treoinfo->gsmenabled)
	{
		FrmCustomAlert(altInfo, 
		"In order to see changes, exit MaTreo and run it again." 
			, NULL, NULL);	
	}
	
	treoinfo->isRefreshed=true;

	FrmReturnToForm(frmMain);
	return true;
}


static Boolean frmOptions_OPTIONS_CANCEL_OnSelect(EventPtr event)
{
    SwitchAppSelectorHandler(NULL, SWITCHAPP_UNLOADCANCEL);

	FrmReturnToForm(frmMain);
	return true;
}

static Boolean frmOptions_OPTIONS_BACKLIGHT_FROM_OnSelect(EventPtr event)
{
	Int16 hh, mm;

	hh=optFromHours;
	mm=optFromMinutes;
	if (SelectOneTime (&hh, &mm, "Backlight on from"))
	{
    	optFromHours=hh;
    	optFromMinutes=mm;
	    BacklightSelectorRefresh();
	}

	return true;
}

static Boolean frmOptions_OPTIONS_BACKLIGHT_TO_OnSelect(EventPtr event)
{
	Int16 hh, mm;

	hh=optToHours;
	mm=optToMinutes;
	if (SelectOneTime (&hh, &mm, "Backlight on to"))
	{
    	optToHours=hh;
    	optToMinutes=mm;
	    BacklightSelectorRefresh();
	}

	return true;
}
