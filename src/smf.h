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

    static inline const uint32_t MAJ_VER_STD = 1;
    static inline const uint32_t MIN_VER_STD_PROTO_WAV = 0; // Seems to be in unused/really old SMFs, unused in this app for now
    static inline const uint32_t MIN_VER_STD_WAV = 1;
    static inline const uint32_t MIN_VER_STD_MP3 = 2;

    static const int L_SMF_SIG = 0x04;
    static const int L_SMF_MAJ_VER = 0x04;
    static const int L_SMF_MIN_VER = 0x04;

//-Instance Variables--------------------------------------------------------------------------------------------
private:
    // Full Data
    QByteArray mFileDataF;
    Type mType;

//-Constructor-------------------------------------------------------------------------------------------------
public:
    Smf(QByteArray rawSMFData);

//-Class Functions-------------------------------------------------------------------------------------------------------
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
