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

// globals variable structure
typedef struct
{
  UInt32    romVersion;                    // the rom version of the device
  UInt32    depthState;                    // the screen depth state (old)
} DeviceGlobals;

/**
 * Initialize the device.
 */
Boolean
DeviceInitialize()
{
  DeviceGlobals *gbls;
  UInt32 myDepth=PDACOLOR_DEPTH;
 
  // create the globals objects, and register it
  gbls = (DeviceGlobals *)MemPtrNew(sizeof(DeviceGlobals));
  MemSet(gbls, sizeof(DeviceGlobals), 0);
  FtrSet(appCreator, ftrDeviceGlobals, (UInt32)gbls);

  // get the rom version and ram size for this device
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &gbls->romVersion);

  // only OS 3.0 and above have > 1bpp display via API's
  if (DeviceSupportsVersion(romVersion3)) {

    // save the current display state
    WinScreenMode(winScreenModeGet,NULL,NULL,&gbls->depthState,NULL);

    WinScreenMode(winScreenModeSet,NULL,NULL,&myDepth,NULL);
  }
  
  return true;
}

/**
 * Check the compatability status of the device we are working with.
 *
 * @return true if the device is supported, false otherwise.
 */
Boolean
DeviceCheckCompatability()
{
  Boolean result = true;

  result = (DeviceSupportsVersion(romVersion3_5));

  if (!result)
  {
    FrmCustomAlert(altAlert, 
    "MaTreo runs on PalmOS3.5 and later."
	    , NULL, NULL);
  }
 
 #ifdef PDACOLOR 
  if ((DeviceGetSupportedDepths() & 128)!= 128)
  {
    FrmCustomAlert(altAlert, 
    "This MaTreo version runs on color devices; please use specific MaTreo release for gray screens."
	    , NULL, NULL);
	
	return false;
  }  
 #endif

  return result;
}

/**
 * Get the supported depths the device can handle. 
 *
 * @return the depths supported (1011b = 2^3 | 2^1 | 2^0 = 4,2,1 bpp).
 */
UInt32  
DeviceGetSupportedDepths()
{
  UInt32 result = 0x00000001;

  // only OS 3.0 and above have > 1bpp display via API's
  if (DeviceSupportsVersion(romVersion3)) {
    WinScreenMode(winScreenModeGetSupportedDepths,NULL,NULL,&result,NULL);
  }

  return result;
}

/**
 * Check if the device is compatable with a particular ROM version.
 *
 * @param version the ROM version to compare against.
 * @return true if it is compatable, false otherwise.
 */
Boolean 
DeviceSupportsVersion(UInt32 version)
{
  UInt32 romVersion;

  // get the rom version for this device
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

  return (Boolean)(romVersion >= version);
}

/**
 * Check if the device is color capable.
 *
 * @return true if it is capable, false otherwise.
 */
Boolean
DeviceSupportsColor()
{
  Boolean color = false;

  if (DeviceSupportsVersion(romVersion3_5)) {
    WinScreenMode(winScreenModeGetSupportsColor,NULL,NULL,NULL,&color);
  }

  return color;
}

/**
 * Determine the pointer to the bitmap data chunk for a specific window.
 *
 * @param win the window.
 * @return a pointer to the bitmap data chunk.
 */
void *
DeviceWindowGetPointer(WinHandle win)
{
  void *result = NULL;

  // palmos 3.5        - use BmpGetBits()
  if (DeviceSupportsVersion(romVersion3_5)) {
    result = BmpGetBits(WinGetBitmap(win));
  }

  // palmos pre 3.5    - use standard technique
  else
    result = (void *)win->displayAddrV20;
    
  return result;
}

/**
 * Reset the device to its original state.
 */
void
DeviceTerminate()
{
  DeviceGlobals *gbls;

  // get a globals reference
  FtrGet(appCreator, ftrDeviceGlobals, (UInt32 *)&gbls);

  // restore the current display state
  if (DeviceSupportsVersion(romVersion3)) {
    WinScreenMode(winScreenModeSet,NULL,NULL,&gbls->depthState,NULL);
  }

  // clean up memory
  MemPtrFree(gbls);

  // unregister global data
  FtrUnregister(appCreator, ftrDeviceGlobals);
}
