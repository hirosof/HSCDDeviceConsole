#include "CHSCDDevice.hpp"


const UINT16  CHSCDDevice::CRCTABLE [ 256 ] = {
		0x0000 ,0x1021 ,0x2042 ,0x3063 ,0x4084 ,0x50A5 ,0x60C6 ,0x70E7 ,
		0x8108 ,0x9129 ,0xA14A ,0xB16B ,0xC18C ,0xD1AD ,0xE1CE ,0xF1EF ,
		0x1231 ,0x0210 ,0x3273 ,0x2252 ,0x52B5 ,0x4294 ,0x72F7 ,0x62D6 ,
		0x9339 ,0x8318 ,0xB37B ,0xA35A ,0xD3BD ,0xC39C ,0xF3FF ,0xE3DE ,
		0x2462 ,0x3443 ,0x0420 ,0x1401 ,0x64E6 ,0x74C7 ,0x44A4 ,0x5485 ,
		0xA56A ,0xB54B ,0x8528 ,0x9509 ,0xE5EE ,0xF5CF ,0xC5AC ,0xD58D ,
		0x3653 ,0x2672 ,0x1611 ,0x0630 ,0x76D7 ,0x66F6 ,0x5695 ,0x46B4 ,
		0xB75B ,0xA77A ,0x9719 ,0x8738 ,0xF7DF ,0xE7FE ,0xD79D ,0xC7BC ,
		0x48C4 ,0x58E5 ,0x6886 ,0x78A7 ,0x0840 ,0x1861 ,0x2802 ,0x3823 ,
		0xC9CC ,0xD9ED ,0xE98E ,0xF9AF ,0x8948 ,0x9969 ,0xA90A ,0xB92B ,
		0x5AF5 ,0x4AD4 ,0x7AB7 ,0x6A96 ,0x1A71 ,0x0A50 ,0x3A33 ,0x2A12 ,
		0xDBFD ,0xCBDC ,0xFBBF ,0xEB9E ,0x9B79 ,0x8B58 ,0xBB3B ,0xAB1A ,
		0x6CA6 ,0x7C87 ,0x4CE4 ,0x5CC5 ,0x2C22 ,0x3C03 ,0x0C60 ,0x1C41 ,
		0xEDAE ,0xFD8F ,0xCDEC ,0xDDCD ,0xAD2A ,0xBD0B ,0x8D68 ,0x9D49 ,
		0x7E97 ,0x6EB6 ,0x5ED5 ,0x4EF4 ,0x3E13 ,0x2E32 ,0x1E51 ,0x0E70 ,
		0xFF9F ,0xEFBE ,0xDFDD ,0xCFFC ,0xBF1B ,0xAF3A ,0x9F59 ,0x8F78 ,
		0x9188 ,0x81A9 ,0xB1CA ,0xA1EB ,0xD10C ,0xC12D ,0xF14E ,0xE16F ,
		0x1080 ,0x00A1 ,0x30C2 ,0x20E3 ,0x5004 ,0x4025 ,0x7046 ,0x6067 ,
		0x83B9 ,0x9398 ,0xA3FB ,0xB3DA ,0xC33D ,0xD31C ,0xE37F ,0xF35E ,
		0x02B1 ,0x1290 ,0x22F3 ,0x32D2 ,0x4235 ,0x5214 ,0x6277 ,0x7256 ,
		0xB5EA ,0xA5CB ,0x95A8 ,0x8589 ,0xF56E ,0xE54F ,0xD52C ,0xC50D ,
		0x34E2 ,0x24C3 ,0x14A0 ,0x0481 ,0x7466 ,0x6447 ,0x5424 ,0x4405 ,
		0xA7DB ,0xB7FA ,0x8799 ,0x97B8 ,0xE75F ,0xF77E ,0xC71D ,0xD73C ,
		0x26D3 ,0x36F2 ,0x0691 ,0x16B0 ,0x6657 ,0x7676 ,0x4615 ,0x5634 ,
		0xD94C ,0xC96D ,0xF90E ,0xE92F ,0x99C8 ,0x89E9 ,0xB98A ,0xA9AB ,
		0x5844 ,0x4865 ,0x7806 ,0x6827 ,0x18C0 ,0x08E1 ,0x3882 ,0x28A3 ,
		0xCB7D ,0xDB5C ,0xEB3F ,0xFB1E ,0x8BF9 ,0x9BD8 ,0xABBB ,0xBB9A ,
		0x4A75 ,0x5A54 ,0x6A37 ,0x7A16 ,0x0AF1 ,0x1AD0 ,0x2AB3 ,0x3A92 ,
		0xFD2E ,0xED0F ,0xDD6C ,0xCD4D ,0xBDAA ,0xAD8B ,0x9DE8 ,0x8DC9 ,
		0x7C26 ,0x6C07 ,0x5C64 ,0x4C45 ,0x3CA2 ,0x2C83 ,0x1CE0 ,0x0CC1 ,
		0xEF1F ,0xFF3E ,0xCF5D ,0xDF7C ,0xAF9B ,0xBFBA ,0x8FD9 ,0x9FF8 ,
		0x6E17 ,0x7E36 ,0x4E55 ,0x5E74 ,0x2E93 ,0x3EB2 ,0x0ED1 ,0x1EF0
};


bool CHSCDDevice::Crc16 ( void * lpData , size_t size , UINT16 *lpCRC16 )
{
	if ( lpData == nullptr ) return false;
	if ( lpCRC16 == nullptr ) return false;
	char *lpByteData = reinterpret_cast< char* >( lpData );
	uint16_t crc = 0;
	size_t tableidx;
	for ( size_t i = 0; i < size; i++ ) {
		tableidx = ( crc >> 8 ) ^ ( *( lpByteData + i ) );
		tableidx &= 0xFF;
		crc = ( crc << 8 ) ^ CHSCDDevice::CRCTABLE [ tableidx ];
	}
	*lpCRC16 = crc ^ 0xFFFF;
	return true;
}

UINT32 CHSCDDevice::MSFToSectors ( UINT8 M , UINT8 S , UINT8 F )
{
	return  ( M * 60 + S ) * 75 + F;
}

BOOL CHSCDDevice::ParseRawTOCInfo ( CDROM_TOC_FULL_TOC_DATA * pFullToc , THSCD_DiscInfo * pInfo )
{

	if ( pInfo == nullptr ) return FALSE;
	if ( pFullToc == nullptr ) return FALSE;

	memset( pInfo, NULL ,sizeof( THSCD_DiscInfo ) );
	int SessionNums = pFullToc->LastCompleteSession - pFullToc->FirstCompleteSession + 1;

	UINT16 StructSize = ( ( pFullToc->Length [ 0 ] << 8 ) | pFullToc->Length [ 1 ] )  + sizeof ( UCHAR ) * 2;
	UINT16 InnerStructSize = StructSize - sizeof ( UCHAR ) * 4;
	UINT16 InnerStructElements = ( InnerStructSize ) / sizeof ( CDROM_TOC_FULL_TOC_DATA_BLOCK );

	SessionInnerData *psid = new SessionInnerData [ SessionNums ];
	SessionInnerData *pInner;

	memset( psid, 0, sizeof( SessionInnerData ) * SessionNums );

	UCHAR SessionNumberTable[VHSCD_MAX_TRACK];

	CDROM_TOC_FULL_TOC_DATA_BLOCK *pBlock;
	THSCD_TrackInfo *pTrack;

#ifdef _DEBUG

	std::string sep( 64, '=' );

	printf( "%s\n", sep.c_str( ) );
	printf( "CHSCDDevice::ParseRawTOCInfo  DebugLogs\n" );
	printf( "%s\n", sep.c_str( ) );

#endif // _DEBUG


	for ( size_t i = 0; i < InnerStructElements; i++ ) {
		pBlock = &pFullToc->Descriptors [ i ];
		pInner = psid + (pBlock->SessionNumber - 1);

#ifdef _DEBUG

		printf( "CDROM_TOC_FULL_TOC_DATA_BLOCK [%zu]\n", i );

		printf( "\tSessionNumber : %u\n", pBlock->SessionNumber );
		printf( "\tControl : 0x%02X (%d)\n", pBlock->Control, pBlock->Control );
		printf( "\tAdr : 0x%02X (%d)\n", pBlock->Adr, pBlock->Adr );
		printf( "\tPoint : 0x%02X (%d)\n", pBlock->Point, pBlock->Point );
		printf( "\tMsfExtra : %02d:%02d:%02d\n", pBlock->MsfExtra[0], pBlock->MsfExtra[1], pBlock->MsfExtra[2] );
		printf( "\tMsf : %02d:%02d:%02d\n", pBlock->Msf[0], pBlock->Msf[1], pBlock->Msf[2] );


		printf( "\n" );
		
#endif // _DEBUG


		switch ( pBlock->Point ) {
			case 0xA0:
				//現在のセッション上での開始トラック番号の通知
				pInner->StartTrackNumber = pBlock->Msf [ 0 ];
				//ディスク種類
				if ( pBlock->Msf [ 1 ] == 0 ) pInfo->DiscType = EHSCD_DiscMode::Mode1;
				else if ( pBlock->Msf [ 1 ] == 0x10 ) pInfo->DiscType = EHSCD_DiscMode::Interactive;
				else if ( pBlock->Msf [ 1 ] == 0x20 ) pInfo->DiscType = EHSCD_DiscMode::Mode2XA;
				else 	pInfo->DiscType = EHSCD_DiscMode::Unknown;
				break;
			case 0xA1:
				//現在のセッション上での終了トラック番号の通知
				pInner->EndTrackNumber = pBlock->Msf [ 0 ];
				break;
			case 0xA2:
				//現在のセッションの終端
				pInner->LastSectors = this->MSFToSectors ( pBlock->Msf [ 0 ] , pBlock->Msf [ 1 ] , pBlock->Msf [ 2 ] );
				break;
			default:
				if ( pBlock->Adr == ADR_ENCODES_CURRENT_POSITION ) {
					if ( ( pBlock->Point >= 1 ) && ( pBlock->Point <= 99 ) ) {
						//トラック1～99に関する情報
						pTrack = &pInfo->TrackInfo [ pBlock->Point - 1 ];
						pTrack->MSFPosition.Start = this->MSFToSectors ( pBlock->Msf [ 0 ] , pBlock->Msf [ 1 ] , pBlock->Msf [ 2 ] );

						SessionNumberTable [ pBlock->Point - 1 ] = pBlock->SessionNumber;
						switch ( pBlock->Control ) {
							case  0:
								pTrack->Type = EHSCD_TrackType::Audio;
								break;
							case AUDIO_WITH_PREEMPHASIS:
								pTrack->Type = EHSCD_TrackType::AudioWithPreemphasis;
								break;
							case DIGITAL_COPY_PERMITTED:
								pTrack->Type = EHSCD_TrackType::DigitalCopyPreemphasis;
								break;
							case AUDIO_DATA_TRACK:
								pTrack->Type = EHSCD_TrackType::Data;
								break;
							case TWO_FOUR_CHANNEL_AUDIO:
								pTrack->Type = EHSCD_TrackType::FourChannelAudio;
								break;
							default:
								pTrack->Type = EHSCD_TrackType::Unknown;
								break;
						}
					}
				}
				break;
		}
	}

	//トラック数に関する計算
	pInfo->StartTrackNumber = psid->StartTrackNumber;
	pInfo->EndTrackNumber = (psid + (SessionNums-1))->EndTrackNumber;
	pInfo->NumberOfTracks = pInfo->EndTrackNumber - pInfo->StartTrackNumber + 1;

	//各トラックの終了地点の計算
	for ( UINT32 t = pInfo->StartTrackNumber; t <= pInfo->EndTrackNumber; t++ ) {
		pInner = psid + ( SessionNumberTable [ t - 1 ] - 1 );
		pTrack = &pInfo->TrackInfo [ t - 1 ];
		if ( pInner->EndTrackNumber == t ) {
			pTrack->MSFPosition.End = pInner->LastSectors - 1;
		} else {
			pTrack->MSFPosition.End = pInfo->TrackInfo [ t ].MSFPosition.Start - 1;
		}
		pTrack->LengthSectors = pTrack->MSFPosition.End - pTrack->MSFPosition.Start + 1;
	}

	UINT32 Minus = pInfo->TrackInfo [ pInfo->StartTrackNumber - 1 ].MSFPosition.Start;
	pInfo->LogicalBlockAddress = Minus;
	for ( UINT32 t = pInfo->StartTrackNumber; t <= pInfo->EndTrackNumber; t++ ) {
		pTrack = &pInfo->TrackInfo [ t - 1 ];
		pTrack->StartSectorPosition =pTrack->MSFPosition.Start;
		pTrack->EndSectorPosition = pTrack->MSFPosition.End;

		pTrack->StartSectorPosition -= Minus;
		pTrack->EndSectorPosition -= Minus;
	}

	this->JugdeDiscType ( pInfo );

	delete [ ]psid;

#ifdef _DEBUG
	printf( "%s\n", std::string(64,'-').c_str() );
	printf( "\n" );
#endif // _DEBUG

	return TRUE;
}

bool CHSCDDevice::JugdeDiscType ( THSCD_DiscInfo * pInfo )
{
	if ( pInfo == nullptr ) return false;

	if ( pInfo->DiscType == EHSCD_DiscMode::Mode2XA ) {
		pInfo->MusicCDType = EHSCD_DiscType::CDExtra;
		return true;
	}
	EHSCD_TrackType TrackType;
	UINT8 st = pInfo->StartTrackNumber;
	UINT8 et = pInfo->EndTrackNumber;

	//純粋なディスクの判定
	UINT32 CountOfAudioTrack = 0;
	for ( UINT8 t = st; t <= et; t++ ) {
		TrackType = pInfo->TrackInfo [ t - 1 ].Type;
		switch ( TrackType ) {
			case EHSCD_TrackType::Audio:
			case EHSCD_TrackType::AudioWithPreemphasis:
			case EHSCD_TrackType::DigitalCopyPreemphasis:
			case EHSCD_TrackType::FourChannelAudio:
				CountOfAudioTrack++;
				break;
		}
	}
	if ( CountOfAudioTrack == 0 ) {
		pInfo->MusicCDType = EHSCD_DiscType::FullData;
	} else if ( CountOfAudioTrack == pInfo->NumberOfTracks ) {
		pInfo->MusicCDType = EHSCD_DiscType::FullAudio;
	} else if ( (CountOfAudioTrack +1) == pInfo->NumberOfTracks ) {

		if ( pInfo->TrackInfo [ st -1].Type == EHSCD_TrackType::Data ) pInfo->MusicCDType = EHSCD_DiscType::MixCD;
		else if ( pInfo->TrackInfo [ et-1 ].Type == EHSCD_TrackType::Data ) pInfo->MusicCDType = EHSCD_DiscType::CDExtra;
		else pInfo->MusicCDType = EHSCD_DiscType::Unknown;
	} else {
		pInfo->MusicCDType = EHSCD_DiscType::Unknown;
	}
	return true;
}

bool CHSCDDevice::GetDeviceInfoFromHandle( HANDLE hOpticalDrive, THSOpticalDriveDeviceInfo* pInfo ) {
	if ( hOpticalDrive == INVALID_HANDLE_VALUE ) return false;
	if ( hOpticalDrive == NULL ) return false;
	if ( pInfo ) {

		INQUIRYDATA data;
		DWORD returnSize;

		BOOL bRet = DeviceIoControl( hOpticalDrive, IOCTL_CDROM_GET_INQUIRY_DATA, 0, 0,
			&data, sizeof( INQUIRYDATA ), &returnSize, NULL );
		if ( bRet ) {
			char space[2] = { ' ' , '\0' };
			lstrcpynA( pInfo->VendorID, reinterpret_cast<char*>( data.VendorId ), DHSOpticalDriveVendorIDLength + 1 );
			lstrcpynA( pInfo->ProductID, reinterpret_cast<char*>( data.ProductId ), DHSOpticalDriveProductIDLength + 1 );
			lstrcpynA( pInfo->ProductRevisionID, reinterpret_cast<char*>( data.ProductRevisionLevel ), DHSOpticalDriveProductRevisionIDLength + 1 );

			pInfo->DeviceName[0] = '\0';

			lstrcatA( pInfo->DeviceName, pInfo->VendorID );
			if ( pInfo->DeviceName[0] != '\0' ) lstrcatA( pInfo->DeviceName, space );
			lstrcatA( pInfo->DeviceName, pInfo->ProductID );
			if ( pInfo->DeviceName[0] != '\0' ) lstrcatA( pInfo->DeviceName, space );
			lstrcatA( pInfo->DeviceName, pInfo->ProductRevisionID );

			return true;
		}
	}
	return false;
}

bool CHSCDDevice::EnumOpticalDrive( THSEnumrateOpticalDriveInfo* pInfo ) {
	if ( pInfo == nullptr ) return false;
	bool bRet;
	pInfo->uOpticalDriveCount = 0;
	for ( int i = 0; i < 26; i++ )pInfo->Drives[i].Letter = '\0';
	for ( char c = 'A'; c <= 'Z'; c++ ) {
		if ( IsCDDevice( c ) ) {
			bRet = GetDeviceInfo( c, &pInfo->Drives[pInfo->uOpticalDriveCount].Info );
			pInfo->Drives[pInfo->uOpticalDriveCount].Letter = c;
			pInfo->Drives[pInfo->uOpticalDriveCount].bIncludedInfo = bRet;
			pInfo->uOpticalDriveCount++;
		}
	}
	return true;
}

bool CHSCDDevice::GetDeviceInfo( const char OpticalDriveLetter, THSOpticalDriveDeviceInfo* pInfo ) {
	if ( pInfo == nullptr ) return false;
	if ( IsCDDevice( OpticalDriveLetter ) ) {

		HANDLE hDrive;

		char FileName[8];

		wsprintfA( FileName, "\\\\.\\%c:", OpticalDriveLetter );

		hDrive = CreateFileA( FileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if ( hDrive != INVALID_HANDLE_VALUE ) {

			bool bRet = GetDeviceInfoFromHandle( hDrive, pInfo );
			CloseHandle( hDrive );
			
			return bRet;
		}
	}
	return false;
}

BOOL CHSCDDevice::IsCDDevice ( const char DriveLetter )
{
	bool Check = ( DriveLetter >= 'A' ) && ( DriveLetter <= 'Z' );
	Check |= ( DriveLetter >= 'a' ) && ( DriveLetter <= 'z' );

	if ( Check ) {

		char DL[4];

		wsprintfA( DL, "%c:\\", DriveLetter );

		return ( GetDriveTypeA( DL ) == DRIVE_CDROM );
	}
	return false;
}

CHSCDDevice::CHSCDDevice ( )
{
	this->hDevice = NULL;
}

CHSCDDevice::~CHSCDDevice ( )
{
	this->Close ( );
}

BOOL CHSCDDevice::Open ( const char DriveLetter )
{
	if ( IsCDDevice ( DriveLetter ) == false ) return FALSE;

	if ( hDevice ) {
		this->Close ( );
	}
	//\\.\C:

	char FileName [ 8 ];

	wsprintfA ( FileName , "\\\\.\\%c:" , DriveLetter );

	this->hDevice = CreateFileA ( FileName , GENERIC_READ | GENERIC_WRITE , FILE_SHARE_READ | FILE_SHARE_WRITE , nullptr , OPEN_EXISTING , FILE_FLAG_NO_BUFFERING , NULL );

	if ( hDevice == INVALID_HANDLE_VALUE ) {
		hDevice = NULL;
		return FALSE;
	}

	return TRUE;
}

void CHSCDDevice::Close ( void )
{
	if ( this->hDevice ) {
		CloseHandle ( this->hDevice );
		hDevice = NULL;
	}
}

HANDLE CHSCDDevice::GetHandle ( void )
{
	return this->hDevice;
}

BOOL CHSCDDevice::EjectMedia ( void )
{
	if ( hDevice ) {
		return DeviceIoControl ( hDevice , IOCTL_CDROM_EJECT_MEDIA , NULL , 0 , NULL , 0 , NULL , NULL );
	} else {
		return FALSE;
	}
}

size_t CHSCDDevice::GetDeviceNameByDeviceProperty ( char * pName , size_t NameLen , bool bExtraSpaceDelete )
{
	if ( hDevice == NULL ) return -1;

	STORAGE_PROPERTY_QUERY spQuery;
	spQuery.PropertyId = StorageDeviceProperty;
	spQuery.QueryType = PropertyStandardQuery;
	spQuery.AdditionalParameters [ 0 ] = 0;

	STORAGE_DEVICE_DESCRIPTOR sdd;
	DWORD retSize;
	BOOL bRet = DeviceIoControl ( hDevice , IOCTL_STORAGE_QUERY_PROPERTY , &spQuery , sizeof ( STORAGE_PROPERTY_QUERY ) , &sdd , sizeof ( STORAGE_DEVICE_DESCRIPTOR ) , &retSize , NULL );
	DWORD gle = GetLastError ( );
	if ( bRet ) {
		BYTE *pData = new BYTE [ sdd.Size ];

		bRet = DeviceIoControl ( hDevice , IOCTL_STORAGE_QUERY_PROPERTY , &spQuery , sizeof ( STORAGE_PROPERTY_QUERY ) , pData , sdd.Size , &retSize , NULL );


		sdd = *( ( STORAGE_DEVICE_DESCRIPTOR* ) pData );
		std::string  str;

		//ベンダーID
		if ( sdd.VendorIdOffset > 0 ) {
			str.append ( ( char* ) ( pData + sdd.VendorIdOffset ) );
			if ( bExtraSpaceDelete ) {
				while ( str.back ( ) == ' ' ) {
					str.pop_back ( );
				}
			}
			str.append ( " " );
		}

		//プロダクトID
		if ( sdd.ProductIdOffset > 0 ) {
			str.append ( ( char* ) ( pData + sdd.ProductIdOffset ) );
			if ( bExtraSpaceDelete ) {
				while ( str.back ( ) == ' ' ) {
					str.pop_back ( );
				}
			}
			str.append ( " " );
		}

		//プロダクトリビジョン
		if ( sdd.ProductRevisionOffset > 0 ) {
			str.append ( ( char* ) ( pData + sdd.ProductRevisionOffset ) );
			if ( bExtraSpaceDelete ) {
				while ( str.back ( ) == ' ' ) {
					str.pop_back ( );
				}
			}
			str.append ( " " );
		}

		if ( str.size ( ) > 0 ) {
			while ( str.back ( ) == ' ' ) str.pop_back ( );
		}

		delete [ ]pData;

		if ( ( pName != nullptr ) && ( NameLen > 0 ) ) {
			if ( str.size ( ) == 0 ) return 0;
			lstrcpynA ( pName , str.c_str ( ) , static_cast<int>(NameLen) );
			size_t nlen = str.length ( );
			return min ( NameLen , nlen );
		}
		return str.length ( );
	}

	return 0;
}

bool CHSCDDevice::GetCurentDeviceInfo( THSOpticalDriveDeviceInfo* pInfo ) {
	return this->GetDeviceInfoFromHandle(this->hDevice , pInfo);
}

BOOL CHSCDDevice::IsPossibleAccess ( void )
{
	if ( hDevice == NULL ) return FALSE;
	return DeviceIoControl ( hDevice , IOCTL_STORAGE_CHECK_VERIFY , NULL , NULL , NULL , NULL , NULL , NULL );
}

BOOL CHSCDDevice::GetDiscInfo ( THSCD_DiscInfo * pInfo )
{
	if ( pInfo == nullptr ) return FALSE;
	if ( hDevice == NULL ) return FALSE;


#ifdef _DEBUG

	std::string sep( 64, '=' );

	printf( "%s\n", sep.c_str( ) );
	printf( "CHSCDDevice::GetDiscInfo  DebugLogs\n" );
	printf( "%s\n", sep.c_str( ) );

#endif // _DEBUG

	CDROM_READ_TOC_EX tocex;
	memset( &tocex, 0, sizeof( CDROM_READ_TOC_EX ) );

	tocex.Format = CDROM_READ_TOC_EX_FORMAT_FULL_TOC;
	tocex.Msf = 1;

	CDROM_TOC_FULL_TOC_DATA full_toc;
	DWORD returnSize;

	BOOL bRet = DeviceIoControl ( this->hDevice , IOCTL_CDROM_READ_TOC_EX , &tocex , sizeof ( CDROM_READ_TOC_EX ) ,
		&full_toc , sizeof ( CDROM_TOC_FULL_TOC_DATA ) , &returnSize , nullptr
	);

	if ( bRet ) {

		UINT32 FullStructSize = ( ( full_toc.Length [ 0 ] << 8 ) | ( full_toc.Length [ 1 ] ) ) + sizeof ( UCHAR ) * 2;

		if(( FullStructSize % 4 )!=0)	FullStructSize = FullStructSize + ( 4 - FullStructSize % 4 );

		tocex.SessionTrack = 0;

#ifdef _DEBUG

		printf( "<<1 try>>\n" );
		printf( "FullStructSize = %u\n", FullStructSize );
		printf( "returnSize = %u\n", returnSize );
		printf( "FirstCall Length = %u\n", ( ( full_toc.Length[0] << 8 ) | ( full_toc.Length[1] ) ) );
		printf( "\n" );

#endif

		unsigned char *lpData = new unsigned char [ FullStructSize];
		memset( lpData, 0, FullStructSize );

		BOOL ReturnValue = FALSE;

		bRet = DeviceIoControl ( this->hDevice , IOCTL_CDROM_READ_TOC_EX , &tocex , sizeof ( CDROM_READ_TOC_EX ) ,
			lpData , FullStructSize , &returnSize , nullptr );



		if ( bRet ) {

#ifdef _DEBUG
			printf( "<<2 try>>\n" );

			printf( "FullStructSize = %u\n", FullStructSize );
			printf( "returnSize = %u\n", returnSize );
			
			printf( "FirstCall Length = %u\n", ( ( lpData[0] << 8 ) | ( lpData[1] ) ) );
			printf( "\n" );

#endif

			ReturnValue = this->ParseRawTOCInfo( reinterpret_cast<CDROM_TOC_FULL_TOC_DATA*>( lpData ), pInfo );
		}
		delete []lpData;

#ifdef _DEBUG
		printf( "%s\n", std::string( 64, '-' ).c_str( ) );
		printf( "\n" );
#endif // _DEBUG

		return ReturnValue;

	}
#ifdef _DEBUG
	printf( "%s\n", std::string( 64, '-' ).c_str( ) );
	printf( "\n" );
#endif // _DEBUG
	return 0;
}

UINT32 CHSCDDevice::Read ( void * lpOutBuffer , size_t OutBufferSize , UINT8 TrackNumber , UINT32 OffsetSector , UINT32 ReadSector )
{
	if ( lpOutBuffer == nullptr ) return 0;
	if ( OutBufferSize == 0 ) return 0;
	if ( ReadSector == 0 )return 0;
	if ( ( TrackNumber < 1 ) || ( TrackNumber > 99 ) )return 0;
	THSCD_DiscInfo disc;

	if ( this->GetDiscInfo ( &disc ) == FALSE ) return 0;

	if ( ( disc.StartTrackNumber <= TrackNumber ) && ( disc.EndTrackNumber >= TrackNumber ) ) {

		THSCD_TrackInfo *pTrack = &disc.TrackInfo [ TrackNumber - 1 ];

		UINT32 Start = pTrack->StartSectorPosition + OffsetSector;
		UINT32 End = Start + ReadSector - 1;

		if ( Start > pTrack->EndSectorPosition ) return 0;
		if ( End > pTrack->EndSectorPosition ) End = pTrack->EndSectorPosition;

		UINT32  Length = End - Start + 1;

		UINT32 BytesLength = HSGetBytesSizeBySector ( Length );

		if ( OutBufferSize < BytesLength ) return 0;

		RAW_READ_INFO RRI;
		RRI.SectorCount = 1;
		RRI.TrackMode = ( pTrack->Type == EHSCD_TrackType::Audio ) ? CDDA : YellowMode2;

		DWORD returnSize;

		DWORD CompliteSize = 0;
		char *pTop = reinterpret_cast< char* >( lpOutBuffer );
		char *pCurrent;
		for ( UINT32 i = 0; i < Length; i++ ) {
			RRI.DiskOffset.QuadPart = (Start + i) * 2048;
			pCurrent = pTop + ( i*VHSCD_BYTES_PER_SECTOR );

			if ( DeviceIoControl ( hDevice , IOCTL_CDROM_RAW_READ , &RRI , sizeof ( RAW_READ_INFO ) , pCurrent , VHSCD_BYTES_PER_SECTOR , &returnSize , 0 ) ) {
				CompliteSize += returnSize;
				if ( VHSCD_BYTES_PER_SECTOR != returnSize ) break;
			}
		}

		return CompliteSize / VHSCD_BYTES_PER_SECTOR;
	}
	return 0;
}

UINT32 CHSCDDevice::Read8 ( void * lpOutBuffer , size_t OutBufferSize , UINT8 TrackNumber , UINT32 OffsetSector , UINT32 ReadSector )
{
	if ( lpOutBuffer == nullptr ) return 0;
	if ( OutBufferSize == 0 ) return 0;
	if ( ReadSector == 0 )return 0;
	if ( ( TrackNumber < 1 ) || ( TrackNumber > 99 ) )return 0;
	THSCD_DiscInfo disc;

	if ( this->GetDiscInfo ( &disc ) == FALSE ) return 0;

	if ( ( disc.StartTrackNumber <= TrackNumber ) && ( disc.EndTrackNumber >= TrackNumber ) ) {

		THSCD_TrackInfo *pTrack = &disc.TrackInfo [ TrackNumber - 1 ];

		UINT32 Start = pTrack->StartSectorPosition + OffsetSector;
		UINT32 End = Start + ReadSector - 1;

		if ( Start > pTrack->EndSectorPosition ) return 0;
		if ( End > pTrack->EndSectorPosition ) End = pTrack->EndSectorPosition;

		UINT32  Length = End - Start + 1;

		UINT32 BytesLength = HSGetBytesSizeBySector ( Length );

		if ( OutBufferSize < BytesLength ) return 0;

		const UINT32 BlockSectorSize = 8;

		UINT32 ReadCountLength = Length / BlockSectorSize;
		UINT32 RestCountLength = Length % BlockSectorSize;

		if ( RestCountLength > 0 ) ReadCountLength++;

		RAW_READ_INFO RRI;
		RRI.TrackMode = ( pTrack->Type == EHSCD_TrackType::Audio ) ? CDDA : YellowMode2;

		DWORD returnSize;

		DWORD CompliteSize = 0;
		char *pTop = reinterpret_cast< char* >( lpOutBuffer );
		char *pCurrent;

		INT64  pos_in;
		DWORD dwStart , dwEnd;
		BOOL bRet;
		for ( UINT32 pos = 0; pos < ReadCountLength; pos++ ) {
			RRI.DiskOffset.QuadPart = ( Start + pos *  BlockSectorSize ) * 2048;
			pos_in = RRI.DiskOffset.QuadPart;
			pCurrent = pTop + ( pos*BlockSectorSize*VHSCD_BYTES_PER_SECTOR );

			RRI.SectorCount = BlockSectorSize;
			if ( ( pos + 1 ) == ReadCountLength ) {
				if ( RestCountLength != 0 ) {
					RRI.SectorCount = RestCountLength;
				}
			}
			//VHSCD_BYTES_PER_SECTOR
			returnSize = 0;
			dwStart = timeGetTime ( );
			bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_RAW_READ , &RRI , sizeof ( RAW_READ_INFO ) , pCurrent , VHSCD_BYTES_PER_SECTOR * RRI.SectorCount , &returnSize , 0 );
			dwEnd = timeGetTime ( );
		//	printf ( "[DeviceIoControl Time : %u , size : %u(%u)] %u\n" ,  dwEnd - dwStart , returnSize , VHSCD_BYTES_PER_SECTOR * RRI.SectorCount  , bRet);
			if ( bRet) {
				CompliteSize += returnSize;
				if ( (VHSCD_BYTES_PER_SECTOR * RRI.SectorCount )!= returnSize ) break;
			} else {
			//	printf ( "Error Code : %u\n" , GetLastError ( ) );
			}

		}
		return CompliteSize / VHSCD_BYTES_PER_SECTOR;

	}

	return 0;
}

UINT32 CHSCDDevice::Read16 (void* lpOutBuffer, size_t OutBufferSize, UINT8 TrackNumber, UINT32 OffsetSector, UINT32 ReadSector) {
	if (lpOutBuffer == nullptr) return 0;
	if (OutBufferSize == 0) return 0;
	if (ReadSector == 0)return 0;
	if ((TrackNumber < 1) || (TrackNumber > 99))return 0;
	THSCD_DiscInfo disc;

	if (this->GetDiscInfo (&disc) == FALSE) return 0;

	if ((disc.StartTrackNumber <= TrackNumber) && (disc.EndTrackNumber >= TrackNumber)) {

		THSCD_TrackInfo* pTrack = &disc.TrackInfo[TrackNumber - 1];

		UINT32 Start = pTrack->StartSectorPosition + OffsetSector;
		UINT32 End = Start + ReadSector - 1;

		if (Start > pTrack->EndSectorPosition) return 0;
		if (End > pTrack->EndSectorPosition) End = pTrack->EndSectorPosition;

		UINT32  Length = End - Start + 1;

		UINT32 BytesLength = HSGetBytesSizeBySector (Length);

		if (OutBufferSize < BytesLength) return 0;

		const UINT32 BlockSectorSize = 16;

		UINT32 ReadCountLength = Length / BlockSectorSize;
		UINT32 RestCountLength = Length % BlockSectorSize;

		if (RestCountLength > 0) ReadCountLength++;

		RAW_READ_INFO RRI;
		RRI.TrackMode = (pTrack->Type == EHSCD_TrackType::Audio) ? CDDA : YellowMode2;

		DWORD returnSize;

		DWORD CompliteSize = 0;
		char* pTop = reinterpret_cast<char*>(lpOutBuffer);
		char* pCurrent;

		INT64  pos_in;
		DWORD dwStart, dwEnd;
		BOOL bRet;
		for (UINT32 pos = 0; pos < ReadCountLength; pos++) {
			RRI.DiskOffset.QuadPart = (Start + pos * BlockSectorSize) * 2048;
			pos_in = RRI.DiskOffset.QuadPart;
			pCurrent = pTop + (pos * BlockSectorSize * VHSCD_BYTES_PER_SECTOR);

			RRI.SectorCount = BlockSectorSize;
			if ((pos + 1) == ReadCountLength) {
				if (RestCountLength != 0) {
					RRI.SectorCount = RestCountLength;
				}
			}
			//VHSCD_BYTES_PER_SECTOR
			returnSize = 0;
			dwStart = timeGetTime ();
			bRet = DeviceIoControl (hDevice, IOCTL_CDROM_RAW_READ, &RRI, sizeof (RAW_READ_INFO), pCurrent, VHSCD_BYTES_PER_SECTOR * RRI.SectorCount, &returnSize, 0);
			dwEnd = timeGetTime ();
			//	printf ( "[DeviceIoControl Time : %u , size : %u(%u)] %u\n" ,  dwEnd - dwStart , returnSize , VHSCD_BYTES_PER_SECTOR * RRI.SectorCount  , bRet);
			if (bRet) {
				CompliteSize += returnSize;
				if ((VHSCD_BYTES_PER_SECTOR * RRI.SectorCount) != returnSize) break;
			} else {
				//	printf ( "Error Code : %u\n" , GetLastError ( ) );
			}

		}
		return CompliteSize / VHSCD_BYTES_PER_SECTOR;

	}

	return 0;
}

UINT32 CHSCDDevice::ReadByReadFile ( void * lpOutBuffer , size_t OutBufferSize , UINT8 TrackNumber , UINT32 OffsetSector , UINT32 ReadSector )
{
	/*
	if ( lpOutBuffer == nullptr ) return 0;
	if ( OutBufferSize == 0 ) return 0;
	if ( ReadSector == 0 )return 0;
	if ( ( TrackNumber < 1 ) || ( TrackNumber > 99 ) )return 0;
	THSCD_DiscInfo disc;

	if ( this->GetDiscInfo ( &disc ) == FALSE ) return 0;

	if ( ( disc.StartTrackNumber <= TrackNumber ) && ( disc.EndTrackNumber >= TrackNumber ) ) {

		THSCD_TrackInfo *pTrack = &disc.TrackInfo [ TrackNumber - 1 ];

		UINT32 Start = pTrack->StartSectorPosition + OffsetSector;
		UINT32 End = Start + ReadSector - 1;

		if ( Start > pTrack->EndSectorPosition ) return 0;
		if ( End > pTrack->EndSectorPosition ) End = pTrack->EndSectorPosition;

		UINT32  Length = End - Start + 1;
		UINT32 BytesLength = HSGetBytesSizeBySector ( Length );

		if ( OutBufferSize < BytesLength ) return 0;

		SetFilePointer ( this->hDevice , 2048 * Start , 0 , FILE_BEGIN );

		DWORD loaded;
		if ( ReadFile ( hDevice , lpOutBuffer , Length * VHSCD_BYTES_PER_SECTOR , &loaded , 0 ) ) {
			return loaded / VHSCD_BYTES_PER_SECTOR;
		} else {
			printf ( "E %u\n" , GetLastError() );
		}

	}
	*/
	return 0;
}
uint32_t CHSCDDevice::GetMaxReadSpeed ( void )
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

UINT8 HSGetMSFMinuteFromSector ( UINT32 Sector )
{
	return ( Sector / 75 ) / 60;
}

UINT8 HSGetMSFSecondFromSector ( UINT32 Sector )
{
	return ( Sector / 75 ) % 60;
}

UINT8 HSGetMSFFrameFromSector ( UINT32 Sector )
{
	return Sector % 75;
}

THSCD_MSF_TIME HSGetMSFTimeBySector ( UINT32 Sector )
{
	THSCD_MSF_TIME t;

	t.Minutes = HSGetMSFMinuteFromSector ( Sector );
	t.Seconds = HSGetMSFSecondFromSector ( Sector );
	t.Frames = HSGetMSFFrameFromSector ( Sector );

	return t;
}

UINT32 HSGetSectorByMSFTime ( THSCD_MSF_TIME msf )
{
	return HSGetSectorByMSFTime ( msf.Minutes , msf.Seconds , msf.Frames );
}

UINT32 HSGetSectorByMSFTime ( UINT8 M , UINT8 S , UINT8 F )
{
	return  ( M * 60 + S ) * 75 + F;
}

UINT32 HSGetBytesSizeBySector ( UINT32 Sector )
{
	return Sector * VHSCD_BYTES_PER_SECTOR;
}

UINT32 HSGetBytesSizeByMSFTime ( THSCD_MSF_TIME msf )
{
	return HSGetBytesSizeBySector ( HSGetSectorByMSFTime ( msf ) );
}

UINT32 HSGetBytesSizeByMSFTime ( UINT8 M , UINT8 S , UINT8 F )
{
	return HSGetBytesSizeBySector ( HSGetSectorByMSFTime(M , S , F ));
}
