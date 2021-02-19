#include "mp3.h"
#include "qx.h"
#include "qx-io.h"

//===============================================================================================================
// MP3
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:
MP3::MP3(QByteArray rawMP3Data) : mFileDataF(rawMP3Data)
{
    if(!fileIsValidMP3())
        mFileDataF = QByteArray();
}

//-Instance Functions------------------------------------------------------------------------------------------------
//Private:
bool MP3::fileIsValidMP3()
{
    // Get header region
    QByteArray header = mFileDataF.left(0x04);

    QByteArray mp3OldSigRegion = header.left(L_MP3_OLD_SIG);
    QByteArray mp3NewSigRegion = header;

    return mp3OldSigRegion == MP3_OLD_SIG_1 || mp3OldSigRegion == MP3_OLD_SIG_2 ||
           mp3OldSigRegion == MP3_OLD_SIG_3 || mp3NewSigRegion == MP3_NEW_SIG;
}

//Public:
bool MP3::isValid() { return !mFileDataF.isNull(); }
QByteArray MP3::getFullData() { return mFileDataF; }
