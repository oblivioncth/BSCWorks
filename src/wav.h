#ifndef WAV_H
#define WAV_H

#include <QFile>
#include "smf.h"
#include "qx-io.h"

//-Related Class Forward Declarations----------------------------------------------------------------------------
class SMF;

class WAV
{
//-Class Variables-----------------------------------------------------------------------------------------------
public:
    static inline const QString FILE_EXT = "wav";
    static inline const QString RIFF_SIG = "RIFF";
    static inline const QString WAV_SIG = "WAVE";
    static inline const QString FORMAT_SIG = "fmt ";

    static const int L_RIFF_SIG = 0x04;
    static const int L_RIFF_LEN = 0x04;
    static const int L_WAV_SIG = 0x04;
    static const int L_FORMAT_SIG = 0x04;

//-Instance Variables--------------------------------------------------------------------------------------------
private:
    QByteArray mFileDataF;

//-Constructor-------------------------------------------------------------------------------------------------
public:
    WAV(QByteArray rawData);

//-Class Functions------------------------------------------------------------------------------------------------------
public:
    static bool fileIsValidWAV(QFile& possibleWAV, Qx::IO::IOOpReport& reportBuffer);

private:

//-Instance Functions---------------------------------------------------------------------------------------------------
public:
    QByteArray getFullData();
    SMF toSMF();

private:
};

#endif // WAV_H
