#ifndef MP3_H
#define MP3_H
#include <QFile>
#include "qx.h"
#include "qx-io.h"

class MP3
{
//-Class Variables-----------------------------------------------------------------------------------------------
public:
    static inline const QString FILE_EXT = "mp3";
    static inline const QByteArray MP3_OLD_SIG_1 = Qx::ByteArray::RAWFromStringHex("FFFB");
    static inline const QByteArray MP3_OLD_SIG_2 = Qx::ByteArray::RAWFromStringHex("FFF3");
    static inline const QByteArray MP3_OLD_SIG_3 = Qx::ByteArray::RAWFromStringHex("FFF2");
    static inline const QByteArray MP3_NEW_SIG = Qx::ByteArray::RAWFromStringHex("494433");
    static const int L_MP3_OLD_SIG = 0x03;
    static const int L_MP3_NEW_SIG = 0x04;

    //-Instance Variables--------------------------------------------------------------------------------------------
    private:
        QByteArray mFileDataF;

    //-Constructor-------------------------------------------------------------------------------------------------
    public:
        MP3(QByteArray rawMP3Data);

    //-Instance Functions---------------------------------------------------------------------------------------------------
    private:
        bool fileIsValidMP3();

    public:
        bool isValid();
        QByteArray getFullData();
};

#endif // MP3_H
