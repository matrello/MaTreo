/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	designed for Handspring Treo (PalmOS3.5H)
*/

#include <PalmOS.h>
#include <HsExt.h>
#include <GSMLibrary.h>
#include "matro2.h"
#include "matro2_res.h"
#include "sysstuff.h"

#define kHandspringPhoneApp      'HsPh'
#define kHandspringDateBookPlus  'HsDB'
#define kPalmDateBook            'date'

Boolean	HwrBacklight(Boolean set, Boolean newState) SYS_TRAP(sysTrapHwrBacklightV33);

static void AddKeyDownEvent(Int16 character)
{
   EventType theEvent;

   theEvent.eType = keyDownEvent;
   theEvent.data.keyDown.chr = character;
   theEvent.data.keyDown.keyCode = 0;
   theEvent.data.keyDown.modifiers = commandKeyMask;

   EvtAddEventToQueue(&theEvent);
}

void AccendoLaLuce()
{	
	// this routine turns on backlight
	// if current hour falls down
	// user selected range

	TreoInfo *treoinfo;	
	DateTimeType Time;
    UInt32 now;
	Int16 iNow, iFrom, iTo;

	if (HwrBacklight(false, false))
		return;
		
    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);
	if (treoinfo->backlightOn) {		
        now=TimGetSeconds();
        TimSecondsToDateTime(now,&Time);
		iNow=(Time.hour*60)+Time.minute;
		iFrom=(treoinfo->backlightFrom_hours*60)+treoinfo->backlightFrom_minutes;
		iTo=(treoinfo->backlightTo_hours*60)+treoinfo->backlightTo_minutes;
    
    	if (iFrom < iTo) {
    		if ( (iNow >= iFrom) && (iNow <= iTo) )		
    			AddKeyDownEvent(vchrBacklight);
    	}
    	else {
    		if ( (iNow >= iFrom) || (iNow <= iTo) )		
    			AddKeyDownEvent(vchrBacklight);
    	}
	}  		 
}

void GetDefaultSwitchTo(UInt16 *cardNo, LocalID *dbID, UInt32 *creator)
{
	DmSearchStateType searchState;
	UInt16 fcardNo;
	LocalID fdbID;
	Err ret=errNone;

	if (*creator)
	{
        ret=DmGetNextDatabaseByTypeCreator(true, &searchState, sysFileTApplication, 
             	*creator, true, &fcardNo, &fdbID);
		while (!(fcardNo==*cardNo && fdbID==*dbID) && ret==errNone)
		{
        ret=DmGetNextDatabaseByTypeCreator(false, &searchState, sysFileTApplication, 
             	*creator, true, &fcardNo, &fdbID);
		}
	}	
	
	if ((!*creator) || ret!=errNone)
	{	
		*creator=kHandspringPhoneApp;
        DmGetNextDatabaseByTypeCreator(true, &searchState, sysFileTApplication, 
             	*creator, true, cardNo, dbID);
        
        if (!dbID)
		{
			*creator=kHandspringDateBookPlus;
        	DmGetNextDatabaseByTypeCreator(true, &searchState, sysFileTApplication, 
                 	*creator, true, cardNo, dbID);
		}
        
        if (!dbID)
		{
			*creator=kPalmDateBook;		
            DmGetNextDatabaseByTypeCreator(true, &searchState, sysFileTApplication, 
                 	*creator, true, cardNo, dbID);
		}
	}
}

Err MaTreoInfoRefresh(TreoInfo *treoinfo)
{

	// TreoInfo is the structure which holds
	// treo state infos. this routine fills
	// the structure used by whole MaTreo

	Err error;

	treoinfo->isRefreshed=true;
	
	if (!treoinfo->gsmenabled)
	{
        StrCopy(treoinfo->carrier,treoinfo->title);
        StrCopy(treoinfo->roaming,"");
		return false;
	}

	if (treoinfo->PowerState==kPowerOff) {
        StrCopy(treoinfo->carrier,"wireless off");
        StrCopy(treoinfo->roaming,treoinfo->title);
	}
	else {
        switch (treoinfo->RegistrationStatus) {
            case registrationHome:
                error=MaTreoLoadCarrierInfo(treoinfo, false);
                break;
             case registrationRoaming:
                error=MaTreoLoadCarrierInfo(treoinfo, true);
                if (StrCompare(treoinfo->roaming, treoinfo->carrier)==0)
	                StrCopy(treoinfo->roaming,"");
                break;
		     default:
        		if (treoinfo->isSearchOnPowerOn) {
        			StrCopy(treoinfo->carrier,"searching...");
        			StrCopy(treoinfo->roaming,treoinfo->title);
				}
				else {
        			StrCopy(treoinfo->carrier,"no service");
        			StrCopy(treoinfo->roaming,treoinfo->title);
				}
				break;
	    }
	}
    return error;	
}

Err MaTreoLoadCarrierInfo(TreoInfo *treoinfo, Boolean isRoaming)
{	 		
	Matro *matro;
	LocalID dbID;
	DmOpenRef dbPtr = NULL;
	MemHandle dbRec;
	MemPtr dbRecPtr;
	Err error;
	Int16 i, matrello, numRecs;
	Boolean carrierSearch=false, roamingSearch=false;
	Boolean carrierFound=false, roamingFound=false;

    FtrGet(appCreator, ftrMatro, (UInt32 *)&matro);

    dbID = DmFindDatabase(0, "CurrentCarrier");
    if(!dbID)
      return true;
  
    dbPtr = DmOpenDatabase(0, dbID, dmModeReadOnly);
    if(dbPtr == NULL)
      return true;

    // roamingID
	if (isRoaming)
	{	
        dbRec=DmQueryRecord(dbPtr, 0);
        dbRecPtr=MemHandleLock(dbRec);	
        StrNCopy((char *)&(treoinfo->roamingID), dbRecPtr,5);
        StrCopy(treoinfo->roaming,"(not found :)");
	    roamingSearch=true;
        error=MemHandleUnlock(dbRec);
	}
	else
		StrCopy(treoinfo->roaming, treoinfo->title);

    // carrierID		
    dbRec=DmQueryRecord(dbPtr, 1);
    dbRecPtr=MemHandleLock(dbRec);	
    StrNCopy((char *)&(treoinfo->carrierID), dbRecPtr,5);
    StrCopy(treoinfo->carrier,"(not found :)");
    carrierSearch=true;
    error=MemHandleUnlock(dbRec);
    error = DmCloseDatabase(dbPtr);
 
 if (carrierSearch || roamingSearch)
 {
	dbID = DmFindDatabase(0, "CarrierList");
	if(!dbID)
		return true;
	
	dbPtr = DmOpenDatabase(0, dbID, dmModeReadOnly);
	if(dbPtr == NULL)
	    return true;
   
  numRecs=DmNumRecords(dbPtr);
  for (i=0;i<numRecs;i++)
  {
	dbRec=DmQueryRecord(dbPtr, i);
	dbRecPtr=MemHandleLock(dbRec);
   
   if (carrierSearch!=carrierFound)
   {
    	if (StrNCompare(dbRecPtr,(const char *)&(treoinfo->carrierID),3)==0)
        {
        	if (StrNCompare(dbRecPtr+4,(const char *) &(treoinfo->carrierID[3]),2)==0)
			{
			matrello=-1;
			while ( *((char *)dbRecPtr+7+ (++matrello) ) != ',')
				treoinfo->carrier[matrello]=* (char *)(dbRecPtr+7+matrello);
			treoinfo->carrier[matrello]=0;
						carrierFound=true;
			}       
        }
   }

   if (roamingSearch!=roamingFound)
   {
    	if (StrNCompare(dbRecPtr,(const char *)&(treoinfo->roamingID),3)==0)
        {
        	if (StrNCompare(dbRecPtr+4,(const char *) &(treoinfo->roamingID[3]),2)==0)
			{
			matrello=-1;
			while ( *((char *)dbRecPtr+7+ (++matrello) ) != ',')
				treoinfo->roaming[matrello]=* (char *)(dbRecPtr+7+matrello);
			treoinfo->roaming[matrello]=0;
						roamingFound=true;
			}       
        }
   }
  
   error=MemHandleUnlock(dbRec);
   if ((carrierSearch==carrierFound) && (roamingSearch==roamingFound))
    break;
  }	
     error = DmCloseDatabase(dbPtr);
   }
}


UInt32 MaTreoHandleEvent(MemPtr cmdPBP, UInt16 launchFlags)
{
	GSMEventRecord* eventP = (GSMEventRecord*)cmdPBP;
	UInt32 flags;
    Err error = 0;

	TreoInfo *treoinfo;	
    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);

	treoinfo->gsmNewEvent=true;
	treoinfo->gsmEV[3]=treoinfo->gsmEV[2];
	treoinfo->gsmEV[2]=treoinfo->gsmEV[1];
	treoinfo->gsmEV[1]=treoinfo->gsmEV[0];
	treoinfo->gsmEV[0]=eventP->event;

	treoinfo->gsmEVpar[3]=treoinfo->gsmEVpar[2];
	treoinfo->gsmEVpar[2]=treoinfo->gsmEVpar[1];
	treoinfo->gsmEVpar[1]=treoinfo->gsmEVpar[0];
	treoinfo->gsmEVpar[0]=99;
	
	switch(eventP->event)
	{
	 	case kPower:
			treoinfo->PowerState=eventP->data.power.state;
			treoinfo->gsmEVpar[0]=eventP->data.power.state;
			if (treoinfo->PowerState==kPowerOff)
				treoinfo->RegistrationStatus=registrationNone;
			else {
				treoinfo->RegistrationStatus=registrationSearch;
				treoinfo->isSearchOnPowerOn=true;
			}
			break;
		case kRegistration:
			treoinfo->RegistrationStatus=eventP->data.registration.status;
			treoinfo->gsmEVpar[0]=eventP->data.registration.status;
			break;
		case kIndication:
			treoinfo->gsmEVpar[0]=eventP->data.indication.kind;
			treoinfo->IndicationStatus=treoinfo->gsmEVpar[0];

			if (treoinfo->IndicationStatus==indicationNetworkSearch)
				treoinfo->RegistrationStatus=registrationSearch;			
			
			if (treoinfo->IndicationStatus==indicationSIMReady)
				treoinfo->isSearchOnPowerOn=false;			
			break;
/*		case kConnectInd:
			// accendo la luce? :-)
			if (treoinfo->onCall)
				AccendoLaLuce();
			break;*/
	}
    eventP->acknowledge = false;
	
	MaTreoInfoRefresh(treoinfo);
	
	return error;
}


static void * GetObjectPtr(UInt16 objectID)
{
	FormPtr frmP;

	frmP = FrmGetActiveForm();
	return FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, objectID));
}

Err MaTreoInitialize(Boolean normal, UInt16* libRefP, Boolean* libLoadedP)
{
	Err error = 0;
	Int32 appType=0, dummy=0;
	DmOpenRef myDB;

	Matro *matro;
	TreoInfo *treoinfo, prefTreoinfo;
	UInt16 treoinfoSize=sizeof(TreoInfo), prefsize;

    FtrGet(appCreator, ftrMatro, (UInt32 *)&matro);

    treoinfo = (TreoInfo *)MemPtrNew(sizeof(TreoInfo));
    MemSet(treoinfo, sizeof(TreoInfo), 0);
    MemSet(&prefTreoinfo, sizeof(TreoInfo), 0);
    FtrSet(appCreator, ftrTreoInfo, (UInt32)treoinfo);
	
	prefsize=PrefGetAppPreferences(appCreator, 0, &prefTreoinfo, &treoinfoSize, false);
	
	if (prefsize==noPreferenceFound || prefsize!=appPrefVersion) {

		 	// default options :)
		 	StrCopy(treoinfo->title, titleDefault);
			treoinfo->switchapp_tap=true;
			treoinfo->switchapp_jog=false;
			treoinfo->switchapp_sec=false;
			treoinfo->switchapp_secnum=3;
			treoinfo->backlightFrom_hours=18;
			treoinfo->backlightFrom_minutes=30;
			treoinfo->backlightTo_hours=7;
			treoinfo->backlightTo_minutes=00;
			treoinfo->backlightOn=false;
			treoinfo->gsmenabled=true;
			treoinfo->gsmenabledreq=true;

			treoinfo->switchapp_creator=0;
			GetDefaultSwitchTo(&treoinfo->switchapp_cardNo, &treoinfo->switchapp_dbID, &treoinfo->switchapp_creator);
			
			treoinfo->switchapp_dbIDJog=treoinfo->switchapp_dbID;
			treoinfo->switchapp_cardNoJog=treoinfo->switchapp_cardNo;						
			treoinfo->switchapp_creatorJog=treoinfo->switchapp_creator;						
		}
	else
	{
		MemMove(treoinfo, &prefTreoinfo,treoinfoSize);

		GetDefaultSwitchTo(&treoinfo->switchapp_cardNo, &treoinfo->switchapp_dbID, &treoinfo->switchapp_creator);
		GetDefaultSwitchTo(&treoinfo->switchapp_cardNoJog, &treoinfo->switchapp_dbIDJog, &treoinfo->switchapp_creatorJog);		
		
	}

	treoinfo->gsmEV[2]=99;
	treoinfo->gsmEV[1]=99;	treoinfo->gsmEV[0]=99;
    StrCopy(treoinfo->carrierID,"matro");
    StrCopy(treoinfo->roamingID,"matro");

	if (treoinfo->switchapp_sec)
	{
		matro->switchapp_ticksStart=TimGetTicks();
		matro->switchapp_ticksEnd=((UInt32)(SysTicksPerSecond()) * treoinfo->switchapp_secnum);
	}	
	
	if (normal)
	{	
		if (treoinfo->gsmenabled)
		{
    		if (MaTreoLoadGSMLibrary(libRefP, libLoadedP))
			{
        		FrmCustomAlert(altAlert, 
        		"Wireless infos are not available.\n\nIf this is a Treo, try a soft or warm reset, otherwise keep on using MaTreo! :)"
        			, NULL, NULL);
    			treoinfo->gsmenabled=false;
    			treoinfo->gsmenabledreq=false;
			}
			else
			{    			
            	if (GSMLibModulePowered(*libRefP))
            	{
            		treoinfo->PowerState=kPowerOn;
            		
        			if (!treoinfo->isNotFirstTime)
        	   			treoinfo->RegistrationStatus=registrationHome;
            	}

        		if (GSMLibRegister(matro->gsmLibRef, appCreator, kVoice))
				{
            		FrmCustomAlert(altAlert, 
            		"Cannot register for retrieving wireless infos; try to reset (normal, warm, hard) your Treo."
            			, NULL, NULL);
        			treoinfo->gsmenabled=false;
				}

			}
		}
	    
		MaTreoInfoRefresh(treoinfo);
	}	
	
	treoinfo->isNotFirstTime=true;
	
	return !error;
}

Err MaTreoLoadGSMLibrary(UInt16* libRefP, Boolean* libLoadedP)
{	
	Err error = 0;

	if (!libRefP)
		return memErrInvalidParam;
    
	error = SysLibFind(GSMLibName, libRefP);
	if (error)
	{
		error = SysLibLoad(GSMLibTypeID, GSMLibCreatorID, libRefP);
		if (error)
			return error;

		if (libLoadedP)
			*libLoadedP = true;
	}

	return error;
}

void MaTreoTerminate(Boolean normal)
{
	Matro *matro;
	TreoInfo *treoinfo, prefTreoinfo;

    FtrGet(appCreator, ftrTreoInfo, (UInt32 *)&treoinfo);

	treoinfo->gsmenabled=treoinfo->gsmenabledreq;
	
    MemMove(&prefTreoinfo, treoinfo, sizeof(TreoInfo));
	PrefSetAppPreferences (appCreator, 0, appPrefVersion, 
		&prefTreoinfo, sizeof (TreoInfo), false);
    MemPtrFree(treoinfo);

	if (normal)
	{
        FtrGet(appCreator, ftrMatro, (UInt32 *)&matro);
    	if (matro->gsmLibLoaded)
    		SysLibRemove(matro->gsmLibRef);
	}		
}

