#include "smf.h"
#include "qx.h"

//===============================================================================================================
// SMC
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:
SMF::SMF(QByteArray rawData)
    : mFileDataF(rawData) {}

//-Class Functions------------------------------------------------------------------------------------------------
//Public:
bool SMF::fileIsValidSMF(QFile &possibleSMF, Qx::IO::IOOpReport& reportBuffer)
{
    // Get header region
    QByteArray headers;
    reportBuffer = Qx::IO::readBytesFromFile(headers, possibleSMF, 0x00, 0x1B);

    if(!reportBuffer.wasSuccessful())
        return false;
    else
    {
        long long fullFileSize = QFileInfo(possibleSMF).size();

        QByteArray smfSigRegion = headers.left(L_SMF_SIG);
        QByteArray smfByteSigRegion = headers.mid(L_SMF_SIG, L_SMF_BYTE_SIG);
        QByteArray riffSigRegion = headers.mid(L_SMF_SIG + L_SMF_BYTE_SIG, L_RIFF_SIG);
        QByteArray riffSizeRegion = headers.mid(L_SMF_SIG + L_SMF_BYTE_SIG + L_RIFF_SIG, L_RIFF_LEN);
        QByteArray wavSigRegion = headers.mid(L_SMF_SIG + L_SMF_BYTE_SIG + L_RIFF_SIG + L_RIFF_LEN, L_WAV_SIG);
        QByteArray formatSigRegion = headers.mid(L_SMF_SIG + L_SMF_BYTE_SIG + L_RIFF_SIG + L_RIFF_LEN + L_WAV_SIG, L_FORMAT_SIG);

        return smfSigRegion == SMF_SIG && smfByteSigRegion == QByteArray(SMF_BYTE_SIG, 8) && riffSigRegion == RIFF_SIG && wavSigRegion == WAV_SIG && formatSigRegion == FORMAT_SIG &&
               Qx::ByteArray::RAWToPrimitive<uint32_t>(riffSizeRegion, Qx::Endian::LE) == fullFileSize - 0x14;
    }
}

//-Instance Functions------------------------------------------------------------------------------------------------
//Public:
QByteArray SMF::getFullData() { return mFileDataF; }
WAV SMF::toWAV() { return WAV(mFileDataF.mid(L_SMF_SIG + L_SMF_BYTE_SIG)); }

//Private:
