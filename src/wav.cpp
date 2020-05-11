#include "wav.h"
#include "qx.h"
#include "qx-io.h"

//===============================================================================================================
// WAV
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:
WAV::WAV(QByteArray rawData)
    : mFileDataF(rawData) {}

//-Class Functions------------------------------------------------------------------------------------------------
//Public:
bool WAV::fileIsValidWAV(QFile &possibleWAV, Qx::IO::IOOpReport& reportBuffer)
{
    // Get header region
    QByteArray headers;
    reportBuffer = Qx::IO::readBytesFromFile(headers, possibleWAV, 0x00, 0x0F);

    if(!reportBuffer.wasSuccessful())
        return false;
    else
    {
        long long fullFileSize = QFileInfo(possibleWAV).size();

        QByteArray riffSigRegion = headers.left(L_RIFF_SIG);
        QByteArray riffSizeRegion = headers.mid(L_RIFF_SIG, L_RIFF_LEN);
        QByteArray wavSigRegion = headers.mid(L_RIFF_SIG + L_RIFF_LEN, L_WAV_SIG);

        return riffSigRegion == RIFF_SIG && wavSigRegion == WAV_SIG &&
               Qx::ByteArray::RAWToPrimitive<uint32_t>(riffSizeRegion, Qx::Endian::LE) == fullFileSize - 0x08;
    }
}

//-Instance Functions------------------------------------------------------------------------------------------------
//Public:
SMF WAV::toSMF() { return SMF(Qx::ByteArray::RAWFromString(SMF::SMF_SIG) + QByteArray(SMF::SMF_CMN_FLAGS, 8) + mFileDataF); }
QByteArray WAV::getFullData() { return mFileDataF; }

//Private:
