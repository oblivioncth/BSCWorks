#ifndef WAV_H
#define WAV_H

#include <QFile>
#include "qx-io.h"

class WAV
{
//-Class Variables-----------------------------------------------------------------------------------------------
public:
    static inline const QString FILE_EXT = "wav";
    static inline const QString RIFF_SIG = "RIFF";
    static inline const QString WAV_SIG = "WAVE";

    static const int L_RIFF_SIG = 0x04;
    static const int L_RIFF_LEN = 0x04;
    static const int L_WAV_SIG = 0x04;

//-Instance Variables--------------------------------------------------------------------------------------------
private:
    QByteArray mFileDataF;

//-Constructor-------------------------------------------------------------------------------------------------
public:
    WAV(QByteArray rawWAVData);

//-Instance Functions---------------------------------------------------------------------------------------------------
private:
    bool fileIsValidWAV();

public:
    bool isValid();
    QByteArray getFullData();
};

#endif // WAV_H
