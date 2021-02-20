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
Smf Smf::fromStandard(Wav wavData) { return addSMFHeader(wavData.getFullData()); }
Smf Smf::fromStandard(Mp3 mp3Data) { return addSMFHeader(mp3Data.getFullData()); }

//-Instance Functions------------------------------------------------------------------------------------------------
//Private:
bool Smf::fileIsValidSMF()
{
    // Get header region
    QByteArray headers = mFileDataF.left(0x1B);

    // General
    QByteArray smfSigRegion = headers.left(L_SMF_SIG);
    QByteArray unkFlagOne = headers.mid(L_SMF_SIG, L_SMF_CMN_FLAGS/2); // Not understood so unused
    QByteArray unkFlagTwo = headers.mid(L_SMF_SIG + L_SMF_CMN_FLAGS/2, L_SMF_CMN_FLAGS/2); // Not understood so unused

    if(smfSigRegion == SMF_SIG)
    {
        // WAV Check
        if(toWav().isValid())
        {
            mType = WAVE;
            return true;
        }
        else if(toMp3().isValid()) // MP3 Check
        {
            mType = MP3;
            return true;
        }
    }

    // Return false if no validity path is successful
    return false;
}

QByteArray Smf::addSMFHeader(const QByteArray& fileData)
{
    return Qx::ByteArray::RAWFromString(Smf::SMF_SIG) + QByteArray(Smf::SMF_CMN_FLAGS, 8) + fileData;
}

//Public:
bool Smf::isValid() { return !mFileDataF.isNull(); }
Smf::Type Smf::getType() { return mType; }
QByteArray Smf::getFullData() { return mFileDataF; }
Wav Smf::toWav() { return Wav(mFileDataF.mid(L_SMF_SIG + L_SMF_CMN_FLAGS)); } // Uses unknown flag values that seem to be common to a huge majority of SMFs
Mp3 Smf::toMp3() { return Mp3(mFileDataF.mid(L_SMF_SIG + L_SMF_CMN_FLAGS)); } // Uses unknown flag values that seem to be common to a huge majority of SMFs

