#include <iostream>
#include <stdio.h>
#include "CHSCDDevice.hpp"
#include "inc/HSWAVE.hpp"
#include "inc/hash/HSHashMD5.hpp"
#include "inc/hash/HSSHA1.hpp"
#include "inc/hash/HSSHA2.hpp"
#include "inc/HSSIPrefix/HSSIPrefix.hpp"
#pragma warning(disable:4091)
#include <ntddscsi.h>
#pragma warning(default:4091)
#pragma comment(lib,"winmm.lib")
void ShowCDROMToc ( HANDLE hDevice );
void ShowCDROMFullToc ( HANDLE hDevice );
void ShowTestResult ( HANDLE hDevice );
void SetSpeed ( HANDLE hDevice , int speed = 1 );
void SetRealSpeed ( HANDLE hDevice , uint32_t speed );
void SetRealSpeedEx ( HANDLE hDevice , uint32_t StartLBA , uint32_t EndLBA  , uint32_t speed);

void ShowDiscInfo ( THSCD_DiscInfo Info );
void  Test ( CHSCDDevice *pcdd );
void  RippingTest ( CHSCDDevice *pcdd );

void SpinUpDrive ( HANDLE hDevice );
void SpinDownDrive ( HANDLE hDevice );

bool SetSeek ( HANDLE hDevice , uint32_t LBA );

const UINT DefaultRippingTrack = 0;


void ShowCDText( HANDLE hDevice );

std::string Console_ReadLine( );
void ProcessMain( void );

THSEnumrateOpticalDriveInfo enumOpticalDrive;
uint32_t selectedOpticalDriveNumber = 0;

int main ( void ) { 
	ProcessMain( );
	system ("pause");
	return 0;
} 


void ProcessMain( void ) {


	if ( !CHSCDDevice::EnumOpticalDrive( &enumOpticalDrive ) ) return;
	
	if ( enumOpticalDrive.uOpticalDriveCount == 0 ) {
		printf( "�L���Ȍ��w�h���C�u���ڑ�����Ă��܂���B\n" );
		return;
	}

	std::string separator( 80, '-' );

	printf( "[���w�h���C�u���X�g]\n\n" );
	printf( "�ԍ��F[�h���C�u����] �f�o�C�X��\n" );
	printf( "%s\n", separator.c_str( ) );
	for ( uint8_t id = 0; id < enumOpticalDrive.uOpticalDriveCount; id++ ) {

		printf( "%4u�F[%c:]", id , enumOpticalDrive.Drives[id].Letter );
		if ( enumOpticalDrive.Drives[id].bIncludedInfo ) {
			printf( " %s", enumOpticalDrive.Drives[id].Info.DeviceName );
		}
		printf( "\n" );
	}
	printf( "%s\n", separator.c_str( ) );
	printf( "\n" );


	if ( enumOpticalDrive.uOpticalDriveCount == 1 ) {
		printf( "�ڑ�����Ă�����w�h���C�u��1�ł����̂ŁA�Y���̃h���C�u�������őI������܂����B\n" );
		selectedOpticalDriveNumber = 0;
	} else {

		printf( "��̃��X�g����g�p������w�h���C�u��ԍ��Ŏw�肵�Ă��������F" );

		while ( true ) {
			(void) scanf_s( "%u", &selectedOpticalDriveNumber );
			(void) Console_ReadLine( );

			if ( selectedOpticalDriveNumber < enumOpticalDrive.uOpticalDriveCount ) {
				break;
			}

			printf( "�����Ȕԍ������͂���܂����B�w�����蒼���Ă��������F" );
		}

	}
	printf( "\n" );

	CHSCDDevice cdd;
	selectedOpticalDriveNumber = min( 25, selectedOpticalDriveNumber );
	if ( cdd.Open( enumOpticalDrive.Drives[selectedOpticalDriveNumber].Letter ) ) {

		printf( "�ySelected Device�z\n%u : [%c:] ", selectedOpticalDriveNumber,
			enumOpticalDrive.Drives[selectedOpticalDriveNumber].Letter );
		if ( enumOpticalDrive.Drives[selectedOpticalDriveNumber].bIncludedInfo ) {
			printf( " %s", enumOpticalDrive.Drives[selectedOpticalDriveNumber].Info.DeviceName );
		}
		printf( "\n\n" );

		printf( "�A�N�Z�X�\���F" );
		if ( cdd.IsPossibleAccess( ) ) {
			printf( "�͂�\n\n" );


			CDROM_STREAMING_CONTROL csc;

			csc.RequestType = CdromStreamingEnableForReadOnly;
			BOOL bret = DeviceIoControl( cdd.GetHandle( ), IOCTL_CDROM_ENABLE_STREAMING, &csc, sizeof( csc ),
				nullptr, 0, 0, 0 );

			printf( "IOCTL_CDROM_ENABLE_STREAMING result : %d\n", bret );

			//Test ( &cdd );
			RippingTest( &cdd );
		} else {
			printf( "������\n\n�w�肳�ꂽ���w�h���C�u�Ƀf�B�X�N�������Ă��Ȃ��ȂǁA�A�N�Z�X���邽�߂̏����������Ă��܂���B\n" );
		}
		cdd.Close( );
	}

}


void Test ( CHSCDDevice * pcdd )
{	
	if ( pcdd == nullptr )  return;

	HANDLE hDevice = pcdd->GetHandle ( );

	THSCD_DiscInfo di;

	if ( pcdd->GetDiscInfo ( &di ) ) {
		ShowDiscInfo ( di );
	}

	printf ( "\n" );
	ShowTestResult ( hDevice );
}


void ShowDiscInfo( THSCD_DiscInfo Info ) {

	std::string separator( 100, '-' );

	printf( "\n�yTrack List�z\n" );

	printf( "%s\n", separator.c_str( ) );

	printf( "[Track] : %10s �` %-10s (%8s)  ", "�J�n�ʒu", "�I���ʒu", "����" );
	printf( "[%02s : %02s : %02s]\t�o�C�g�P�ʂ̃T�C�Y\n", "��", "�b", "�t���[��" );

	printf( "%s\n", separator.c_str( ) );

	HSPrefixDoubleNumber dnum;

	for ( UINT8 t = Info.StartTrackNumber; t <= Info.EndTrackNumber; t++ ) {


		printf( "[%5u] : %10u �` %-10u (%8u)  ", t, Info.TrackInfo[t - 1].StartSectorPosition,
			Info.TrackInfo[t - 1].EndSectorPosition, Info.TrackInfo[t - 1].LengthSectors );
		printf( "[%02uM : %02uS : %02uF]\t", HSGetMSFMinuteFromSector( Info.TrackInfo[t - 1].LengthSectors ),
			HSGetMSFSecondFromSector( Info.TrackInfo[t - 1].LengthSectors ),
			HSGetMSFFrameFromSector( Info.TrackInfo[t - 1].LengthSectors )
		);

		dnum = HSGetPrefixDoubleNumber( Info.TrackInfo[t - 1].LengthSectors * VHSCD_BYTES_PER_SECTOR );

		if ( dnum.Prefix == HSSIPrefix::None ) {
			printf( "%.f Bytes", dnum.Number );
		} else {
			printf( "%.2f %cB", dnum.Number, HSGetPrefixCharA( dnum ) );
		}

		printf( "\n" );
	}

	printf( "%s\n", separator.c_str( ) );

	UINT32 beginsector = Info.TrackInfo[Info.StartTrackNumber - 1].StartSectorPosition;
	UINT32 endsector = Info.TrackInfo[Info.EndTrackNumber - 1].EndSectorPosition;
	UINT32 lengthsector = endsector - beginsector + 1;

	printf( "[Total] : %10u �` %-10u (%8u)  ", beginsector, endsector, lengthsector );
	printf( "[%02uM : %02uS : %02uF]\t", HSGetMSFMinuteFromSector( lengthsector ),
		HSGetMSFSecondFromSector( lengthsector ),
		HSGetMSFFrameFromSector( lengthsector )
	);

	dnum = HSGetPrefixDoubleNumber( lengthsector * VHSCD_BYTES_PER_SECTOR );

	if ( dnum.Prefix == HSSIPrefix::None ) {
		printf( "%.f Bytes", dnum.Number );
	} else {
		printf( "%.2f %cB", dnum.Number, HSGetPrefixCharA( dnum ) );
	}
	printf( "\n%s\n", separator.c_str( ) );

}

std::string Console_ReadLine( ) {

	std::string input;

	std::getline( std::cin, input );

	return input;
}



void ShowCDText( HANDLE hDevice ) {

	printf( "[IOCTL_CDROM_READ_TOC_EX(CD-TEXT)]\n" );

	CDROM_READ_TOC_EX tocex = { 0 };
	tocex.Format = CDROM_READ_TOC_EX_FORMAT_CDTEXT;

	CDROM_TOC_CD_TEXT_DATA tocCDText;


	DWORD returnSize = 0;

	BOOL bRet = DeviceIoControl( hDevice, IOCTL_CDROM_READ_TOC_EX, &tocex, sizeof( CDROM_READ_TOC_EX ), &tocCDText, sizeof( CDROM_TOC_CD_TEXT_DATA ), &returnSize, NULL );
	if ( bRet ) {

		UINT16  StructSize = 2 + ( ( tocCDText.Length[0] << 8 ) | tocCDText.Length[1] );

		UINT16 InnerStructSize = StructSize - 4;
		UINT16 InnerStructElements = ( InnerStructSize ) / sizeof( CDROM_TOC_CD_TEXT_DATA_BLOCK );

		UCHAR* lpData = new UCHAR[StructSize];
		returnSize = 0;
		bRet = DeviceIoControl( hDevice, IOCTL_CDROM_READ_TOC_EX, &tocex, sizeof( CDROM_READ_TOC_EX ), lpData, StructSize, &returnSize, NULL );

		if ( bRet ) {

			CDROM_TOC_CD_TEXT_DATA* pTocData = reinterpret_cast<CDROM_TOC_CD_TEXT_DATA*>( lpData );

			printf( "�f�[�^���F%u\n", InnerStructElements );
			std::string str;
			uint32_t OldPosition = 0;
			uint32_t TrackID = 100;
			bool bQuitFlag;
			for ( UINT32 i = 0; i < InnerStructElements; i++ ) {
				CDROM_TOC_CD_TEXT_DATA_BLOCK* pBlock = &pTocData->Descriptors[i];

				printf( "%02X\t%02X\n", pBlock->PackType , pBlock->TrackNumber );

			}

		}
		delete[]lpData;
	}
}


void  RippingTest ( CHSCDDevice *pcdd ) {
	if ( pcdd == nullptr )  return;

	THSCD_DiscInfo di;
	memset( &di, 0, sizeof( di ) );

	if ( pcdd->GetDiscInfo ( &di ) ) {

#ifdef _DEBUG
		ShowCDText( pcdd->GetHandle( ) );
#endif

		ShowDiscInfo ( di );
		printf ("\n");

		hirosof::Hash::CSHA1 hash;
		hirosof::Hash::CSHA1Value toc_hash_value;
		hash.Put<UINT8>( di.NumberOfTracks );
		for ( size_t i = 0; i < di.NumberOfTracks; i++ ) {
			hash.Put<UINT32>( di.TrackInfo[i].LengthSectors );
		}
		hash.Finalize( );

		hash.GetHash( &toc_hash_value );
		wprintf( L"TOC information based hash value : %s\n\n", toc_hash_value.ToWString( ).c_str( ) );

		UINT RippingTrack = DefaultRippingTrack;

		printf ("���b�s���O����g���b�N�ԍ�����͂��Ă��������F");
		scanf_s ("%u", &RippingTrack);
		(void) Console_ReadLine( );

		if ((RippingTrack < di.StartTrackNumber)|| (di.EndTrackNumber < RippingTrack)) {
			printf ("\n�s���ȃg���b�N�ԍ����w�肳��܂����B\n");
			return;
		}

		CHSWaveWriterW wave;
		SYSTEMTIME stf;
		GetLocalTime ( &stf );

		wchar_t foldername [ ] = L"waveout";
		CreateDirectoryW ( foldername , nullptr );
		SetCurrentDirectoryW( foldername );

		std::wstring wshash = toc_hash_value.ToWString( );
		CreateDirectoryW ( wshash.c_str(), nullptr );
		SetCurrentDirectoryW( wshash.c_str( ) );

		wchar_t output_file_name [ 260 ];
		swprintf_s ( output_file_name , L"%04d%02d%02d_%02d%02d%02d_Track-%02u.wav" , 
			stf.wYear , stf.wMonth , stf.wDay ,
			stf.wHour , stf.wMinute , stf.wSecond,
			RippingTrack);

		if ( wave.Create( output_file_name ) ) {

			char trackStr[3];

			std::string isrc_value = "CDDA";

			isrc_value.append( " (" );
			isrc_value.append( toc_hash_value.ToString( ) );
			isrc_value.append( ")" );

			sprintf_s( trackStr, "%u", RippingTrack );

			wave.BeginListChunk( "INFO" );


			printf( "�o�̓t�@�C���ɏ������ދȏ�����͂��Ă��������B(�K�v���Ȃ����͉������͂����ɂ��̂܂�ENTER�L�[�������Ă�������)" );
			printf( "\n\t�Ȗ�����͂��Ă��������F" );
			std::string title = Console_ReadLine( );
			if ( title.empty( ) == false ) wave.WriteListMemberChunkString( HSRIFF_FOURCC_LIST_INFO_INAM, title.c_str( ), true );

			printf( "\t�A�[�e�B�X�g������͂��Ă��������F" );
			std::string artist = Console_ReadLine( );;
			if ( artist.empty( ) == false ) wave.WriteListMemberChunkString( HSRIFF_FOURCC_LIST_INFO_IART, artist.c_str( ), true );

			printf( "\t�A���o��������͂��Ă��������F" );
			std::string album = Console_ReadLine( );
			if ( album.empty( ) == false ) wave.WriteListMemberChunkString( HSRIFF_FOURCC_LIST_INFO_IPRD, album.c_str( ), true );
			printf( "\n" );

			wave.WriteListMemberChunkString( HSRIFF_FOURCC_LIST_INFO_ISFT, "HSCDDeviceConsole", true );
			wave.WriteListMemberChunkString( HSRIFF_FOURCC_LIST_INFO_ISRC, isrc_value.c_str( ), true );

			if ( enumOpticalDrive.Drives[selectedOpticalDriveNumber].bIncludedInfo ) {
				wave.BeginListMemberChunk( HSRIFF_FOURCC_LIST_INFO_ICMT );
				wave.AdditionalListMemberChunkStringData( "[Optical Drive Device Name] ", false );
				wave.AdditionalListMemberChunkStringData( enumOpticalDrive.Drives[selectedOpticalDriveNumber].Info.DeviceName, true );
				wave.EndListMemberChunk( );
			}

			wave.WriteListMemberChunkString( HSRIFF_FOURCC_LIST_INFO_ITRK, trackStr, true );
			wave.EndListChunk( );



			PCMWAVEFORMAT pcm;
			pcm.wBitsPerSample = 16;
			pcm.wf.wFormatTag = WAVE_FORMAT_PCM;
			pcm.wf.nSamplesPerSec = 44100;
			pcm.wf.nChannels = 2;
			pcm.wf.nBlockAlign = pcm.wBitsPerSample / 8 * pcm.wf.nChannels;
			pcm.wf.nAvgBytesPerSec = pcm.wf.nBlockAlign * pcm.wf.nSamplesPerSec;

			wave.WriteFormatChunkType( pcm );


			UINT32  SectorSeconds = HSGetSectorByMSFTime( 0, 1, 0 );
			UINT32 BytesSeconds = HSGetBytesSizeBySector( SectorSeconds );
			char* pData = new char[BytesSeconds];
			wave.BeginDataChunk( );

			UINT Track = RippingTrack;
			UINT Count = di.TrackInfo[Track - 1].LengthSectors / SectorSeconds;

			if ( di.TrackInfo[Track - 1].LengthSectors % SectorSeconds ) Count++;


			DWORD dwStart, dwEnd;
			DWORD dwSum = 0;


			SYSTEMTIME st, st2;
			GetLocalTime( &st );

			SetRealSpeedEx( pcdd->GetHandle(), di.TrackInfo[Track - 1].StartSectorPosition, di.TrackInfo[Track - 1].EndSectorPosition, 0xFFFF );

			printf( "�X�s���A�b�v���E�E�E" );
			SpinUpDrive( pcdd->GetHandle( ) );
			printf( "OK\n" );


			printf( "\n Target Track Number : %u\n\n", Track );

			for ( UINT32 i = 0; i < Count; i++ ) {
				dwStart = timeGetTime( );
				UINT32 Ret = pcdd->Read16( pData, BytesSeconds, Track, i * SectorSeconds, SectorSeconds );
				dwEnd = timeGetTime( );
				if ( Ret > 0 ) {
					wave.AdditionalDataChunkContent( pData, HSGetBytesSizeBySector( Ret ) );
					printf( "%u / %u : OK(%02dM:%02dS:%02dF)   ReadTime(ms) : %u\n", i + 1, Count,
						HSGetMSFMinuteFromSector( Ret ), HSGetMSFSecondFromSector( Ret ), HSGetMSFFrameFromSector( Ret ),

						dwEnd - dwStart );
					dwSum += ( dwEnd - dwStart );
				}
			}

			GetLocalTime( &st2 );

			printf( "�X�s���_�E�����E�E�E" );
			SpinDownDrive( pcdd->GetHandle( ) );
			printf( "OK\n" );

			wave.EndDataChunk( );
			wave.Close( );
			delete[]pData;

			printf( "�ǂݎ��ɗv�������ԁF%u ms\n", dwSum );
			printf( "Start Time : %02d:%02d:%02d.%03d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
			printf( "End   Time : %02d:%02d:%02d.%03d\n", st2.wHour, st2.wMinute, st2.wSecond, st2.wMilliseconds );

			wprintf( L"\nTOC information based hash value : %s\n", toc_hash_value.ToWString( ).c_str( ) );
			wprintf( L"Output Filename : %s\n", output_file_name );
			wprintf( L"\n" );

		}
	}
	printf ( "\n");
}

void SetSpeed ( HANDLE hDevice , int speed ) {

	printf ( "[IOCTL_CDROM_SET_SPEED]\n" );

	CDROM_SET_SPEED SpeedInfo;

	SpeedInfo.RequestType = CDROM_SPEED_REQUEST::CdromSetSpeed;

	SpeedInfo.ReadSpeed = speed*150;

	SpeedInfo.WriteSpeed = speed * 150;

	SpeedInfo.RotationControl = CdromDefaultRotation;

	DWORD retSize;
	BOOL bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_SET_SPEED , &SpeedInfo , sizeof ( CDROM_SET_SPEED ) , 0 , 0 , &retSize , 0 );
	if ( bRet ) {
		printf ( "Set Speed : OK\n" );
	} else {
		printf ( "Set Speed : NG(%u)\n"  , GetLastError());

	}

}

void SetRealSpeed ( HANDLE hDevice , uint32_t speed )
{
	printf ( "[IOCTL_CDROM_SET_SPEED(CdromSetSpeed)]\n" );

	CDROM_SET_SPEED SpeedInfo;
	memset ( &SpeedInfo , 0 , sizeof ( CDROM_SET_SPEED ) );

	SpeedInfo.RequestType = CDROM_SPEED_REQUEST::CdromSetSpeed;

	SpeedInfo.ReadSpeed = speed;

	SpeedInfo.WriteSpeed = speed;

	SpeedInfo.RotationControl = CdromDefaultRotation;

	DWORD retSize;
	BOOL bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_SET_SPEED , &SpeedInfo , sizeof ( CDROM_SET_SPEED ) , 0 , 0 , &retSize , 0 );
	if ( bRet ) {
		printf ( "OK\n" );
	}
}

void SetRealSpeedEx ( HANDLE hDevice , uint32_t StartLBA , uint32_t EndLBA , uint32_t speed )
{
	printf ( "[IOCTL_CDROM_SET_SPEED(CdromSetStreaming)]\n" );

	CDROM_SET_STREAMING  SpeedInfo;
	memset ( &SpeedInfo , 0 , sizeof ( CDROM_SET_STREAMING ) );
	SpeedInfo.RequestType = CDROM_SPEED_REQUEST::CdromSetStreaming;

	//SpeedInfo.StartLba = StartLBA;
	//SpeedInfo.EndLba = EndLBA;
	SpeedInfo.StartLba = 0;
	SpeedInfo.EndLba = 0;

	SpeedInfo.ReadSize = speed;
	SpeedInfo.ReadTime = 50;
	SpeedInfo.WriteSize = speed;
	SpeedInfo.WriteTime = 50;

	SpeedInfo.RotationControl = CdromDefaultRotation;

	SpeedInfo.RestoreDefaults = FALSE;
	SpeedInfo.SetExact = FALSE;
	SpeedInfo.RandomAccess = FALSE;
	SpeedInfo.Persistent = TRUE;
	DWORD retSize;
	BOOL bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_SET_SPEED , &SpeedInfo , sizeof ( CDROM_SET_STREAMING ) , 0 , 0 , &retSize , 0 );
	if ( bRet ) {
		printf ( "OK\n" );
	}

}


void ShowCDROMToc ( HANDLE hDevice ) {
	printf ( "[IOCTL_CDROM_READ_TOC]\n" );
	CDROM_TOC toc = { 0 };
	DWORD returnSize;

	BOOL bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_READ_TOC , nullptr , NULL , &toc , sizeof ( CDROM_TOC ) , &returnSize , NULL );
	
	if ( bRet ) {

		int len = ( ( toc.Length [ 0 ] << 8 ) | toc.Length [ 1 ] ) - sizeof ( UCHAR ) * 4;
		len /= sizeof ( TRACK_DATA );

		printf ( "First Track : %d\n" , toc.FirstTrack );
		printf ( "Last Track : %d\n" , toc.LastTrack );
		printf ( "NumberOfTrackData : %d\n" , len );

		for ( int i = 0; i < len; i++ ) {
			TRACK_DATA *ptd = &toc.TrackData [ i ];
			printf ( "Track %3d : [Control : %d][Adr : %d][Address :  %02X %02X %02X %02X]\n" , ptd->TrackNumber , ptd->Control , ptd->Adr ,
				ptd->Address [ 0 ] , ptd->Address [ 1 ] , ptd->Address [ 2 ] , ptd->Address [ 3 ] );
		}

		printf ( "\n" );
	} else {
		printf ( "���̎擾�Ɏ��s���܂���\n\n" );
	}

}


void ShowCDROMFullToc ( HANDLE hDevice ) {
	printf ( "[IOCTL_CDROM_READ_TOC_EX(CDROM_READ_TOC_EX_FORMAT_FULL_TOC)]\n" );
	CDROM_READ_TOC_EX tocex = { 0 };
	tocex.Msf = 1;
	tocex.Format = CDROM_READ_TOC_EX_FORMAT_FULL_TOC;
	CDROM_TOC_FULL_TOC_DATA full_toc = { 0 };
	DWORD returnSize;

	BOOL bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_READ_TOC_EX , &tocex,sizeof(CDROM_READ_TOC_EX) , &full_toc , sizeof ( CDROM_TOC_FULL_TOC_DATA ) , &returnSize , NULL );
	if ( bRet ) {
		UINT16 StructSize = 2 + ( ( full_toc.Length [ 0 ] << 8 ) + full_toc.Length [ 1 ] );
		UINT16 InnerStructSize = StructSize - 4;
		UINT16 InnerStructElements = ( InnerStructSize ) / sizeof ( CDROM_TOC_FULL_TOC_DATA_BLOCK );		

		DWORD OutBufferSize = StructSize;
		if ( OutBufferSize % 2 )OutBufferSize++;
		UCHAR *lpData = new UCHAR [ OutBufferSize ];
		CDROM_TOC_FULL_TOC_DATA *lpToc = reinterpret_cast< CDROM_TOC_FULL_TOC_DATA* >( lpData );

		bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_READ_TOC_EX , &tocex , sizeof ( CDROM_READ_TOC_EX ) , lpToc , OutBufferSize , &returnSize , NULL );

		if ( bRet ) {

			CDROM_TOC_FULL_TOC_DATA_BLOCK *pBlock;


			for ( size_t i = 0; i < InnerStructElements; i++ ) {
				pBlock = &lpToc->Descriptors [ i ];

				switch ( pBlock->Point ) {
					case 0xA0:
						printf ( "[�J�n�g���b�N�ԍ�(�Z�b�V�����ԍ�)]\n%d (%d)\n" , pBlock->Msf [ 0 ] , pBlock->SessionNumber );
						printf ( "Disc Type�F%d\n" , pBlock->Msf [ 1 ] );
						printf ( "ATIME : %02X:%02X:%02X\n" , pBlock->MsfExtra [ 0 ] , pBlock->MsfExtra [ 1 ] , pBlock->MsfExtra [ 2 ] );
						printf ( "\n" );

						break;
					case 0xA1:
						printf ( "[�I���g���b�N�ԍ�(�Z�b�V�����ԍ�)]\n%d (%d)\n" , pBlock->Msf [ 0 ] , pBlock->SessionNumber );
						printf ( "ATIME : %02X:%02X:%02X\n" , pBlock->MsfExtra [ 0 ] , pBlock->MsfExtra [ 1 ] , pBlock->MsfExtra [ 2 ] );
						printf ( "\n" );
						break;
					case 0xA2:
						printf ( "[�Z�b�V�����I�[�ʒu]\nMSF���ԁF%02d:%02d:%02d (%u �Z�N�^)\n�ΏۃZ�b�V�����ԍ��F%d\n"  , pBlock->Msf [ 0 ] , pBlock->Msf [ 1 ] , pBlock->Msf [ 2 ] , ( pBlock->Msf [ 0 ] * 60 + pBlock->Msf [ 1 ] ) * 75 + pBlock->Msf [ 2 ] ,  pBlock->SessionNumber );
						printf ( "ATIME : %02X:%02X:%02X\n" , pBlock->MsfExtra [ 0 ] , pBlock->MsfExtra [ 1 ] , pBlock->MsfExtra [ 2 ] );
						printf ( "\n" );
						break;
					default:
						{
							
							UCHAR t = pBlock->Point;
							if ( ( t >= 1 ) && ( t <= 99 ) ) {

								printf ( "[Track %u]\n�J�n�ʒu�F%02d:%02d:%02d (%u �Z�N�^) \n" ,  t , pBlock->Msf [ 0 ] , pBlock->Msf [ 1 ] , pBlock->Msf [ 2 ] , 
									( pBlock->Msf [ 0 ] * 60 + pBlock->Msf [ 1 ]) * 75 + pBlock->Msf [2 ] );

								printf ( "ATIME : %02X:%02X:%02X\n" , pBlock->MsfExtra [ 0 ] , pBlock->MsfExtra [ 1 ] , pBlock->MsfExtra [ 2 ] );


								printf ( "\n" ); 
							}
						}
						break;
				}
			}



		} else {
			DWORD erno = GetLastError ( );
			printf ( "���̎擾�Ɏ��s���܂���(ErCode : %u)\n\n"  , erno);

		}
		delete [ ]lpData;

	} else {
		printf ( "���̎擾�Ɏ��s���܂���\n\n" );
	}

}


void ShowTestResult ( HANDLE hDevice ) {


	INQUIRYDATA data;
	DWORD returnSize;

	BOOL bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_GET_INQUIRY_DATA ,0,0 , &data , sizeof ( INQUIRYDATA ) , &returnSize , NULL );
	if ( bRet ) {
		printf ( "[IOCTL_CDROM_GET_INQUIRY_DATA]\n" );
		printf ( "Vendor ID : " );
		for ( int i = 0; i < 8; i++ ) {
			if ( data.VendorId [ i ] == '\0' )break;
			printf ( "%c" , data.VendorId [ i ] );
		}
		printf ( "\n" );

		printf ( "Product ID : " );
		for ( int i = 0; i < 16; i++ ) {
			if ( data.ProductId [ i ] == '\0' )break;
			printf ( "%c" , data.ProductId [ i ] );
		}
		printf ( "\n" );

		printf ( "Product Revision Level : " );
		for ( int i = 0; i < 4; i++ ) {
			if ( data.ProductRevisionLevel [ i ] == '\0' )break;
			printf ( "%c" , data.ProductRevisionLevel [ i ] );
		}
		printf ( "\n\n" );
	}

	GET_CONFIGURATION_IOCTL_INPUT  ConfInput;
	GET_CONFIGURATION_HEADER ConfHeader;


	ConfInput.Feature = FeatureProfileList;
	ConfInput.RequestType = SCSI_GET_CONFIGURATION_REQUEST_TYPE_CURRENT;
	ConfInput.Reserved [ 0 ] = 0;
	ConfInput.Reserved [ 1 ] = 0;


	bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_GET_CONFIGURATION , &ConfInput , sizeof ( GET_CONFIGURATION_IOCTL_INPUT ) , &ConfHeader , sizeof ( GET_CONFIGURATION_HEADER ) , &returnSize , 0 );
	if ( bRet ) {
		printf ( "[IOCTL_CDROM_GET_CONFIGURATION]\n" );
		UINT16 CP =( ConfHeader.CurrentProfile [ 0 ] << 8) | ConfHeader.CurrentProfile [ 1 ];
		printf ( "%X\n\n" , CP );
	}

	CDROM_PERFORMANCE_REQUEST  PerfReq;
	CDROM_PERFORMANCE_HEADER PerfHeader;
	PerfReq.RequestType = CdromPerformanceRequest;
	PerfReq.PerformanceType = CdromReadPerformance;
	PerfReq.Exceptions = CdromNominalPerformance;
	PerfReq.Tolerance = Cdrom10Nominal20Exceptions;
	PerfReq.StaringLba = 0;
	bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_GET_PERFORMANCE , &PerfReq , sizeof ( CDROM_PERFORMANCE_REQUEST ) , &PerfHeader , sizeof ( CDROM_PERFORMANCE_HEADER ) , &returnSize , 0 );
	if ( bRet ) {
		UINT32  DataLength = 0;
		for ( int i = 0; i < 4; i++ ) DataLength = ( DataLength << 8 ) | PerfHeader.DataLength [ i ];
		DataLength += sizeof ( UCHAR ) * 4;

		UINT32 DataSize = DataLength;
		if ( DataLength % 2 ) DataSize++;

		
		char *lpData = new char [ DataSize ];

		bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_GET_PERFORMANCE , &PerfReq , sizeof ( CDROM_PERFORMANCE_REQUEST ) , lpData , DataSize , &returnSize , 0 );

		if ( bRet ) {
			printf ( "[IOCTL_CDROM_GET_PERFORMANCE(CdromPerformanceRequest) ]\n" );

			CDROM_PERFORMANCE_HEADER *pFullHeader = reinterpret_cast< CDROM_PERFORMANCE_HEADER* >( lpData );

			if ( pFullHeader->Write == 0 ) {
				
				if ( pFullHeader->Except == 0 ) {
					CDROM_NOMINAL_PERFORMANCE_DESCRIPTOR *pdesc = reinterpret_cast< CDROM_NOMINAL_PERFORMANCE_DESCRIPTOR* >( pFullHeader->Data );
					printf ( "Type : CDROM_NOMINAL_PERFORMANCE_DESCRIPTOR\n" );

					UINT32 StartLBA = 0;
					UINT32 StartPerf = 0;
					UINT32 EndLBA = 0;
					UINT32 EndPerf = 0;

					for ( int i = 0; i < 4; i++ ) {
						StartLBA <<= 8;
						StartPerf <<= 8;
						EndLBA <<= 8;
						EndPerf <<= 8;

						StartLBA |= pdesc->StartLba [ i ];
						StartPerf |= pdesc->StartPerformance [ i ];
						EndLBA |= pdesc->EndLba [ i ];
						EndPerf |= pdesc->EndPerformance [ i ];
					}

					printf ( "LBA Range =  %u to %u\n" , StartLBA , EndLBA );
					printf ( "Perf Range =  %u to %u\n\n" , StartPerf  , EndPerf );


				} else {
					CDROM_EXCEPTION_PERFORMANCE_DESCRIPTOR  *pdesc = reinterpret_cast< CDROM_EXCEPTION_PERFORMANCE_DESCRIPTOR * >( pFullHeader->Data );
					printf ( "Type : CDROM_EXCEPTION_PERFORMANCE_DESCRIPTOR\n" );

				}


			}



		}

		delete [ ]lpData;
	}


	CDROM_WRITE_SPEED_REQUEST  wreq;
	wreq.RequestType = CdromWriteSpeedRequest;
	bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_GET_PERFORMANCE , &wreq , sizeof ( CDROM_WRITE_SPEED_REQUEST ) , &PerfHeader , sizeof ( CDROM_PERFORMANCE_HEADER ) , &returnSize , 0 );
	if ( bRet ) {


		UINT32  DataLength = 0;
		for ( int i = 0; i < 4; i++ ) DataLength = ( DataLength << 8 ) | PerfHeader.DataLength [ i ];
		DataLength += sizeof ( UCHAR ) * 4;

		UINT32 DataSize = DataLength;
		if ( DataLength % 2 ) DataSize++;

		char *lpData = new char [ DataSize ];

		bRet = DeviceIoControl ( hDevice , IOCTL_CDROM_GET_PERFORMANCE , &wreq , sizeof ( CDROM_WRITE_SPEED_REQUEST ) , lpData , DataSize , &returnSize , 0 );

		if ( bRet ) {
			printf ( "[IOCTL_CDROM_GET_PERFORMANCE(CdromWriteSpeedRequest) ]\n" );

			CDROM_PERFORMANCE_HEADER *pFullHeader = reinterpret_cast< CDROM_PERFORMANCE_HEADER* >( lpData );

			if ( pFullHeader->Write == 0 ) {

				if ( pFullHeader->Except == 0 ) {
					CDROM_WRITE_SPEED_DESCRIPTOR  *pdesc = reinterpret_cast< CDROM_WRITE_SPEED_DESCRIPTOR * >( pFullHeader->Data );
					printf ( "Type : CDROM_WRITE_SPEED_DESCRIPTOR \n" );
					UINT32 ReadSpeed = 0;
					UINT32 WriteSpeed = 0;
					for ( int i = 0; i < 4; i++ ) {
						ReadSpeed <<= 8;
						WriteSpeed <<= 8;
						ReadSpeed |= pdesc->ReadSpeed [ i ];
						WriteSpeed |= pdesc->WriteSpeed [ i ];
					}

					printf ( "Read Speed = %u (0x%X)\n" , ReadSpeed , ReadSpeed );
					printf ( "Write Speed = %u (0x%X)\n" , WriteSpeed , WriteSpeed );


				} else {
					CDROM_EXCEPTION_PERFORMANCE_DESCRIPTOR  *pdesc = reinterpret_cast< CDROM_EXCEPTION_PERFORMANCE_DESCRIPTOR * >( pFullHeader->Data );
					printf ( "Type : CDROM_EXCEPTION_PERFORMANCE_DESCRIPTOR\n" );

				}

			}
		}

		delete [ ]lpData;
	}


}

void SpinUpDrive ( HANDLE hDevice ) {
	if ( hDevice ) {
		SCSI_PASS_THROUGH_DIRECT direct;
		memset ( &direct , 0 , sizeof ( SCSI_PASS_THROUGH_DIRECT ) );

		direct.Length = ( USHORT )sizeof ( SCSI_PASS_THROUGH_DIRECT );
		direct.TimeOutValue = 5;

		direct.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;
		direct.DataTransferLength = 0;
		direct.DataBuffer = nullptr;

		direct.CdbLength = 6;
		direct.Cdb [ 0 ] = 0x1B; /* START STOP UNIT Command */
		direct.Cdb [ 4 ] = 1; /*Start Disc*/

		DWORD retsize;
		BOOL bRet = DeviceIoControl ( hDevice , IOCTL_SCSI_PASS_THROUGH_DIRECT ,
			&direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , nullptr , 0 , &retsize , 0 );
	}
}

void SpinDownDrive ( HANDLE hDevice ) {
	if ( hDevice ) {
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
		BOOL bRet = DeviceIoControl ( hDevice , IOCTL_SCSI_PASS_THROUGH_DIRECT ,
			&direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , nullptr , 0 , &retsize , 0 );
	}
}

bool SetSeek ( HANDLE hDevice ,uint32_t LBA )
{
	BOOL bRet = FALSE;
	if ( hDevice ) {
		SCSI_PASS_THROUGH_DIRECT direct;
		memset ( &direct , 0 , sizeof ( SCSI_PASS_THROUGH_DIRECT ) );

		direct.Length = ( USHORT )sizeof ( SCSI_PASS_THROUGH_DIRECT );
		direct.TimeOutValue = 5;

		direct.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;
		direct.DataTransferLength = 0;
		direct.DataBuffer = nullptr;

		direct.CdbLength = 10;
		direct.Cdb [ 0 ] = 0x2b; /* SEEK(10) Command */

		uint8_t *pAddress = reinterpret_cast< uint8_t* >( &LBA );

		for ( int i = 0; i < 4; i++ ) {
			direct.Cdb [ 2 + i ] = *( pAddress + ( 3 - i ) );
		}

		DWORD retsize;
		bRet = DeviceIoControl ( hDevice , IOCTL_SCSI_PASS_THROUGH_DIRECT ,
			&direct , sizeof ( SCSI_PASS_THROUGH_DIRECT ) , nullptr , 0 , &retsize , 0 );
	}

	return ( bRet == TRUE );
}

