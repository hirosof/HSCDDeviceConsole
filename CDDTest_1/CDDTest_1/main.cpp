

#include <stdio.h>
#include "CHSCompactDiscReader.hpp"

int main ( void ) {

	THSEnumrateOpticalDriveInfo info;
	if ( CHSOpticalDeviceControl::EnumOpticalDrive ( &info ) ) {
		printf ( "<<光学ドライブ一覧>>\n" );
		for ( int i = 0; i < info.uOpticalDriveCount; i++ ) {
			printf ( "[%c:\\] " , info.Drives[i].Letter);
			if (info.Drives[i].bIncludedInfo ) {
				printf ( "%s" , info.Drives[i].Info.DeviceName );
			}
			printf ( "\n" );
		}


		printf ( "\n<<Test>>\n" );
		CHSCompactDiscReader  odc;
		//CHSOpticalDeviceControl odc;
		const uint8_t SpeedTable [10 ] = { 1,2,4,8,10,12,16,24,32,48 };

		if ( odc.Open ( info.Drives [0].Letter ) ) {
				THSOpticalDiscMediaTypeInfo MediaInfo;

				EHSOpticalDiscStatus s;
				odc.GetDiscStatus ( &s );
				
				if ( odc.GetCurrentMediaTypeInfo ( &MediaInfo ) ) {
					printf ( "Disc Type : %s\n" , MediaInfo.szTypeString );
				}

				DISK_GEOMETRY  dg;

				if ( odc.GetDiskGeometry ( &dg ) ) {
					printf ( "Cylinders : %I64u\n" , dg.Cylinders.QuadPart );
					printf ( "BytesPerSector : %u\n" , dg.BytesPerSector );
					printf ( "SectorsPerTrack : %u\n" , dg.SectorsPerTrack );
					printf ( "TracksPerCylinder : %u\n" , dg.TracksPerCylinder );
				}

				if ( odc.Tray_IsOpened ( ) ) {
					printf ( "トレイは開かれています。\n" );
				} else {
					printf ( "トレイは閉じられています。\n" );
				}

				odc.SpinUp( );


	
		}
	}
	return 0;
}