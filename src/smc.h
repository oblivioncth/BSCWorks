#ifndef SMC_H
#define SMC_H

#include <QByteArray>
#include <QByteArrayList>

class Smc
{
//-Class Variables-----------------------------------------------------------------------------------------------
public:
    static inline const QString FILE_EXT = "cache";

    static const int L_ENTRY_LEN = 0x04;

    static const int ENTRY_VALID_ASCII_MIN = 0x21;
    static const int ENTRY_VALID_ASCII_MAX = 0x7E;

//-Instance Variables--------------------------------------------------------------------------------------------
private:
    // Raw Index Cursor
    int mDataCursor;

    // Full Data
    QByteArray mFileDataF;

    // Segmented Raw Data
    QByteArrayList mCacheListR;

    // Segmented Interpreted Data
    QStringList mCacheListI;

    // File Validity
    bool mValidFile;

//-Constructor-------------------------------------------------------------------------------------------------
public:
    Smc(QByteArray rawData);
    Smc();

//-Class Functions------------------------------------------------------------------------------------------------------
public:

//-Instance Functions---------------------------------------------------------------------------------------------------
public:
    QStringList& getInterpretedCacheListR();
    QStringList getInterpretedCacheListV();
    bool isValidCache();

    void deinterpretData();
    QByteArray rebuildRawFile();

private:
    bool separateData();
    void interpretData();
    bool rawEntryIsValid(QByteArray entry);
};

#endif // SMC_H
