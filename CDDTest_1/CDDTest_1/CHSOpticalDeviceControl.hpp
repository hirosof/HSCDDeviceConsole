#pragma once

//---------------------------------------------------
//Start : 2017 - 03 - 10
//---------------------------------------------------
//2017-03-11
//2017-03-12
//2017-03-13
//2017-03-14
//-------------------------------------------------------


#pragma warning(disable:4091)

#define _NTSCSI_USER_MODE_
#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <ntddcdrm.h>
#include <ntddmmc.h>
#include <ntddstor.h>
#include <ntddscsi.h>
#include <scsi.h>


#define DHS_SCSI_STATUS_GOOD 0x00


//メディア種類
enum struct  EHSOpticalDiscMediaType {
	Unknown = 0 ,	//不明
	NoDisc,	//ディスク無し
	CD_ROM ,	//CD-ROM
	CD_R ,	//CD-R
	CD_RW ,	//CD-RW
	DVD_ROM ,	//DVD-ROM
	DVD_R ,	//DVD-R
	DVD_R_DL ,	//DVD-R DL
	DVD_RW ,	//DVD-RW
	DVD_RAM ,	//DVD_RAM
	DVD_Plus_R ,	//DVD+R
	DVD_Plus_R_DL ,	//DVD+R DL
	DVD_Plus_RW ,	//DVD+RW
	BD_ROM ,	//BD-ROM
	BD_R ,	//BD-R
	BD_RE ,	//BD-RE
};


enum struct EHSOpticalDiscStatus {
	Empty = 0,
	Incomplete,
	Finalized,
	Other
};

struct  THSOpticalDiscMediaTypeInfo {
	EHSOpticalDiscMediaType Type;
	char szTypeString [ 12 ];
};



#define DHSOpticalDriveVendorIDLength 8U
#define DHSOpticalDriveProductIDLength 16U
#define DHSOpticalDriveProductRevisionIDLength 4U
#define DHSOpticalDriveDeviceNameLength 30U




struct THSOpticalDriveDeviceInfo {
	char VendorID [ DHSOpticalDriveVendorIDLength + 1 ];
	char ProductID [ DHSOpticalDriveProductIDLength + 1 ];
	char ProductRevisionID [ DHSOpticalDriveProductRevisionIDLength + 1 ];
	char DeviceName [ DHSOpticalDriveDeviceNameLength + 1 ];
};


struct THSEnumrateOpticalDriveInfo {
	uint8_t  uOpticalDriveCount;
	struct tagDrive {
		char Letter;
		bool bIncludedInfo;
		THSOpticalDriveDeviceInfo Info;
	}Drives[26];
};


struct THSFeatureHeader {
	uint8_t DataLength [ 4 ];
	uint16_t Reserved;
	uint8_t CurrentProfile;
};

struct THSFeatureDescriptorHeader {
	uint8_t FeatureCode [ 2 ];
	uint8_t Current : 1;
	uint8_t Persistent : 1;
	uint8_t Version : 4;
	uint8_t Reversed : 2;
	uint8_t AdditionalLength;
};

template <typename FeatureDescDataType> struct THSFeatureDataBase {
	THSFeatureHeader Header;
	THSFeatureDescriptorHeader DescHeader;
	FeatureDescDataType  DescData;
};

struct THSFeatureData_Profile {
	uint8_t ProfileNumber [ 2 ];
	uint8_t CurrentP : 1;
	uint8_t Reserved : 7;
	uint8_t Reserved2;
};



class CHSOpticalDeviceControl {
protected:
	HANDLE hDevice;
	char cOpenedDriveLetter;
	static bool GetDeviceInfoFromHandle ( HANDLE hOpticalDrive , THSOpticalDriveDeviceInfo *pInfo );

public:


	static bool IsOpticalDrive ( const char DriveLetter );

	static bool EnumOpticalDrive ( THSEnumrateOpticalDriveInfo *pInfo );
	static bool GetDeviceInfo ( const char OpticalDriveLetter , THSOpticalDriveDeviceInfo *pInfo );

	CHSOpticalDeviceControl ( void );
	~CHSOpticalDeviceControl ( );



	bool Open ( const char OpticalDriveLetter );
	bool Close ( void );
	bool IsOpened ( void );
	bool IsReady ( void );
	bool HasMedia ( void );
	EHSOpticalDiscMediaType GetCurrentMediaType ( void );

	bool GetCurrentMediaTypeInfo ( THSOpticalDiscMediaTypeInfo  *pInfo );
	char GetCurrentDriveLetter ( void );
	char GetCurrentDeviceInfo ( THSOpticalDriveDeviceInfo *pInfo );

	bool Eject ( void );

	bool Tray_Open ( void );
	bool Tray_Close ( void );
	bool Tray_Lock ( void );
	bool Tray_UnLock ( void );
	bool Tray_IsOpened ( void );

	bool GetDiskGeometry ( DISK_GEOMETRY *pGeometry );
	uint32_t GetMaxReadSpeed ( void );

	bool GetDiscStatus ( EHSOpticalDiscStatus *pStatus );


	bool SpinUp ( void );
	bool SpinDown ( void );

};