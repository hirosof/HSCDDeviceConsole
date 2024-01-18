#include "CHSCompactDiscReader.hpp"

bool CHSCompactDiscReader::IsCDTextSupport ( void )
{
	if ( this->hDevice ) {

		SCSI_PASS_THROUGH_DIRECT direct;
		memset ( &direct , 0 , sizeof ( SCSI_PASS_THROUGH_DIRECT ) );

		direct.Length = ( USHORT )sizeof ( SCSI_PASS_THROUGH_DIRECT );
		direct.TimeOutValue = 5;
		direct.ScsiStatus = 0;


	}
	return false;
}
