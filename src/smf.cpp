#include "smf.h"
#include "wav.h"
#include "mp3.h"
#include "qx.h"

//===============================================================================================================
// SMC
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:
Smf::Smf(QByteArray rawData) : mFileDataF(rawData)
{
    if(!fileIsValidSMF())
        mFileDataF = QByteArray();
}

//-Class Functions---------------------------------------------------------------------------------------------------
Smf Smf::fromStandard(Wav wavData)
{
    QByteArray sig = Qx::ByteArray::RAWFromString(SMF_SIG);
    QByteArray maj = Qx::ByteArray::RAWFromPrimitive(MAJ_VER_STD);
    QByteArray min = Qx::ByteArray::RAWFromPrimitive(MIN_VER_STD_WAV);

    return Smf(sig + maj + min + wavData.getFullData());
}
Smf Smf::fromStandard(Mp3 mp3Data)
{
    QByteArray sig = Qx::ByteArray::RAWFromString(SMF_SIG);
    QByteArray maj = Qx::ByteArray::RAWFromPrimitive(MAJ_VER_STD);
    QByteArray min = Qx::ByteArray::RAWFromPrimitive(MIN_VER_STD_MP3);

    return Smf(sig + maj + min + mp3Data.getFullData());
}

//-Instance Functions------------------------------------------------------------------------------------------------
//Private:
bool Smf::fileIsValidSMF()
{
    // Get header region
    QByteArray headers = mFileDataF.left(0x1B);

    // General
    QByteArray smfSigRegion = headers.left(L_SMF_SIG);
    uint32_t majorVersion = Qx::ByteArray::RAWToPrimitive<uint32_t>(headers.mid(L_SMF_SIG, L_SMF_MAJ_VER)); // So far haven't seen SMF with non-1 here so unused
    uint32_t minorVersion = Qx::ByteArray::RAWToPrimitive<uint32_t>(headers.mid(L_SMF_SIG + L_SMF_MAJ_VER, L_SMF_MIN_VER));

    if(smfSigRegion == SMF_SIG)
    {
        // WAV Check
        if((minorVersion == MIN_VER_STD_PROTO_WAV || minorVersion == MIN_VER_STD_WAV) && toWav().isValid())
        {
            mType = WAVE;
            return true;
        }
        else if(minorVersion == MIN_VER_STD_MP3 && toMp3().isValid()) // MP3 Check
        {
            mType = MP3;
            return true;
        }
    }

    // Return false if no validity path is successful
    return false;
}

//Public:
bool Smf::isValid() { return !mFileDataF.isNull(); }
Smf::Type Smf::getType() { return mType; }
QByteArray Smf::getFullData() { return mFileDataF; }
Wav Smf::toWav() { return Wav(mFileDataF.mid(L_SMF_SIG + L_SMF_MAJ_VER + L_SMF_MIN_VER)); }
Mp3 Smf::toMp3() { return Mp3(mFileDataF.mid(L_SMF_SIG + L_SMF_MAJ_VER + L_SMF_MIN_VER)); }

