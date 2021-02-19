#include "bsc.h"
#include <QtEndian>
#include <QObject>
#include <assert.h>
#include <QVector>
#include <functional>
#include "qx.h"

//===============================================================================================================
// BSC
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:

BSC::BSC(QByteArray rawData)
    : mFileDataF(rawData)
{
    separateData();
    interpretData();
}

BSC::BSC()
{
    // Initialize default vars
    mDataCursor = 0;

    // Full Data
    mFileDataF = "";

    // Segmented Raw Data
    mTweenerR = "";
    mFormatVerR = "";
    mPlaylistCountR = "";

    // Segmented Interpreted Data
    mTweenerI = "00 00 00 00 00";
    mFormatVerI = FORMAT_VER_DEFAULT;
    mPlaylistCountI = PLAYLIST_COUNT_DEFAULT;
}

//-Class Functions------------------------------------------------------------------------------------------------
//Public:
bool BSC::fileIsValidBSC(QFile& possibleBSC, Qx::IOOpReport& reportBuffer, bool& versionMismatch)
{
    QByteArray headerSection;
    reportBuffer = Qx::readBytesFromFile(headerSection, possibleBSC, 0, 16);
    versionMismatch = headerSection.mid(13, 4) != FORMAT_VER_TARGET_RAW;

    return reportBuffer.wasSuccessful() &&
           Qx::String::fromByteArrayDirectly(headerSection.left(4)) == EA_SHARK_SIG && Qx::String::fromByteArrayDirectly(headerSection.mid(9, 4)) == BSC::BSC_SIG;
}

//-Instance Functions------------------------------------------------------------------------------------------------
//Public:
QString BSC::getInterpretedTweener() const { return mTweenerI; }
uint32_t BSC::getInterpretedFormatVer() const { return mFormatVerI; }
uint8_t BSC::getInterpretedPlayListCount() const { return static_cast<uint8_t>(mPlaylists.length()); }
QList<BSC::Playlist> BSC::getPlaylistsV() const { return mPlaylists; }
QList<BSC::Playlist>& BSC::getPlaylistsR() { return mPlaylists; }
bool BSC::hasPlaylists() const { return mPlaylists.length() > 0; }
int BSC::totalPlaylistCount()
{
    int totalCount = 0;

    for(int i = 0; i < mPlaylists.length(); i++)
    {
        totalCount++;
        totalCount += recursiveSubPlaylistCounter(mPlaylists[i]);
    }

    return totalCount;
}

QByteArray BSC::rebuildRawFile()
{
    // Recursive Lambda for sub-playlists
    std::function<void(BSC::Playlist& parentPlaylist)> recursiveSubPlaylistDeinterpriation;
    recursiveSubPlaylistDeinterpriation = [&recursiveSubPlaylistDeinterpriation](BSC::Playlist& parentPlaylist)->void
    {
        for(int i = 0; i < parentPlaylist.subPlaylistCount(); i++)
        {
            parentPlaylist.getSubPlaylistsR()[i].deinterpretData();
            recursiveSubPlaylistDeinterpriation(parentPlaylist.getSubPlaylistsR()[i]);
        }
    };

    // Dump all interpreted data back to raw

    // Top level data
    deinterpretData();

    // Playlist data
    for(int i = 0; i < mPlaylists.length(); i++)
    {
        // Top level playlist
        mPlaylists[i].deinterpretData();

        // Sub playlists
        recursiveSubPlaylistDeinterpriation(mPlaylists[i]);
    }

    // Re-join separated data
    QByteArray rebuiltFile = Qx::ByteArray::RAWFromString(EA_SHARK_SIG);
    rebuiltFile += mTweenerR;
    rebuiltFile += Qx::ByteArray::RAWFromString(BSC_SIG);
    rebuiltFile += mFormatVerR;
    rebuiltFile += mPlaylistCountR;

    for(int i = 0; i < mPlaylists.length(); i++)
        rebuiltFile += mPlaylists[i].getRestitchedData();

    return rebuiltFile;
}

//Private:
void BSC::separateData()
{
    mDataCursor = 0x00; // Start of data

    mDataCursor += L_FORMAT_TAG_1; // Move to tweener start
        mTweenerR = mFileDataF.mid(mDataCursor,L_TWEENER);
    mDataCursor += L_TWEENER + L_FORMAT_TAG_2; // Move to format ver start
        mFormatVerR = mFileDataF.mid(mDataCursor,L_FORMAT_VER);
    mDataCursor += L_FORMAT_VER; // Move to playlist count start
        mPlaylistCountR = mFileDataF.mid(mDataCursor,L_PLAYLIST_COUNT);
    mDataCursor += L_PLAYLIST_COUNT; // Move to first playlist mode name length start

    // Create Playlists in order to continue sequential movement of the data cursor
    for(int i = 0; i < qFromLittleEndian<qint8>(mPlaylistCountR); i++)
        mPlaylists.append(Playlist(mDataCursor, mFileDataF, false));
}

void BSC::interpretData()
{
    mTweenerI = Qx::String::fromByteArrayHex(mTweenerR,TWEENER_BYTE_SEP, Qx::Endian::LE);
    mFormatVerI = qFromLittleEndian<quint32>(mFormatVerR);
    mPlaylistCountI = qFromLittleEndian<quint8>(mPlaylistCountR);
}

void BSC::deinterpretData()
{
    mTweenerR = Qx::ByteArray::RAWFromStringHex(mTweenerI.remove(TWEENER_BYTE_SEP));
    mFormatVerR = Qx::ByteArray::RAWFromPrimitive(mFormatVerI, Qx::Endian::LE);
    mPlaylistCountR = Qx::ByteArray::RAWFromPrimitive(static_cast<uint8_t>(mPlaylists.length()), Qx::Endian::LE);
}

int BSC::recursiveSubPlaylistCounter(BSC::Playlist& parentPlaylist)
{
    int subCount = 0;

    for (int i = 0; i < parentPlaylist.subPlaylistCount(); i++)
    {
        subCount++;
        subCount += recursiveSubPlaylistCounter(parentPlaylist.getSubPlaylistsR()[i]);
    }

    return subCount;
}

//===============================================================================================================
// PLAYLIST
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:

BSC::Playlist::Playlist(int &parentDataCursor, QByteArray &parentRawData, bool subPlaylist)
    : mSubPlaylist(subPlaylist)
{
    separateData(parentDataCursor, parentRawData);
    interpretData();
}

BSC::Playlist::Playlist(bool subPlaylist)
{
    defaultInitialization();
    mSubPlaylist = subPlaylist;
}

BSC::Playlist::Playlist() { defaultInitialization(); }

//-Instance Functions------------------------------------------------------------------------------------------------
//Public:
bool BSC::Playlist::getInitialTreeExpansion() const { return mInitiallyExpandedInTree; }
BSC::Playlist::PlaybackMode BSC::Playlist::getInterpretedPlaybackMode() const { return mPlaybackModeI; }
uint32_t BSC::Playlist::getInterpretedMaxInstances() const { return mMaxInstancesI; }
uint32_t BSC::Playlist::getInterpretedPolyphony() const { return mPolyphonyI; }
uint32_t BSC::Playlist::getInterpretedMinRepeatTime() const { return mMinRepeatTimeI; }
uint32_t BSC::Playlist::getInterpretedMaxRepeatTime() const { return mMaxRepeatTimeI; }
uint32_t BSC::Playlist::getInterpretedMinRepeatCount() const { return mMinRepeatCountI; }
uint32_t BSC::Playlist::getInterpretedMaxRepeatCount() const { return mMaxRepeatCountI; }
uint32_t BSC::Playlist::getInterpretedRandomPlaylistWeight() const { return mRandomPlaylistWeightI; }
float BSC::Playlist::getInterpretedLength() const { return mLengthI; }
float BSC::Playlist::getInterpretedReleaseTime() const { return mReleaseTimeI; }
QStringList BSC::Playlist::getInterpretedEffectBehaviorsV() const { return mEffectBehaviorsI; }
QStringList& BSC::Playlist::getInterpretedEffectBehaviorsR() { return mEffectBehaviorsI; }
bool BSC::Playlist::getInterpretedOverrideParentBehaviors() const { return mOverrideParentBehaviorsI; }
BSC::Playlist::Emitter BSC::Playlist::getInterpretedEmitter() const { return mEmitterI; }
QList<BSC::SoundContainer> BSC::Playlist::getSoundContainersV() const { return mSoundContainers; }
QList<BSC::SoundContainer>& BSC::Playlist::getSoundContainersR() { return mSoundContainers; }
QList<BSC::Playlist> BSC::Playlist::getSubPlaylistsV() const { return mSubPlaylists; }
QList<BSC::Playlist>& BSC::Playlist::getSubPlaylistsR() { return mSubPlaylists; }
bool BSC::Playlist::hasSoundContainers() const { return mSoundContainers.length() > 0; }
bool BSC::Playlist::hasSubPlaylists() const { return mSubPlaylists.length() > 0; }
bool BSC::Playlist::hasEffectBehaviors() const { return mEffectBehaviorsI.length() > 0; }
int BSC::Playlist::soundContainerCount() const { return mSoundContainers.length(); }
int BSC::Playlist::subPlaylistCount() const { return mSubPlaylists.length(); }
int BSC::Playlist::effectBehaviorCount() const { return mEffectBehaviorsI.length(); }

void BSC::Playlist::setInitialTreeExpansion(bool expanded) { mInitiallyExpandedInTree = expanded; }
void BSC::Playlist::setInterpretedPlaybackMode(BSC::Playlist::PlaybackMode playbackMode) { mPlaybackModeI = playbackMode; }
void BSC::Playlist::setInterpretedMaxInstances(uint32_t maxInstances) { mMaxInstancesI = maxInstances; }
void BSC::Playlist::setInterpretedPolyphony(uint32_t polphony) { mPolyphonyI = polphony; }
void BSC::Playlist::setInterpretedMinRepeatTime(uint32_t minRepeatTime) { mMinRepeatTimeI = minRepeatTime; }
void BSC::Playlist::setInterpretedMaxRepeatTime(uint32_t maxRepeatTime) { mMaxRepeatTimeI = maxRepeatTime; }
void BSC::Playlist::setInterpretedMinRepeatCount(uint32_t minRpeatCount) { mMinRepeatCountI = minRpeatCount; }
void BSC::Playlist::setInterpretedMaxRepeatCount(uint32_t maxRepeatCount) { mMaxRepeatCountI = maxRepeatCount; }
void BSC::Playlist::setInterpretedRandomPlaylistWeight(uint32_t randomPlaylistWeight) { mRandomPlaylistWeightI = randomPlaylistWeight; }
void BSC::Playlist::setInterpretedLength(float length) { mLengthI = length; }
void BSC::Playlist::setInterpretedReleaseTime(float releaseTime) { mReleaseTimeI = releaseTime; }
void BSC::Playlist::setInterpretedOverrideParentBehaviors(bool overrideParentBehaviors) { mOverrideParentBehaviorsI = overrideParentBehaviors; }
void BSC::Playlist::setInterpretedEmitter(BSC::Playlist::Emitter emitter) { mEmitterI = emitter; }

void BSC::Playlist::addInterpretedEffectBehavior(QString effectBehavior, int index)
{
    if(index == -1)
        mEffectBehaviorsI.append(effectBehavior);
    else
        mEffectBehaviorsI.insert(index, effectBehavior);
}
void BSC::Playlist::removeInterpretedEffectBehavior(int index) { mEffectBehaviorsI.removeAt(index); }
void BSC::Playlist::copyInterpretedEffectBehavior(int indexFrom, int indexTo) { mEffectBehaviorsI.insert(indexTo, mEffectBehaviorsI.value(indexFrom)); }
void BSC::Playlist::moveInterpretedEffectBehavior(int indexFrom, int indexTo) { mEffectBehaviorsI.move(indexFrom, indexTo); }
void BSC::Playlist::addSoundContainer(SoundContainer container, int index)
{
    if(index == -1)
        mSoundContainers.append(container);
    else
        mSoundContainers.insert(index, container);
}
void BSC::Playlist::removeSoundContainer(int index) { mSoundContainers.removeAt(index); }
void BSC::Playlist::copySoundContainer(int indexFrom, int indexTo) { mSoundContainers.insert(indexTo, mSoundContainers.value(indexFrom)); }
void BSC::Playlist::moveSoundContainer(int indexFrom, int indexTo) { mSoundContainers.move(indexFrom, indexTo); }

void BSC::Playlist::deinterpretData()
{
    mPlaybackModeR = enumPBModeToRaw(mPlaybackModeI);
    mMaxInstancesR = Qx::ByteArray::RAWFromPrimitive(mMaxInstancesI, Qx::Endian::LE);
    mPolyphonyR = Qx::ByteArray::RAWFromPrimitive(mPolyphonyI, Qx::Endian::LE);
    mMinRepeatTimeR = Qx::ByteArray::RAWFromPrimitive(mMinRepeatTimeI, Qx::Endian::LE);
    mMaxRepeatTimeR = Qx::ByteArray::RAWFromPrimitive(mMaxRepeatTimeI, Qx::Endian::LE);
    mMinRepeatCountR = Qx::ByteArray::RAWFromPrimitive(mMinRepeatCountI, Qx::Endian::LE);
    mMaxRepeatCountR = Qx::ByteArray::RAWFromPrimitive(mMaxRepeatCountI, Qx::Endian::LE);
    mRandomPlaylistWeightR = Qx::ByteArray::RAWFromPrimitive(mRandomPlaylistWeightI, Qx::Endian::LE);
    mLengthR = Qx::ByteArray::RAWFromPrimitive(mLengthI, Qx::Endian::LE);
    mReleaseTimeR = Qx::ByteArray::RAWFromPrimitive(mReleaseTimeI, Qx::Endian::LE);

    mEffectBehaviorsR.clear();
    for (int i = 0; i < mEffectBehaviorsI.length(); i++)
        mEffectBehaviorsR.append(Qx::ByteArray::RAWFromString(mEffectBehaviorsI.value(i)));

    mOverrideParentBehaviorsR = Qx::ByteArray::RAWFromPrimitive(mOverrideParentBehaviorsI);
    mEmitterR = enumEmitterToRaw(mEmitterI);

    for(int i = 0; i < mSoundContainers.length(); i++)
        mSoundContainers[i].deinterpretData();
}

QByteArray BSC::Playlist::getRestitchedData()
{
    QByteArray stitchedChunk;

    stitchedChunk = Qx::ByteArray::RAWFromPrimitive(mPlaybackModeR.length(), Qx::Endian::LE);
    stitchedChunk += mPlaybackModeR;
    stitchedChunk += mMaxInstancesR;
    stitchedChunk += mPolyphonyR;
    stitchedChunk += mMinRepeatTimeR;
    stitchedChunk += mMaxRepeatTimeR;
    stitchedChunk += mMinRepeatCountR;
    stitchedChunk += mMaxRepeatCountR;
    stitchedChunk += mRandomPlaylistWeightR;
    stitchedChunk += mLengthR;
    stitchedChunk += mReleaseTimeR;
    stitchedChunk += Qx::ByteArray::RAWFromPrimitive(mEffectBehaviorsR.length(), Qx::Endian::LE);

    for(int i = 0; i < mEffectBehaviorsR.length(); i++)
    {
        stitchedChunk += Qx::ByteArray::RAWFromPrimitive(mEffectBehaviorsR[i].length(), Qx::Endian::LE);
        stitchedChunk += mEffectBehaviorsR.value(i);
    }

    stitchedChunk += mOverrideParentBehaviorsR;
    stitchedChunk += Qx::ByteArray::RAWFromPrimitive(mEmitterR.length(), Qx::Endian::LE);
    stitchedChunk += mEmitterR;
    stitchedChunk += Qx::ByteArray::RAWFromPrimitive(mSoundContainers.length(), Qx::Endian::LE);

    for(int i = 0; i < mSoundContainers.length(); i++)
    {
        stitchedChunk += Qx::ByteArray::RAWFromPrimitive(i, Qx::Endian::LE);
        stitchedChunk += mSoundContainers[i].getRestitchedData();
    }

    stitchedChunk += Qx::ByteArray::RAWFromPrimitive(mSubPlaylists.length(), Qx::Endian::LE);

    for(int i = 0; i < mSubPlaylists.length(); i++)
    {
        stitchedChunk += Qx::ByteArray::RAWFromPrimitive(i, Qx::Endian::LE); //Sub-playlist index
        stitchedChunk += mSubPlaylists[i].getRestitchedData();
    }

    return stitchedChunk;
}

//Private:
void BSC::Playlist::defaultInitialization()
{
    // Initialize the only variables needed for a new playlist entry

    mSubPlaylist = false;
    mPlaybackModeI = BSC::Playlist::PlaybackMode::Normal;
    mMaxInstancesI = 0;
    mPolyphonyI = 1;
    mMinRepeatTimeI = 0;
    mMaxRepeatTimeI = 0;
    mMinRepeatCountI = 0;
    mMaxRepeatCountI = 0;
    mRandomPlaylistWeightI = 50;
    mLengthI = 0;
    mReleaseTimeI = 0;
    mOverrideParentBehaviorsI = false;
    mEmitterI = BSC::Playlist::Emitter::None;
}

void BSC::Playlist::separateData(int &dataCursor, QByteArray &rawData)
{
    // Temporary backend only data
    uint32_t pbModeNameLengthB;
    uint32_t ebNum;
    uint32_t ebNameLength;
    uint32_t emitterNameLength;
    uint32_t scNum;
    uint32_t spNum;

    // Data cursor starts on container index (or playback mode name length if not sub-playlist) from parent BSC
    if(mSubPlaylist)
    {
            mPlaylistIndexR = rawData.mid(dataCursor,L_SUB_PL_INDEX);
        dataCursor += L_SUB_PL_INDEX; // Move to playback mode name length start
    }

        pbModeNameLengthB = qFromLittleEndian<quint8>(rawData.mid(dataCursor,L_PB_MODE_LEN));
    dataCursor += L_PB_MODE_LEN; // Move to playback mode name length start
        mPlaybackModeR = rawData.mid(dataCursor,int(pbModeNameLengthB));
    dataCursor += pbModeNameLengthB; // Move to max instances start
        mMaxInstancesR = rawData.mid(dataCursor,L_MAX_INST);
    dataCursor += L_MAX_INST; // Move to polyphony start
        mPolyphonyR = rawData.mid(dataCursor,L_POLYHONY);
    dataCursor += L_POLYHONY; // Move to min repeat time start
        mMinRepeatTimeR = rawData.mid(dataCursor,L_MIN_RT);
    dataCursor += L_MIN_RT; // Move to max repeat time start
        mMaxRepeatTimeR = rawData.mid(dataCursor,L_MAX_RT);
    dataCursor += L_MAX_RT; // Move to min repeat count start
        mMinRepeatCountR = rawData.mid(dataCursor,L_MIN_RC);
    dataCursor += L_MIN_RC; // Move to max repeat count start
        mMaxRepeatCountR = rawData.mid(dataCursor,L_MAX_RC);
    dataCursor += L_MAX_RC; // Move to random weight start
        mRandomPlaylistWeightR = rawData.mid(dataCursor,L_RAND_PLYLST_WEIGHT);
    dataCursor += L_RAND_PLYLST_WEIGHT; // Move to length start
        mLengthR = rawData.mid(dataCursor,L_LENGTH);
    dataCursor += L_LENGTH; // Move to release time start
        mReleaseTimeR = rawData.mid(dataCursor,L_RELEASE_TIME);
    dataCursor += L_RELEASE_TIME; // Move to number of effect behaviors
        ebNum = qFromLittleEndian<quint32>(rawData.mid(dataCursor,L_EFF_BEH_NUM));
    dataCursor += L_EFF_BEH_NUM; // Move to first effect behavior name length byte\override flag if no effect behaviors

    for(int i = 0; i < int(ebNum); i++)
    {
            ebNameLength = qFromLittleEndian<quint32>(rawData.mid(dataCursor,L_EFF_BEH_NAME_LEN));
        dataCursor += L_EFF_BEH_NAME_LEN; // Move to first character of effect behavior name
            mEffectBehaviorsR.append(rawData.mid(dataCursor,int(ebNameLength)));
        dataCursor += ebNameLength; // Move to first byte after name
    }

        mOverrideParentBehaviorsR = rawData.mid(dataCursor,L_OVERRIDE_PB);
    dataCursor += L_OVERRIDE_PB; // Move to emitter name length start
        emitterNameLength = qFromLittleEndian<quint32>(rawData.mid(dataCursor,L_EMITTER_NAME_LEN));
    dataCursor += L_EMITTER_NAME_LEN; // Move to emitter name start
        mEmitterR = rawData.mid(dataCursor,int(emitterNameLength));
    dataCursor += emitterNameLength; // Move to number of sound containers start
        scNum = qFromLittleEndian<quint32>(rawData.mid(dataCursor,L_SND_CNTR_NUM));
    dataCursor += L_SND_CNTR_NUM; // Move to first sound container index start/sub-playlist count if no containers

    // Create SoundContainers in order to continue sequential movement of the data cursor
    for(int i = 0; i < int(scNum); i++)
        mSoundContainers.append(SoundContainer(dataCursor, rawData, i == int(scNum) - 1));

        spNum = qFromLittleEndian<quint32>(rawData.mid(dataCursor,L_SUB_PLYLST_COUNT));
    dataCursor += L_SUB_PLYLST_COUNT; // Move to beggining of sub-playlist

    // Create Sub-playlists in order to continue sequential movement of the data cursor
    for(int i = 0; i < int(spNum); i++)
        mSubPlaylists.append(Playlist(dataCursor, rawData, true));
}

void BSC::Playlist::interpretData()
{
//    if(mSubPlaylist)
//        mPlaylistIndexI = qFromLittleEndian<qint32>(mPlaylistIndexR); // Not needed, index can be infered from list index when writing
    mPlaybackModeI = rawPBModeToEnum(mPlaybackModeR);
    //mMaxInstancesI = qFromLittleEndian<quint32>(mMaxInstancesR);
    mMaxInstancesI = Qx::ByteArray::RAWToPrimitive<uint32_t>(mMaxInstancesR, Qx::Endian::LE);
    mPolyphonyI = qFromLittleEndian<quint32>(mPolyphonyR);
    mMinRepeatTimeI = qFromLittleEndian<quint32>(mMinRepeatTimeR);
    mMaxRepeatTimeI = qFromLittleEndian<quint32>(mMaxRepeatTimeR);
    mMinRepeatCountI = qFromLittleEndian<quint32>(mMinRepeatCountR);
    mMaxRepeatCountI = qFromLittleEndian<quint32>(mMaxRepeatCountR);
    mRandomPlaylistWeightI = qFromLittleEndian<quint32>(mRandomPlaylistWeightR);
    mLengthI = Qx::ByteArray::RAWToPrimitive<float>(mLengthR, Qx::Endian::LE);
    mReleaseTimeI = Qx::ByteArray::RAWToPrimitive<float>(mLengthR, Qx::Endian::LE);

    for (int i = 0; i < mEffectBehaviorsR.length(); i++)
        mEffectBehaviorsI.append(mEffectBehaviorsR.value(i));

    mOverrideParentBehaviorsI = Qx::ByteArray::RAWToPrimitive<bool>(mOverrideParentBehaviorsR);
    mEmitterI = rawEmitterToEnum(mEmitterR);
}

BSC::Playlist::PlaybackMode BSC::Playlist::rawPBModeToEnum(QByteArray rawPBMode)
{
    QString modeName = rawPBMode;
    int index = PLAYBACK_MODE_STRINGS.indexOf(modeName);

    if(index == -1)
        return PlaybackMode::Invalid_PBM;
    else
        return static_cast<PlaybackMode>(index);
}

QByteArray BSC::Playlist::enumPBModeToRaw(BSC::Playlist::PlaybackMode enumPBMode) { return Qx::ByteArray::RAWFromString(PLAYBACK_MODE_STRINGS.value(static_cast<int>(enumPBMode))); }

BSC::Playlist::Emitter BSC::Playlist::rawEmitterToEnum(QByteArray rawEmitter)
{
    QString emitter = rawEmitter;
    int index = EMITTER_STRINGS.indexOf(emitter);

    if(index == -1)
        return Emitter::Invalid_E;
    else
        return static_cast<Emitter>(index);
}

QByteArray BSC::Playlist::enumEmitterToRaw(Emitter enumEmitter) { return Qx::ByteArray::RAWFromString(EMITTER_STRINGS.value(static_cast<int>(enumEmitter))); }


//===============================================================================================================
// SOUND CONTAINER
//===============================================================================================================

//-Constructor------------------------------------------------------------------------------------------------
//Public:

BSC::SoundContainer::SoundContainer(int &parentDataCursor, QByteArray &parentRawData, bool lastSCInPL)
{
    separateData(parentDataCursor, parentRawData, lastSCInPL);
    interpretData();
}

BSC::SoundContainer::SoundContainer()
{
    // Initialize the only variables that are needed for a new blank sound container
    mVoiceGroupFrontI = BSC::SoundContainer::VOICE_GROUP_FRONT_STRINGS.value(0);
    mVoiceGroupBackI = BSC::SoundContainer::VOICE_GROUP_BACK_STRINGS[0].value(0);
    mIs3DI = false;
    mIsLoopedI = false;
    mUsingSpeedVolumeTriggerI = false;
    mRandomWeightI = 50;
    mAmplitudeFactorI = 0;
    mDopplerRatioI = 0;
    mPlaybackFrequenceI = 0;
    mStartI = 0;
    mEndI = 0;
    mMaskSampleI = 0;
    mLinkedFilesI = QStringList();
    mEffectContainerCountI = 0;
    mEffectContainerDataI = QString();
}

//-Instance Functions------------------------------------------------------------------------------------------------
//Public:
//int BSC::SoundContainer::getInterpretedContainerIndex() const { return mContainerIndexI; }
QString BSC::SoundContainer::getInterpretedVoiceGroupFront() const { return mVoiceGroupFrontI; }
QString BSC::SoundContainer::getInterpretedVoiceGroupBack() const { return mVoiceGroupBackI; }
bool BSC::SoundContainer::getInterpretedIs3D() const { return mIs3DI; }
bool BSC::SoundContainer::getInterpretedIsLooped() const { return mIsLoopedI; }
bool BSC::SoundContainer::getInterpretedUsingSpeedVolumeTrigger() const { return mUsingSpeedVolumeTriggerI; }
uint32_t BSC::SoundContainer::getInterpretedRandomWeight() const { return mRandomWeightI; }
float BSC::SoundContainer::getInterpretedAmplitudeFactor() const { return mAmplitudeFactorI; }
float BSC::SoundContainer::getInterpretedDopplerRatio() const { return mDopplerRatioI; }
float BSC::SoundContainer::getInterpretedPlaybackFrequence() const { return mPlaybackFrequenceI; }
float BSC::SoundContainer::getInterpretedStart() const { return mStartI; }
float BSC::SoundContainer::getInterpretedEnd() const { return mEndI; }
float BSC::SoundContainer::getInterpretedMaskSample() const { return mMaskSampleI; }
QStringList BSC::SoundContainer::getInterpretedLinkedFilesV() const { return mLinkedFilesI; }
QStringList& BSC::SoundContainer::getInterpretedLinkedFilesR() { return mLinkedFilesI; }
uint32_t BSC::SoundContainer::getInterpretedEffectContainerCount() const { return mEffectContainerCountI; }
QString BSC::SoundContainer::getInterpretedEffectContainerData() const { return mEffectContainerDataI; }
bool BSC::SoundContainer::hasLinkedFiles() const { return mLinkedFilesI.length() > 0; }
int BSC::SoundContainer::linkedFileCount() const { return mLinkedFilesI.length(); }

void BSC::SoundContainer::setInterpretedVoiceGroupFront(QString voiceGroup) { mVoiceGroupFrontI = voiceGroup; }
void BSC::SoundContainer::setInterpretedVoiceGroupBack(QString voiceGroup) { mVoiceGroupBackI = voiceGroup; }
void BSC::SoundContainer::setInterpretedIs3D(bool is3D) { mIs3DI = is3D; }
void BSC::SoundContainer::setInterpretedIsLooped(bool isLopped) { mIsLoopedI = isLopped; }
void BSC::SoundContainer::setInterpretedUsingSpeedVolumeTrigger(bool usingSpeedVolumeTrigger) { mUsingSpeedVolumeTriggerI = usingSpeedVolumeTrigger; }
void BSC::SoundContainer::setInterpretedRandomWeight(uint32_t randomWeight) { mRandomWeightI = randomWeight; }
void BSC::SoundContainer::setInterpretedAmplitudeFactor(float amplitudeFactor) { mAmplitudeFactorI = amplitudeFactor; }
void BSC::SoundContainer::setInterpretedDopplerRatio(float dopplerRatio) { mDopplerRatioI = dopplerRatio; }
void BSC::SoundContainer::setInterpretedPlaybackFrequence(float playbackFrequence) { mPlaybackFrequenceI = playbackFrequence; }
void BSC::SoundContainer::setInterpretedStart(float start) { mStartI = start; }
void BSC::SoundContainer::setInterpretedEnd(float end) { mEndI = end; }
void BSC::SoundContainer::setInterpretedMaskSample(float maskSample) { mMaskSampleI = maskSample; }
void BSC::SoundContainer::setInterpretedEffectContainerCount(uint32_t effectContainerCount) { mEffectContainerCountI = effectContainerCount; }
void BSC::SoundContainer::setInterpretedEffectContainerData(QString effectContainerData) { mEffectContainerDataI = effectContainerData; }

void BSC::SoundContainer::addInterpretedLinkedFile(QString linkedFile, int index)
{
    if(index == -1)
        mLinkedFilesI.append(linkedFile);
    else
        mLinkedFilesI.insert(index, linkedFile);
}
void BSC::SoundContainer::removeInterpretedLinkedFile(int index) { mLinkedFilesI.removeAt(index); }
void BSC::SoundContainer::copyInterpretedLinkedFile(int indexFrom, int indexTo) { mLinkedFilesI.insert(indexTo, mLinkedFilesI.value(indexFrom)); }
void BSC::SoundContainer::moveInterpretedLinkedFile(int indexFrom, int indexTo) { mLinkedFilesI.move(indexFrom, indexTo); }

void BSC::SoundContainer::deinterpretData()
{
    // Voice Group
    mVoiceGroupR = Qx::ByteArray::RAWFromString((mVoiceGroupFrontI + VOICE_GROU_SEP + mVoiceGroupBackI));

    // Others
    mIs3DR = Qx::ByteArray::RAWFromPrimitive(mIs3DI);
    mIsLoopedR = Qx::ByteArray::RAWFromPrimitive(mIsLoopedI);
    mUsingSpeedVolumeTriggerR = Qx::ByteArray::RAWFromPrimitive(mUsingSpeedVolumeTriggerI);
    mRandomWeightR = Qx::ByteArray::RAWFromPrimitive(mRandomWeightI, Qx::Endian::LE);
    mAmplitudeFactorR = Qx::ByteArray::RAWFromPrimitive(mAmplitudeFactorI, Qx::Endian::LE);
    mDopplerRatioR = Qx::ByteArray::RAWFromPrimitive(mDopplerRatioI, Qx::Endian::LE);
    mPlaybackFrequenceR = Qx::ByteArray::RAWFromPrimitive(mPlaybackFrequenceI, Qx::Endian::LE);
    mStartR = Qx::ByteArray::RAWFromPrimitive(mStartI, Qx::Endian::LE);
    mEndR = Qx::ByteArray::RAWFromPrimitive(mEndI, Qx::Endian::LE);
    mMaskSampleR = Qx::ByteArray::RAWFromPrimitive(mMaskSampleI, Qx::Endian::LE);

    mLinkedFilesR.clear();
    for(int i = 0; i < mLinkedFilesI.length(); i++)
        mLinkedFilesR.append(Qx::ByteArray::RAWFromString(mLinkedFilesI.value(i)));

    mEffectContainerCountR = Qx::ByteArray::RAWFromPrimitive(mEffectContainerCountI, Qx::Endian::LE);
    mEffectContainerDataR = Qx::ByteArray::RAWFromString(mEffectContainerDataI);
}

QByteArray BSC::SoundContainer::getRestitchedData()
{
    QByteArray stitchedChunk;

    stitchedChunk += Qx::ByteArray::RAWFromPrimitive(mVoiceGroupR.length(), Qx::Endian::LE);
    stitchedChunk += mVoiceGroupR;
    stitchedChunk += mIs3DR;
    stitchedChunk += mIsLoopedR;
    stitchedChunk += mUsingSpeedVolumeTriggerR;
    stitchedChunk += mRandomWeightR;
    stitchedChunk += mAmplitudeFactorR;
    stitchedChunk += mDopplerRatioR;
    stitchedChunk += mPlaybackFrequenceR;
    stitchedChunk += mStartR;
    stitchedChunk += mEndR;
    stitchedChunk += mMaskSampleR;
    stitchedChunk += Qx::ByteArray::RAWFromPrimitive(static_cast<uint8_t>(mLinkedFilesR.length()), Qx::Endian::LE);

    for(int i = 0; i < mLinkedFilesR.length(); i++)
    {
        stitchedChunk += Qx::ByteArray::RAWFromPrimitive(mLinkedFilesR[i].length(), Qx::Endian::LE);
        stitchedChunk += mLinkedFilesR.value(i);
    }

    stitchedChunk += mEffectContainerCountR;

    if(mEffectContainerCountI > 0)
        stitchedChunk += mEffectContainerDataR;

    return stitchedChunk;
}

//Private:
void BSC::SoundContainer::separateData(int &dataCursor, QByteArray &rawData, bool lastSCInPL)
{
    // Temporary backend only data
    int vgNameLength;
    int lfNum;
    int lfNameLength;

    // Data cursor starts on container index from parent playlist
        mContainerIndexR = rawData.mid(dataCursor,L_CNTR_INDEX);
    dataCursor += L_CNTR_INDEX; // Move to voice group name length start
        vgNameLength = qFromLittleEndian<qint32>(rawData.mid(dataCursor,L_VOICE_GRP_NAM_LEN));
    dataCursor += L_VOICE_GRP_NAM_LEN; // Move to voice group name start
        mVoiceGroupR = rawData.mid(dataCursor,vgNameLength);
    dataCursor += vgNameLength; // Move to is 3d start
        mIs3DR = rawData.mid(dataCursor,L_IS_3D);
    dataCursor += L_IS_3D; // Move to is looped start
        mIsLoopedR = rawData.mid(dataCursor,L_IS_LOOPED);
    dataCursor += L_IS_LOOPED; // Move to using speed/volume trigger start
        mUsingSpeedVolumeTriggerR = rawData.mid(dataCursor,L_USE_SV_TRIG);
    dataCursor += L_USE_SV_TRIG; // Move to random weight start
        mRandomWeightR = rawData.mid(dataCursor,L_RAND_WEIGHT);
    dataCursor += L_RAND_WEIGHT; // Move to amplitude factor start
        mAmplitudeFactorR = rawData.mid(dataCursor,L_AMP_FACTOR);
    dataCursor += L_AMP_FACTOR; // Move to doppler ratio start
        mDopplerRatioR = rawData.mid(dataCursor,L_DOP_RATIO);
    dataCursor += L_DOP_RATIO; // Move to playback frequence start
        mPlaybackFrequenceR = rawData.mid(dataCursor,L_PLAY_FREQ);
    dataCursor += L_PLAY_FREQ; // Move to start start
        mStartR = rawData.mid(dataCursor,L_START);
    dataCursor += L_START; // Move to end start
        mEndR = rawData.mid(dataCursor,L_END);
    dataCursor += L_END; // Move to mask sample start;
        mMaskSampleR = rawData.mid(dataCursor,L_MSK_SAMP);
    dataCursor += L_MSK_SAMP; // Move to number of linked file num start
        lfNum = qFromLittleEndian<qint8>(rawData.mid(dataCursor,L_LNKD_FILE_NUM));
    dataCursor += L_LNKD_FILE_NUM; // Move to first linked file name length byte

    for(int i = 0; i < lfNum; i++)
    {
            lfNameLength = qFromLittleEndian<qint32>(rawData.mid(dataCursor,L_LNKD_FILE_NAM_LEN));
        dataCursor += L_LNKD_FILE_NAM_LEN; // Move to first character of linked file name
            mLinkedFilesR.append(rawData.mid(dataCursor,lfNameLength));
        dataCursor += lfNameLength; // Move to first byte after name
    }

        mEffectContainerCountR = rawData.mid(dataCursor,L_EFF_CONT_NUM);
    dataCursor += L_EFF_CONT_NUM; // Move to next byte after effect container number

    // Import raw effect contianer data if present
    if(qFromLittleEndian<quint32>(mEffectContainerCountR) > 0)
    {
        int effectContainerEnd;

        // Get effect container data based on next sound container OR next playlist or EOF
        if(!lastSCInPL)
            effectContainerEnd = findEffectContainerEndMidSC(rawData, dataCursor);
        else
            effectContainerEnd = findEffectContainerEndLastSC(rawData, dataCursor);

        mEffectContainerDataR = rawData.mid(dataCursor, Qx::rangeToLength(dataCursor, effectContainerEnd));

        // Move cursor to next sound container
        dataCursor = effectContainerEnd + 1;
    }
}

void BSC::SoundContainer::interpretData()
{
    // mContainerIndexI = qFromLittleEndian<qint32>(mContainerIndexR); // Not needed, index can be infered from list index when writing

    // Voice Group
    QString wholeString = Qx::String::fromByteArrayDirectly(mVoiceGroupR);
    int sepPos = wholeString.indexOf(VOICE_GROU_SEP);
    mVoiceGroupFrontI = wholeString.left(sepPos);
    mVoiceGroupBackI = wholeString.mid(sepPos + VOICE_GROU_SEP.length());

    // Others
    mIs3DI = Qx::ByteArray::RAWToPrimitive<bool>(mIs3DR);
    mIsLoopedI = Qx::ByteArray::RAWToPrimitive<bool>(mIsLoopedR);
    mUsingSpeedVolumeTriggerI = Qx::ByteArray::RAWToPrimitive<bool>(mUsingSpeedVolumeTriggerR);
    mRandomWeightI = qFromLittleEndian<quint32>(mRandomWeightR);
    mAmplitudeFactorI = Qx::ByteArray::RAWToPrimitive<float>(mAmplitudeFactorR, Qx::Endian::LE);
    mDopplerRatioI = Qx::ByteArray::RAWToPrimitive<float>(mDopplerRatioR, Qx::Endian::LE);
    mPlaybackFrequenceI = Qx::ByteArray::RAWToPrimitive<float>(mPlaybackFrequenceR, Qx::Endian::LE);
    mStartI = Qx::ByteArray::RAWToPrimitive<float>(mStartR, Qx::Endian::LE);
    mEndI = Qx::ByteArray::RAWToPrimitive<float>(mEndR, Qx::Endian::LE);
    mMaskSampleI = Qx::ByteArray::RAWToPrimitive<float>(mMaskSampleR, Qx::Endian::LE);

    for (int i = 0; i < mLinkedFilesR.length(); i++)
        mLinkedFilesI.append(mLinkedFilesR.value(i));

    mEffectContainerCountI = qFromLittleEndian<quint32>(mEffectContainerCountR);
    mEffectContainerDataI = Qx::String::fromByteArrayDirectly(mEffectContainerDataR);
}

BSC::SoundContainer::ReasonableSCType BSC::SoundContainer::determineReasonableSCDataType(QByteArray rawVal)
{
    assert(rawVal.size() == 4); // Data must be 32-bit (4 bytes)
    uint32_t dataAsInt = qFromLittleEndian<quint32>(rawVal);

    if(dataAsInt <= BSC::SoundContainer::RES_SC_INT_CUTOFF)
        return RES_INT;
    else
        return RES_FLOAT;
}

int BSC::SoundContainer::findEffectContainerEndMidSC(QByteArray &fullData, int startOffset)
{
    QVector<int> voiceGroupHits;

    // Search for next sound container by finding the next voice group
    for(int i = 0; i < BSC::SoundContainer::VOICE_GROUP_FRONT_STRINGS.length(); i++)
        voiceGroupHits.append(fullData.indexOf(BSC::SoundContainer::VOICE_GROUP_FRONT_STRINGS.value(i),startOffset));

    // Remove misses
    voiceGroupHits.removeAll(-1);

    assert(!voiceGroupHits.isEmpty()); // If no voice group is found this function shouldn't have been called

    int firstIndex = *std::min_element(voiceGroupHits.begin(), voiceGroupHits.end()); // Iterates over vector using std::iterator and dereferences the index pointer to get value

    // Walk backwards to get effect container end
    return firstIndex - L_VOICE_GRP_NAM_LEN - L_CNTR_INDEX - 0x01;
}

int BSC::SoundContainer::findEffectContainerEndLastSC(QByteArray &fullData, int startOffset)
{
    QVector<int> modeNameHits;

    // Search for next playlist by finding the next voice group
    for(int i = 0; i < BSC::Playlist::PLAYBACK_MODE_STRINGS.length(); i++)
        modeNameHits.append(fullData.indexOf(BSC::Playlist::PLAYBACK_MODE_STRINGS.value(i),startOffset));

    // Remove misses
    modeNameHits.removeAll(-1);

    int firstIndex = *std::min_element(modeNameHits.begin(), modeNameHits.end()); // Iterates over vector using std::iterator and dereferences the index pointer to get value

    // All Possible Situations:
    // Situation A - End of playlist that has sub playlists
    // Situation B - End of playlist that has no sub playlists, sub playlist is next
    // Situation C - End of playlist that has no sub playlists, top level playlist is next
    // Situation D - End of any playlist that has no sub playlists and is last playlist


    // Determine the current situation and calculate effect container end from there
    if(modeNameHits.isEmpty()) // No Mode Name Found = Situation D
        return (fullData.length() - 0x01) - BSC::Playlist::L_SUB_PLYLST_COUNT;
    else
    {
        // Walk back to first byte of mode name length
        int modeNameLengthOffset = firstIndex - BSC::Playlist::L_PB_MODE_LEN;

        // Get the two values before playback mode name length (luckily all involved values are exactly 4 bytes long)
        QByteArray prevVal = fullData.mid(firstIndex - BSC::Playlist::L_PB_MODE_LEN - 0x04, 0x04);
        QByteArray secondPrevVal = fullData.mid(firstIndex - BSC::Playlist::L_PB_MODE_LEN - 0x08, 0x04);

        if(qFromLittleEndian<quint32>(prevVal) == 0x00)
        {
           if(determineReasonableSCDataType(secondPrevVal) == RES_INT && qFromLittleEndian<quint32>(secondPrevVal) > 0x00) // Situation A
               return modeNameLengthOffset - BSC::Playlist::L_SUB_PL_INDEX - BSC::Playlist::L_SUB_PLYLST_COUNT - 0x01; // Extra -1 to go to end of EC from start of sub-playlist count
           else // Situation C
               return modeNameLengthOffset - BSC::Playlist::L_SUB_PLYLST_COUNT - 0x01; // Extra -1 to go to end of EC from start of sub-playlist count
        }
        else // Situation B
            return modeNameLengthOffset - BSC::Playlist::L_SUB_PL_INDEX - BSC::Playlist::L_SUB_PLYLST_COUNT - 0x01; // Extra -1 to go to end of EC from start of sub-playlist count
    }
}


