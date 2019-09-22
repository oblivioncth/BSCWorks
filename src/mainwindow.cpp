#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QtEndian>
#include <QDebug>
#include <QScrollBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QProgressDialog>
#include <QMimeData>

//===============================================================================================================
// MAIN WINDOW
//===============================================================================================================

//-Constructor---------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Initialize UI
    ui->setupUi(this);
    QApplication::setApplicationName(VER_PRODUCTNAME_STR);
    setWindowTitle(VER_PRODUCTNAME_STR);
    setAcceptDrops(true);
    initializeForms();
    initializeWidgetEnableConditionMap();
    initializeEnableWidgetGroups();
    mSlotsEnabled = true;
}

//-Destructor----------------------------------------------------------------------------------------------------
MainWindow::~MainWindow() { delete ui; }

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
void MainWindow::initializeForms()
{
    // Populate constant input widget data
    ui->comboBox_playbackMode->addItems(BSC::Playlist::PLAYBACK_MODE_STRINGS);
    //ui->comboBox_playbackMode->setLayoutDirection(Qt::RightToLeft);
    ui->comboBox_maxInst->addItems(BSC::Playlist::MAX_INSTANCES_STRINGS);
    ui->comboBox_poly->addItems(BSC::Playlist::POLYPHONY_STRINGS);
    ui->comboBox_emitter->addItems(BSC::Playlist::EMITTER_STRINGS);
    ui->treeWidget_playlists->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui->comboBox_voiceGroupFront->addItems(BSC::SoundContainer::VOICE_GROUP_FRONT_STRINGS);
    ui->lineEdit_effectContainerCount->setValidator( new QIntValidator(0, 999, ui->lineEdit_effectContainerCount) ); // Limit count line edit to numbers only
    ui->treeWidget_playlists->setColumnCount(1);

    // Link plain text scroll bars
    connect(ui->plainTextEdit_effectContainerDataRaw->verticalScrollBar(), &QScrollBar::sliderMoved,
            ui->plainTextEdit_effectContainerDataString->verticalScrollBar(), &QScrollBar::setValue);
    connect(ui->plainTextEdit_effectContainerDataString->verticalScrollBar(), &QScrollBar::sliderMoved,
            ui->plainTextEdit_effectContainerDataRaw->verticalScrollBar(), &QScrollBar::setValue);

    // Set table properties
    ui->tableWidget_linkedFiles->setColumnCount(1);
    ui->tableWidget_linkedFiles->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Disable window resizing
    this->setFixedSize(QSize(this->width(), this->height()));
    this->setMaximumSize(QSize(this->width(), this->height()));
    this->setMinimumSize(QSize(this->width(), this->height()));
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint); // Forces resize grip to not show (WINDOWS DEPENDENT)

    // Install event filter for QPlainTextEdito
    ui->plainTextEdit_effectContainerDataRaw->installEventFilter(this);

    // Initially hide widgets
    updatePrimaryWidgetStates();
}

void MainWindow::initializeWidgetEnableConditionMap()
{
    // Populate hashmap of ui element enable conditions
    mWidgetEnableConditionMap.insert(ui->checkBox_3D, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->checkBox_loop, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->checkBox_speedVolumeTrigger, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->checkBox_effectContainerDataEdit, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->checkBox_parentBehaviorOverride, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->comboBox_emitter, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->comboBox_maxInst, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->comboBox_playbackMode, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->comboBox_poly, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->comboBox_soundContainer, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->comboBox_voiceGroupFront, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->comboBox_voiceGroupBack, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->doubleSpinBox_amplitudeFactor, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->doubleSpinBox_dopplerRatio, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->doubleSpinBox_end, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->doubleSpinBox_length, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->doubleSpinBox_maskSample, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->doubleSpinBox_playbackFrequence, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->doubleSpinBox_releaseTime, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->doubleSpinBox_start, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->lineEdit_tweener, [&]()->bool{ return true;});
    mWidgetEnableConditionMap.insert(ui->lineEdit_formatVer, [&]()->bool{ return true;});
    mWidgetEnableConditionMap.insert(ui->lineEdit_playlistCount, [&]()->bool{ return true;});
    mWidgetEnableConditionMap.insert(ui->lineEdit_effectContainerCount, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->plainTextEdit_effectContainerDataRaw, [&]()->bool{ return (mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                            getSelectedSoundContainer()->getInterpretedEffectContainerCount() > 0);});
    mWidgetEnableConditionMap.insert(ui->plainTextEdit_effectContainerDataString, [&]()->bool{ return (mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                               getSelectedSoundContainer()->getInterpretedEffectContainerCount() > 0);});
    mWidgetEnableConditionMap.insert(ui->spinBox_randomPlaylistWeight, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->spinBox_randomWeight, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->spinBox_repeatCountMax, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->spinBox_repeatCountMin, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->spinBox_repeatTimeMax, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->spinBox_repeatTimeMin, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->tableWidget_effectBehaviors, [&]()->bool{ return mBSCPtr->hasPlaylists()  && getSelectedPlaylist()->hasEffectBehaviors();});
    mWidgetEnableConditionMap.insert(ui->tableWidget_linkedFiles, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                               getSelectedSoundContainer()->hasLinkedFiles();});
    mWidgetEnableConditionMap.insert(ui->toolButton_effectBehaviorsAdd, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->toolButton_effectBehaviorsCopy, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasEffectBehaviors() &&
                                                                                      ui->tableWidget_effectBehaviors->currentRow() > -1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_effectBehaviorsDown, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasEffectBehaviors() &&
                                                                                      ui->tableWidget_effectBehaviors->currentRow() > -1 &&
                                                                                      ui->tableWidget_effectBehaviors->currentRow() < getSelectedPlaylist()->effectBehaviorCount() - 1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_effectBehaviorsRemove, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasEffectBehaviors() &&
                                                                                        ui->tableWidget_effectBehaviors->currentRow() > -1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_effectBehaviorsUp, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasEffectBehaviors() &&
                                                                                    ui->tableWidget_effectBehaviors->currentRow() > 0;});
    mWidgetEnableConditionMap.insert(ui->toolButton_linkedFilesAdd, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->toolButton_linkedFilesCopy, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                  getSelectedSoundContainer()->hasLinkedFiles() &&
                                                                                   ui->tableWidget_linkedFiles->currentRow() > -1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_linkedFilesDown, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                  getSelectedSoundContainer()->hasLinkedFiles() &&
                                                                                  ui->tableWidget_linkedFiles->currentRow() > -1 &&
                                                                                  ui->tableWidget_linkedFiles->currentRow() < getSelectedSoundContainer()->linkedFileCount() - 1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_linkedFilesRemove, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                    getSelectedSoundContainer()->hasLinkedFiles() &&
                                                                                    ui->tableWidget_linkedFiles->currentRow() > -1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_linkedFilesUp, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                getSelectedSoundContainer()->hasLinkedFiles() &&
                                                                                ui->tableWidget_linkedFiles->currentRow() > 0;});
    mWidgetEnableConditionMap.insert(ui->toolButton_playlistsAdd, [&]()->bool{ return true;});
    mWidgetEnableConditionMap.insert(ui->toolButton_playlistsCopy, [&]()->bool{ return mBSCPtr->hasPlaylists() && ui->treeWidget_playlists->currentItem() != nullptr;});
    mWidgetEnableConditionMap.insert(ui->toolButton_playlistsDown, [&]()->bool{ return mBSCPtr->hasPlaylists() && ui->treeWidget_playlists->currentItem() != nullptr &&
                                                                                PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get()).getTargetIndex() <
                                                                                PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get()).getTargetPlaylistList()->length() - 1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_playlistsIn, [&]()->bool{ return mBSCPtr->hasPlaylists() && ui->treeWidget_playlists->currentItem() != nullptr &&
                                                                              PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get()).getTargetIndex() > 0;});
    mWidgetEnableConditionMap.insert(ui->toolButton_playlistsOut, [&]()->bool{ return mBSCPtr->hasPlaylists() && ui->treeWidget_playlists->currentItem() != nullptr &&
                                                                               !PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get()).targetIsTopLevel();});
    mWidgetEnableConditionMap.insert(ui->toolButton_playlistsRemove, [&]()->bool{ return mBSCPtr->hasPlaylists() && ui->treeWidget_playlists->currentItem() != nullptr;});
    mWidgetEnableConditionMap.insert(ui->toolButton_playlistsUp, [&]()->bool{ return mBSCPtr->hasPlaylists() && ui->treeWidget_playlists->currentItem() != nullptr &&
                                                                              PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get()).getTargetIndex() > 0;});
    mWidgetEnableConditionMap.insert(ui->toolButton_soundContainerAdd, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->toolButton_soundContainerCopy, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                     ui->comboBox_soundContainer->currentIndex() > -1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_soundContainerDown, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                     ui->comboBox_soundContainer->currentIndex() > -1 &&
                                                                                     ui->comboBox_soundContainer->currentIndex() < getSelectedPlaylist()->soundContainerCount() - 1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_soundContainerRemove, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                       ui->comboBox_soundContainer->currentIndex() > -1;});
    mWidgetEnableConditionMap.insert(ui->toolButton_soundContainerUp, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                   ui->comboBox_soundContainer->currentIndex() > 0;});
    mWidgetEnableConditionMap.insert(ui->treeWidget_playlists, [&]()->bool{ return mBSCPtr->hasPlaylists();});
    mWidgetEnableConditionMap.insert(ui->label_voiceGroup, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_voiceGroupSep, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_randomWeight, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_amplitudeFactor, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_start, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_playbackFrequence, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_dopplerRatio, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_end, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_maskSample, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_effectContainerCount, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers();});
    mWidgetEnableConditionMap.insert(ui->label_linkedFiles, [&]()->bool{ return mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                         getSelectedSoundContainer()->hasLinkedFiles();});
    mWidgetEnableConditionMap.insert(ui->label_effectContainerDataRaw, [&]()->bool{ return (mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                    getSelectedSoundContainer()->getInterpretedEffectContainerCount() > 0);});
    mWidgetEnableConditionMap.insert(ui->label_effectContainerDataString, [&]()->bool{ return (mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers() &&
                                                                                       getSelectedSoundContainer()->getInterpretedEffectContainerCount() > 0);});
}

void MainWindow::initializeEnableWidgetGroups()
{
    mEnableGroupTopLevel.append(ui->lineEdit_tweener);
    mEnableGroupTopLevel.append(ui->lineEdit_formatVer);
    mEnableGroupTopLevel.append(ui->lineEdit_playlistCount);

    mEnableGroupPlaylistSel.append(ui->treeWidget_playlists);
    mEnableGroupPlaylistSel.append(ui->toolButton_playlistsAdd);
    mEnableGroupPlaylistSel.append(ui->toolButton_playlistsRemove);
    mEnableGroupPlaylistSel.append(ui->toolButton_playlistsCopy);
    mEnableGroupPlaylistSel.append(ui->toolButton_playlistsOut);
    mEnableGroupPlaylistSel.append(ui->toolButton_playlistsIn);
    mEnableGroupPlaylistSel.append(ui->toolButton_playlistsUp);
    mEnableGroupPlaylistSel.append(ui->toolButton_playlistsDown);

    mEnableGroupPlaylistSet.append(ui->comboBox_playbackMode);
    mEnableGroupPlaylistSet.append(ui->comboBox_maxInst);
    mEnableGroupPlaylistSet.append(ui->comboBox_poly);
    mEnableGroupPlaylistSet.append(ui->spinBox_repeatTimeMin);
    mEnableGroupPlaylistSet.append(ui->spinBox_repeatTimeMax);
    mEnableGroupPlaylistSet.append(ui->spinBox_repeatCountMin);
    mEnableGroupPlaylistSet.append(ui->spinBox_repeatCountMax);
    mEnableGroupPlaylistSet.append(ui->spinBox_randomPlaylistWeight);
    mEnableGroupPlaylistSet.append(ui->doubleSpinBox_length);
    mEnableGroupPlaylistSet.append(ui->doubleSpinBox_releaseTime);

    mEnableGroupEffectBeh.append(ui->tableWidget_effectBehaviors);
    mEnableGroupEffectBeh.append(ui->toolButton_effectBehaviorsAdd);
    mEnableGroupEffectBeh.append(ui->toolButton_effectBehaviorsRemove);
    mEnableGroupEffectBeh.append(ui->toolButton_effectBehaviorsCopy);
    mEnableGroupEffectBeh.append(ui->toolButton_effectBehaviorsUp);
    mEnableGroupEffectBeh.append(ui->toolButton_effectBehaviorsDown);
    mEnableGroupEffectBeh.append(ui->checkBox_parentBehaviorOverride);
    mEnableGroupEffectBeh.append(ui->comboBox_emitter);

    mEnableGroupSoundCon.append(ui->comboBox_soundContainer);
    mEnableGroupSoundCon.append(ui->toolButton_soundContainerAdd);
    mEnableGroupSoundCon.append(ui->toolButton_soundContainerRemove);
    mEnableGroupSoundCon.append(ui->toolButton_soundContainerCopy);
    mEnableGroupSoundCon.append(ui->toolButton_soundContainerUp);
    mEnableGroupSoundCon.append(ui->toolButton_soundContainerDown);
    mEnableGroupSoundCon.append(ui->comboBox_voiceGroupFront);
    mEnableGroupSoundCon.append(ui->comboBox_voiceGroupBack);
    mEnableGroupSoundCon.append(ui->checkBox_3D);
    mEnableGroupSoundCon.append(ui->checkBox_loop);
    mEnableGroupSoundCon.append(ui->checkBox_speedVolumeTrigger);
    mEnableGroupSoundCon.append(ui->spinBox_randomWeight);
    mEnableGroupSoundCon.append(ui->doubleSpinBox_amplitudeFactor);
    mEnableGroupSoundCon.append(ui->doubleSpinBox_dopplerRatio);
    mEnableGroupSoundCon.append(ui->doubleSpinBox_start);
    mEnableGroupSoundCon.append(ui->doubleSpinBox_end);
    mEnableGroupSoundCon.append(ui->doubleSpinBox_playbackFrequence);
    mEnableGroupSoundCon.append(ui->doubleSpinBox_maskSample);
    mEnableGroupSoundCon.append(ui->lineEdit_effectContainerCount);
    mEnableGroupSoundCon.append(ui->checkBox_effectContainerDataEdit);
    mEnableGroupSoundCon.append(ui->tableWidget_linkedFiles);
    mEnableGroupSoundCon.append(ui->toolButton_linkedFilesAdd);
    mEnableGroupSoundCon.append(ui->toolButton_linkedFilesRemove);
    mEnableGroupSoundCon.append(ui->toolButton_linkedFilesCopy);
    mEnableGroupSoundCon.append(ui->toolButton_linkedFilesUp);
    mEnableGroupSoundCon.append(ui->toolButton_linkedFilesDown);
    mEnableGroupSoundCon.append(ui->plainTextEdit_effectContainerDataRaw);
    mEnableGroupSoundCon.append(ui->plainTextEdit_effectContainerDataString);

    mEnableGroupSoundCon.append(ui->label_voiceGroup);
    mEnableGroupSoundCon.append(ui->label_voiceGroupSep);
    mEnableGroupSoundCon.append(ui->label_randomWeight);
    mEnableGroupSoundCon.append(ui->label_amplitudeFactor);
    mEnableGroupSoundCon.append(ui->label_start);
    mEnableGroupSoundCon.append(ui->label_playbackFrequence);
    mEnableGroupSoundCon.append(ui->label_dopplerRatio);
    mEnableGroupSoundCon.append(ui->toolButton_linkedFilesUp);
    mEnableGroupSoundCon.append(ui->label_end);
    mEnableGroupSoundCon.append(ui->label_maskSample);
    mEnableGroupSoundCon.append(ui->label_effectContainerCount);
    mEnableGroupSoundCon.append(ui->label_linkedFiles);
    mEnableGroupSoundCon.append(ui->label_effectContainerDataRaw);
    mEnableGroupSoundCon.append(ui->label_effectContainerDataString);
}

void MainWindow::updatePrimaryWidgetStates()
{
    if(mFileOpen)
    {
        // Reveal everything and hide instructions
        QList<QGroupBox*> allUIGroups = ui->centralWidget->findChildren<QGroupBox*>();
        for(QGroupBox* gb : allUIGroups)
            gb->show();

        ui->label_instructions->hide();

        // Enable all file menu actions that had been disabled
        if(!mAllChangesSaved)
            ui->actionSave->setEnabled(true);
        ui->actionSave_As->setEnabled(true);
        ui->actionClose->setEnabled(true);
    }
    else
    {
        // Hide everything except for the start instructions
        QList<QGroupBox*> allUIGroups = ui->centralWidget->findChildren<QGroupBox*>();
        for(QGroupBox* gb : allUIGroups)
            gb->hide();

        ui->label_instructions->show();

        // Disable all file menu options except for New, Open, and Exit
        ui->actionSave->setEnabled(false);
        ui->actionSave_As->setEnabled(false);
        ui->actionClose->setEnabled(false);
    }
}

void MainWindow::updateSecondaryWidgetStates(QWidgetList widgetsToUpdate)
{
    for(QWidget* currentWidget : widgetsToUpdate)
        currentWidget->setEnabled(mWidgetEnableConditionMap.value(currentWidget)());
}

void MainWindow::recursivePlaylistEnumerator(QTreeWidgetItem* parentTreeItem, BSC::Playlist &parentPlaylist, QString parentIndexMarker)
{
    for (int i = 0; i < parentPlaylist.getSubPlaylistsV().length(); i++)
    {
        // Update root index marker
        QString childIndexMarker = parentIndexMarker + BSC::Playlist::SUB_INDEX_LABEL_SEP + QString::number(i);

        // Create and configure child item
        QTreeWidgetItem* childTreeItem = new QTreeWidgetItem;
        childTreeItem->setText(0, BSC::Playlist::SUB_INDEX_LABEL.arg(childIndexMarker));

        // Restore enable state
        childTreeItem->setExpanded(parentPlaylist.getSubPlaylistsV()[i].getInitialTreeExpansion());

        // Add child to parent
        parentTreeItem->addChild(childTreeItem);

        // Process any children of this child playlist
        recursivePlaylistEnumerator(childTreeItem, parentPlaylist.getSubPlaylistsR()[i], childIndexMarker);
    }
}

void MainWindow::updateBSCTopLevel()
{
    // Tweener
    ui->lineEdit_tweener->setText(mBSCPtr->getInterpretedTweener());

    // Format Version
    ui->lineEdit_formatVer->setText(QString::number(mBSCPtr->getInterpretedFormatVer()));

    // Playlist Count
    ui->lineEdit_playlistCount->setText(QString::number(mBSCPtr->getInterpretedPlayListCount()));

    // Playlist Select
    updatePlaylists();
}

void MainWindow::updatePlaylists()
{
    // Clear playlist tree
    mSlotsEnabled = false;
    ui->treeWidget_playlists->clear();
    mSlotsEnabled = true;

    // Populate playlist tree

    for(int i = 0; i < mBSCPtr->getInterpretedPlayListCount(); i++)
    {
        // Create root item
        QTreeWidgetItem* rootTreeItem = new QTreeWidgetItem(ui->treeWidget_playlists);

        // Create Initial Index Marker
        QString rootIndexMarker = QString::number(i);

        // Set root item details
        rootTreeItem->setText(0, BSC::Playlist::ROOT_INDEX_LABEL.arg(rootIndexMarker));

        // Restore enable state
        rootTreeItem->setExpanded(mBSCPtr->getPlaylistsV()[i].getInitialTreeExpansion());

        // Process sub-playlists
        recursivePlaylistEnumerator(rootTreeItem, mBSCPtr->getPlaylistsR()[i], rootIndexMarker);
    }

    updateSecondaryWidgetStates(mEnableGroupTopLevel + mEnableGroupPlaylistSel);
}

void MainWindow::updateEntirePlaylist()
{
   if(mBSCPtr->hasPlaylists())
   {
       // Refill combo boxes if thery were previously cleared
       if(ui->comboBox_playbackMode->count() == 0)
           ui->comboBox_playbackMode->addItems(BSC::Playlist::PLAYBACK_MODE_STRINGS);
       if(ui->comboBox_maxInst->count() == 0)
           ui->comboBox_maxInst->addItems(BSC::Playlist::MAX_INSTANCES_STRINGS);
       if(ui->comboBox_poly->count() == 0)
           ui->comboBox_poly->addItems(BSC::Playlist::POLYPHONY_STRINGS);
       if(ui->comboBox_emitter->count() == 0)
           ui->comboBox_emitter->addItems(BSC::Playlist::EMITTER_STRINGS);

       // Playback Mode
       ui->comboBox_playbackMode->setCurrentIndex(static_cast<int>(getSelectedPlaylist()->getInterpretedPlaybackMode()));

       // Max Instances
       ui->comboBox_maxInst->setCurrentIndex(int(getSelectedPlaylist()->getInterpretedMaxInstances()));

       // Polyphony
       ui->comboBox_poly->setCurrentIndex(int(getSelectedPlaylist()->getInterpretedPolyphony() - 1)); // Index 0 is "One" so need to subtract 1 to get correct index

       // Integer Spinner Data
       ui->spinBox_repeatTimeMin->setValue(int(getSelectedPlaylist()->getInterpretedMinRepeatTime()));
       ui->spinBox_repeatTimeMax->setValue(int(getSelectedPlaylist()->getInterpretedMaxRepeatTime()));
       ui->spinBox_repeatCountMin->setValue(int(getSelectedPlaylist()->getInterpretedMinRepeatCount()));
       ui->spinBox_repeatCountMax->setValue(int(getSelectedPlaylist()->getInterpretedMaxRepeatTime()));
       ui->spinBox_randomPlaylistWeight->setValue(int(getSelectedPlaylist()->getInterpretedRandomPlaylistWeight()));

       // Double Spinner Data
       ui->doubleSpinBox_length->setValue(double(getSelectedPlaylist()->getInterpretedLength()));
       ui->doubleSpinBox_releaseTime->setValue(double(getSelectedPlaylist()->getInterpretedReleaseTime()));

       // Effect Behaviors
       updateEffectBehaviors();
       ui->checkBox_parentBehaviorOverride->setChecked(getSelectedPlaylist()->getInterpretedOverrideParentBehaviors());
       ui->comboBox_emitter->setCurrentIndex(static_cast<int>(getSelectedPlaylist()->getInterpretedEmitter()));

   }
   else
   {
       // Clear
       mSlotsEnabled = false;
       ui->comboBox_playbackMode->clear();
       ui->comboBox_maxInst->clear();
       ui->comboBox_poly->clear();
       ui->spinBox_repeatTimeMin->clear();
       ui->spinBox_repeatTimeMax->clear();
       ui->spinBox_repeatCountMin->clear();
       ui->spinBox_repeatCountMax->clear();
       ui->spinBox_randomPlaylistWeight->clear();
       ui->doubleSpinBox_length->clear();
       ui->doubleSpinBox_releaseTime->clear();
       ui->checkBox_parentBehaviorOverride->setChecked(false);
       ui->comboBox_emitter->clear();
       updateEffectBehaviors();
       mSlotsEnabled = false;
   }

    //  Sound Containers
    updateSoundContainers();
    ui->comboBox_soundContainer->setCurrentIndex(0); // Pre-select first container (comboBox slot SHOULD handle calling updateSoundContainerGroup()
    updateSoundContainerGroup(); // Combo box signal "currentIndexChanged" is not emitted if the index did not actually change which means that by the
                                 // the time the above line fires the index is already set to 0 and therefore it may be unecessary. Additionally, this
                                 // means that "updateSoundContainerGroup" is not called since the corresponding slot isn't triggered either so it must
                                 // be called manually here.

    // Update widget states
    updateSecondaryWidgetStates(mEnableGroupPlaylistSet + mEnableGroupEffectBeh + mEnableGroupSoundCon);
}

void MainWindow::updateEffectBehaviors()
{
    //Clear table widget
    ui->tableWidget_effectBehaviors->clear();

    if(mBSCPtr->hasPlaylists())
    {
        ui->tableWidget_effectBehaviors->setRowCount(getSelectedPlaylist()->getInterpretedEffectBehaviorsV().length());

        for(int i = 0; i < getSelectedPlaylist()->getInterpretedEffectBehaviorsV().length(); i++)
        {
            QTableWidgetItem *effectBehaviorItem = new QTableWidgetItem(getSelectedPlaylist()->getInterpretedEffectBehaviorsR()[i]);
            ui->tableWidget_effectBehaviors->setItem(i,0, effectBehaviorItem);
        }
    }
    else
        ui->tableWidget_effectBehaviors->setRowCount(0);
}

void MainWindow::updateSoundContainers()
{
    mAlteringSoundContainerBoxEntries = true; // Prevents out of bounds exception due to "comboBox_currentIndexChanged" getting called from "addItem"
                                              // since the sound group combo box index isn't set till after this function completes

    // Clear combo box
    ui->comboBox_soundContainer->clear();

    if(mBSCPtr->hasPlaylists())
    {
        // Update combo box
        for(int i = 0; i < getSelectedPlaylist()->soundContainerCount(); i++)
            ui->comboBox_soundContainer->addItem(BSC::SoundContainer::INDEX_LABEL.arg(i));
    }

    mAlteringSoundContainerBoxEntries = false;
}

void MainWindow::updateSoundContainerGroup()
{
    if(mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers())
    {
        // Voice Group
        int frontIndex = BSC::SoundContainer::VOICE_GROUP_FRONT_STRINGS.indexOf(getSelectedSoundContainer()->getInterpretedVoiceGroupFront());

        // Refill front combo box if it was previously cleared
        if(ui->comboBox_voiceGroupFront->count() == 0)
            ui->comboBox_voiceGroupFront->addItems(BSC::SoundContainer::VOICE_GROUP_FRONT_STRINGS);

        ui->comboBox_voiceGroupFront->setCurrentIndex(frontIndex);
        updateVoiceGroupBack(frontIndex);

        // Effect Containers
        ui->lineEdit_effectContainerCount->setText(QString::number(getSelectedSoundContainer()->getInterpretedEffectContainerCount()));
        ui->checkBox_effectContainerDataEdit->setChecked(false);

        bool slotsEnabled = mSlotsEnabled;
        mSlotsEnabled = false;
        if(getSelectedSoundContainer()->getInterpretedEffectContainerCount() > 0)
        {
            ui->plainTextEdit_effectContainerDataRaw->setPlainText(Qx::String::fromByteArrayHex(
                                                                   Qx::ByteArray::RAWFromString(getSelectedSoundContainer()->getInterpretedEffectContainerData()))
                                                                   .toUpper());
            ui->plainTextEdit_effectContainerDataString->setPlainText(getSelectedSoundContainer()->getInterpretedEffectContainerData());
        }
        else
        {
            ui->plainTextEdit_effectContainerDataRaw->clear();
            ui->plainTextEdit_effectContainerDataString->clear();
        }
        mSlotsEnabled = slotsEnabled;

        // Others
        ui->checkBox_3D->setChecked(getSelectedSoundContainer()->getInterpretedIs3D());
        ui->checkBox_loop->setChecked(getSelectedSoundContainer()->getInterpretedIsLooped());
        ui->checkBox_speedVolumeTrigger->setChecked(getSelectedSoundContainer()->getInterpretedUsingSpeedVolumeTrigger());
        ui->spinBox_randomWeight->setValue(int(getSelectedSoundContainer()->getInterpretedRandomWeight()));
        ui->doubleSpinBox_amplitudeFactor->setValue(double(getSelectedSoundContainer()->getInterpretedAmplitudeFactor()));
        ui->doubleSpinBox_dopplerRatio->setValue(double(getSelectedSoundContainer()->getInterpretedDopplerRatio()));
        ui->doubleSpinBox_playbackFrequence->setValue(double(getSelectedSoundContainer()->getInterpretedPlaybackFrequence()));
        ui->doubleSpinBox_start->setValue(double(getSelectedSoundContainer()->getInterpretedStart()));
        ui->doubleSpinBox_end->setValue(double(getSelectedSoundContainer()->getInterpretedEnd()));
        ui->doubleSpinBox_maskSample->setValue(double(getSelectedSoundContainer()->getInterpretedMaskSample()));
    }
    else
    {
        // Clear all widgets
        mSlotsEnabled = false;
        ui->comboBox_voiceGroupFront->clear();
        ui->comboBox_voiceGroupBack->clear();
        ui->lineEdit_effectContainerCount->clear();
        ui->checkBox_effectContainerDataEdit->setChecked(false);
        ui->plainTextEdit_effectContainerDataRaw->clear();
        ui->plainTextEdit_effectContainerDataString->clear();
        ui->checkBox_3D->setChecked(false);
        ui->checkBox_loop->setChecked(false);
        ui->checkBox_speedVolumeTrigger->setChecked(false);
        ui->spinBox_randomWeight->clear();
        ui->doubleSpinBox_amplitudeFactor->clear();
        ui->doubleSpinBox_dopplerRatio->clear();
        ui->doubleSpinBox_playbackFrequence->clear();
        ui->doubleSpinBox_start->clear();
        ui->doubleSpinBox_end->clear();
        ui->doubleSpinBox_maskSample->clear();
        mSlotsEnabled = true;
    }

    updateLinkedFiles();
}

void MainWindow::updateVoiceGroupBack(int frontIndex)
{
    bool slotsEnabled = mSlotsEnabled;
    mSlotsEnabled = false;
    ui->comboBox_voiceGroupBack->clear();
    ui->comboBox_voiceGroupBack->addItems(BSC::SoundContainer::VOICE_GROUP_BACK_STRINGS[frontIndex]);
    ui->comboBox_voiceGroupBack->setCurrentIndex(BSC::SoundContainer::VOICE_GROUP_BACK_STRINGS[frontIndex].indexOf(getSelectedSoundContainer()->getInterpretedVoiceGroupBack()));
    mSlotsEnabled = slotsEnabled;
}

void MainWindow::updateLinkedFiles()
{
    // Clear table
    ui->tableWidget_linkedFiles->clear();

    // Update table
    if(mBSCPtr->hasPlaylists() && getSelectedPlaylist()->hasSoundContainers())
    {
        ui->tableWidget_linkedFiles->setRowCount(getSelectedSoundContainer()->getInterpretedLinkedFilesV().length());

        for(int i = 0; i < getSelectedSoundContainer()->getInterpretedLinkedFilesV().length(); i++)
        {
            QTableWidgetItem* linkedFileItem = new QTableWidgetItem(getSelectedSoundContainer()->getInterpretedLinkedFilesV().value(i));
            ui->tableWidget_linkedFiles->setItem(i,0, linkedFileItem);
        }

        // Fit long text
        //ui->tableWidget_linkedFiles->resizeRowsToContents();
    }
    else
        ui->tableWidget_linkedFiles->setRowCount(0);
}

void MainWindow::setChangesSavedState(bool changesSaved)
{
    mAllChangesSaved = changesSaved;
    ui->actionSave->setEnabled(!changesSaved); // Disables save button when all changes are saved
}

bool MainWindow::saveChangesPrompt(bool exit)
{
    // Ask user to save
    QMessageBox saveChanges;
    saveChanges.setText(MSG_SAVE_PROMPT_TXT);
    if(exit)
        saveChanges.setInformativeText(MSG_SAVE_PROMPT_EXIT_INFO_TXT);
    else
        saveChanges.setInformativeText(MSG_SAVE_PROMPT_ELSE_INFO_TXT);
    saveChanges.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    saveChanges.setDefaultButton(QMessageBox::Yes);
    QApplication::beep();
    int choice = saveChanges.exec();

    if(choice == QMessageBox::Yes)
    {
        ui->actionSave->trigger();
        return mAllChangesSaved; // Ensures that whatever triggered this function is canceled if they user backs out of a "Save As" prompt (if file hasn't been saved before)
    }
    else if(choice == QMessageBox::No)
        return true;
    else
        return false;
}

void MainWindow::openBSCFile(QFile& bscFile)
{
    // Make sure file is valid
    Qx::IO::IOOpReport openReport;
    bool fileIsValid = BSC::fileIsValidBSC(bscFile, openReport);

    if(openReport.wasSuccessful())
    {
        if(fileIsValid)
        {
            QByteArray fullFile;
            openReport = Qx::IO::readAllBytesFromFile(fullFile, bscFile);

            if(openReport.wasSuccessful())
            {
                mCurrentFileName = bscFile.fileName();
                mCurrentFileDir = QDir(bscFile.fileName()).absolutePath();
                mFileOpen = true;
                mBSCPtr = std::make_unique<BSC>(fullFile);
                updateBSCTopLevel();
                updatePrimaryWidgetStates();
                if(mBSCPtr->hasPlaylists())
                    ui->treeWidget_playlists->setCurrentItem(ui->treeWidget_playlists->topLevelItem(0));

                setChangesSavedState(true);
            }
            else
                execIOReport(openReport);
        }
        else
            QMessageBox::critical(this, MENU_OPEN_TITLE, MSG_OPEN_FILE_INVALID.arg(QDir::toNativeSeparators(bscFile.fileName())),
                                                                                   QMessageBox::Ok, QMessageBox::Ok);
    }
    else
        execIOReport(openReport);
}

BSC::Playlist* MainWindow::getSelectedPlaylist() const
{
    MainWindow::PlaylistItemMap selectedItemMap = PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get());
    return selectedItemMap.getTargetPlaylist();
}

BSC::SoundContainer* MainWindow::getSelectedSoundContainer() const { return &getSelectedPlaylist()->getSoundContainersR()[ui->comboBox_soundContainer->currentIndex()]; }

void MainWindow::storeTreeItemExpandedStates() const
{
    // Recursive Lambda for sub-playlists
    std::function<void(QTreeWidgetItem* parentItem, BSC* bsc)> recursiveSubPlaylistExpansionStore;
    recursiveSubPlaylistExpansionStore = [&recursiveSubPlaylistExpansionStore](QTreeWidgetItem* parentItem, BSC* bsc)->void
    {
        for(int i = 0; i < parentItem->childCount(); i++)
        {
            QTreeWidgetItem* currentItem = parentItem->child(i);
            PlaylistItemMap(currentItem, bsc).getTargetPlaylist()->setInitialTreeExpansion(currentItem->isExpanded());
            recursiveSubPlaylistExpansionStore(currentItem, bsc);
        }
    };

    for(int i = 0; i < ui->treeWidget_playlists->topLevelItemCount(); i++)
    {
        // Top level QTreeWidgetItems
        QTreeWidgetItem* currentItem = ui->treeWidget_playlists->topLevelItem(i);
        PlaylistItemMap(currentItem, mBSCPtr.get()).getTargetPlaylist()->setInitialTreeExpansion(currentItem->isExpanded());

        // Sub playlists
        recursiveSubPlaylistExpansionStore(currentItem, mBSCPtr.get());
    }
}

void MainWindow::cacheSubRoutine(bool existing)
{
    QString cachePath;
    if(existing)
        cachePath = QFileDialog::getOpenFileName(this, MENU_CACHE_OPEN_TITLE, QDir::currentPath(), MENU_CACHE_FILE_FILTER);
    else
        cachePath = QFileDialog::getSaveFileName(this, MENU_CACHE_NEW_TITLE, QDir::currentPath(), MENU_CACHE_FILE_FILTER);

    if(cachePath != "")
    {
        QFile cacheFile(cachePath);

        SMC cache = SMC();

        if(existing)
        {
            QByteArray cacheData;
            Qx::IO::IOOpReport openReport = Qx::IO::readAllBytesFromFile(cacheData, cacheFile);

            if(openReport.wasSuccessful())
                cache = SMC(cacheData);
            else
            {
                execIOReport(openReport);
                return;
            }
        }

        if(existing && !cache.isValidCache())
            QMessageBox::critical(this, QApplication::applicationName(), MSG_CACHE_INVALID.arg(QDir::toNativeSeparators(cachePath)),
                                  QMessageBox::Ok, QMessageBox::Ok);
        else
        {
            if(existing && cache.getInterpretedCacheListR().removeDuplicates())
            {
                QApplication::beep();
                QMessageBox::information(this, QApplication::applicationName(), MSG_CACHE_HAS_DUPES, QMessageBox::Ok, QMessageBox::Ok);
            }

            QString scanFolderPath = QFileDialog::getExistingDirectory(this, MENU_CACHE_FOLDER_TITLE, cachePath);
            QDir scanFolderDir(scanFolderPath);

            if(scanFolderPath != "")
            {
                int originalLength = cache.getInterpretedCacheListV().length();
                bool includeSubFolders = QMessageBox::question(this, QApplication::applicationName(),
                                                               MSG_CACHE_FOLDER_SUB_PROMPT,
                                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes;

                QStringList bscList;
                Qx::IO::IOOpReport folderScanReport = Qx::IO::getDirFileList(bscList, scanFolderDir, includeSubFolders, {BSC::FILE_EXT});

                if(folderScanReport.wasSuccessful())
                {
                    QStringList invalidBSCPaths;
                    QStringList invalidReasons;

                    bool aborted = false;

                    QProgressDialog conversionProgress(MSG_CACHE_SCAN_IN_PROGRESS, MSG_CACHE_SCAN_ABORT, 0, bscList.length(), this);
                    conversionProgress.setWindowModality(Qt::WindowModal);

                    for(int i = 0; i < bscList.length(); i++)
                    {
                        conversionProgress.setValue(i);

                        if(conversionProgress.wasCanceled())
                        {
                            aborted = true;
                            break;
                        }

                        QString currentFileName = QFileInfo(bscList.value(i)).baseName();

                        if(!currentFileName.contains(' '))
                        {
                            QFile currentBSCFile(bscList.value(i));
                            Qx::IO::IOOpReport currentOpenReport;
                            bool currentFileIsValid = BSC::fileIsValidBSC(currentBSCFile, currentOpenReport);

                            if(currentOpenReport.wasSuccessful() && currentFileIsValid)
                            {
                                if(!cache.getInterpretedCacheListR().contains(currentFileName, Qt::CaseInsensitive))
                                    cache.getInterpretedCacheListR().append(currentFileName);
                            }
                            else if(!currentOpenReport.wasSuccessful())
                            {
                                invalidBSCPaths.append(currentFileName);
                                invalidReasons.append(currentOpenReport.getOutcomeInfo());
                            }
                            else
                            {
                                invalidBSCPaths.append(currentFileName);
                                invalidReasons.append(MSG_CACHE_INV_FILES_CAT_BAD);
                            }
                        }
                        else
                        {
                            invalidBSCPaths.append(currentFileName);
                            invalidReasons.append(MSG_CACHE_INV_FILES_CAT_SPACE);
                        }
                    }
                    conversionProgress.setValue(bscList.length()); // Finish progress dialog

                    if(!aborted)
                    {
                        cache.deinterpretData();
                        Qx::IO::IOOpReport writeReport = Qx::IO::writeBytesAsFile(cacheFile, cache.rebuildRawFile(), true);

                        QMessageBox cacheStatus;
                        cacheStatus.setStandardButtons(QMessageBox::Ok);
                        cacheStatus.setDefaultButton(QMessageBox::Ok);
                        cacheStatus.setIcon(QMessageBox::Information);

                        if(!writeReport.wasSuccessful())
                        {
                            execIOReport(writeReport);

                            cacheStatus.setIcon(QMessageBox::Critical);
                            cacheStatus.setText(MSG_CACHE_FAIL.arg(QDir::toNativeSeparators(cachePath)));
                        }
                        else if(existing)
                        {
                            if(cache.getInterpretedCacheListV().length() == originalLength)
                                cacheStatus.setText(MSG_CACHE_UP_TO_DATE);
                            else if(cache.getInterpretedCacheListV().length() == originalLength + bscList.length())
                                cacheStatus.setText(MSG_CACHE_UPDATED_TXT);
                            else if(cache.getInterpretedCacheListV().length() > originalLength && cache.getInterpretedCacheListV().length() < originalLength + bscList.length())
                            {
                                cacheStatus.setText(MSG_CACHE_UPDATED_TXT);
                                cacheStatus.setInformativeText(MSG_CACHE_UPDATED_INFO_TXT);
                            }
                            else
                                throw std::out_of_range("Updated Speechmanager Cache entry count is unexpected!");
                        }
                        else
                            cacheStatus.setText(MSG_CACHE_NEW_SUCCESS_TXT);

                        if(invalidBSCPaths.count() > 0)
                        {
                            QString invalidReport = MSG_CACHE_INV_FILES_DETAILS;
                            QString infoText = cacheStatus.informativeText();

                            for(int i = 0; i < invalidBSCPaths.count(); i++)
                                invalidReport += "\n- " + invalidBSCPaths.value(i) + " " + invalidReasons.value(i);

                            cacheStatus.setIcon(QMessageBox::Warning);
                            if(infoText != "")
                                infoText += " ";
                            cacheStatus.setInformativeText(infoText + MSG_CACHE_INV_FILES_INFO);
                            cacheStatus.setDetailedText(invalidReport);
                        }

                        QApplication::beep();
                        cacheStatus.exec();
                    }
                    else
                        QMessageBox::critical(this, QApplication::applicationName(), MSG_CACHE_SCAN_ABORTED, QMessageBox::Ok, QMessageBox::Ok);
                }
                else
                    execIOReport(folderScanReport);
            }
        }
    }
}

void MainWindow::execIOReport(Qx::IO::IOOpReport ioReport)
{
    QMessageBox ioMessage;
    ioMessage.setStandardButtons(QMessageBox::Ok);
    ioMessage.setDefaultButton(QMessageBox::Ok);
    ioMessage.setText(ioReport.getOutcome());

    if(ioReport.wasSuccessful())
        ioMessage.setIcon(QMessageBox::Information);
    else
    {
        ioMessage.setIcon(QMessageBox::Critical);
        ioMessage.setInformativeText(ioReport.getOutcomeInfo());
    }

    if(ioReport.wasSuccessful())
        QApplication::beep();

    ioMessage.exec();
}

//-Overrides-----------------------------------------------------------------------------------------------------
void MainWindow::closeEvent (QCloseEvent *event)
{
    bool closeApp = true;

    //Close app
    if(mFileOpen && !mAllChangesSaved)
        closeApp = saveChangesPrompt(true);

    if(closeApp)
        event->accept();
    else
        event->ignore();
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->plainTextEdit_effectContainerDataRaw)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            if(!(keyEvent->modifiers() & Qt::ControlModifier) && !(keyEvent->modifiers() & Qt::AltModifier) &&
               !(keyEvent->key() == Qt::Key_Backspace) && !(keyEvent->key() == Qt::Key_Delete) &&
               !(keyEvent->key() == Qt::Key_Up) && !(keyEvent->key() == Qt::Key_Down) &&
                !(keyEvent->key() == Qt::Key_Left) && !(keyEvent->key() == Qt::Key_Right)) // Don't filter shortcuts, del, or arrows
            {
                if (Qx::String::isHexNumber(keyEvent->text()))
                {
                    // Prevent unecessary processing in TextEdit slot
                    mEffConRawInputGuaranteedValid = true;

                    if(!(keyEvent->modifiers() & Qt::ShiftModifier))
                    {
                        // Force capitalize input
                        QApplication::sendEvent(target, new QKeyEvent(QEvent::KeyPress, keyEvent->key(), keyEvent->modifiers() | Qt::ShiftModifier, keyEvent->text().toUpper()));
                        QApplication::sendEvent(target, new QKeyEvent(QEvent::KeyRelease, keyEvent->key(), keyEvent->modifiers() | Qt::ShiftModifier, keyEvent->text().toUpper()));

                        return true; // Consume event
                    }
                    // else - Let keyevent through
                }
                else
                    return true; //Block non-hexadecimal numbers
            }
            else if(keyEvent->key() & Qt::Key_Backspace || keyEvent->key() & Qt::Key_Delete)
                mEffConRawInputGuaranteedValid = true;
        }
    }

    return QMainWindow::eventFilter(target, event); // Pass the event through its normal path
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    // Only accept single BSC drops
    if(event->mimeData()->urls().length() == 1 && event->mimeData()->urls().value(0).isLocalFile())
    {
        QString filePath = event->mimeData()->urls().value(0).toLocalFile();
        if(QFileInfo(filePath).suffix() == BSC::FILE_EXT)
            event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    bool openFile = true;

    if(mFileOpen && !mAllChangesSaved)
        openFile = saveChangesPrompt(false);

    if(openFile)
    {
        QFile fileToOpen(event->mimeData()->urls().value(0).toLocalFile());
        openBSCFile(fileToOpen);
    }
}

//-Slots---------------------------------------------------------------------------------------------------------
//Private:

void MainWindow::all_on_menuAction_triggered()
{
    // Get the object that called this slot
    QAction *senderAction = qobject_cast<QAction *>(sender());

    // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
    if(senderAction == nullptr)
        throw "Pointer conversion to tree widget failed";

    // Menu "File"
    if(senderAction == ui->actionSave_As)
    {
        QString saveAsPath = QFileDialog::getSaveFileName(this, tr(MENU_SAVE_AS_TITLE.toStdString().c_str()), mCurrentFileDir,
                                                          tr(MENU_FILE_FILTER.toStdString().c_str()));

        if(saveAsPath != "")
        {
            QFile saveAsFile(saveAsPath);
            Qx::IO::IOOpReport saveReport = Qx::IO::writeBytesAsFile(saveAsFile, mBSCPtr->rebuildRawFile(), true);
            if(saveReport.wasSuccessful())
            {
                QApplication::beep();
                QMessageBox::information(this, MENU_SAVE_AS_TITLE, MSG_SAVE_SUCCESS.arg(QDir::toNativeSeparators(saveAsPath)),
                                         QMessageBox::Ok, QMessageBox::Ok);
                mCurrentFileDir = QDir(saveAsPath).absolutePath();
                mCurrentFileName = saveAsFile.fileName();
                setChangesSavedState(true);
            }
            else
            {
                execIOReport(saveReport);
                QMessageBox::critical(this, MENU_SAVE_AS_TITLE, MSG_SAVE_FAIL.arg(QDir::toNativeSeparators(saveAsPath)), QMessageBox::Ok, QMessageBox::Ok);
            }

        }
    }
    else if(senderAction == ui->actionSave)
    {
        if(mCurrentFileName == "")
            ui->actionSave_As->trigger();
        else
        {
            QString savePath = mCurrentFileDir + mCurrentFileName;
            QFile saveFile(savePath);
            Qx::IO::IOOpReport saveReport = Qx::IO::writeBytesAsFile(saveFile, mBSCPtr->rebuildRawFile(), true);
            if(saveReport.wasSuccessful())
            {
                QApplication::beep();
                QMessageBox::information(this, MENU_SAVE_TITLE, MSG_SAVE_SUCCESS.arg(QDir::toNativeSeparators(savePath)),
                                         QMessageBox::Ok, QMessageBox::Ok);
                setChangesSavedState(true);
            }
            else
            {
                execIOReport(saveReport);
                QMessageBox::critical(this, MENU_SAVE_AS_TITLE, MSG_SAVE_FAIL.arg(QDir::toNativeSeparators(savePath)),
                                      QMessageBox::Ok, QMessageBox::Ok);
            }

        }
    }
    else if(senderAction == ui->actionNew)
    {
        bool makeNew = true;

        if(mFileOpen && !mAllChangesSaved)
            makeNew = saveChangesPrompt(false);

        if(makeNew)
        {
            mCurrentFileName = "";
            mFileOpen = true;
            mBSCPtr = std::make_unique<BSC>();
            updateBSCTopLevel();
            updatePrimaryWidgetStates();
            updateSecondaryWidgetStates(mEnableGroupTopLevel + mEnableGroupPlaylistSel + mEnableGroupPlaylistSet + mEnableGroupEffectBeh + mEnableGroupSoundCon);
            updateEntirePlaylist();
            setChangesSavedState(false);
        }
    }
    else if(senderAction == ui->actionOpen)
    {
        bool openFile = true;

        if(mFileOpen && !mAllChangesSaved)
            openFile = saveChangesPrompt(false);

        if(openFile)
        {
            QString openFilePath = QFileDialog::getOpenFileName(this, MENU_OPEN_TITLE, mCurrentFileDir, MENU_FILE_FILTER);

            if(openFilePath != "")
            {
                QFile fileToOpen(openFilePath);
                openBSCFile(fileToOpen);
            }
        }
    }
    else if(senderAction == ui->actionClose)
    {
        bool closeFile = true;

        if(mFileOpen && !mAllChangesSaved)
            closeFile = saveChangesPrompt(false);

        if(closeFile)
        {
            mCurrentFileName = "";
            mFileOpen = false;
            updateBSCTopLevel();
            updatePrimaryWidgetStates();
            mBSCPtr.reset(); // Deletes ptr contents
        }
    }
    else if(senderAction == ui->actionExit)
    {
        bool closeApp = true;

        //Close app
        if(mFileOpen && !mAllChangesSaved)
            closeApp = saveChangesPrompt(true);

        if(closeApp)
            QApplication::quit();
    }

    // Menu "Tools"
    else if (senderAction == ui->actionBuild_New_Speechmanager_Cache)
    {
        cacheSubRoutine(false);
    }
    else if(senderAction == ui->actionUpdate_Speechmanager_Cache)
    {
       cacheSubRoutine(true);
    }
    else if(senderAction == ui->actionConvert_SMF_WAV)
    {
        QString smfPath = QFileDialog::getOpenFileName(this, MENU_CONV_SMF_IN_TITLE, QDir::currentPath(), MENU_SMF_FILE_FILTER);

        if(smfPath != "")
        {
            QFile smfFile(smfPath);
            Qx::IO::IOOpReport openReport;
            bool smfIsValid = SMF::fileIsValidSMF(smfFile, openReport);

            if(openReport.wasSuccessful())
            {
                if(smfIsValid)
                {
                    QString wavPath = QFileDialog::getSaveFileName(this, MENU_CONV_SMF_OUT_TITLE, QDir::currentPath(), MENU_WAV_FILE_FILTER);

                    if(wavPath != "")
                    {
                        QByteArray smfData;
                        Qx::IO::IOOpReport smfRead = Qx::IO::readAllBytesFromFile(smfData, smfFile);

                        if(smfRead.wasSuccessful())
                        {
                            SMF smfIn(smfData);
                            QFile wavFile(wavPath);

                            Qx::IO::IOOpReport writeReport = Qx::IO::writeBytesAsFile(wavFile, smfIn.toWAV().getFullData(), true);
                            if(writeReport.wasSuccessful())
                            {
                                QApplication::beep();
                                QMessageBox::information(this, QApplication::applicationName(), MSG_CONV_SMF_SUCCESS, QMessageBox::Ok, QMessageBox::Ok);
                            }
                            else
                            {
                                execIOReport(writeReport);
                                QMessageBox::critical(this, QApplication::applicationName(), MSG_CONV_SMF_FAIL.arg(QDir::toNativeSeparators(wavPath)), QMessageBox::Ok, QMessageBox::Ok);
                            }
                        }
                        else
                            execIOReport(smfRead);
                    }
                }
                else
                    QMessageBox::critical(this, QApplication::applicationName(), MSG_CONV_SMF_INVALID.arg(QDir::toNativeSeparators(smfPath)), QMessageBox::Ok, QMessageBox::Ok);
            }
            else
                execIOReport(openReport);
        }
    }
    else if(senderAction == ui->actionConvert_WAV_SMF)
    {
        QString wavPath = QFileDialog::getOpenFileName(this, MENU_CONV_WAV_IN_TITLE, QDir::currentPath(), MENU_WAV_FILE_FILTER);

        if(wavPath != "")
        {
            QFile wavFile(wavPath);
            Qx::IO::IOOpReport wavRead;
            bool fileIsValid = WAV::fileIsValidWAV(wavFile, wavRead);

            if(wavRead.wasSuccessful())
            {
                if(fileIsValid)
                {
                    QString smfPath = QFileDialog::getSaveFileName(this, MENU_CONV_WAV_OUT_TITLE, QDir::currentPath(), MENU_SMF_FILE_FILTER);

                    if(smfPath != "")
                    {
                        QByteArray wavData;
                        wavRead = Qx::IO::readAllBytesFromFile(wavData, wavFile);

                        if(wavRead.wasSuccessful())
                        {
                            WAV wavIn(wavData);
                            QFile smfFile(smfPath);

                            Qx::IO::IOOpReport writeReport = Qx::IO::writeBytesAsFile(smfFile, wavIn.toSMF().getFullData(), true);
                            if(writeReport.wasSuccessful())
                                QMessageBox::information(this, QApplication::applicationName(), MSG_CONV_WAV_SUCCESS, QMessageBox::Ok, QMessageBox::Ok);
                            else
                            {
                                execIOReport(writeReport);
                                QMessageBox::critical(this, QApplication::applicationName(), MSG_CONV_WAV_FAIL.arg(QDir::toNativeSeparators(wavPath)), QMessageBox::Ok, QMessageBox::Ok);
                            }
                        }
                        else
                            execIOReport(wavRead);
                    }
                }
                else
                    QMessageBox::critical(this, QApplication::applicationName(), MSG_CONV_WAV_INVALID.arg(QDir::toNativeSeparators(wavPath)), QMessageBox::Ok, QMessageBox::Ok);
            }
            else
                execIOReport(wavRead);
        }
    }
    else if(senderAction == ui->actionBatch_SMF_WAV)
    {
        QString smfFolderPath = QFileDialog::getExistingDirectory(this, MENU_BATCH_SMF_IN_TITLE, QDir::currentPath());

        if(smfFolderPath != "")
        {
            bool includeSubFolders = QMessageBox::question(this, QApplication::applicationName(),
                                                                                MSG_BATCH_SMF_SUB_PROMPT,
                                                                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes;

            QString wavFolderPath = QFileDialog::getExistingDirectory(this, MENU_BATCH_SMF_OUT_TITLE, QDir::currentPath());

            if(wavFolderPath != "")
            {
                QDir smfFolder(smfFolderPath);
                QStringList smfList;
                Qx::IO::IOOpReport dirRead = Qx::IO::getDirFileList(smfList, smfFolder, includeSubFolders, {SMF::FILE_EXT});

                if(dirRead.wasSuccessful())
                {
                    QStringList failedConvList;
                    QStringList failedReasonList;

                    bool showOverwritePrompt = true;
                    bool overwriteExisting = false;
                    bool aborted = false;

                    int filesProcessed = 0;

                    QProgressDialog conversionProgress(MSG_BATCH_SMF_IN_PROGRESS, MSG_BATCH_ALL_ABORT, 0, smfList.length(), this);
                    conversionProgress.setWindowModality(Qt::WindowModal);

                    for(int i = 0; i < smfList.length(); i++)
                    {
                        conversionProgress.setValue(i);

                        if(conversionProgress.wasCanceled())
                        {
                            aborted = true;
                            filesProcessed = i;
                            break;
                        }

                        QFile currentFile(smfList.value(i));
                        Qx::IO::IOOpReport smfRead;
                        bool fileIsValid = SMF::fileIsValidSMF(currentFile, smfRead);

                        if(smfRead.wasSuccessful())
                        {
                            if(fileIsValid)
                            {
                                QByteArray smfData;
                                smfRead = Qx::IO::readAllBytesFromFile(smfData, currentFile);

                                if(smfRead.wasSuccessful())
                                {
                                    SMF currentSMF(smfData);
                                    QString wavOutPath = smfList.value(i).remove(smfFolderPath).prepend(wavFolderPath);
                                    wavOutPath = wavOutPath.left(wavOutPath.length() - 3).append(WAV::FILE_EXT);
                                    QFile wavOut(wavOutPath);

                                    if(wavOut.exists() && QFileInfo(wavOut).isFile() && showOverwritePrompt)
                                    {
                                        int overwriteChoice = QMessageBox::question(this, QApplication::applicationName(), MSG_BATCH_ALL_FILE_EXST.arg(QDir::toNativeSeparators(wavOutPath)),
                                                                                    QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll, QMessageBox::No);

                                        switch(overwriteChoice)
                                        {
                                            case QMessageBox::YesToAll:
                                                showOverwritePrompt = false;
                                                [[fallthrough]];
                                            case QMessageBox::Yes:
                                                overwriteExisting = true;
                                            break;

                                            case QMessageBox::NoToAll:
                                                showOverwritePrompt = false;
                                                [[fallthrough]];
                                            case QMessageBox::No:
                                                overwriteExisting = false;
                                        }
                                    }

                                    Qx::IO::IOOpReport writeReport = Qx::IO::writeBytesAsFile(wavOut, currentSMF.toWAV().getFullData(), overwriteExisting);
                                    if(!writeReport.wasSuccessful())
                                    {
                                        failedConvList.append(smfList.value(i));
                                        failedReasonList.append(writeReport.getOutcomeInfo());
                                    }
                                }
                                else
                                {
                                    failedConvList.append(smfList.value(i));
                                    failedReasonList.append(smfRead.getOutcomeInfo());
                                }
                            }
                            else
                            {
                                failedConvList.append(smfList.value(i));
                                failedReasonList.append(MSG_BATCH_SMF_INV_FILES_CAT_BAD);
                            }
                        }
                        else
                        {
                            failedConvList.append(smfList.value(i));
                            failedReasonList.append(smfRead.getOutcomeInfo());
                        }
                    }
                    conversionProgress.setValue(smfList.length()); // Finish progress dialog

                    QMessageBox batchResult;
                    batchResult.setIcon(QMessageBox::Information);
                    batchResult.setStandardButtons(QMessageBox::Ok);
                    batchResult.setDefaultButton(QMessageBox::Ok);

                    if(smfList.isEmpty())
                    {
                        batchResult.setIcon(QMessageBox::Critical);
                        batchResult.setText(MSG_BATCH_SMF_NO_FILES);
                    }
                    else if(smfList.length() == failedConvList.length())
                    {
                        batchResult.setIcon(QMessageBox::Critical);
                        batchResult.setText(MSG_BATCH_SMF_NO_VALID_FILES_TXT.arg(QDir::toNativeSeparators(smfFolderPath)));
                        batchResult.setInformativeText(MSG_BATCH_SMF_NO_VALID_FILES_INFO);

                        QString details = MSG_BATCH_SMF_INV_FILES_DETAILS + "\n";

                        for(int i = 0; i < failedConvList.length(); i ++)
                            details.append("\n- " + failedConvList.value(i) + " " + failedReasonList.value(i));

                        batchResult.setDetailedText(details);
                    }
                    else if(aborted)
                    {
                        batchResult.setIcon(QMessageBox::Warning);
                        batchResult.setText(MSG_BATCH_ALL_ABORT);
                        batchResult.setInformativeText(MSG_BATCH_ALL_ABORTED_INFO.arg(filesProcessed).arg(smfList.length()));
                    }
                    else if(!failedConvList.isEmpty())
                    {
                        batchResult.setIcon(QMessageBox::Warning);
                        batchResult.setText(MSG_BATCH_SMF_SUCCESS_TXT);
                        batchResult.setInformativeText(MSG_BATCH_SMF_INV_FILES_INFO);

                        QString details = MSG_BATCH_SMF_INV_FILES_DETAILS + "\n";

                        for(int i = 0; i < failedConvList.length(); i ++)
                            details.append("\n- " + failedConvList.value(i) + " " + failedReasonList.value(i));

                        batchResult.setDetailedText(details);
                    }
                    else
                        batchResult.setText(MSG_BATCH_SMF_SUCCESS_TXT);

                    QApplication::beep();
                    batchResult.exec();
                }
                else
                    execIOReport(dirRead);
            }
        }
    }
    else if(senderAction == ui->actionBatch_WAV_SMF)
    {
        QString wavFolderPath = QFileDialog::getExistingDirectory(this, MENU_BATCH_WAV_IN_TITLE, QDir::currentPath());

        if(wavFolderPath != "")
        {
            bool includeSubFolders = QMessageBox::question(this, QApplication::applicationName(),
                                                                                MSG_BATCH_WAV_SUB_PROMPT,
                                                                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes;

            QString smfFolderPath = QFileDialog::getExistingDirectory(this, MENU_BATCH_WAV_OUT_TITLE, QDir::currentPath());

            if(wavFolderPath != "")
            {
                QDir wavFolder(wavFolderPath);
                QStringList wavList;
                Qx::IO::IOOpReport dirRead = Qx::IO::getDirFileList(wavList, wavFolder, includeSubFolders, {WAV::FILE_EXT});

                if(dirRead.wasSuccessful())
                {
                    QStringList failedConvList;
                    QStringList failedReasonList;

                    bool showOverwritePrompt = true;
                    bool overwriteExisting = false;
                    bool aborted = false;

                    int filesProcessed = 0;

                    QProgressDialog conversionProgress(MSG_BATCH_WAV_IN_PROGRESS, MSG_BATCH_ALL_ABORT, 0, wavList.length(), this);
                    conversionProgress.setWindowModality(Qt::WindowModal);

                    for(int i = 0; i < wavList.length(); i++)
                    {
                        conversionProgress.setValue(i);

                        if(conversionProgress.wasCanceled())
                        {
                            aborted = true;
                            filesProcessed = i;
                            break;
                        }

                        QFile currentFile(wavList.value(i));
                        Qx::IO::IOOpReport wavRead;
                        bool fileIsValid = WAV::fileIsValidWAV(currentFile, wavRead);

                        if(wavRead.wasSuccessful())
                        {
                            if(fileIsValid)
                            {
                                QByteArray wavData;
                                wavRead = Qx::IO::readAllBytesFromFile(wavData, currentFile);

                                if(wavRead.wasSuccessful())
                                {
                                    WAV currentWAV(wavData);
                                    QString smfOutPath = wavList.value(i).remove(wavFolderPath).prepend(smfFolderPath);
                                    smfOutPath = smfOutPath.left(smfOutPath.length() - 3).append(WAV::FILE_EXT);

                                    QFile smfOut(smfOutPath);

                                    if(smfOut.exists() && QFileInfo(smfOut).isFile() && showOverwritePrompt)
                                    {
                                        int overwriteChoice = QMessageBox::question(this, QApplication::applicationName(), MSG_BATCH_ALL_FILE_EXST.arg(QDir::toNativeSeparators(smfOutPath)),
                                                                                    QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll, QMessageBox::No);

                                        switch(overwriteChoice)
                                        {
                                            case QMessageBox::YesToAll:
                                                showOverwritePrompt = false;
                                                [[fallthrough]];
                                            case QMessageBox::Yes:
                                                overwriteExisting = true;
                                            break;

                                            case QMessageBox::NoToAll:
                                                showOverwritePrompt = false;
                                                [[fallthrough]];
                                            case QMessageBox::No:
                                                overwriteExisting = false;
                                        }
                                    }

                                    Qx::IO::IOOpReport writeReport = Qx::IO::writeBytesAsFile(smfOut, currentWAV.toSMF().getFullData(), overwriteExisting);

                                    if(!writeReport.wasSuccessful())
                                    {
                                        failedConvList.append(wavList.value(i));
                                        failedReasonList.append(writeReport.getOutcomeInfo());
                                    }
                                }
                                else
                                {
                                    failedConvList.append(wavList.value(i));
                                    failedReasonList.append(wavRead.getOutcomeInfo());
                                }
                            }
                            else
                            {
                                failedConvList.append(wavList.value(i));
                                failedReasonList.append(MSG_BATCH_WAV_INV_FILES_CAT_BAD);
                            }
                        }
                        else
                        {
                            failedConvList.append(wavList.value(i));
                            failedReasonList.append(wavRead.getOutcomeInfo());
                        }
                    }
                    conversionProgress.setValue(wavList.length()); // Finish progress dialog

                    QMessageBox batchResult;
                    batchResult.setIcon(QMessageBox::Information);
                    batchResult.setStandardButtons(QMessageBox::Ok);
                    batchResult.setDefaultButton(QMessageBox::Ok);

                    if(wavList.isEmpty())
                    {
                        batchResult.setIcon(QMessageBox::Critical);
                        batchResult.setText(MSG_BATCH_WAV_NO_FILES);
                    }
                    else if(wavList.length() == failedConvList.length())
                    {
                        batchResult.setIcon(QMessageBox::Critical);
                        batchResult.setText(MSG_BATCH_WAV_NO_VALID_FILES_TXT.arg(QDir::toNativeSeparators(wavFolderPath)));
                        batchResult.setInformativeText(MSG_BATCH_WAV_NO_VALID_FILES_INFO);

                        QString details = MSG_BATCH_WAV_INV_FILES_DETAILS + "\n";

                        for(int i = 0; i < failedConvList.length(); i ++)
                            details.append("\n- " + failedConvList.value(i) + " " + failedReasonList.value(i));

                        batchResult.setDetailedText(details);
                    }
                    else if(aborted)
                    {
                        batchResult.setIcon(QMessageBox::Warning);
                        batchResult.setText(MSG_BATCH_ALL_ABORTED_TXT);
                        batchResult.setInformativeText(MSG_BATCH_ALL_ABORTED_INFO.arg(filesProcessed).arg(wavList.length()));
                    }
                    else if(!failedConvList.isEmpty())
                    {
                        batchResult.setIcon(QMessageBox::Warning);
                        batchResult.setText(MSG_BATCH_WAV_SUCCESS_TXT);
                        batchResult.setInformativeText(MSG_BATCH_WAV_INV_FILES_INFO);

                        QString details = MSG_BATCH_WAV_INV_FILES_DETAILS + "\n";

                        for(int i = 0; i < failedConvList.length(); i ++)
                            details.append("\n- " + failedConvList.value(i) + " " + failedReasonList.value(i));

                        batchResult.setDetailedText(details);
                    }
                    else
                        batchResult.setText(MSG_BATCH_WAV_SUCCESS_TXT);

                    QApplication::beep();
                    batchResult.exec();
                }
                else
                    execIOReport(dirRead);
            }
        }
    }

    // Menu "Help"
    else if(senderAction == ui->actionAbout)
    {
        QMessageBox aboutDiag;
        aboutDiag.setText(MSG_ABOUT_TXT);
        aboutDiag.setInformativeText(MSG_ABOUT_INFO_TXT);
        aboutDiag.setIcon(QMessageBox::Information);
        aboutDiag.setStandardButtons(QMessageBox::Ok);
        aboutDiag.setDefaultButton(QMessageBox::Ok);
        aboutDiag.exec();
    }
}

void MainWindow::all_on_treeWidget_currentItemChanged()
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QTreeWidget *senderTreeWidget = qobject_cast<QTreeWidget *>(sender());

        // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
        if(senderTreeWidget == nullptr)
            throw "Pointer conversion to tree widget failed";

        if(senderTreeWidget == ui->treeWidget_playlists)
        {
            updateSecondaryWidgetStates(mEnableGroupPlaylistSel);
            updateEntirePlaylist();
        }
    }
}

void MainWindow::all_on_tableWidget_itemChanged(QTableWidgetItem* item)
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QTableWidget *senderTableWidget = qobject_cast<QTableWidget *>(sender());

        // Ensure the signal thateff trigged this slot belongs to the above class by checking for null pointer
        if(senderTableWidget == nullptr)
            throw "Pointer conversion to tree widget failed";

        if(senderTableWidget == ui->tableWidget_linkedFiles)
        {
            QString newLinkedFile = item->text();
            if(getSelectedSoundContainer()->getInterpretedLinkedFilesV()[item->row()] != newLinkedFile)
            {
                getSelectedSoundContainer()->getInterpretedLinkedFilesR()[item->row()] = newLinkedFile;
                setChangesSavedState(false);
            }
        }
        else if (senderTableWidget == ui->tableWidget_effectBehaviors)
        {
            QString newEffectBehavior = item->text();
            if(getSelectedPlaylist()->getInterpretedEffectBehaviorsV()[item->row()] != newEffectBehavior)
            {
                getSelectedPlaylist()->getInterpretedEffectBehaviorsR()[item->row()] = newEffectBehavior;
                setChangesSavedState(false);
            }
        }
    }
}

void MainWindow::all_on_tableWidget_currentItemChanged()
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QTableWidget *senderTableWidget = qobject_cast<QTableWidget *>(sender());

        // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
        if(senderTableWidget == nullptr)
            throw "Pointer conversion to tree widget failed";

        if(senderTableWidget == ui->tableWidget_effectBehaviors)
            updateSecondaryWidgetStates(mEnableGroupEffectBeh);
        else if (senderTableWidget == ui->tableWidget_linkedFiles)
            updateSecondaryWidgetStates(mEnableGroupSoundCon);
    }
}

void MainWindow::all_on_linedEdit_textEdited()
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QLineEdit *senderLineEdit = qobject_cast<QLineEdit *>(sender());

        // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
        if(senderLineEdit == nullptr)
            throw "Pointer conversion to line edit failed";

        // Grab new text
        QString newText = senderLineEdit->text();

        if(senderLineEdit == ui->lineEdit_effectContainerCount)
        {
            if(getSelectedSoundContainer()->getInterpretedEffectContainerCount() != uint32_t(newText.toInt()))
            {
                getSelectedSoundContainer()->setInterpretedEffectContainerCount(uint32_t(newText.toInt()));
                setChangesSavedState(false);
            }
        }

        // Refresh Enabled States
        updateSecondaryWidgetStates({ui->plainTextEdit_effectContainerDataRaw, ui->label_effectContainerDataRaw,
                                     ui->plainTextEdit_effectContainerDataString, ui->label_effectContainerDataString});
    }
}

void MainWindow::all_on_comboBox_currentIndexChanged(int newIndex)
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QComboBox *senderComboBox = qobject_cast<QComboBox *>(sender());

        // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
        if(senderComboBox == nullptr)
            throw "Pointer conversion to combo box failed";

        if(senderComboBox == ui->comboBox_playbackMode)
        {
            BSC::Playlist::PlaybackMode newPlaybackMode = static_cast<BSC::Playlist::PlaybackMode>(newIndex);
            if(getSelectedPlaylist()->getInterpretedPlaybackMode() != newPlaybackMode)
            {
                getSelectedPlaylist()->setInterpretedPlaybackMode(newPlaybackMode);
                setChangesSavedState(false);
            }
        }
        else if(senderComboBox == ui->comboBox_maxInst)
        {
            uint32_t newMaxInstances = uint32_t(newIndex);
            if(getSelectedPlaylist()->getInterpretedMaxInstances() != newMaxInstances)
            {
                getSelectedPlaylist()->setInterpretedMaxInstances(newMaxInstances);
                setChangesSavedState(false);
            }
        }
        else if(senderComboBox == ui->comboBox_poly)
        {
            uint32_t newPolyphony = uint32_t(newIndex + 1);
            if(getSelectedPlaylist()->getInterpretedPolyphony() != newPolyphony) // Index 0 is "One" so need to add 1 to get true value
            {
                getSelectedPlaylist()->setInterpretedPolyphony(newPolyphony);
                setChangesSavedState(false);
            }
        }
        else if(senderComboBox == ui->comboBox_emitter)
        {
            BSC::Playlist::Emitter newEmitter = static_cast<BSC::Playlist::Emitter>(newIndex);
            if(getSelectedPlaylist()->getInterpretedEmitter() != newEmitter)
            {
                getSelectedPlaylist()->setInterpretedEmitter(static_cast<BSC::Playlist::Emitter>(newIndex));
                setChangesSavedState(false);
            }

        }
        else if(senderComboBox == ui->comboBox_soundContainer)
        {
            if(!mAlteringSoundContainerBoxEntries)
            {
                updateSoundContainerGroup();

                // Refresh Enabled States
                updateSecondaryWidgetStates(mEnableGroupSoundCon);
            }
        }
        else if(senderComboBox == ui->comboBox_voiceGroupFront)
        {
            QString newVoiceGroupFront = BSC::SoundContainer::VOICE_GROUP_FRONT_STRINGS.value(ui->comboBox_voiceGroupFront->currentIndex());
            QString newVoiceGroupBack = BSC::SoundContainer::VOICE_GROUP_BACK_STRINGS[ui->comboBox_voiceGroupFront->currentIndex()].value(0);

            if(getSelectedSoundContainer()->getInterpretedVoiceGroupFront() != newVoiceGroupFront)
            {
                getSelectedSoundContainer()->setInterpretedVoiceGroupFront(newVoiceGroupFront);
                getSelectedSoundContainer()->setInterpretedVoiceGroupBack(newVoiceGroupBack); // First corresponding back value
                updateVoiceGroupBack(ui->comboBox_voiceGroupFront->currentIndex());
                setChangesSavedState(false);
            }
        }
        else if(senderComboBox == ui->comboBox_voiceGroupBack)
        {
            QString newVoiceGroupBack = BSC::SoundContainer::VOICE_GROUP_BACK_STRINGS[ui->comboBox_voiceGroupFront->currentIndex()].value(ui->comboBox_voiceGroupBack->currentIndex());

            if(getSelectedSoundContainer()->getInterpretedVoiceGroupBack() != newVoiceGroupBack)
            {
                getSelectedSoundContainer()->setInterpretedVoiceGroupBack(newVoiceGroupBack);
                setChangesSavedState(false);
            }
        }
    }
}

void MainWindow::all_on_spinBox_valueChanged(int newValue)
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QSpinBox *senderSpinBox = qobject_cast<QSpinBox *>(sender());

        // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
        if(senderSpinBox == nullptr)
            throw "Pointer conversion to spin box failed";

        if(senderSpinBox == ui->spinBox_repeatTimeMin)
        {
            uint32_t newRepeatTimeMin = uint32_t(newValue);
            if(getSelectedPlaylist()->getInterpretedMinRepeatTime() != newRepeatTimeMin)
            {
                getSelectedPlaylist()->setInterpretedMinRepeatTime(newRepeatTimeMin);
                setChangesSavedState(false);
            }
        }
        else if(senderSpinBox == ui->spinBox_repeatTimeMax)
        {
            uint32_t newRepeatTimeMax = uint32_t(newValue);
            if(getSelectedPlaylist()->getInterpretedMaxRepeatTime() != newRepeatTimeMax)
            {
                getSelectedPlaylist()->setInterpretedMaxRepeatTime(newRepeatTimeMax);
                setChangesSavedState(false);
            }
        }
        else if(senderSpinBox == ui->spinBox_repeatCountMin)
        {
            uint32_t newRepeatCountMin = uint32_t(newValue);
            if(getSelectedPlaylist()->getInterpretedMinRepeatCount() != newRepeatCountMin)
            {
                getSelectedPlaylist()->setInterpretedMinRepeatCount(newRepeatCountMin);
                setChangesSavedState(false);
            }
        }
        else if(senderSpinBox == ui->spinBox_repeatCountMax)
        {
            uint32_t newRepeatCountMax = uint32_t(newValue);
            if(getSelectedPlaylist()->getInterpretedMaxRepeatCount() != newRepeatCountMax)
            {
                getSelectedPlaylist()->setInterpretedMaxRepeatCount(newRepeatCountMax);
                setChangesSavedState(false);
            }
        }
        else if(senderSpinBox == ui->spinBox_randomPlaylistWeight)
        {
            uint32_t newRandomPlalistWeight = uint32_t(newValue);
            if(getSelectedPlaylist()->getInterpretedRandomPlaylistWeight() != newRandomPlalistWeight)
            {
                getSelectedPlaylist()->setInterpretedRandomPlaylistWeight(newRandomPlalistWeight);
                setChangesSavedState(false);
            }
        }
        else if(senderSpinBox == ui->spinBox_randomWeight)
        {
            uint32_t newRandomWeight = uint32_t(newValue);
            if(getSelectedSoundContainer()->getInterpretedRandomWeight() != newRandomWeight)
            {
                getSelectedSoundContainer()->setInterpretedRandomWeight(newRandomWeight);
                setChangesSavedState(false);
            }
        }
    }
}

void MainWindow::all_on_doubleSpinBox_valueChanged(double newValue)
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QDoubleSpinBox *senderDoubleSpinBox = qobject_cast<QDoubleSpinBox *>(sender());

        // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
        if(senderDoubleSpinBox == nullptr)
            throw "Pointer conversion to double spin box failed";

        // Since floats in this editor are only ever read, written, and changed incrementally (no floating-point math is performed)
        // comparing floats directly in this context is safe and ==/!= related warning can be ignored. There is conversion between
        // double and float but this is always performed using the same method and with values inder the 32-bit float limit and
        // therefore gives consistant results, so it is also a non-issue. std::equal_to is used for float comparisons so that this
        // warning is effectively supressed (the warning has no ID so its impossible to supress directly)

        if(senderDoubleSpinBox == ui->doubleSpinBox_length)
        {
            float newLength = float(newValue);
            if(!std::equal_to<float>()(getSelectedPlaylist()->getInterpretedLength(), newLength))
            {
                getSelectedPlaylist()->setInterpretedLength(float(newValue));
                float test1 = 2;
                float test2 = 3;
                std::equal_to<float>()(test1, test2);
            }
        }
        else if(senderDoubleSpinBox == ui->doubleSpinBox_releaseTime)
        {
            float newReleaseTime = float(newValue);
            if(!std::equal_to<float>()(getSelectedPlaylist()->getInterpretedReleaseTime(), newReleaseTime))
            {
                getSelectedPlaylist()->setInterpretedReleaseTime(newReleaseTime);
                setChangesSavedState(false);
            }
        }
        else if(senderDoubleSpinBox == ui->doubleSpinBox_amplitudeFactor)
        {
            float newAmplitudeFactor = float(newValue);
            if(!std::equal_to<float>()(getSelectedSoundContainer()->getInterpretedAmplitudeFactor(), newAmplitudeFactor))
            {
                getSelectedSoundContainer()->setInterpretedAmplitudeFactor(newAmplitudeFactor);
                setChangesSavedState(false);
            }
        }
        else if(senderDoubleSpinBox == ui->doubleSpinBox_dopplerRatio)
        {
            float newDopplerRatio = float(newValue);
            if(!std::equal_to<float>()(getSelectedSoundContainer()->getInterpretedDopplerRatio(), newDopplerRatio))
            {
                getSelectedSoundContainer()->setInterpretedDopplerRatio(newDopplerRatio);
                setChangesSavedState(false);
            }
        }
        else if(senderDoubleSpinBox == ui->doubleSpinBox_playbackFrequence)
        {
            float newPlaybackFrequence = float(newValue);
            if(!std::equal_to<float>()(getSelectedSoundContainer()->getInterpretedPlaybackFrequence(), newPlaybackFrequence))
            {
                getSelectedSoundContainer()->setInterpretedPlaybackFrequence(newPlaybackFrequence);
                setChangesSavedState(false);
            }
        }
        else if(senderDoubleSpinBox == ui->doubleSpinBox_start)
        {
            float newStart = float(newValue);
            if(!std::equal_to<float>()(getSelectedSoundContainer()->getInterpretedStart(), newStart))
            {
                getSelectedSoundContainer()->setInterpretedStart(newStart);
                setChangesSavedState(false);
            }
        }
        else if(senderDoubleSpinBox == ui->doubleSpinBox_end)
        {
            float newEnd = float(newValue);
            if(!std::equal_to<float>()(getSelectedSoundContainer()->getInterpretedEnd(), newEnd))
            {
                getSelectedSoundContainer()->setInterpretedEnd(newEnd);
                setChangesSavedState(false);
            }
        }
        else if(senderDoubleSpinBox == ui->doubleSpinBox_maskSample)
        {
            float newMaskSample = float(newValue);
            if(!std::equal_to<float>()(getSelectedSoundContainer()->getInterpretedMaskSample(), newMaskSample))
            {
                getSelectedSoundContainer()->setInterpretedMaskSample(newMaskSample);
                setChangesSavedState(false);
            }
        }
    }
}

void MainWindow::all_on_checkBox_stateChanged(int newState)
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QCheckBox *senderCheckBox = qobject_cast<QCheckBox *>(sender());

        // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
        if(senderCheckBox == nullptr)
            throw "Pointer conversion to check box failed";

        if(senderCheckBox == ui->checkBox_parentBehaviorOverride)
        {
            bool newParentBehaviorOverride = newState;
            if(getSelectedPlaylist()->getInterpretedOverrideParentBehaviors() != newParentBehaviorOverride)
            {
                getSelectedPlaylist()->setInterpretedOverrideParentBehaviors(newParentBehaviorOverride);
                setChangesSavedState(false);
            }
        }
        else if(senderCheckBox == ui->checkBox_3D)
        {
            bool new3D = newState;
            if(getSelectedSoundContainer()->getInterpretedIs3D() != new3D)
            {
                getSelectedSoundContainer()->setInterpretedIs3D(new3D);
                setChangesSavedState(false);
            }
        }
        else if(senderCheckBox == ui->checkBox_loop)
        {
            bool newLoop = newState;
            if(getSelectedSoundContainer()->getInterpretedIsLooped() != newLoop)
            {
                getSelectedSoundContainer()->setInterpretedIsLooped(newLoop);
                setChangesSavedState(false);
            }
        }
        else if(senderCheckBox == ui->checkBox_speedVolumeTrigger)
        {
            bool newSpeedVolumeTrigger = newState;
            if(getSelectedSoundContainer()->getInterpretedUsingSpeedVolumeTrigger() != newSpeedVolumeTrigger)
            {
                getSelectedSoundContainer()->setInterpretedUsingSpeedVolumeTrigger(newSpeedVolumeTrigger);
                setChangesSavedState(false);
            }
        }
        else if(senderCheckBox == ui->checkBox_effectContainerDataEdit)
        {
            if(!mGaveEffectContainerWarning)
            {
                QMessageBox effConWarning;
                effConWarning.setIcon(QMessageBox::Warning);
                effConWarning.setText(MSG_EFF_CON_EDIT_WARN_TXT);
                effConWarning.setInformativeText(MSG_EFF_CON_EDIT_WARN_INFO_TXT);
                effConWarning.setStandardButtons(QMessageBox::Ok);
                effConWarning.setDefaultButton(QMessageBox::Ok);
                QApplication::beep();
                effConWarning.exec();

                mGaveEffectContainerWarning = true;
            }

            ui->lineEdit_effectContainerCount->setReadOnly(!newState);
            ui->plainTextEdit_effectContainerDataRaw->setReadOnly(!newState);
            ui->plainTextEdit_effectContainerDataString->setReadOnly(!newState);
        }
    }
}

void MainWindow::all_on_toolButton_clicked()
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QToolButton *senderToolbutton = qobject_cast<QToolButton *>(sender());

        // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
        if(senderToolbutton == nullptr)
            throw "Pointer conversion to tool button failed";

        // Playlists
        if(senderToolbutton == ui->toolButton_playlistsAdd)
        {
            // Save TreeWidgetExpanded states
            storeTreeItemExpandedStates();

            if(ui->treeWidget_playlists->currentItem() == nullptr)
            {
                mBSCPtr->getPlaylistsR().append(BSC::Playlist(false));
                updatePlaylists();
                ui->treeWidget_playlists->setCurrentItem(ui->treeWidget_playlists->topLevelItem(ui->treeWidget_playlists->topLevelItemCount() - 1));

            }
            else
            {
                PlaylistItemMap selectedItemMap = PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get());
                selectedItemMap.getTargetPlaylistList()->insert(selectedItemMap.getTargetIndex() + 1, BSC::Playlist(!selectedItemMap.targetIsTopLevel()));
                updatePlaylists();
                ui->treeWidget_playlists->setCurrentItem(PlaylistItemMap::getItemRelativeToMap(ui->treeWidget_playlists, selectedItemMap, selectedItemMap.targetDepth(),
                                                                                               selectedItemMap.getTargetIndex() + 1));
            }
        }
        else if(senderToolbutton == ui->toolButton_playlistsRemove)
        {
            assert(ui->treeWidget_playlists->currentItem() != nullptr); // The remove button should be inactive/disabled if nothing is selected

            // Save TreeWidgetExpanded states
            storeTreeItemExpandedStates();

            PlaylistItemMap selectedItemMap = PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get());
            selectedItemMap.getTargetPlaylistList()->removeAt(selectedItemMap.getTargetIndex());
            updatePlaylists();

            if(mBSCPtr->hasPlaylists())
            {
                if(selectedItemMap.targetIsTopLevel())
                    ui->treeWidget_playlists->setCurrentItem(PlaylistItemMap::getItemRelativeToMap(ui->treeWidget_playlists, selectedItemMap, 0, selectedItemMap.getTargetIndex() - 1));
                else if(selectedItemMap.getTargetParentPlaylist()->subPlaylistCount() == 0)
                    ui->treeWidget_playlists->setCurrentItem(PlaylistItemMap::getItemRelativeToMap(ui->treeWidget_playlists, selectedItemMap, selectedItemMap.targetDepth() - 1,
                                                                                                   selectedItemMap.getTargetParentIndex()));
                else
                    ui->treeWidget_playlists->setCurrentItem(PlaylistItemMap::getItemRelativeToMap(ui->treeWidget_playlists, selectedItemMap, selectedItemMap.targetDepth(),
                                                                                                   selectedItemMap.getTargetIndex() - 1));
            }
            else
                updateEntirePlaylist();

             ui->lineEdit_playlistCount->setText(QString::number(mBSCPtr->getPlaylistsV().length())); // Updates playlist count in ui
        }
        else if(senderToolbutton == ui->toolButton_playlistsCopy)
        {
            assert(ui->treeWidget_playlists->currentItem() != nullptr); // The copy button should be inactive/disabled if nothing is selected

            // Save TreeWidgetExpanded states
            storeTreeItemExpandedStates();

            PlaylistItemMap selectedItemMap = PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get());
            selectedItemMap.getTargetPlaylistList()->insert(selectedItemMap.getTargetIndex() + 1, *selectedItemMap.getTargetPlaylist()); // Dereferrence pointer so copy is made
            updatePlaylists();

            ui->treeWidget_playlists->setCurrentItem(PlaylistItemMap::getItemRelativeToMap(ui->treeWidget_playlists, selectedItemMap, selectedItemMap.targetDepth(),
                                                                                           selectedItemMap.getTargetIndex() + 1));

             ui->lineEdit_playlistCount->setText(QString::number(mBSCPtr->getPlaylistsV().length())); // Updates playlist count in ui
        }
        else if(senderToolbutton == ui->toolButton_playlistsOut)
        {
            assert(ui->treeWidget_playlists->currentItem() != nullptr || !ui->treeWidget_playlists->currentItem()->parent()); // The out button should be inactive/disabled if nothing
                                                                                                                              // is selected or selection is a top level
            // Save TreeWidgetExpanded states
            storeTreeItemExpandedStates();

            PlaylistItemMap selectedItemMap = PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get());

            selectedItemMap.getTargetParentPlaylistList()->insert(selectedItemMap.getTargetParentIndex() + 1, *selectedItemMap.getTargetPlaylist()); // Dereferrence pointer so copy is made
            selectedItemMap.getTargetPlaylistList()->removeAt(selectedItemMap.getTargetIndex()); // Delete original playlist
            updatePlaylists();

            ui->treeWidget_playlists->setCurrentItem(PlaylistItemMap::getItemRelativeToMap(ui->treeWidget_playlists, selectedItemMap, selectedItemMap.targetDepth() - 1,
                                                                                           selectedItemMap.getTargetIndex() + 1));
        }
        else if(senderToolbutton == ui->toolButton_playlistsIn)
        {
            assert(ui->treeWidget_playlists->currentItem() != nullptr); // The in button should be inactive/disabled if nothing is selected

            // Save TreeWidgetExpanded states
            storeTreeItemExpandedStates();

            PlaylistItemMap selectedItemMap = PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get());

            assert(selectedItemMap.getTargetIndex() > 0); // The in button should be inactive/disabled if the selection has nothing above it

            // Simply using "[]" here returns a nullptr (must have to do with being a list of pointers) so "operator[]()" must be used instead
            selectedItemMap.getTargetPlaylistList()->operator[](selectedItemMap.getTargetIndex() - 1).getSubPlaylistsR().append((*selectedItemMap.getTargetPlaylist()));
            selectedItemMap.getTargetPlaylistList()->removeAt(selectedItemMap.getTargetIndex()); // Delete original playlist
            updatePlaylists();

            // Item playlist went into
            QTreeWidgetItem* aboveItem = PlaylistItemMap::getItemRelativeToMap(ui->treeWidget_playlists, selectedItemMap, selectedItemMap.targetDepth(),
                                                                     selectedItemMap.getTargetIndex() - 1);

            // Selec the child that the playlist now is
            ui->treeWidget_playlists->setCurrentItem(aboveItem->child(aboveItem->childCount() - 1));
        }
        else if(senderToolbutton == ui->toolButton_playlistsUp)
        {
            assert(ui->treeWidget_playlists->currentItem() != nullptr); // The move up button should be inactive/disabled if nothing is selected

            // Save TreeWidgetExpanded states
            storeTreeItemExpandedStates();

            PlaylistItemMap selectedItemMap = PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get());

            assert(selectedItemMap.getTargetIndex() > 0); // The move up button should be inactive/disabled if the selected item if the top entry

            selectedItemMap.getTargetPlaylistList()->move(selectedItemMap.getTargetIndex(), selectedItemMap.getTargetIndex() - 1);
            updatePlaylists();

            ui->treeWidget_playlists->setCurrentItem(PlaylistItemMap::getItemRelativeToMap(ui->treeWidget_playlists, selectedItemMap, selectedItemMap.targetDepth(),
                                                                                           selectedItemMap.getTargetIndex() - 1));
        }
        else if(senderToolbutton == ui->toolButton_playlistsDown)
        {
            assert(ui->treeWidget_playlists->currentItem() != nullptr); // The move down button should be inactive/disabled if nothing is selected

            // Save TreeWidgetExpanded states
            storeTreeItemExpandedStates();

            PlaylistItemMap selectedItemMap = PlaylistItemMap(ui->treeWidget_playlists->currentItem(), mBSCPtr.get());

            assert(selectedItemMap.getTargetIndex() < selectedItemMap.getTargetPlaylistList()->length() - 1); // The move down button should be inactive/disabled if the
                                                                                                              // selected item if the bottom entry
            selectedItemMap.getTargetPlaylistList()->move(selectedItemMap.getTargetIndex(), selectedItemMap.getTargetIndex() + 1);
            updatePlaylists();

            ui->treeWidget_playlists->setCurrentItem(PlaylistItemMap::getItemRelativeToMap(ui->treeWidget_playlists, selectedItemMap, selectedItemMap.targetDepth(),
                                                                                           selectedItemMap.getTargetIndex() + 1));
        }

        // Effect Behaviors
        else if(senderToolbutton == ui->toolButton_effectBehaviorsAdd)
        {
            int selectedIndex = ui->tableWidget_effectBehaviors->currentRow();

            if(selectedIndex < 0)
            {
                getSelectedPlaylist()->addInterpretedEffectBehavior(BSC::Playlist::EFFECT_BEHAVIOR_DEFAULT);
                updateEffectBehaviors();
                ui->tableWidget_effectBehaviors->setCurrentCell(getSelectedPlaylist()->getInterpretedEffectBehaviorsV().length() - 1, 0);

            }
            else
            {
                getSelectedPlaylist()->addInterpretedEffectBehavior(BSC::Playlist::EFFECT_BEHAVIOR_DEFAULT, selectedIndex + 1);
                updateEffectBehaviors();
                ui->tableWidget_effectBehaviors->setCurrentCell(selectedIndex + 1, 0);
            }
        }
        else if(senderToolbutton == ui->toolButton_effectBehaviorsRemove)
        {
            int selectedIndex = ui->tableWidget_effectBehaviors->currentRow();
            assert(selectedIndex > -1); // The remove button should be inactive/disabled if nothing is selected

            getSelectedPlaylist()->removeInterpretedEffectBehavior(selectedIndex);
            updateEffectBehaviors();
            if(ui->tableWidget_effectBehaviors->rowCount() > 0)
                ui->tableWidget_effectBehaviors->setCurrentCell(selectedIndex - 1, 0);
        }
        else if(senderToolbutton == ui->toolButton_effectBehaviorsCopy)
        {
            int selectedIndex = ui->tableWidget_effectBehaviors->currentRow();
            assert(selectedIndex > -1); // The copy button should be inactive/disabled if nothing is selected

            getSelectedPlaylist()->copyInterpretedEffectBehavior(selectedIndex, selectedIndex + 1);
            updateEffectBehaviors();
            ui->tableWidget_effectBehaviors->setCurrentCell(selectedIndex + 1, 0);
        }
        else if(senderToolbutton == ui->toolButton_effectBehaviorsUp)
        {
            int selectedIndex = ui->tableWidget_effectBehaviors->currentRow();
            assert(selectedIndex > 0); // The move up button should be inactive/disabled if nothing is selected or the selected item if the top entry

            getSelectedPlaylist()->moveInterpretedEffectBehavior(selectedIndex, selectedIndex - 1);
            updateEffectBehaviors();
            ui->tableWidget_effectBehaviors->setCurrentCell(selectedIndex - 1, 0);
        }
        else if(senderToolbutton == ui->toolButton_effectBehaviorsDown)
        {
            int selectedIndex = ui->tableWidget_effectBehaviors->currentRow();
            assert(selectedIndex > -1 && selectedIndex < ui->tableWidget_effectBehaviors->rowCount() - 1); // The move down button should be inactive/disabled
                                                                                                        // if nothing is selected or the selected item if the bottom entry
            getSelectedPlaylist()->moveInterpretedEffectBehavior(selectedIndex, selectedIndex + 1);
            updateEffectBehaviors();
            ui->tableWidget_effectBehaviors->setCurrentCell(selectedIndex + 1, 0);
        }

        // Sound Containers
        else if(senderToolbutton == ui->toolButton_soundContainerAdd)
        {
            int selectedIndex = ui->comboBox_soundContainer->currentIndex();
            if(selectedIndex < 0)
            {
                getSelectedPlaylist()->addSoundContainer(BSC::SoundContainer());
                updateSoundContainers();
                ui->comboBox_soundContainer->setCurrentIndex(getSelectedPlaylist()->soundContainerCount() - 1);

            }
            else
            {
                getSelectedPlaylist()->addSoundContainer(BSC::SoundContainer(), selectedIndex + 1);
                updateSoundContainers();
                ui->comboBox_soundContainer->setCurrentIndex(selectedIndex + 1);
            }

            updateSoundContainerGroup(); // Must call manually to ensure update since the above may have moved the selected index
                                         // to the desired index so using "setCurrentIndex()" on the same index won't trigger the corresponding slot
        }
        else if(senderToolbutton == ui->toolButton_soundContainerRemove)
        {
            int selectedIndex = ui->comboBox_soundContainer->currentIndex();
            assert(selectedIndex > -1); // The remove button should be inactive/disabled if nothing is selected

            getSelectedPlaylist()->removeSoundContainer(selectedIndex);
            updateSoundContainers();
            if(ui->comboBox_soundContainer->count() > 0)
                ui->comboBox_soundContainer->setCurrentIndex(selectedIndex - 1);

            updateSoundContainerGroup(); // Must call manually to ensure update since the above may have moved the selected index
                                         // to the desired index so using "setCurrentIndex()" on the same index won't trigger the corresponding slot
        }
        else if(senderToolbutton == ui->toolButton_soundContainerCopy)
        {
            int selectedIndex = ui->comboBox_soundContainer->currentIndex();
            assert(selectedIndex > -1); // The copy button should be inactive/disabled if nothing is selected

            getSelectedPlaylist()->copySoundContainer(selectedIndex, selectedIndex + 1);
            updateSoundContainers();
            ui->comboBox_soundContainer->setCurrentIndex(selectedIndex + 1);

            updateSoundContainerGroup(); // Must call manually to ensure update since the above may have moved the selected index
                                         // to the desired index so using "setCurrentIndex()" on the same index won't trigger the corresponding slot

        }
        else if(senderToolbutton == ui->toolButton_soundContainerUp)
        {
            int selectedIndex = ui->comboBox_soundContainer->currentIndex();
            assert(selectedIndex > 0); // The move up button should be inactive/disabled if nothing is selected or the selected item if the top entry

            getSelectedPlaylist()->moveSoundContainer(selectedIndex, selectedIndex - 1);
            updateSoundContainers();
            ui->comboBox_soundContainer->setCurrentIndex(selectedIndex - 1);

            updateSoundContainerGroup(); // Must call manually to ensure update since the above may have moved the selected index
                                         // to the desired index so using "setCurrentIndex()" on the same index won't trigger the corresponding slot
        }
        else if(senderToolbutton == ui->toolButton_soundContainerDown)
        {
            int selectedIndex = ui->comboBox_soundContainer->currentIndex();
            assert(selectedIndex > -1 && selectedIndex <= ui->comboBox_soundContainer->count() - 1); // The move down button should be inactive/disabled
                                                                                                        // if nothing is selected or the selected item if the bottom entry
            getSelectedPlaylist()->moveSoundContainer(selectedIndex, selectedIndex + 1);
            updateSoundContainers();
            ui->comboBox_soundContainer->setCurrentIndex(selectedIndex + 1);

            updateSoundContainerGroup(); // Must call manually to ensure update since the above may have moved the selected index
                                         // to the desired index so using "setCurrentIndex()" on the same index won't trigger the corresponding slot
        }

        // Linked Files
        else if(senderToolbutton == ui->toolButton_linkedFilesAdd)
        {
            BSC::SoundContainer* currentSoundContainer = getSelectedSoundContainer();
            int selectedIndex = ui->tableWidget_linkedFiles->currentRow();

            if(selectedIndex < 0)
            {
                currentSoundContainer->addInterpretedLinkedFile(BSC::SoundContainer::LINKED_FILE_DEFAULT);
                updateLinkedFiles();
                ui->tableWidget_linkedFiles->setCurrentCell(currentSoundContainer->getInterpretedLinkedFilesV().length() - 1, 0);

            }
            else
            {
                currentSoundContainer->addInterpretedLinkedFile(BSC::SoundContainer::LINKED_FILE_DEFAULT, selectedIndex + 1);
                updateLinkedFiles();
                ui->tableWidget_linkedFiles->setCurrentCell(selectedIndex + 1, 0);
            }
        }
        else if(senderToolbutton == ui->toolButton_linkedFilesRemove)
        {
            BSC::SoundContainer* currentSoundContainer = getSelectedSoundContainer();
            int selectedIndex = ui->tableWidget_linkedFiles->currentRow();
            assert(selectedIndex > -1); // The remove button should be inactive/disabled if nothing is selected

            currentSoundContainer->removeInterpretedLinkedFile(selectedIndex);
            updateLinkedFiles();
            if(ui->tableWidget_linkedFiles->rowCount() > 0)
                ui->tableWidget_linkedFiles->setCurrentCell(selectedIndex - 1, 0);
        }
        else if(senderToolbutton == ui->toolButton_linkedFilesCopy)
        {
            BSC::SoundContainer* currentSoundContainer = getSelectedSoundContainer();
            int selectedIndex = ui->tableWidget_linkedFiles->currentRow();
            assert(selectedIndex > -1); // The copy button should be inactive/disabled if nothing is selected

            currentSoundContainer->copyInterpretedLinkedFile(selectedIndex, selectedIndex + 1);
            updateLinkedFiles();
            ui->tableWidget_linkedFiles->setCurrentCell(selectedIndex + 1, 0);
        }
        else if(senderToolbutton == ui->toolButton_linkedFilesUp)
        {
            BSC::SoundContainer* currentSoundContainer = getSelectedSoundContainer();
            int selectedIndex = ui->tableWidget_linkedFiles->currentRow();
            assert(selectedIndex > 0); // The move up button should be inactive/disabled if nothing is selected or the selected item if the top entry

            currentSoundContainer->moveInterpretedLinkedFile(selectedIndex, selectedIndex - 1);
            updateLinkedFiles();
            ui->tableWidget_linkedFiles->setCurrentCell(selectedIndex - 1, 0);
        }
        else if(senderToolbutton == ui->toolButton_linkedFilesDown)
        {
            BSC::SoundContainer* currentSoundContainer = getSelectedSoundContainer();
            int selectedIndex = ui->tableWidget_linkedFiles->currentRow();
            assert(selectedIndex > -1 && selectedIndex <= ui->tableWidget_linkedFiles->rowCount() - 1); // The move down button should be inactive/disabled
                                                                                                        // if nothing is selected or the selected item if the bottom entry
            currentSoundContainer->moveInterpretedLinkedFile(selectedIndex, selectedIndex + 1);
            updateLinkedFiles();
            ui->tableWidget_linkedFiles->setCurrentCell(selectedIndex + 1, 0);
        }

        // Update enable states
        updateSecondaryWidgetStates((*Qx::List::getSubListThatContains(qobject_cast<QWidget*>(senderToolbutton),
                                                                          {&mEnableGroupSoundCon, &mEnableGroupTopLevel, &mEnableGroupEffectBeh,
                                                                           &mEnableGroupPlaylistSel, &mEnableGroupPlaylistSet})));

        // Update save check
        setChangesSavedState(false); // All toolButton actions change the file in someway
    }   
}

void MainWindow::all_on_plainTextEdit_textChanged()
{
    if(mSlotsEnabled)
    {
        // Get the object that called this slot
        QPlainTextEdit *senderPlainTextEdit = qobject_cast<QPlainTextEdit *>(sender());

        // Ensure the signal that trigged this slot belongs to the above class by checking for null pointer
        if(senderPlainTextEdit == nullptr)
            throw "Pointer conversion to plain text edit failed";

        // Get new text
        QString newText = senderPlainTextEdit->toPlainText();

        if(senderPlainTextEdit == ui->plainTextEdit_effectContainerDataRaw)
        {
            if(!mECLoopProtection)
            {
                if(mEffConRawInputGuaranteedValid)
                {
                    mECLoopProtection = true;

                    QByteArray dataRaw = Qx::ByteArray::RAWFromStringHex(newText);
                    QString dataText = Qx::String::fromByteArray(dataRaw);

                    if(getSelectedSoundContainer()->getInterpretedEffectContainerData() != dataText)
                    {
                        getSelectedSoundContainer()->setInterpretedEffectContainerData(dataText);
                        setChangesSavedState(false);
                    }
                    ui->plainTextEdit_effectContainerDataString->setPlainText(dataText);

                    // Push cursor to end
                    ui->plainTextEdit_effectContainerDataRaw->moveCursor(QTextCursor::End);

                    mEffConRawInputGuaranteedValid = false; // Clear valid flag
                }
                else
                {
                    // Ensure input is valid
                    mEffConRawInputGuaranteedValid = true;
                    senderPlainTextEdit->setPlainText(Qx::String::stripToHexOnly(newText).toUpper());
                }
            }

            mECLoopProtection = false;
        }
        else if(senderPlainTextEdit == ui->plainTextEdit_effectContainerDataString)
        {
            if(!mECLoopProtection)
            {
                mECLoopProtection = true;
                QByteArray dataRaw = Qx::ByteArray::RAWFromString(newText);
                QString dataText = dataRaw.toHex().toUpper();

                if(getSelectedSoundContainer()->getInterpretedEffectContainerData() != dataText)
                {
                    getSelectedSoundContainer()->setInterpretedEffectContainerData(newText);
                    setChangesSavedState(false);
                }
                ui->plainTextEdit_effectContainerDataRaw->setPlainText(dataText);
            }

            mECLoopProtection = false;
        }
    }
}

//===============================================================================================================
// PLAYLIST ITEM MAP
//===============================================================================================================

//-Constructor---------------------------------------------------------------------------------------------------
MainWindow::PlaylistItemMap::PlaylistItemMap(const QTreeWidgetItem* treeItem, BSC* headBSC)
    : mOriginalTreeItem(treeItem)
{
    // Index code to unravel
    QString indexCode = treeItem->text(0).mid(treeItem->text(0).indexOf(BSC::Playlist::PLAYLIST_INDEX_SEP) + BSC::Playlist::PLAYLIST_INDEX_SEP.length());

    // Number of segments
    int segments = indexCode.count(BSC::Playlist::SUB_INDEX_LABEL_SEP) + 1;

    assert(segments > 0);

    // Unravel
    if(segments == 1)
    {
        mIndexPath.append(indexCode.toInt());
        mPlaylistPath.append(&headBSC->getPlaylistsR()[indexCode.toInt()]);
        mPlaylistListPath.append(&headBSC->getPlaylistsR());
    }
    else
    {
        // Step through first segment
        int end = indexCode.indexOf(BSC::Playlist::SUB_INDEX_LABEL_SEP);
        mIndexPath.append(indexCode.left(end).toInt());
        mPlaylistListPath.append(&headBSC->getPlaylistsR());
        mPlaylistPath.append(&headBSC->getPlaylistsR()[indexCode.left(end).toInt()]);

        // Step through middle segments
        for(int i = 1; i < segments - 1; i++)
        {
            int start = end + 1;
            end = indexCode.indexOf(BSC::Playlist::SUB_INDEX_LABEL_SEP, start);
            mIndexPath.append(indexCode.mid(start, end - start).toInt());
            mPlaylistListPath.append(&mPlaylistPath.last()->getSubPlaylistsR());
            mPlaylistPath.append(&mPlaylistPath.last()->getSubPlaylistsR()[indexCode.mid(start, end - start).toInt()]);
        }

        // Step through last segment
        mIndexPath.append(indexCode.mid(end + 1).toInt());
        mPlaylistListPath.append((&mPlaylistPath.last()->getSubPlaylistsR()));
        mPlaylistPath.append(&mPlaylistPath.last()->getSubPlaylistsR()[indexCode.mid(end + 1).toInt()]);
    }
}

//-Class Functions--------------------------------------------------------------------------------------------
//Public:
QTreeWidgetItem* MainWindow::PlaylistItemMap::getItemRelativeToMap(QTreeWidget* treeWidget, const PlaylistItemMap itemMap, int depth, int target)
{
    assert(depth >= 0);

    // Tree item to return
    QTreeWidgetItem* relativeItem;

    if(depth == 0)
        relativeItem = treeWidget->topLevelItem(target);
    else
    {
        // Get top level item
        relativeItem = treeWidget->topLevelItem(itemMap.getTopLevelIndex());

        // Iterate through middle children
        for(int i = 1; i < depth; i++)
            relativeItem = relativeItem->child(itemMap.getIndexAtLevel(i));

        // Get target
        relativeItem = relativeItem->child(target);
    }

    return relativeItem;
}

//-Instance Functions--------------------------------------------------------------------------------------------
//Public:
const QTreeWidgetItem* MainWindow::PlaylistItemMap::getOriginalTreeItem() const { return mOriginalTreeItem; }
int MainWindow::PlaylistItemMap::getIndexAtLevel(int level) const { return mIndexPath.value(level); }
BSC::Playlist* MainWindow::PlaylistItemMap::getPlaylistAtLevel(int level) const { return mPlaylistPath.value(level); }
QList<BSC::Playlist>* MainWindow::PlaylistItemMap::getPlaylistListAtLevel(int level) const { return mPlaylistListPath.value(level); }
int MainWindow::PlaylistItemMap::getTopLevelIndex() const { return mIndexPath.first(); }
BSC::Playlist* MainWindow::PlaylistItemMap::getTopLeveLPlaylist() const { return mPlaylistPath.first(); }
QList<BSC::Playlist>* MainWindow::PlaylistItemMap::getTopLevelPlaylistList() const { return mPlaylistListPath.first(); }
int MainWindow::PlaylistItemMap::getTargetIndex() const { return mIndexPath.last(); }
BSC::Playlist* MainWindow::PlaylistItemMap::getTargetPlaylist() const { return mPlaylistPath.last(); }
QList<BSC::Playlist>* MainWindow::PlaylistItemMap::getTargetPlaylistList() const { return mPlaylistListPath.last(); }

int MainWindow::PlaylistItemMap::getTargetParentIndex() const
{
    if(mIndexPath.length() < 2)
        return -1;
    else
        return mIndexPath.value(mIndexPath.length() - 2);
}

BSC::Playlist* MainWindow::PlaylistItemMap::getTargetParentPlaylist() const
{
    if(mIndexPath.length() < 2)
        return nullptr;
    else
        return mPlaylistPath.value(mPlaylistPath.length() - 2);
}

QList<BSC::Playlist>* MainWindow::PlaylistItemMap::getTargetParentPlaylistList() const
{
    if(mPlaylistListPath.length() < 2)
        return nullptr;
    else
        return mPlaylistListPath.value(mPlaylistListPath.length() - 2);
}

bool MainWindow::PlaylistItemMap::targetIsTopLevel() const { return mIndexPath.length() == 1; }
int MainWindow::PlaylistItemMap::targetDepth() const { return mIndexPath.length() - 1; }
int MainWindow::PlaylistItemMap::length() const { return mIndexPath.length(); }
