#pragma once

enum PowerMode 
{
	LowPower = 1,		//minimum power consumption with standby allowed
	Standby = 127,		//maximum power consumption with standby allowed
	NoStandby = 128,	//minimum power consumption with standby not allowed
	HighPerf = 254,		//maximum power consumption with standby not allowed
};

#define DefaultDrive L"\\\\.\\PhysicalDrive1"
#define MAX_DATASTRING 100
extern PowerMode PMode;
extern TCHAR PhysicalDrive[MAX_DATASTRING];

void SetPowerMode(PowerMode mode);
bool PowerModeIs(PowerMode mode);
void Refresh();
void InitPowerMode();