#ifndef SMF_H
#define SMF_H

#include <QFile>
#include "wav.h"
#include "mp3.h"
#include "qx-io.h"

class Smf
{
//-Class Enums---------------------------------------------------------------------------------------------------
public:
    enum Type {WAVE, MP3};

//-Class Variables-----------------------------------------------------------------------------------------------
public:
    static inline const QString FILE_EXT = "smf";
    static inline const QString SMF_SIG = "fssm";


    static inline const char SMF_CMN_FLAGS[] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};  //This appears to actually be two back-to-back,
    // 32-bit, little-endian values of 1 that may be some kind of count. A few older looking BSCs use 1,0 instead, perhaps its a version tag? Initialized
    // using char[] because QByteArray seems to be bugged when using "inline" with null (0x00) characters as part of the array

    static const int L_SMF_SIG = 0x04;
    static const int L_SMF_CMN_FLAGS = 0x08;

//-Instance Variables--------------------------------------------------------------------------------------------
private:
    // Full Data
    QByteArray mFileDataF;
    Type mType;

//-Constructor-------------------------------------------------------------------------------------------------
public:
    Smf(QByteArray rawSMFData);

//-Class Functions-------------------------------------------------------------------------------------------------------
private:
    static QByteArray addSMFHeader(const QByteArray& fileData);

public:
    static Smf fromStandard(Wav wavData);
    static Smf fromStandard(Mp3 mp3Data);

//-Instance Functions---------------------------------------------------------------------------------------------------
private:
    bool fileIsValidSMF();

public:
    bool isValid();
    Type getType();
    QByteArray getFullData();
    Wav toWav();
    Mp3 toMp3();
};

#endif // SMF_H
