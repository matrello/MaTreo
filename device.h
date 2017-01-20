/*
	MaTreo
	Code by Matro
	Rome, Italy, 5-2002

	portions by Aaron Ardiri
	
	designed for Handspring Treo (PalmOS3.5H)
*/

#ifndef _DEVICE_H
#define _DEVICE_H

#define romVersion1   sysMakeROMVersion(1,0,0,sysROMStageRelease,0)
#define romVersion2   sysMakeROMVersion(2,0,0,sysROMStageRelease,0)
#define romVersion3   sysMakeROMVersion(3,0,0,sysROMStageRelease,0)
#define romVersion3_1 sysMakeROMVersion(3,1,0,sysROMStageRelease,0)
#define romVersion3_2 sysMakeROMVersion(3,2,0,sysROMStageRelease,0)
#define romVersion3_5 sysMakeROMVersion(3,5,0,sysROMStageRelease,0)
#define romVersion4   sysMakeROMVersion(3,5,0,sysROMStageRelease,0)

Boolean DeviceCheckCompatability();
Boolean    DeviceInitialize();
UInt32  DeviceGetSupportedDepths();
Boolean DeviceSupportsVersion(UInt32);
Boolean DeviceSupportsColor();
void    *DeviceWindowGetPointer(WinHandle);
void    DeviceTerminate();

#endif
