#ifndef BSC_H
#define BSC_H
#include <QByteArray>
#include <QFile>
#include <QStringList>
#include "qx.h"
#include "qx-io.h"

class BSC
{
//-Inner Class Forward Declarations---------------------------------------------------------------------------------------
public:
    class Playlist;
    class SoundContainer;

//-Class Variables
public:
    static inline const QString FILE_EXT = "bsc";
    static inline const QChar TWEENER_BYTE_SEP = ' ';
    static inline const QString EA_SHARK_SIG = "easb";
    static inline const QString BSC_SIG = "sbhs";

    static inline const QString TWEENER_DEFAULT = "00" + QString(TWEENER_BYTE_SEP) + "00" + QString(TWEENER_BYTE_SEP) + "00"
                                                + QString(TWEENER_BYTE_SEP) + "00" + QString(TWEENER_BYTE_SEP) + "00";
    static const uint32_t FORMAT_TAG_DEFAULT = 14;
    static const uint8_t PLAYLIST_COUNT_DEFAULT = 0;

    static const int L_FORMAT_TAG_1 = 0x04; // easb
    static const int L_FORMAT_TAG_2 = 0x04; // sbhs
    static const int L_TWEENER = 0x05;
    static const int L_FORMAT_VER = 0x04;
    static const int L_PLAYLIST_COUNT = 0x01;

//-Instance Variables--------------------------------------------------------------------------------------------
private:
    // Raw Index Cursor
    int mDataCursor;

    // Full Data
    QByteArray mFileDataF;

    // Segmented Raw Data
    QByteArray mTweenerR;
    QByteArray mFormatVerR;
    QByteArray mPlaylistCountR;

    // Segmented Interpreted Data
    QString mTweenerI;
    uint32_t mFormatVerI;
    uint8_t mPlaylistCountI;

    // Segmented Sub-section Data
    QList<Playlist> mPlaylists;

//-Constructor-------------------------------------------------------------------------------------------------
public:
    BSC(QByteArray rawData);
    BSC();

//-Class Functions------------------------------------------------------------------------------------------------------
public:
    static bool fileIsValidBSC(QFile& possibleBSC, Qx::IO::IOOpReport& reportBuffer);

private:

//-Instance Functions---------------------------------------------------------------------------------------------------
public:
    QString getInterpretedTweener() const;
    uint32_t getInterpretedFormatVer() const;
    uint8_t getInterpretedPlayListCount() const;
    QList<Playlist> getPlaylistsV() const;
    QList<Playlist>& getPlaylistsR();
    bool hasPlaylists() const;
    int playlistCount() const;
    int totalPlaylistCount();

    void addInterpretedEffectBehavior(QString effectBehavior, int index = -1);
    void removeInterpretedEffectBehavior(int index);
    void copyInterpretedEffectBehavior(int indexFrom, int indexTo);
    void moveInterpretedEffectBehavior(int indexFrom, int indexTo);

    QByteArray rebuildRawFile();

private:
    void separateData();
    void interpretData();
    void deinterpretData();
    int recursiveSubPlaylistCounter(BSC::Playlist& parentPlaylist);

//-Inner Classes---------------------------------------------------------------------------------------
public:

    class Playlist
    {
        //-Class Types------------------------------------------------------------------------------------------------------
        public:
            enum PlaybackMode {Normal, Random, RandomPlaylist, Sequential, Invalid_PBM};
            enum Emitter {None, AlliedArtilleryTinitus, Arty_Tinitus, Authie_Planes, Bergetiger_Recovery, BigGun, BuildingAwareness, Causeway_Marsh,
                          Causeway_Music_Mixer, Decoy_Arty_Fire, DLC_BC_Mute, DLC_M02_Tiger_Mute, DLC_SP_Rulz, DLC_Tiger_Fire, DLC_Tiger_Trouble,
                          DLC_Under_Fire, DLC_VBB_Tiger_Mute, Engine_Rev_Gate, M01_MG42Volume, M01_SurfVolume, M02_GunsVolume, M02_InlandVolume,
                          OceanWater, P47_shot_down, RainSurface_Canvas, RainSurface_Cement, RainSurface_Foliage, RainSurface_Metal, RainSurface_Shingle,
                          RainSurface_Stone, RainSurface_Tin, RainSurface_Vehicle, RainSurface_Water, RainSurface_Wood, ReverbLarge, ReverbMedium, SFXRadio,
                          SFXRadio_Combat, SIR_Field, SIR_Forest, SIR_Narrow, SIR_Wide, SP_MP_overlap_control, StingerDuck, TankMixer, Tank_BG_radio, Tank_Shock,
                          Tiger_Radio_Fuzz, Tiger_Reload,Tinitus, TownVerb, UltraDecryption_Unmute, Vehicle_Gear_Shift, Vehicle_Rev, WaterCanal, Weapons,
                          Invalid_E};

        //-Class Variables
        public:
            static inline const QStringList PLAYBACK_MODE_STRINGS {"Normal", "Random", "RandomPlaylist", "Sequential"};
            static inline const QStringList MAX_INSTANCES_STRINGS {"Infinite", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten"};
            static inline const QStringList POLYPHONY_STRINGS {"One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten"};
            static inline const QStringList EMITTER_STRINGS {"<none>", "AlliedArtilleryTinitus", "Arty_Tinitus", "Authie_Planes", "Bergetiger_Recovery", "BigGun",
                                                            "BuildingAwareness", "Causeway_Marsh", "Causeway_Music_Mixer", "Decoy_Arty_Fire", "DLC_BC_Mute",
                                                            "DLC_M02_Tiger_Mute", "DLC_SP_Rulz", "DLC_Tiger_Fire", "DLC_Tiger_Trouble", "DLC_Under_Fire",
                                                            "DLC_VBB_Tiger_Mute", "Engine_Rev_Gate", "M01_MG42Volume", "M01_SurfVolume", "M02_GunsVolume",
                                                            "M02_InlandVolume", "OceanWater", "P47_shot_down", "RainSurface_Canvas", "RainSurface_Cement",
                                                            "RainSurface_Foliage", "RainSurface_Metal", "RainSurface_Shingle", "RainSurface_Stone", "RainSurface_Tin",
                                                            "RainSurface_Vehicle", "RainSurface_Water", "RainSurface_Wood", "ReverbLarge", "ReverbMedium", "SFXRadio",
                                                            "SFXRadio_Combat", "SIR_Field", "SIR_Forest", "SIR_Narrow", "SIR_Wide", "SP_MP_overlap_control",
                                                            "StingerDuck", "TankMixer", "Tank_BG_radio", "Tank_Shock", "Tiger_Radio_Fuzz", "Tiger_Reload,Tinitus",
                                                            "TownVerb", "UltraDecryption_Unmute", "Vehicle_Gear_Shift", "Vehicle_Rev", "WaterCanal", "Weapons"};

            static inline const QString EFFECT_BEHAVIOR_DEFAULT = "[NEW EFFECT BEHAVIOR]";
            static inline const QString PLAYLIST_INDEX_SEP = " - ";
            static inline const QString ROOT_INDEX_LABEL = "Root Playlist" + PLAYLIST_INDEX_SEP + "%1"; // Cannot be changed without changing PlaylistItemMap!
            static inline const QString SUB_INDEX_LABEL = "Sub-Playlist" + PLAYLIST_INDEX_SEP + "%1"; // Cannot be changed without changing PlaylistItemMap!
            static inline const QString SUB_INDEX_LABEL_SEP = ":";

            static const int L_SUB_PL_INDEX = 0x04;
            static const int L_PB_MODE_LEN = 0x04;
            static const int L_MAX_INST = 0x04;
            static const int L_POLYHONY = 0x04;
            static const int L_MIN_RT = 0x04;
            static const int L_MAX_RT = 0x04;
            static const int L_MIN_RC = 0x04;
            static const int L_MAX_RC = 0x04;
            static const int L_RAND_PLYLST_WEIGHT = 0x04;
            static const int L_LENGTH = 0x04;
            static const int L_RELEASE_TIME = 0x04;
            static const int L_EFF_BEH_NUM = 0x04;
            static const int L_EFF_BEH_NAME_LEN = 0x04;
            static const int L_OVERRIDE_PB = 0x01;
            static const int L_EMITTER_NAME_LEN = 0x04;
            static const int L_SND_CNTR_NUM = 0x04;
            static const int L_SUB_PLYLST_COUNT = 0x04;

        //-Instance Variables--------------------------------------------------------------------------------------------
        private:
            // UI Flags For QTreeWidget
            bool mInitiallyExpandedInTree = false;

            // Instance details
            bool mSubPlaylist;

            // Segmented Raw Data
            QByteArray mPlaylistIndexR; // Only used for sub-playlists
            QByteArray mPlaybackModeR;
            QByteArray mMaxInstancesR;
            QByteArray mPolyphonyR;
            QByteArray mMinRepeatTimeR;
            QByteArray mMaxRepeatTimeR;
            QByteArray mMinRepeatCountR;
            QByteArray mMaxRepeatCountR;
            QByteArray mRandomPlaylistWeightR; // May only be present for Random Playlist mode
            QByteArray mLengthR;
            QByteArray mReleaseTimeR;
            QByteArrayList mEffectBehaviorsR;
            QByteArray mOverrideParentBehaviorsR;
            QByteArray mEmitterR;

            // Segmented Interpreted Data
            //int mPlaylistIndexI; // Not needed, index can be infered from list index when writing
            PlaybackMode mPlaybackModeI;
            uint32_t mMaxInstancesI;
            uint32_t mPolyphonyI;
            uint32_t mMinRepeatTimeI;
            uint32_t mMaxRepeatTimeI;
            uint32_t mMinRepeatCountI;
            uint32_t mMaxRepeatCountI;
            uint32_t mRandomPlaylistWeightI; // May only be present for Random Playlist mode
            float mLengthI;
            float mReleaseTimeI;
            QStringList mEffectBehaviorsI;
            bool mOverrideParentBehaviorsI;
            Emitter mEmitterI;

            // Segmented Sub-section Data
            QList<SoundContainer> mSoundContainers;
            QList<Playlist> mSubPlaylists;

        //-Constructor-------------------------------------------------------------------------------------------------
        public:
            Playlist(int &parentDataCursor, QByteArray &parentRawData, bool subPlaylist);
            Playlist(bool subPlaylist);
            Playlist();

        //-Class Functions------------------------------------------------------------------------------------------------------
        public:

        //-Instance Functions---------------------------------------------------------------------------------------------------
        public:
            bool getInitialTreeExpansion() const;
            PlaybackMode getInterpretedPlaybackMode() const;
            uint32_t getInterpretedMaxInstances() const;
            uint32_t getInterpretedPolyphony() const;
            uint32_t getInterpretedMinRepeatTime() const;
            uint32_t getInterpretedMaxRepeatTime() const;
            uint32_t getInterpretedMinRepeatCount() const;
            uint32_t getInterpretedMaxRepeatCount() const;
            uint32_t getInterpretedRandomPlaylistWeight() const;
            float getInterpretedLength() const;
            float getInterpretedReleaseTime() const;
            QStringList getInterpretedEffectBehaviorsV() const;
            QStringList& getInterpretedEffectBehaviorsR();
            bool getInterpretedOverrideParentBehaviors() const;
            Emitter getInterpretedEmitter() const;
            QList<SoundContainer> getSoundContainersV() const;
            QList<SoundContainer>& getSoundContainersR();
            QList<Playlist> getSubPlaylistsV() const;
            QList<Playlist>& getSubPlaylistsR();
            bool hasSoundContainers() const;
            bool hasSubPlaylists() const;
            bool hasEffectBehaviors() const;
            int soundContainerCount() const;
            int subPlaylistCount() const;
            int effectBehaviorCount() const;

            void setInitialTreeExpansion(bool expanded);
            void setInterpretedPlaybackMode(PlaybackMode playbackMode);
            void setInterpretedMaxInstances(uint32_t maxInstances);
            void setInterpretedPolyphony(uint32_t polphony);
            void setInterpretedMinRepeatTime(uint32_t minRepeatTime);
            void setInterpretedMaxRepeatTime(uint32_t maxRepeatTime);
            void setInterpretedMinRepeatCount(uint32_t minRpeatCount);
            void setInterpretedMaxRepeatCount(uint32_t maxRepeatCount);
            void setInterpretedRandomPlaylistWeight(uint32_t randomPlaylistWeight);
            void setInterpretedLength(float length);
            void setInterpretedReleaseTime(float releaseTime);
            void setInterpretedOverrideParentBehaviors(bool overrideParentBehaviors);
            void setInterpretedEmitter(Emitter emitter);

            void addInterpretedEffectBehavior(QString effectBehavior, int index = -1);
            void removeInterpretedEffectBehavior(int index);
            void copyInterpretedEffectBehavior(int indexFrom, int indexTo);
            void moveInterpretedEffectBehavior(int indexFrom, int indexTo);
            void addSoundContainer(SoundContainer container, int index = -1);
            void removeSoundContainer(int index);
            void copySoundContainer(int indexFrom, int indexTo);
            void moveSoundContainer(int indexFrom, int indexTo);

            void deinterpretData();
            QByteArray getRestitchedData();

        private:
            void defaultInitialization();
            void separateData(int &dataCursor, QByteArray &rawData);
            void interpretData();
            PlaybackMode rawPBModeToEnum(QByteArray rawPBMode);
            QByteArray enumPBModeToRaw(PlaybackMode enumPBMode);
            Emitter rawEmitterToEnum(QByteArray rawEmitter);
            QByteArray enumEmitterToRaw(Emitter enumEmitter);
    };


    class SoundContainer
    {
    //-Class Types------------------------------------------------------------------------------------------------------
    public:
        enum ReasonableSCType { RES_INT, RES_FLOAT };

    //-Class Variables
    public:
        static inline const QStringList VOICE_GROUP_FRONT_STRINGS {"Default", "Ambience", "Foley", "Hits", "Vehicles", "Weapon"};
        static inline const QList<QStringList> VOICE_GROUP_BACK_STRINGS
        {
            {"Default"},
            {"Ambience_Hi", "Ambience_Lo"},
            {"BuildingFoley", "Footfalls", "GunFoley", "SpecialFX", "VehicleFoley"},
            {"BigHits", "Explosions", "LittleHits", "MediumHits"},
            {"Engines", "Engines_LoPriority", "FOWEngines", "VehicleDestroys"},
            {"MGBelts", "MGsHeavy", "MGsSmall", "WeaponsHeavy", "WeaponsMedium", "WeaponsSmall"}
        };

        static inline const QString LINKED_FILE_DEFAULT = "[NEW FILE LINK]";
        static inline const QString INDEX_LABEL = "Container - %1";

        static inline const QString VOICE_GROU_SEP = "::";

        static const int RES_SC_INT_CUTOFF = 0xFF; // 16-bit limit - Int: 65,535 Float: 9.18340948595269E-41 // Extremely high for an int in the context of this program,
                                                   // but extremely low for a float in the context of this program, so this is a good dividing line

        static const int L_CNTR_INDEX = 0x04;
        static const int L_VOICE_GRP_NAM_LEN = 0x04;
        static const int L_IS_3D = 0x01;
        static const int L_IS_LOOPED = 0x01;
        static const int L_USE_SV_TRIG = 0x01;
        static const int L_RAND_WEIGHT = 0x04;
        static const int L_AMP_FACTOR = 0x04;
        static const int L_DOP_RATIO = 0x04;
        static const int L_PLAY_FREQ = 0x04;
        static const int L_START = 0x04;
        static const int L_END = 0x04;
        static const int L_MSK_SAMP = 0x04;
        static const int L_LNKD_FILE_NUM = 0x01;
        static const int L_LNKD_FILE_NAM_LEN = 0x04;
        static const int L_EFF_CONT_NUM = 0x04;

    //-Instance Variables--------------------------------------------------------------------------------------------
    private:
        // Segmented Raw Data
        QByteArray mContainerIndexR;
        QByteArray mVoiceGroupR;
        QByteArray mIs3DR;
        QByteArray mIsLoopedR;
        QByteArray mUsingSpeedVolumeTriggerR;
        QByteArray mRandomWeightR;
        QByteArray mAmplitudeFactorR;
        QByteArray mDopplerRatioR;
        QByteArray mPlaybackFrequenceR;
        QByteArray mStartR;
        QByteArray mEndR;
        QByteArray mMaskSampleR;
        QByteArrayList mLinkedFilesR;
        QByteArray mEffectContainerCountR;
        QByteArray mEffectContainerDataR;

        // Segmented Interpreted Data
        // int mContainerIndexI; // Not needed, index can be infered from list index when writing
        QString mVoiceGroupFrontI;
        QString mVoiceGroupBackI;
        bool mIs3DI;
        bool mIsLoopedI;
        bool mUsingSpeedVolumeTriggerI;
        uint32_t mRandomWeightI;
        float mAmplitudeFactorI;
        float mDopplerRatioI;
        float mPlaybackFrequenceI;
        float mStartI;
        float mEndI;
        float mMaskSampleI;
        QStringList mLinkedFilesI;
        uint32_t mEffectContainerCountI;
        QString mEffectContainerDataI;

    //-Constructor-------------------------------------------------------------------------------------------------
    public:
        SoundContainer(int &parentDataCursor, QByteArray &parentRawData, bool lastSCInPL);
        SoundContainer();

    //-Instance Functions---------------------------------------------------------------------------------------------------
    public:
        //int getInterpretedContainerIndex() const; // Not needed, index can be infered from list index when writing
        QString getInterpretedVoiceGroupFront() const;
        QString getInterpretedVoiceGroupBack() const;
        bool getInterpretedIs3D() const;
        bool getInterpretedIsLooped() const;
        bool getInterpretedUsingSpeedVolumeTrigger() const;
        uint32_t getInterpretedRandomWeight() const;
        float getInterpretedAmplitudeFactor() const;
        float getInterpretedDopplerRatio() const;
        float getInterpretedPlaybackFrequence() const;
        float getInterpretedStart() const;
        float getInterpretedEnd() const;
        float getInterpretedMaskSample() const;
        QStringList getInterpretedLinkedFilesV() const;
        QStringList& getInterpretedLinkedFilesR();
        uint32_t getInterpretedEffectContainerCount() const;
        QString getInterpretedEffectContainerData() const;
        bool hasLinkedFiles() const;
        int linkedFileCount() const;

        void setInterpretedVoiceGroupFront(QString voiceGroup);
        void setInterpretedVoiceGroupBack(QString voiceGroup);
        void setInterpretedIs3D(bool is3D);
        void setInterpretedIsLooped(bool isLopped);
        void setInterpretedUsingSpeedVolumeTrigger(bool usingSpeedVolumeTrigger);
        void setInterpretedRandomWeight(uint32_t randomWeight);
        void setInterpretedAmplitudeFactor(float amplitudeFactor);
        void setInterpretedDopplerRatio(float dopplerRatio);
        void setInterpretedPlaybackFrequence(float playbackFrequence);
        void setInterpretedStart(float start);
        void setInterpretedEnd(float end);
        void setInterpretedMaskSample(float maskSample);
        void setInterpretedEffectContainerCount(uint32_t effectContainerCount);
        void setInterpretedEffectContainerData(QString effectContainerData);

        void addInterpretedLinkedFile(QString linkedFile, int index = -1);
        void removeInterpretedLinkedFile(int index);
        void copyInterpretedLinkedFile(int indexFrom, int indexTo);
        void moveInterpretedLinkedFile(int indexFrom, int indexTo);

        void deinterpretData();
        QByteArray getRestitchedData();

    private:
        void separateData(int &dataCursor, QByteArray &rawData, bool lastSCInPL);
        void interpretData();
        ReasonableSCType determineReasonableSCDataType(QByteArray rawVal);
        int findEffectContainerEndMidSC(QByteArray &fullData, int startOffset);
        int findEffectContainerEndLastSC(QByteArray &fullData, int startOffset);
    };
};

#endif // BSC_H
