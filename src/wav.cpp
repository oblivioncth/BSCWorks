#include "wav.h"
#include "qx.h"
#include "qx-io.h"

//===============================================================================================================
// WAV
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:
WAV::WAV(QByteArray rawWAVData) : mFileDataF(rawWAVData)
{
    if(!fileIsValidWAV())
        mFileDataF = QByteArray();
}

//-Instance Functions------------------------------------------------------------------------------------------------
//Private:
bool WAV::fileIsValidWAV()
{
    // Get header region
    QByteArray headers = mFileDataF.left(0x0F);

    long long fullFileSize = mFileDataF.size();

    QByteArray riffSigRegion = headers.left(L_RIFF_SIG);
    QByteArray riffSizeRegion = headers.mid(L_RIFF_SIG, L_RIFF_LEN);
    QByteArray wavSigRegion = headers.mid(L_RIFF_SIG + L_RIFF_LEN, L_WAV_SIG);

    return riffSigRegion == RIFF_SIG && wavSigRegion == WAV_SIG &&
           Qx::ByteArray::RAWToPrimitive<uint32_t>(riffSizeRegion, Qx::Endian::LE) == fullFileSize - 0x08;
}

//Public:
bool WAV::isValid() { return !mFileDataF.isNull(); }
QByteArray WAV::getFullData() { return mFileDataF; }

