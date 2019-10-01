
# BSCWorks

BSCWorks is a somewhat feature rich editor for the Burnt Sound Container format, version 14, that Company of Heroes uses and also provides several tools for handling related files. This editor is about a decade too late to be relevant, but was made to more easily edit sound related aspects of the game in the unreleased quality-of-life mod I develop in my spare time for use with a small group of friends/family that still play the original. I am sharing it here in the hopes that it could be useful the remnants of CoH modders or research purposes, such as a starting point for examining other versions of this format from other Relic Games (i.e. CoH 2, Dawn of War, Warhammer 40k, etc.), or if someone ever decides to make a source port of the original game (if we are lucky enough to ever get the source, instead of just a really random licensed iPad port >>) .

## Features

 - Nearly full editing of Company of Heroes BSC files (format version 14)
 - Standard editor functions (Open, New, Save, Save-as, and so forth though no Undo/Redo as of yet), toolbar, etc.
 - Clearly labeled forms to directly edit each attribute of the BSC, with most of the data interpreted into human-readable fields
 - Fully-featured editing of:
	 - Playlists, including sub-playlists, playlist order, and insertion/removal of new/existing playlists
	 - Playlist settings
	 -  Effect Behaviors, including effect behavior order and insertion/removal of new/existing Effect Behaviors
	 - Sound Container settings
 - Mostly complete editing of Sound Containers including sound container order insertion/removal of new/existing Sound Containers
 - Basic editing of *uninterpreted* Effect Container data
 - Single and batch conversion of both SMF->WAV and WAV->SMF
 - Creation and updating of Speechmanager Caches that performs byte-level validity checking, avoids adding redundant entries and removes any existing ones from the cache

## Usage

Use of BSCWorks is fairly straightforward aside from the obvious experience required in Company of Heroes modding. BSCs can be created, opened, and saved using the File menu or toolbar, while the utilities that don't deal with BSCs directly and instead related formats are accessible at any time from the Tools menu as well as the toolbar. The editor also supports drag and drop to open files (currently for BSCs only).

This format has a general hierarchy as follows:
**Top-level Information -> Playlists -> Sub-playlists & Sound Containers -> Effect Containers -> ?**

where each of these segments resides in a parent segment of the type that proceeds it in this hierarchy.  Navigation is primarily directed by Playlist selection on the left hand side of the tool which will load all of the appropriate child data  for that playlist into the rest of the editor; fields that are not present for a given playlist will be disabled, unless they are later added using the editor function. Switching between Sound Containers follows a similar principle, but only affects the 'Sound Container' view portion of the editor. 

Start Screen:
![BSCWorks start screen](https://i.imgur.com/lFpTqst.png)

Editor with file open:
![BSCWorks with file open](https://i.imgur.com/6YSuQZK.pngg)

The major actions within the editor have tooltips that are triggered by hovering over their corresponding buttons if their function is not clear.

Since Playlists and Sound Containers do not have names or tags associated with them they are only labeled by the order they appear within the file; therefore, it can be somewhat confusing at first exactly what is being altered when re-positioning or adding said entries to a file. The effect of such changes is made more clear by the fact that the currently selected entry will always stay selected as it moves or other entries are moved around it. For example, if you have 'Sound Containerr - 14' selected and select "Move Down", that entry will become 'Sound Container - 15' and remain selected, while what was 'Sound Container - 15' will now be 'Sound Container - 14'.

All variable options for BSCs, other than Linked-Files and Effect Behaviors, such as Emitters and Voice Groups are all present in the editor via combo boxes and do not need to be entered by hand. If you have created custom variants of these members then the editor will not support choosing those directly, though doing such a thing is not very common. With some work it would be possible to have the editor load a list of this entries via a user-editable database instead of hard-coded enumerations.

It is not 100% clear exactly what every setting changes about a Burnt Sound Container, but most settings are intuitive. Some settings, such as 'Doppler Ratio' or 'Playback Frequency' had question marks in original notes I used to help develop this utility and were slightly reinterpreted by myself based on my familiarity with the game; therefore, the effect these values have of the BSC is not fully understood and much more experimentation is required to ensure these labels are accurate.

## Limitations
There are still considerable unknowns regarding the BSC format that unfortunately impose some limitations on this editor. Additionally, since for the time being I only need to use this editor personally for editing certain partitions of BSCs, some features are more polished than others:

 - The Effect Container portion of BSC files has not be fully reversed engineered so the section of the editor that handles them is an extremely minimal hex-editor that can manipulates the data in a completely uninterpreted state. A few pieces of the Effect Containers can be interpreted, but at this point in-time the data was left completely grouped because of how small that portion is. The primary purpose of the built-in hex-editor is to make it clear which segment of each BSC is related to Effect Container data so that the data may be more easily edited with a more robust editor, given how bare-bones the it is.
 - While the Effect Container data is not interpreted, and therefore its length is initially unknown, I developed a process that can determine at exactly which byte each Effect Container ends for any given file. See the source for details, this is carried out by the functions:
	 -   int  BSC::SoundContianer::findEffectContainerEndMidSC(QByteArray  &fullData,  int  startOffset);
	  - int  BSC::SoundContianer::findEffectContainerEndLastSC(QByteArray  &fullData,  int  startOffset);
 - The built-in hex-editor does not ignore end-line (\n) characters and does not display extended ASCII codes as periods like most proper editors do
 - The 5-byte segment between the BSC file signatures (referred to as "Tweener" in the source) is completely uninterpreted. It typically is completely null (00 00 00 00 00), but it is known that this value may change when multiple playlists are present in a given file; because of this, while the editor can fully manipulate the playlists themselves, I cannot guarantee that a file with altered playlist positions/count will be loaded by the game correctly. Try starting with a file that already has playlists in the arrangement you need or experiment with preexisting BSCs.
- The editor has some level of bounds checking but it is not fully-inclusive given some of the previously mentioned unknowns, so files that are moderately corrupted in a particular manner may not be detected as such and will crash the editor.
- Because of a limitation with QTableWidget the lengthy names of 'Linked Files' had to be center-elided instead of spanning multiple lines as that would require sub-classing QTableWidget and implementing a word-wrap like draw function that does not rely on spaces for separation

## To-do

 - [ ] Fully reverse engineer the Effect Container portion of the format
 - [ ] Determine the purpose and implementation of the 5-byte "Tweener" section at the start of the format
 - [ ] Improve the built-in hex-editor to be closer in function to that of a standard hex-editor
 - [ ] Implement a SMF player for quick previews of game related audio
 - [ ] Implement an undo/redo function
 - [ ] Add support for window resizing/fullscreen

## Source
This tool was written in C++ 17 along with Qt 5 and currently only targets Windows Vista and above; however, this tool can easily be ported to Linux with minimal changes, though to what end I am not sure since this is for a Windows game. The source includes an easy-to-use .pro file if you wish to build the application in Qt Creator and the available release was compiled in Qt Creator 4.10.0 using MSVC 2017 and a static compilation of Qt 5.12.3. Other than a C++ 17 capable compiler and Qt 5.12.x+ all files required to compile this software are included.

All functions/variables under the "Qx" (QExtended) namespace belong to a small, personal library I maintain to always have access frequently used functionality in my projects. A pre-compiled static version of this library is provided with the source for this tool. If anyone truly needs it, I can provide the source for this library as well.

## Thanks
Developing this tool would not have been possible without the work of the user **Copernicus** from the now defunct Relicnews Forums (RIP), who figured out most of the format. I simply dug a little bit deeper and then crafted this editor after deciphering his/her original notes and using what else I had determined on my own. His/her original notes are shown below

Source: [http://web.archive.org/web/20140916062828/http://forums.relicnews.com/showthread.php?254902-HOW-TO-Setting-up-Custom-Sounds](http://web.archive.org/web/20140916062828/http://forums.relicnews.com/showthread.php?254902-HOW-TO-Setting-up-Custom-Sounds)

    EA Shark b ?
    0x04 - ascii string - easb
    0x05 - ?? - 0x0000000000
    used when there's more than just one PlayList in the file

    BSC Format (Burnt Sound Container):
    Header:
    0x04 - ascii string -'sbhs'
    0x04 - uint32 - Version, CoH version = 14 (0x0E)
    0x01 - uint8 - PlayList count

    PlayList:
    0x04 - uint32 - index (only for Sub-PlayLists)
    0x04 - uint32 - length of ModeName
    0x^^ - ascii string - ModeName (either Normal, Random, RandomPlaylist or Sequential)
    0x04 - uint32 - max. Instances (0 = infinite, 1-10)
    0x04 - uint32 - Polyphony (min 1, max 10)
    0x04 - uint32 - min. RepeatTime in ms (0 = infinite)
    0x04 - uint32 - max. RepeatTime in ms (0 = infinite)
    0x04 - uint32 - min. RepeatCount (0 = infinite)
    0x04 - uint32 - max. RepeatCount (0 = infinite)
    0x04 - uint32 - RandomWeight(min 1, max 100), weight in Random Playlist
    0x04 - float - Length in ms (0 = normal)
    0x04 - float - ReleaseTime in seconds (0 = no ReleaseTime)
    0x04 - uint32 - number of EffectBehaviors
    **^^
	    0x04 - uint32 - length of EffectBehavior name
	    0x^^ - ascii string - Name of EffectBehavior (see files from EffectBehaviors-directory)
    //**
    0x01 - bool - OverrideParentBehaivors
    0x04 - uint32 - length of emitter name
    0x^^ - ascii string - Emitter name (see emitters.ems for possible values, <none> is also a possible value)
    0x04 - uint32 - number of SoundContainers contained in the playlist
    **^^
	    SoundContainers
    //**
    0x04 - uint32 - number of Sub-PlayLists
    **^^
	    Sub-PlayLists
    //**

    SoundContainer:
    0x04 - uint32 - PlayList-Index of this Container
    0x04 - uint32 - length of VoiceGroupName
    0x^^ - ascii string - VoiceGroupName
    0x01 - bool - Is3D
    0x01 - bool - Looping
    0x01 - bool - UseSoundSpeed/Volume Trigger
    0x04 - uint32 - RandomWeight (min 1, max 100), weight in Random Playlist
    0x04 - float - Amplitude factor
    0x04 - float - Doppler ?
    0x04 - float - PlayingFrequence
    0x04 - float - Start ?
    0x04 - float - End ?
    0x04 - float - MaskSample ?
    0x01 - uint8 - number of linked files
    0x04 - uint32 - length of linked file's name
    0x^^ - ascii string - linked file's name
    0x04 - EffectContainer count
    -- 010100000010 etc. <- THIS IS UNKNOWN!!
    0x?? tables
    0x?? Control Sources with parameters

    ControlSource:
    0x04 - uint32 - Control Source Name length
    0x^^ - string - control Source Name
    0x04 - uint32 - Number of param-tables
    **^^
	    0x04 - uint32 - Number of Parameters
	    **^^
		    0x04 - float - Parameter condition
		    0x04 - float - Parameter modificator for value
	    //**
    //**
