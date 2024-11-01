#include <game.h>
#include <newer.h>
#include "levelinfo.h"

extern char CurrentLevel;
extern char CurrentWorld;

// This is pretty much identical to the code in preGame, but the panes here have slightly
// different names, and I'd rather not change the pane names of one of the LYTs
extern "C" void InsertPauseWindowText(void *thing) {
	// Pointer to pauseMenu layout
	m2d::EmbedLayout_c *layout = (m2d::EmbedLayout_c*)(((u8*)thing)+0x70);
	
	// Get our text boxes
	nw4r::lyt::TextBox
		*T_worldNum_00, *T_levelName_00,
		*T_corseNum_00, *T_corsePic_00;
	
	T_worldNum_00 = layout->findTextBoxByName("T_worldNum_00");
	T_levelName_00 = layout->findTextBoxByName("T_levelName_00");
	T_corseNum_00 = layout->findTextBoxByName("T_corseNum_00");
	T_corsePic_00 = layout->findTextBoxByName("T_corsePic_00");

	// Now get the level info from the LevelInfo
	dLevelInfo_c::entry_s *level = dLevelInfo_c::s_info.searchBySlot(CurrentWorld, CurrentLevel);
	if (level) {
		// Get our level name...
		wchar_t convLevelName[160];
		const char *srcLevelName = dLevelInfo_c::s_info.getNameForLevel(level);
		int i = 0;
		while (i < 159 && srcLevelName[i]) {
			convLevelName[i] = srcLevelName[i];
			i++;
		}
		convLevelName[i] = 0;
		// ...and write it to the text box
		T_levelName_00->SetString(convLevelName);

		// Get our level ID
		wchar_t worldNumber[16];
		getWorldNumber(level->displayWorld, worldNumber);
		T_worldNum_00->SetString(worldNumber);
		
		
		wchar_t levelNumber[16];
		getLevelNumber(level->displayLevel, levelNumber);
		if (showCoursePic) {
			T_corseNum_00->SetVisible(false);
			T_corsePic_00->SetVisible(true);
			T_corsePic_00->SetString(levelNumber);
		} else {
			T_corsePic_00->SetVisible(false);
			T_corseNum_00->SetVisible(true);
			T_corseNum_00->SetString(levelNumber);
		}

	} else {
		// We have absolutely no clue where the player is
		T_levelName_00->SetString(L"Unknown Level Name!");
		T_worldNum_00->SetString(L"?");
		T_corseNum_00->SetString(L"?");
	}
}

