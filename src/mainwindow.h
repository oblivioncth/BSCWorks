#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bsc.h"
#include "smc.h"
#include "smf.h"
#include "wav.h"

#include <QMainWindow>
#include <QTreeWidget>
#include <QTableWidget>
#include <QDir>
#include <QLabel>
#include "version.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT // Required for classes that use Qt elements

//-Class Variables----------------------------------------------------------------------------------------------
private:
    static inline const QString MENU_FILE_FILTER = "Burnt Sound Container (*." + BSC::FILE_EXT + ")";
    static inline const QString MENU_SAVE_AS_TITLE = "Save As";
    static inline const QString MENU_SAVE_TITLE = "Save";
    static inline const QString MENU_OPEN_TITLE = "Open";
    static inline const QString MSG_SAVE_SUCCESS = "File ""%1"" was saved successfully.";
    static inline const QString MSG_SAVE_FAIL = "Error saving ""%1""!";
    static inline const QString MSG_SAVE_PROMPT_TXT = "The currently open file has unsaved changes.";
    static inline const QString MSG_SAVE_PROMPT_EXIT_INFO_TXT = "Do you want to save the file before exiting?";
    static inline const QString MSG_SAVE_PROMPT_ELSE_INFO_TXT = "Do you want to save the file before proceeding?";
    static inline const QString MSG_OPEN_FILE_INVALID = """%1"" is not a valid BSC!";
    static inline const QString MSG_EFF_CON_EDIT_WARN_TXT = "Caution! Editing the Effect Container region of Sound Containers directly can render the Entire "
                                                        "BSC inoperable if performed incorrectly. Only edit this section if you know what you're doing.";
    static inline const QString MSG_EFF_CON_EDIT_WARN_INFO_TXT = "Effect Container count must be 1 or greater before any data can be changed.";

    static inline const QString MENU_CACHE_FILE_FILTER = "Speechmanager Cache (*." + SMC::FILE_EXT + ")";
    static inline const QString MENU_CACHE_OPEN_TITLE = "Select Speechmanager Cache";
    static inline const QString MENU_CACHE_FOLDER_TITLE = "Select folder to scan for BSC files";
    static inline const QString MSG_CACHE_INVALID = """%1"" is not a valid Speechmanager Cache!";
    static inline const QString MSG_CACHE_HAS_DUPES = "The selected Speechmanager contains duplicate entries that will be removed if you proceed. These entries are redundant"
                                                      " and keeping them serves no purpose.";
    static inline const QString MSG_CACHE_FOLDER_SUB_PROMPT = "Include sub-directories?";
    static inline const QString MSG_CACHE_UPDATED_TXT = "The selected Speechmanager Cache was updated succesfully.";
    static inline const QString MSG_CACHE_UPDATED_INFO_TXT = "Some detected files were already present in the cache and were skipped.";
    static inline const QString MSG_CACHE_UP_TO_DATE = "The selected cache is already up-to-date with this directory. No changes were necessary.";
    static inline const QString MSG_CACHE_FAIL = "Error writting ""%1""";
    static inline const QString MSG_CACHE_INV_FILES_INFO = "Some files were skipped because they did not appear to be valid BSCs. See details below.";
    static inline const QString MSG_CACHE_INV_FILES_DETAILS = "The following files appeared to be invalid and were skipped:";
    static inline const QString MSG_CACHE_INV_FILES_CAT_BAD = "(Invalid data)";
    static inline const QString MSG_CACHE_INV_FILES_CAT_SPACE = "(Space in filename)";

    static inline const QString MENU_CACHE_NEW_TITLE = "Enter Speechmanager Cache Destina;tion";
    static inline const QString MSG_CACHE_NEW_SUCCESS_TXT = "The Speechmanager Cache was created successfully.";

    static inline const QString MSG_CACHE_SCAN_IN_PROGRESS = "Scanning BSC files...";
    static inline const QString MSG_CACHE_SCAN_ABORT = "Abort";
    static inline const QString MSG_CACHE_SCAN_ABORTED = "Speechmanager cache update aborted by user.";

    static inline const QString MENU_SMF_FILE_FILTER = "SMF Audio (*." + SMF::FILE_EXT + ")";
    static inline const QString MENU_WAV_FILE_FILTER = "WAVE Audio (*." + WAV::FILE_EXT + ")";

    static inline const QString MENU_CONV_SMF_IN_TITLE = "Select SMF to convert";
    static inline const QString MENU_CONV_SMF_OUT_TITLE = "Enter output WAV destination";
    static inline const QString MSG_CONV_SMF_INVALID = """%1"" is not a valid SMF file!";
    static inline const QString MSG_CONV_SMF_SUCCESS = "Conversion completed successfully.";
    static inline const QString MSG_CONV_SMF_FAIL = "Error writting ""%1""";

    static inline const QString MENU_CONV_WAV_IN_TITLE = "Select WAV to convert";
    static inline const QString MENU_CONV_WAV_OUT_TITLE = "Enter output SMF destination";
    static inline const QString MSG_CONV_WAV_INVALID = """%1"" is not a valid WAVE file!";
    static inline const QString MSG_CONV_WAV_SUCCESS = "Conversion completed successfully.";
    static inline const QString MSG_CONV_WAV_FAIL = "Error writting ""%1""";

    static inline const QString MSG_BATCH_ALL_FILE_EXST = """%1"" already exists. Do you want to overwrite this file?";
    static inline const QString MSG_BATCH_ALL_ABORT = "Abort Conversion";
    static inline const QString MSG_BATCH_ALL_ABORTED_TXT = "Batch conversion aborted by user.";
    static inline const QString MSG_BATCH_ALL_ABORTED_INFO = "%1/%2 files were processed.";

    static inline const QString MENU_BATCH_SMF_IN_TITLE = "Select folder to scan for SMF files";
    static inline const QString MENU_BATCH_SMF_OUT_TITLE = "Select output folder";
    static inline const QString MSG_BATCH_SMF_SUB_PROMPT = "Include sub-directories?";
    static inline const QString MSG_BATCH_SMF_NO_FILES = "No SMF files found in %1";
    static inline const QString MSG_BATCH_SMF_NO_VALID_FILES_TXT = "No valid/convertable SMF files found in %1";
    static inline const QString MSG_BATCH_SMF_NO_VALID_FILES_INFO = "All present SMF files are corrupt, or experienced IO errors during conversion!";
    static inline const QString MSG_BATCH_SMF_SUCCESS_TXT = "Batch SMF conversion completed successfully.";
    static inline const QString MSG_BATCH_SMF_INV_FILES_INFO = "Some files were skipped due to read/write issues. See details below";
    static inline const QString MSG_BATCH_SMF_INV_FILES_DETAILS  = "The following files were skipped:";
    static inline const QString MSG_BATCH_SMF_INV_FILES_CAT_BAD = "(Invalid data)";
    static inline const QString MSG_BATCH_SMF_IN_PROGRESS = "Converting SMF files to WAV...";

    static inline const QString MENU_BATCH_WAV_IN_TITLE = "Select folder to scan for WAV files";
    static inline const QString MENU_BATCH_WAV_OUT_TITLE = "Select output folder";
    static inline const QString MSG_BATCH_WAV_SUB_PROMPT = "Include sub-directories?";
    static inline const QString MSG_BATCH_WAV_NO_FILES = "No WAV files found in %1";
    static inline const QString MSG_BATCH_WAV_NO_VALID_FILES_TXT = "No valid/convertable WAV files found in %1";
    static inline const QString MSG_BATCH_WAV_NO_VALID_FILES_INFO = "All present WAV files are corrupt, or experienced IO errors during conversion!";
    static inline const QString MSG_BATCH_WAV_SUCCESS_TXT = "Batch WAV conversion completed successfully.";
    static inline const QString MSG_BATCH_WAV_INV_FILES_INFO = "Some files were skipped due to read/write issues. See details below";
    static inline const QString MSG_BATCH_WAV_INV_FILES_DETAILS = "The following files were skipped:";
    static inline const QString MSG_BATCH_WAV_INV_FILES_CAT_BAD = "(Invalid data)";
    static inline const QString MSG_BATCH_WAV_IN_PROGRESS = "Converting WAV files to SMF...";

    static inline const QString MSG_ABOUT_TXT = "BSCWorks " + QString::fromStdString(VER_PRODUCTVERSION_STR) + " @oblivioncth";
    static inline const QString MSG_ABOUT_INFO_TXT = "Distrubted under the GNU General Public License V3.0. See " + QString::fromStdString(VER_COMPANYDOMAIN_STR) + "/BSCWorks for more information.";

    static inline const QString STATUS_BAR_NEW_FILE = "[NEW UNSAVED FILE]";
    static const int STATUS_BAR_INDENT = 10;

//-Instance Variables--------------------------------------------------------------------------------------------
private:
    Ui::MainWindow *ui; 
    QHash<QWidget*,std::function<bool(void)>> mWidgetEnableConditionMap;
    QHash<QTreeWidgetItem, bool> mTreeWidgetItemExpandedState;
    std::unique_ptr<BSC> mBSCPtr;
    bool mSlotsEnabled = false;
    bool mECLoopProtection = false;
    bool mAlteringSoundContainerBoxEntries = false;
    bool mFileOpen = false;
    bool mAllChangesSaved = true;
    bool mGaveEffectContainerWarning = false;
    QString mCurrentFileDir = QDir::currentPath();
    QString mCurrentFileName = "";

    //UI Enable Groups
    QWidgetList mEnableGroupTopLevel;
    QWidgetList mEnableGroupPlaylistSel;
    QWidgetList mEnableGroupPlaylistSet;
    QWidgetList mEnableGroupEffectBeh;
    QWidgetList mEnableGroupSoundCon;

    // Slot flags
    bool mEffConRawInputGuaranteedValid = false;

    // Status bar
    QLabel* label_statusBarCurrentFile;

//-Constructor---------------------------------------------------------------------------------------------------
public:
    explicit MainWindow(QWidget *parent = nullptr);

//-Destructor----------------------------------------------------------------------------------------------------
public:
    ~MainWindow();

//-Instance Functions--------------------------------------------------------------------------------------------
private:
    void initializeForms();
    void initializeWidgetEnableConditionMap();
    void initializeEnableWidgetGroups();
    void updatePrimaryWidgetStates();
    void updateSecondaryWidgetStates(QWidgetList widgetsToUpdate);
    void recursivePlaylistEnumerator(QTreeWidgetItem* parentTreeItem, BSC::Playlist &parentPlaylist, QString parentIndexMarker); // Only reads playlists so a copy is fine (2nd arg)
    void updateBSCTopLevel();
    void updatePlaylists();
    void updateEntirePlaylist();
    void updateEffectBehaviors();
    void updateSoundContainers();
    void updateSoundContainerGroup();
    void updateVoiceGroupBack(int frontIndex);
    void updateLinkedFiles();
    void updateStatusBar();
    void setChangesSavedState(bool changesSaved);
    bool saveChangesPrompt(bool exit);
    void openBSCFile(QFile& bscFile);
    BSC::Playlist* getSelectedPlaylist() const;
    BSC::SoundContainer* getSelectedSoundContainer() const;
    void storeTreeItemExpandedStates() const;
    void cacheSubRoutine(bool existing);
    void execIOReport(Qx::IO::IOOpReport ioReport);

//-Overrides----------------------------------------------------------------------------------------------------
private:
    void closeEvent (QCloseEvent* event);
    bool eventFilter(QObject* target, QEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

//-Slots---------------------------------------------------------------------------------------------------------
private slots: // Start with "all" to avoid Qt calling "connectSlotsByName" on these slots (slots that start with "on_")
    void all_on_menuAction_triggered();
    void all_on_treeWidget_currentItemChanged();
    void all_on_tableWidget_itemChanged(QTableWidgetItem* item);
    void all_on_tableWidget_currentItemChanged();
    void all_on_linedEdit_textEdited();
    void all_on_comboBox_currentIndexChanged(int newIndex);
    void all_on_spinBox_valueChanged(int newValue);
    void all_on_doubleSpinBox_valueChanged(double newValue);
    void all_on_checkBox_stateChanged(int newState);
    void all_on_toolButton_clicked();
    void all_on_plainTextEdit_textChanged();

//-Inner Classes---------------------------------------------------------------------------------------
private:
    class PlaylistItemMap
    {
        //-Instance Variables--------------------------------------------------------------------------------------------
        private:
            const QTreeWidgetItem* mOriginalTreeItem;
            QVector<int> mIndexPath;
            QList<BSC::Playlist*> mPlaylistPath;
            QList<QList<BSC::Playlist>*> mPlaylistListPath;

        //-Constructor---------------------------------------------------------------------------------------------------
        public:
            PlaylistItemMap(const QTreeWidgetItem* treeItem, BSC* headBSC);

        //-Class Functions-----------------------------------------------------------------------------------------------
        public:
            static QTreeWidgetItem* getItemRelativeToMap(QTreeWidget* treeWidget, const PlaylistItemMap itemMap, int depth, int target);

        //-Instance Functions--------------------------------------------------------------------------------------------
        public:
            const QTreeWidgetItem* getOriginalTreeItem() const;
            int getIndexAtLevel(int level) const;
            BSC::Playlist* getPlaylistAtLevel(int level) const;
            QList<BSC::Playlist>* getPlaylistListAtLevel(int level) const;
            int getTopLevelIndex() const;
            BSC::Playlist* getTopLeveLPlaylist() const;
            QList<BSC::Playlist>* getTopLevelPlaylistList() const;
            int getTargetIndex() const;
            BSC::Playlist* getTargetPlaylist() const;
            QList<BSC::Playlist>* getTargetPlaylistList() const;
            int getTargetParentIndex() const;
            BSC::Playlist* getTargetParentPlaylist() const;
            QList<BSC::Playlist>* getTargetParentPlaylistList() const;
            int targetDepth() const;
            bool targetIsTopLevel() const;
            int length() const;
    };
};

#endif // MAINWINDOW_H
