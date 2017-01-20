/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	designed for Handspring Treo (PalmOS3.5H)
*/

//#define SHOWDEBUG
//#define PDACOLOR
//#define PDACREATE
// #define PDACOLORCREATE
// #define PDAGREYCREATE

#ifdef  PDACOLOR
#define PDACOLOR_DEPTH	8
#else
#define PDACOLOR_DEPTH	4
#endif
 
#define ANIM_FPS		8
#define SCREEN_WIDTH	100
#define SCREEN_HEIGHT	100
#define SCREEN_X		55
#define SCREEN_Y		38 

Boolean ApplicationHandleMenu(UInt16 menuID);
Boolean frmMain_HandleEvent(EventPtr event);
Boolean frmOptions_HandleEvent(EventPtr event);
static Boolean frmOptions_OPTIONS_OK_OnSelect(EventPtr event);
static Boolean frmOptions_OPTIONS_CANCEL_OnSelect(EventPtr event);
static Boolean frmOptions_OPTIONS_BACKLIGHT_FROM_OnSelect(EventPtr event);
static Boolean frmOptions_OPTIONS_BACKLIGHT_TO_OnSelect(EventPtr event);

typedef struct
{
  Int32        evtTimeOut;		// these variables
  UInt32       timerDiff;		// handles doublebuffering
  UInt32       ticksPerFrame;	// timing, which must follow
  UInt32       timerPointA;		// real time. timeslices must
  UInt32       timerPointB;		// be integer part of 60 seconds
  								// in order the clock movement
								// to be synchronized

  UInt32 switchapp_ticksStart, switchapp_ticksEnd;
  
  Boolean gsmLibLoaded, LaunchOtherApp, LaunchOtherAppJog;
  UInt16 gsmLibRef;
} Matro;

#define appCreator         'MTRO'
#define appVersion         0x01
#define appPrefVersion     0x01
#define titleDefault       "MaTreo" 
#define ftrMatro           1000
#define ftrDeviceGlobals   1001
#define ftrGraphicsGlobals 1002
#define ftrAnimGlobals	   1003
#define ftrTreoInfo        1004

