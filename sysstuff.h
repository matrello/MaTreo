/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	designed for Handspring Treo (PalmOS3.5H)
*/

#ifndef SYSSTUFF_H_INCLUDED
#define SYSSTUFF_H_INCLUDED

typedef struct TreoInfo
{

	/* this structure holds Treo infos, loaded
	   by MaTreoInfoRefresh().
	  
	   note: these infos were created by Matro and
	   may not reflect Handspring registers, even if
	   they are based on them
	   
	   the structure also holds MaTreo options
	*/

	// MaTreo options	
	Boolean gsmenabled, gsmenabledreq;

	char title[20];
	
	Boolean switchapp_jog, switchapp_tap, switchapp_sec;
	UInt32  switchapp_secnum, switchapp_creator, switchapp_creatorJog;
	LocalID switchapp_dbID, switchapp_dbIDJog;
	UInt16  switchapp_cardNo, switchapp_cardNoJog;
	
	Boolean backlightOn;
	Int16   backlightFrom_hours, backlightFrom_minutes;
	Int16   backlightTo_hours, backlightTo_minutes;	

	
	// Treo infos	
	Boolean isRefreshed, isSearchOnPowerOn, isNotFirstTime;
	Int8 PowerState, RegistrationStatus, IndicationStatus, filler;
	char carrierID[6];
	char roamingID[6];
	char carrier[64];
	char roaming[64];


    short	gsmEV[4];
    short	gsmEVpar[4];
    Boolean	gsmNewEvent;
} TreoInfo;

void AccendoLaLuce();
void GetDefaultSwitchTo(UInt16 *cardNo, LocalID *dbID, UInt32 *creator);
Err MaTreoInfoRefresh(TreoInfo *treoinfo);
Err MaTreoLoadCarrierInfo(TreoInfo *treoinfo, Boolean isRoaming);
UInt32 MaTreoHandleEvent(MemPtr cmdPBP, UInt16 launchFlags);
Err MaTreoInitialize(Boolean normal, UInt16* libRefP, Boolean* libLoadedP);
Err MaTreoLoadGSMLibrary(UInt16* libRefP, Boolean* libLoadedP);
void MaTreoTerminate(Boolean normal);

#endif
