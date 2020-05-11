#ifndef SMF_H
#define SMF_H

#include <QFile>
#include "wav.h"
#include "qx-io.h"


//-Related Class Forward Declarations----------------------------------------------------------------------------
class WAV;

class SMF
{
//-Class Variables-----------------------------------------------------------------------------------------------
public:
    static inline const QString FILE_EXT = "smf";
    static inline const QString SMF_SIG = "fssm";


    static inline const char SMF_CMN_FLAGS[] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};  //This appears to actually be two back-to-back,
    // 32-bit, little-endian values of 1 that may be some kind of count. A few older looking BSCs use 1,0 instead, perhaps its a version tag? Initialized
    // using char[] because QByteArray seems to be bugged when using "inline" with null (0x00) characters as part of the array

    static inline const QString RIFF_SIG = "RIFF";
    static inline const QString WAV_SIG = "WAVE";

    static const int L_SMF_SIG = 0x04;
    static const int L_SMF_CMN_FLAGS = 0x08;
    static const int L_RIFF_SIG = 0x04;
    static const int L_RIFF_LEN = 0x04;
    static const int L_WAV_SIG = 0x04;


//-Instance Variables--------------------------------------------------------------------------------------------
private:
    // Full Data
    QByteArray mFileDataF;

//-Constructor-------------------------------------------------------------------------------------------------
public:
    SMF(QByteArray rawData);

//-Class Functions------------------------------------------------------------------------------------------------------
public:
    static bool fileIsValidSMF(QFile& possibleSMF, Qx::IO::IOOpReport& reportBuffer);

private:

//-Instance Functions---------------------------------------------------------------------------------------------------
public:
    QByteArray getFullData();
    WAV toWAV();

private:
};

#endif // SMF_H
