#pragma once

#include <Windows.h>
#include <ntddcdrm.h>
#include <ntddmmc.h>
#include <ntddstor.h>
#include <string>
#include <stdio.h>
#define _NTSCSI_USER_MODE_
#include <scsi.h>

const UINT8 VHSCD_MAX_TRACK = 99;
const UINT16 VHSCD_BYTES_PER_SECTOR = 2352U; //CDDA Sector Size

enum struct EHSCD_DiscMode {
	Unknown = 0,
	Mode1,
	Mode2XA,
	Interactive
};

enum struct EHSCD_DiscType {
	FullData = 0,
	FullAudio ,
	MixCD,
	CDExtra,
	Unknown
};

enum struct EHSCD_TrackType {
	Unknown = 0,
	Audio ,
	AudioWithPreemphasis,
	DigitalCopyPreemphasis,
	FourChannelAudio,
	Data
};

struct THSCD_TrackInfo {
	EHSCD_TrackType Type;
	UINT32 StartSectorPosition;
	UINT32 EndSectorPosition;
	struct {
		UINT32 Start;
		UINT32 End;
	}MSFPosition;
	UINT32 LengthSectors;
};

struct THSCD_DiscInfo {
	EHSCD_DiscMode DiscType;
	EHSCD_DiscType MusicCDType;
	UINT8 StartTrackNumber;
	UINT8 EndTrackNumber;
	UINT8 NumberOfTracks;
	UINT32 LogicalBlockAddress;
	THSCD_TrackInfo TrackInfo[ VHSCD_MAX_TRACK ];
};


struct THSCD_MSF_TIME {
	UINT8 Minutes;
	UINT8 Seconds;
	UINT8 Frames;
};

#define DHSOpticalDriveVendorIDLength 8U
#define DHSOpticalDriveProductIDLength 16U
#define DHSOpticalDriveProductRevisionIDLength 4U
#define DHSOpticalDriveDeviceNameLength 30U


struct THSOpticalDriveDeviceInfo {
	char VendorID[DHSOpticalDriveVendorIDLength + 1];
	char ProductID[DHSOpticalDriveProductIDLength + 1];
	char ProductRevisionID[DHSOpticalDriveProductRevisionIDLength + 1];
	char DeviceName[DHSOpticalDriveDeviceNameLength + 1];
};


struct THSEnumrateOpticalDriveInfo {
	uint8_t  uOpticalDriveCount;
	struct tagDrive {
		char Letter;
		bool bIncludedInfo;
		THSOpticalDriveDeviceInfo Info;
	}Drives[26];
};


class CHSCDDevice {

private:
	struct SessionInnerData {
		UINT8 StartTrackNumber;
		UINT8 EndTrackNumber;
		UINT32 LastSectors;
	};
	HANDLE hDevice;

	static const UINT16  CRCTABLE [ 256 ];
	static bool Crc16 ( void *lpData , size_t size  , UINT16 *lpCRC16);
	static UINT32  MSFToSectors ( UINT8 M , UINT8 S , UINT8 F );
	BOOL ParseRawTOCInfo ( CDROM_TOC_FULL_TOC_DATA *pFullToc , THSCD_DiscInfo *pInfo );
	static bool JugdeDiscType ( THSCD_DiscInfo *pInfo );
	static bool GetDeviceInfoFromHandle( HANDLE hOpticalDrive, THSOpticalDriveDeviceInfo* pInfo );

public:

	static bool EnumOpticalDrive( THSEnumrateOpticalDriveInfo* pInfo );
	static bool GetDeviceInfo( const char OpticalDriveLetter, THSOpticalDriveDeviceInfo* pInfo );

	static BOOL IsCDDevice ( const char DriveLetter );

	CHSCDDevice ( );
	~CHSCDDevice ( );


	BOOL Open ( const char DriveLetter );
	void Close ( void );

	HANDLE GetHandle ( void );

	BOOL EjectMedia ( void );

	size_t GetDeviceNameByDeviceProperty ( char *pName , size_t NameLen , bool bExtraSpaceDelete = false );
	bool GetCurentDeviceInfo( THSOpticalDriveDeviceInfo* pInfo );


	BOOL IsPossibleAccess ( void );

	BOOL GetDiscInfo ( THSCD_DiscInfo *pInfo );

	UINT32  Read ( void *lpOutBuffer , size_t OutBufferSize , UINT8 TrackNumber , UINT32 OffsetSector , UINT32 ReadSector );
	UINT32  Read8 ( void *lpOutBuffer , size_t OutBufferSize , UINT8 TrackNumber , UINT32 OffsetSector , UINT32 ReadSector );
	UINT32  Read16 ( void *lpOutBuffer , size_t OutBufferSize , UINT8 TrackNumber , UINT32 OffsetSector , UINT32 ReadSector );

	UINT32 ReadByReadFile ( void *lpOutBuffer , size_t OutBufferSize , UINT8 TrackNumber , UINT32 OffsetSector , UINT32 ReadSector );
	uint32_t GetMaxReadSpeed ( void );

};


UINT8 HSGetMSFMinuteFromSector ( UINT32 Sector );
UINT8 HSGetMSFSecondFromSector ( UINT32 Sector );
UINT8 HSGetMSFFrameFromSector ( UINT32 Sector );

THSCD_MSF_TIME HSGetMSFTimeBySector ( UINT32 Sector );

UINT32 HSGetSectorByMSFTime ( THSCD_MSF_TIME msf );
UINT32 HSGetSectorByMSFTime ( UINT8 M , UINT8 S , UINT8 F );
UINT32 HSGetBytesSizeBySector ( UINT32 Sector );
UINT32 HSGetBytesSizeByMSFTime ( THSCD_MSF_TIME msf );
UINT32 HSGetBytesSizeByMSFTime ( UINT8 M , UINT8 S , UINT8 F );
