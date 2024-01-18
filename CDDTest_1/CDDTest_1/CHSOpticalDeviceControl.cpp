#include "CHSOpticalDeviceControl.hpp"

bool CHSOpticalDeviceControl::GetDeviceInfoFromHandle ( HANDLE hOpticalDrive , THSOpticalDriveDeviceInfo * pInfo )
{
	if ( hOpticalDrive == INVALID_HANDLE_VALUE ) return false;
	if ( hOpticalDrive == NULL ) return false;
	if ( pInfo ) {

		INQUIRYDATA data;
		DWORD returnSize;

		BOOL bRet = DeviceIoControl ( hOpticalDrive , IOCTL_CDROM_GET_INQUIRY_DATA , 0 , 0 , 
			&data , sizeof ( INQUIRYDATA ) , &returnSize , NULL );
		if ( bRet ) {
			char space [ 2 ] = { ' ' , '\0' };
			lstrcpynA ( pInfo->VendorID , reinterpret_cast<char*>( data.VendorId )  , DHSOpticalDriveVendorIDLength + 1 );
			lstrcpynA ( pInfo->ProductID , reinterpret_cast<char*>( data.ProductId )  , DHSOpticalDriveProductIDLength  + 1);
			lstrcpynA ( pInfo->ProductRevisionID , reinterpret_cast<char*>( data.ProductRevisionLevel ) , DHSOpticalDriveProductRevisionIDLength + 1);

			/*
			for ( uint8_t i = DHSOpticalDriveVendorIDLength - 1; i >= 0; i-- ) {
				if (( pInfo->VendorID [ i ] != ' ' )&&(pInfo->VendorID [ i ] != '\t')) break;
				pInfo->VendorID [ i ] = '\0';
			}
			for ( uint8_t i = DHSOpticalDriveProductIDLength - 1; i >= 0; i-- ) {
				if ( ( pInfo->ProductID [ i ] != ' ' ) && ( pInfo->ProductID [ i ] != '\t' ) ) break;
				pInfo->ProductID [ i ] = '\0';
			}
			for ( uint8_t i = DHSOpticalDriveProductRevisionIDLength - 1; i >= 0; i-- ) {
				if ( ( pInfo->ProductRevisionID [ i ] != ' ' ) && ( pInfo->ProductRevisionID [ i ] != '\t' ) ) break;
				pInfo->ProductRevisionID [ i ] = '\0';
			}
			*/

			pInfo->DeviceName [ 0 ] = '\0';

			lstrcatA ( pInfo->DeviceName , pInfo->VendorID );
			if ( pInfo->DeviceName [ 0 ] != '\0' ) lstrcatA ( pInfo->DeviceName , space );
			lstrcatA ( pInfo->DeviceName , pInfo->ProductID );
			if ( pInfo->DeviceName [ 0 ] != '\0' ) lstrcatA ( pInfo->DeviceName , space );
			lstrcatA ( pInfo->DeviceName , pInfo->ProductRevisionID );

			return true;
		}
	}
	return false;
}

bool CHSOpticalDeviceControl::IsOpticalDrive ( const char DriveLetter )
{
	
	bool Check = ( DriveLetter >= 'A' ) && ( DriveLetter <= 'Z' );
	Check |= ( DriveLetter >= 'a' ) && ( DriveLetter <= 'z' );

	if ( Check ) {

		char DL [ 4 ];

		wsprintfA ( DL , "%c:\\" , DriveLetter );

		return ( GetDriveTypeA ( DL ) == DRIVE_CDROM );
	}
	return false;
}

bool CHSOpticalDeviceControl::EnumOpticalDrive ( THSEnumrateOpticalDriveInfo * pInfo )
{
	if ( pInfo == nullptr ) return false;
	bool bRet;
	pInfo->uOpticalDriveCount = 0;
	for ( int i = 0; i < 26; i++ )pInfo->Drives[ i ].Letter = '\0';
	for ( char c = 'A'; c <= 'Z'; c++ ) {
		if ( CHSOpticalDeviceControl::IsOpticalDrive ( c ) ) {
			bRet = CHSOpticalDeviceControl::GetDeviceInfo ( c ,&pInfo->Drives [ pInfo->uOpticalDriveCount ].Info );
			pInfo->Drives [ pInfo->uOpticalDriveCount ].Letter = c;
			pInfo->Drives [ pInfo->uOpticalDriveCount ].bIncludedInfo = bRet;
			pInfo->uOpticalDriveCount++;
		}
	}
	return true;
}

bool CHSOpticalDeviceControl::GetDeviceInfo ( const char OpticalDriveLetter , THSOpticalDriveDeviceInfo * pInfo )
{
	if ( pInfo == nullptr ) return false;
	if ( CHSOpticalDeviceControl::IsOpticalDrive ( OpticalDriveLetter ) ) {

		HANDLE hDrive;

		char FileName [ 8 ];

		wsprintfA ( FileName , "\\\\.\\%c:" , OpticalDriveLetter );

		hDrive = CreateFileA ( FileName , GENERIC_READ | GENERIC_WRITE , FILE_SHARE_READ | FILE_SHARE_WRITE , nullptr , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL );

		if ( hDrive != INVALID_HANDLE_VALUE ) {

			bool bRet = CHSOpticalDeviceControl::GetDeviceInfoFromHandle ( hDrive , pInfo );
			CloseHandle ( hDrive );

			return bRet;
		}
	}
	return false;
}

CHSOpticalDeviceControl::CHSOpticalDeviceControl ( void )
{
	this->hDevice = NULL;
	this->cOpenedDriveLetter = '\0';
}

CHSOpticalDeviceControl::~CHSOpticalDeviceControl ( )
{
	this->Close ( );
}

bool CHSOpticalDeviceControl::Open ( const char OpticalDriveLetter )
{

	if ( this->hDevice ) return false;

	if ( CHSOpticalDeviceControl::IsOpticalDrive ( OpticalDriveLetter ) ) {


		char FileName [ 8 ];

		wsprintfA ( FileName , "\\\\.\\%c:" , OpticalDriveLetter );

		this->hDevice = CreateFileA ( FileName , GENERIC_READ | GENERIC_WRITE , FILE_SHARE_READ | FILE_SHARE_WRITE , nullptr , OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL );
		
		if ( hDevice == INVALID_HANDLE_VALUE ) {
			hDevice = NULL;
			return FALSE;
		} else {
			cOpenedDriveLetter = OpticalDriveLetter;
		}
		return true;
	}
	return false;
}

bool CHSOpticalDeviceControl::Close ( void )
{
	if ( this->hDevice ) {
		CloseHandle ( this->hDevice );
		this->hDevice = NULL;
		this->cOpenedDriveLetter = '\0';
		return true;
	}

	return false;
}

bool CHSOpticalDeviceControl::IsOpened ( void )
{
	if ( this->hDevice ) {
		return true;
	}
	return false;
}

bool CHSOpticalDeviceControl::IsReady ( void )
{
	if ( this->IsOpened() ) {
		BOOL bret = DeviceIoControl ( hDevice , IOCTL_STORAGE_CHECK_VERIFY , NULL , NULL , NULL , NULL , NULL , NULL );
		return ( bret == TRUE );
	}
	return false;
}

bool CHSOpticalDeviceControl::HasMedia ( void )
{
	return this->IsReady();
}


EHSOpticalDiscMediaType CHSOpticalDeviceControl::GetCurrentMediaType ( void )
{
	if ( this->IsOpened ( ) == false ) return EHSOpticalDiscMediaType::Unknown;
	if ( this->IsReady ( ) == false ) return EHSOpticalDiscMediaType::NoDisc;

	GET_CONFIGURATION_IOCTL_INPUT  ConfInput;
	GET_CONFIGURATION_HEADER ConfHeader;
	DWORD returnSize;

	ConfInput.Feature = FeatureProfileList;
	ConfInput.RequestType = SCSI_GET_CONFIGURATION_REQUEST_TYPE_CURRENT;
	ConfInput.Reserved [ 0 ] = 0;
	ConfInput.Reserved [ 1 ] = 0;

	BOOL bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_GET_CONFIGURATION , 
		&ConfInput , sizeof ( GET_CONFIGURATION_IOCTL_INPUT ) ,
		&ConfHeader , sizeof ( GET_CONFIGURATION_HEADER ) , &returnSize , 0 );

	if ( bRet ) {
		UINT16 CuurentProfile = ( ConfHeader.CurrentProfile [ 0 ] << 8 ) | ConfHeader.CurrentProfile [ 1 ];
		switch ( CuurentProfile ) {
				//CD-ROM
			case ProfileCdrom:
				return EHSOpticalDiscMediaType::CD_ROM;

				//CD-R
			case ProfileCdRecordable:
				return EHSOpticalDiscMediaType::CD_R;

				//CD-RW
			case ProfileCdRewritable:
				return EHSOpticalDiscMediaType::CD_RW;

				//DVD-ROM
			case ProfileDvdRom:
				return EHSOpticalDiscMediaType::DVD_ROM;

				//DVD-R
			case ProfileDvdRecordable:
				return EHSOpticalDiscMediaType::DVD_R;

				//DVD-RAM
			case ProfileDvdRam:
				return EHSOpticalDiscMediaType::DVD_RAM;

				//DVD-RW
			case ProfileDvdRewritable:
				return EHSOpticalDiscMediaType::DVD_RW;

				//DVD-RW(Sequential)
			case ProfileDvdRWSequential:
				return EHSOpticalDiscMediaType::DVD_RW;
				
				//DVD-R DL(Sequential))
			case ProfileDvdDashRDualLayer:
				return EHSOpticalDiscMediaType::DVD_R_DL;

				//DVD-R DL (Layer Jump)
			case ProfileDvdDashRLayerJump:
				return EHSOpticalDiscMediaType::DVD_R_DL;
				
				//DVD+RW
			case ProfileDvdPlusRW:
				return EHSOpticalDiscMediaType::DVD_Plus_RW;

				//DVD+R
			case ProfileDvdPlusR:
				return EHSOpticalDiscMediaType::DVD_Plus_R;

				//DVD+R DL
			case ProfileDvdPlusRDualLayer:
				return EHSOpticalDiscMediaType::DVD_Plus_R_DL;

				//BD-ROM
			case ProfileBDRom:
				return EHSOpticalDiscMediaType::BD_ROM;

				//BD-R
			case ProfileBDRSequentialWritable:
				return EHSOpticalDiscMediaType::BD_R;

				//BD-R(Random Writable)
			case ProfileBDRRandomWritable:
				return EHSOpticalDiscMediaType::BD_R;

				//BD-RE
			case ProfileBDRewritable:
				return EHSOpticalDiscMediaType::BD_RE;
		}
	}
	return EHSOpticalDiscMediaType::Unknown;
}

bool CHSOpticalDeviceControl::GetCurrentMediaTypeInfo ( THSOpticalDiscMediaTypeInfo * pInfo )
{

	if ( pInfo == nullptr ) return false;
	EHSOpticalDiscMediaType  mType = this->GetCurrentMediaType ( );

	pInfo->Type = mType;

	switch ( mType ) {
		case EHSOpticalDiscMediaType::NoDisc:
			strcpy_s ( pInfo->szTypeString , "No Disc" );
			break;
		case EHSOpticalDiscMediaType::CD_ROM:
			strcpy_s ( pInfo->szTypeString , "CD-ROM" );
			break;
		case EHSOpticalDiscMediaType::CD_R:
			strcpy_s ( pInfo->szTypeString , "CD-R" );
			break;
		case EHSOpticalDiscMediaType::CD_RW:
			strcpy_s ( pInfo->szTypeString , "CD-RW" );
			break;
		case EHSOpticalDiscMediaType::DVD_ROM:
			strcpy_s ( pInfo->szTypeString , "DVD-ROM" );
			break;
		case EHSOpticalDiscMediaType::DVD_R:
			strcpy_s ( pInfo->szTypeString , "DVD-R" );
			break;
		case EHSOpticalDiscMediaType::DVD_R_DL:
			strcpy_s ( pInfo->szTypeString , "DVD-R DL" );
			break;
		case EHSOpticalDiscMediaType::DVD_RW:
			strcpy_s ( pInfo->szTypeString , "DVD-RW" );
			break;
		case EHSOpticalDiscMediaType::DVD_RAM:
			strcpy_s ( pInfo->szTypeString , "DVD-RAM" );
			break;
		case EHSOpticalDiscMediaType::DVD_Plus_R:
			strcpy_s ( pInfo->szTypeString , "DVD+R" );
			break;
		case EHSOpticalDiscMediaType::DVD_Plus_R_DL:
			strcpy_s ( pInfo->szTypeString , "DVD+R DL" );
			break;
		case EHSOpticalDiscMediaType::DVD_Plus_RW:
			strcpy_s ( pInfo->szTypeString , "DVD+RW" );
			break;
		case EHSOpticalDiscMediaType::BD_ROM:
			strcpy_s ( pInfo->szTypeString , "BD-ROM" );
			break;
		case EHSOpticalDiscMediaType::BD_R:
			strcpy_s ( pInfo->szTypeString , "BD-R" );
			break;
		case EHSOpticalDiscMediaType::BD_RE:
			strcpy_s ( pInfo->szTypeString , "BD-RE" );
			break;
		default:
			strcpy_s ( pInfo->szTypeString , "Unknown" );
			break;
	}
	return TRUE;
}

char CHSOpticalDeviceControl::GetCurrentDriveLetter ( void )
{
	return this->cOpenedDriveLetter;
}

char CHSOpticalDeviceControl::GetCurrentDeviceInfo ( THSOpticalDriveDeviceInfo * pInfo )
{
	return this->GetDeviceInfoFromHandle(this->hDevice , pInfo);
}

bool CHSOpticalDeviceControl::Eject ( void )
{
	return this->Tray_Open();
}


bool CHSOpticalDeviceControl::Tray_Open ( void )
{
	if ( this->hDevice ) {
		DWORD unused;
		return ( DeviceIoControl ( this->hDevice , IOCTL_STORAGE_EJECT_MEDIA , nullptr , 0 , nullptr , 0 , &unused , 0 ) == TRUE );
	}
	return false;
}

bool CHSOpticalDeviceControl::Tray_Close ( void )
{
	if ( this->hDevice ) {
		DWORD unused;
		return ( DeviceIoControl ( this->hDevice , IOCTL_STORAGE_LOAD_MEDIA , nullptr , 0 , nullptr , 0 , &unused , 0 ) == TRUE );
	}
	return false;
}

bool CHSOpticalDeviceControl::Tray_Lock ( void )
{
	if ( this->hDevice ) {
		PREVENT_MEDIA_REMOVAL rem;
		DWORD unused;
		rem.PreventMediaRemoval = TRUE;
		return ( DeviceIoControl ( this->hDevice , IOCTL_STORAGE_MEDIA_REMOVAL , 
			&rem ,	sizeof(PREVENT_MEDIA_REMOVAL) ,
			nullptr , 0 , &unused , 0 ) == TRUE );
	}
	return false;
}

bool CHSOpticalDeviceControl::Tray_UnLock ( void )
{
	if ( this->hDevice ) {
		PREVENT_MEDIA_REMOVAL rem;
		DWORD unused;
		rem.PreventMediaRemoval = FALSE;
		return ( DeviceIoControl ( this->hDevice , IOCTL_STORAGE_MEDIA_REMOVAL ,
			&rem , sizeof ( PREVENT_MEDIA_REMOVAL ) ,
			nullptr , 0 , &unused , 0 ) == TRUE );
	}

	return false;
}

bool CHSOpticalDeviceControl::Tray_IsOpened ( void )
{

	if ( this->hDevice ) {
		SCSI_PASS_THROUGH_DIRECT direct;
		memset ( &direct , 0 , sizeof ( SCSI_PASS_THROUGH_DIRECT ) );
		UCHAR  ResultData [ 8 ];
		memset ( ResultData , 0 , sizeof ( ResultData ) );
		direct.Length = (USHORT)sizeof ( SCSI_PASS_THROUGH_DIRECT );
		direct.TimeOutValue = 5;

		direct.DataIn = SCSI_IOCTL_DATA_IN;
		direct.DataTransferLength = 8; /*ResultDataのサイズ*/
		direct.DataBuffer = ResultData;

		direct.CdbLength = 12;
		direct.Cdb [ 0 ] = 0xBD; /* MECHANISM STATUS Command */
		direct.Cdb [ 9 ] = 8; /*ResultDataのサイズ*/

		DWORD retsize;
		BOOL bRet = DeviceIoControl ( this->hDevice , IOCTL_SCSI_PASS_THROUGH_DIRECT ,
			&direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , &direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , &retsize , 0 );

		if ( bRet ) {
			return ( ( ResultData [ 1 ] & 0x10 ) == 0x10 );
		}
	}
	return false;
}

bool CHSOpticalDeviceControl::GetDiskGeometry ( DISK_GEOMETRY * pGeometry )
{
	if ( pGeometry ) {
		if ( this->hDevice ) {

			DWORD unused;
			return ( DeviceIoControl ( this->hDevice , IOCTL_DISK_GET_DRIVE_GEOMETRY ,
				nullptr , 0 ,
				pGeometry , sizeof ( DISK_GEOMETRY ) , &unused , 0 ) == TRUE );
		}
	}
	return false;
}

uint32_t CHSOpticalDeviceControl::GetMaxReadSpeed ( void )
{
	uint32_t Speed = 0;
	if ( this->hDevice ) {
		BOOL bRet;
		CDROM_PERFORMANCE_HEADER PerfHeader;
		CDROM_WRITE_SPEED_REQUEST  wreq;
		DWORD returnSize;

		wreq.RequestType = CdromWriteSpeedRequest;
		bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_GET_PERFORMANCE , &wreq , sizeof ( CDROM_WRITE_SPEED_REQUEST ) , &PerfHeader , sizeof ( CDROM_PERFORMANCE_HEADER ) , &returnSize , 0 );

		UINT32  DataLength = 0;
		for ( int i = 0; i < 4; i++ ) DataLength = ( DataLength << 8 ) | PerfHeader.DataLength [ i ];
		DataLength += sizeof ( UCHAR ) * 4;

		UINT32 DataSize = DataLength;
		if ( DataLength % 2 ) DataSize++;

		char *lpData = new char [ DataSize ];

		bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_GET_PERFORMANCE , &wreq , sizeof ( CDROM_WRITE_SPEED_REQUEST ) , lpData , DataSize , &returnSize , 0 );
		if ( bRet ) {
			CDROM_PERFORMANCE_HEADER *pFullHeader = reinterpret_cast< CDROM_PERFORMANCE_HEADER* >( lpData );
			if ( pFullHeader->Write == 0 ) {
				if ( pFullHeader->Except == 0 ) {
					CDROM_WRITE_SPEED_DESCRIPTOR  *pdesc = reinterpret_cast< CDROM_WRITE_SPEED_DESCRIPTOR * >( pFullHeader->Data );
					UINT32 ReadSpeed = 0;
					for ( int i = 0; i < 4; i++ ) {
						ReadSpeed <<= 8;
						ReadSpeed |= pdesc->ReadSpeed [ i ];
					}
					Speed = ReadSpeed;
				}
			}
		}
		delete [ ]lpData;

	}

	return Speed;
}

bool CHSOpticalDeviceControl::GetDiscStatus ( EHSOpticalDiscStatus * pStatus )
{
	if ( pStatus == nullptr )  return false;
	if ( this->hDevice ) {
		SCSI_PASS_THROUGH_DIRECT direct;
		memset ( &direct , 0 , sizeof ( SCSI_PASS_THROUGH_DIRECT ) );
		UCHAR  ResultData [ 34 ];

		memset ( ResultData , 0 , sizeof(ResultData) );

		direct.Length = (USHORT)sizeof ( SCSI_PASS_THROUGH_DIRECT );
		direct.TimeOutValue = 5;

		direct.DataIn = SCSI_IOCTL_DATA_IN;
		direct.DataTransferLength = 34;
		direct.DataBuffer = ResultData;

		direct.CdbLength = 10;
		direct.Cdb [ 0 ] = 0x51; /* Read Disc Information Command */
		direct.Cdb [ 1 ] = 0; /* Standard Disc Information */

		direct.Cdb [ 8 ] = 34;

		DWORD retsize;
		BOOL bRet = DeviceIoControl ( this->hDevice , IOCTL_SCSI_PASS_THROUGH_DIRECT , 
			&direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , &direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , &retsize , 0 );
		if ( bRet ) {

			uint16_t StructFullSize = ( ResultData [ 0 ] << 16 ) | ( ResultData [ 1 ] ) + 2;

			printf ( "[IOCTL_SCSI_PASS_THROUGH_DIRECT(0x51)]\n" );
			printf ( "OK (%d %d)\n" ,StructFullSize, ResultData[2] & 0x3);
			printf ( "\n" );
		}
	}
	return false;
}


bool CHSOpticalDeviceControl::SpinUp ( void )
{
	if ( this->hDevice ) {
		SCSI_PASS_THROUGH_DIRECT direct;
		memset ( &direct , 0 , sizeof ( SCSI_PASS_THROUGH_DIRECT ) );

		direct.Length = ( USHORT )sizeof ( SCSI_PASS_THROUGH_DIRECT );
		direct.TimeOutValue = 5;
		direct.ScsiStatus = 0;
		direct.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;
		direct.DataTransferLength = 0;
		direct.DataBuffer = nullptr;

		direct.CdbLength = 6;
		direct.Cdb [ 0 ] = 0x1B; /* START STOP UNIT Command */
		direct.Cdb [ 4 ] =  0x1; /*Start Disc*/

		DWORD retsize;
		BOOL bRet = DeviceIoControl ( this->hDevice , IOCTL_SCSI_PASS_THROUGH_DIRECT ,
			&direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , &direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , &retsize , 0 );
		if ( bRet ) {
			return ( direct.ScsiStatus == DHS_SCSI_STATUS_GOOD );
		}
	}
	return false;
}

bool CHSOpticalDeviceControl::SpinDown ( void )
{
	if ( this->hDevice ) {
		SCSI_PASS_THROUGH_DIRECT direct;
		memset ( &direct , 0 , sizeof ( SCSI_PASS_THROUGH_DIRECT ) );

		direct.Length = ( USHORT )sizeof ( SCSI_PASS_THROUGH_DIRECT );
		direct.TimeOutValue = 5;

		direct.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;
		direct.DataTransferLength = 0;
		direct.DataBuffer = nullptr;

		direct.CdbLength = 6;
		direct.Cdb [ 0 ] = 0x1B; /* START STOP UNIT Command */
		direct.Cdb [ 4 ] = 0; /*Stop Disc*/

		DWORD retsize;
		BOOL bRet = DeviceIoControl ( this->hDevice , IOCTL_SCSI_PASS_THROUGH_DIRECT ,
			&direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , &direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , &retsize , 0 );
		if ( bRet ) {
			return ( direct.ScsiStatus == DHS_SCSI_STATUS_GOOD );
		}
	}
	return false;
}
