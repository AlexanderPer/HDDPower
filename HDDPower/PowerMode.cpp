#include "stdafx.h"
#include <winioctl.h>
#include <Ntddscsi.h>
#include <stddef.h>
#include "PowerMode.h"
#include "FileSettings.h"

#define UNICODE 1
#define _UNICODE 1

struct ATA_PASS_THROUGH_EX_WITH_DATA {
	ATA_PASS_THROUGH_EX aptex;
	DWORD data;
};

PowerMode PMode = HighPerf;
TCHAR PhysicalDrive[MAX_DATASTRING];

void UpdatePowerMode()
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	BOOL bResult   = FALSE;
	DWORD junk     = 0;
	
	hDevice = CreateFileW(PhysicalDrive,					// drive to open
	                      GENERIC_READ | GENERIC_WRITE,		// access to the drive
	                      FILE_SHARE_READ |					// share mode
	                      FILE_SHARE_WRITE, 
	                      NULL,								// default security attributes
	                      OPEN_EXISTING,					// disposition
	                      0,								// file attributes
	                      NULL);							// do not copy file attributes
	
	if (hDevice == INVALID_HANDLE_VALUE)					// cannot open the drive
	{
		TCHAR* msg = L"Cannot open the drive (INVALID_HANDLE_VALUE)!";
		ErrorLog(msg);
		return;
	}

	DWORD adSize = sizeof(ATA_PASS_THROUGH_EX_WITH_DATA);

	ATA_PASS_THROUGH_EX_WITH_DATA ad;
	memset(&ad, 0, sizeof(ad));

	ad.aptex.Length				= sizeof(ATA_PASS_THROUGH_EX);
	ad.aptex.AtaFlags			= ATA_FLAGS_DRDY_REQUIRED;
	ad.aptex.TimeOutValue		= 60;
	ad.aptex.ReservedAsUlong	= 1024;
	ad.aptex.DataBufferOffset	= adSize;
	ad.aptex.CurrentTaskFile[0] = 0x05;
	ad.aptex.CurrentTaskFile[1] = (UCHAR)PMode;//0xfe;
	ad.aptex.CurrentTaskFile[5] = 0xa0;
	ad.aptex.CurrentTaskFile[6] = 0xef;

	ad.data = 1024;
	
	DWORD operation = IOCTL_ATA_PASS_THROUGH;
	bResult = DeviceIoControl(hDevice,							// device to be queried
	                          operation,						// operation to perform
	                          &ad, adSize,						// input buffer
	                          &ad, adSize,						// output buffer
	                          &junk,							// # bytes returned
	                          (LPOVERLAPPED) NULL);				// synchronous I/O
	
	CloseHandle(hDevice);

	if (!bResult) 
	{
		TCHAR msg[MAX_DATASTRING];
		wsprintf (msg, L"Power mode setting failed! Error %ld.", GetLastError ());
		ErrorLog(msg);
	}
}

void SetPowerMode(PowerMode mode)
{
	PMode = mode;
	UpdatePowerMode();
	SaveParameters();
}

bool PowerModeIs(PowerMode mode)
{
	return PMode == mode;
}

void Refresh()
{
	UpdatePowerMode();
}

void InitPowerMode()
{
	LoadParameters();
	UpdatePowerMode();
}

