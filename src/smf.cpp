#include "smf.h"
#include "wav.h"
#include "mp3.h"
#include "qx.h"

//===============================================================================================================
// SMC
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:
SMF::SMF(QByteArray rawData) : mFileDataF(rawData)
{
    if(!fileIsValidSMF())
        mFileDataF = QByteArray();
}

//-Class Functions---------------------------------------------------------------------------------------------------
SMF SMF::fromStandard(WAV wavData) { return addSMFHeader(wavData.getFullData()); }
SMF SMF::fromStandard(MP3 mp3Data) { return addSMFHeader(mp3Data.getFullData()); }

//-Instance Functions------------------------------------------------------------------------------------------------
//Private:
bool SMF::fileIsValidSMF()
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
        if(toWAV().isValid())
        {
            mType = Wav;
            return true;
        }
        else if(toMP3().isValid()) // MP3 Check
        {
            mType = Mp3;
            return true;
        }
    }

    // Return false if no validity path is successful
    return false;
}

QByteArray SMF::addSMFHeader(const QByteArray& fileData)
{
    return Qx::ByteArray::RAWFromString(SMF::SMF_SIG) + QByteArray(SMF::SMF_CMN_FLAGS, 8) + fileData;
}

//Public:
bool SMF::isValid() { return !mFileDataF.isNull(); }
QByteArray SMF::getFullData() { return mFileDataF; }
WAV SMF::toWAV() { return WAV(mFileDataF.mid(L_SMF_SIG + L_SMF_CMN_FLAGS)); } // Uses unknown flag values that seem to be common to a huge majority of SMFs
MP3 SMF::toMP3() { return MP3(mFileDataF.mid(L_SMF_SIG + L_SMF_CMN_FLAGS)); } // Uses unknown flag values that seem to be common to a huge majority of SMFs

