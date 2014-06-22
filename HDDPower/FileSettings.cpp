#include "stdafx.h"
#include "FileSettings.h"
#include "PowerMode.h"
#include <stdio.h>
#include <fstream>

#define MAX_DATASTRING 100

char* SettingsFileName = "Settings.ini";
char* ErrorFileName = "Errors.log";

void SetDefaultParameters()
{
	PMode = HighPerf;
	wcscpy(PhysicalDrive, DefaultDrive);
}

void SaveParameters()
{
	std::wofstream outIniFile(SettingsFileName);
	if (outIniFile.bad())
	{
		ErrorLog(L"Save parameters failed!");
	}
	else
	{
		outIniFile << "Disk: " << PhysicalDrive << std::endl  << "Mode: " << (int)PMode ;
		outIniFile.close();
	}
}

void LoadParameters()
{
	std::wifstream infile(SettingsFileName);
	if (infile.fail())
	{
		SetDefaultParameters();
		SaveParameters();
	}
	else
	{
		TCHAR paramName[MAX_DATASTRING];
		infile >> paramName;
		infile >> PhysicalDrive;
		
		int mode;
		infile >> paramName;
		infile >> mode;
		switch (mode)
		{
			case LowPower:
			case Standby:
			case NoStandby:
			case HighPerf:
				PMode = (PowerMode) mode;
				break;
			default:
				ErrorLog(L"Input file settings is bad!");
				SetDefaultParameters();
				SaveParameters();
		}		
		infile.close();
	}
}

void ErrorLog(TCHAR* errMessage)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	TCHAR szDateTime[MAX_DATASTRING];
	wsprintf(szDateTime, L"%d:%d:%d - ", st.wHour, st.wMinute, st.wSecond);
	std::wofstream out(ErrorFileName, std::ios::app);
	out << szDateTime << errMessage << std::endl;
	out.close();
}