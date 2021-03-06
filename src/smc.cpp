#include "smc.h"
#include "qx.h"
#include <QtEndian>

//===============================================================================================================
// SMC
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:

Smc::Smc(QByteArray rawData)
    : mFileDataF(rawData)
{
    mValidFile = separateData();
    if(mValidFile)
        interpretData();
}

Smc::Smc()
{
    mValidFile = true;
}

//-Instance Functions------------------------------------------------------------------------------------------------
//Public:
QStringList& Smc::getInterpretedCacheListR() { return mCacheListI; }
QStringList Smc::getInterpretedCacheListV() { return mCacheListI; }
bool Smc::isValidCache() { return mValidFile; }

void Smc::deinterpretData()
{
    mCacheListR.clear();

    for (int i = 0; i< mCacheListI.length(); i++)
        mCacheListR.append(Qx::ByteArray::RAWFromString(mCacheListI.value(i)));
}

QByteArray Smc::rebuildRawFile()
{
    QByteArray rawFile;

    for (int i = 0; i < mCacheListR.length(); i++)
    {
        // Add entry length then entry
        rawFile.append(Qx::ByteArray::RAWFromPrimitive(uint32_t(mCacheListR.value(i).length()), Qx::Endian::LE));
        rawFile.append(mCacheListR.value(i));
    }

    return rawFile;
}

//Private:
bool Smc::separateData()
{
    bool fileIsValid = true;
    mDataCursor = 0x00; // Start of data

    while(mDataCursor != mFileDataF.length() && fileIsValid)
    {
        // Get length of next entry
        uint32_t nextEntryLength = qFromLittleEndian<quint32>(mFileDataF.mid(mDataCursor, L_ENTRY_LEN));

        // Advance to start of next entry
        mDataCursor += L_ENTRY_LEN;

        // Get next entry
        QByteArray nextEntry = mFileDataF.mid(mDataCursor, int(nextEntryLength));

        // Ensure file validity
        if(!rawEntryIsValid(nextEntry))
            return false;
        else
            mCacheListR.append(nextEntry);

        // Advance to start of the next size bytes
        mDataCursor += nextEntryLength;
    }

    return true;
}

void Smc::interpretData()
{
    for (int i = 0; i< mCacheListR.length(); i++)
        mCacheListI.append(Qx::String::fromByteArrayDirectly(mCacheListR.value(i)));
}

bool Smc::rawEntryIsValid(QByteArray entry)
{
    for(int i = 0; i < entry.length(); i++)
    {
        if(entry.at(i) < ENTRY_VALID_ASCII_MIN || entry.at(i) > ENTRY_VALID_ASCII_MAX )
            return false;
    }

    return true;
}
