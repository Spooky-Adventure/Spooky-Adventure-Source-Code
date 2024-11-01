#include <game.h>
#include "levelinfo.h"
#include <newer.h>

class PregameLytHandler {
	public:
		m2d::EmbedLayout_c layout;

		nw4r::lyt::Pane *rootPane;

		nw4r::lyt::TextBox
			*T_minus_00, *T_world_00, *T_worldNum_00,
			*T_pictureFont_00, *T_corseNum_00,
			*T_remainder_00, *T_remainder_01, *T_remainder_02, *T_remainder_03,
			*T_remainder_10, *T_remainder_11, *T_remainder_12, *T_remainder_13,
			*T_x_00, *T_x_01, *T_x_02, *T_x_03, *T_x_10, *T_x_11, *T_x_12, *T_x_13,
			*T_x_00_o, *T_x_10_o,
			*T_otasukePlay_00, *T_otasukePlay_01,
			*T_recommend_00, *T_remainder_00_o, *T_remainder_10_o;

		nw4r::lyt::Picture
			*P_Wx_00[9], *P_coin_00, *P_free_00,
			*P_batB_0x[4], *P_bat_00,
			*P_batB_1x[4], *P_bat_01,
			*P_batB_2x[4], *P_bat_02,
			*P_batB_3x[4], *P_bat_03,
			*P_luijiIcon_00_o, *P_luijiIcon_10_o, *P_coinStage_00;

		nw4r::lyt::Pane
			*N_mario_00, *N_luiji_00, *N_kinoB_01, *N_kinoY_00,
			*N_zankiPos_x[4], *N_zanki_00,
			*Null_battPosxP[4], *N_batt_x[4],
			*N_batt, *N_otasukePlay_00;

		u8 layoutLoaded, somethingHasBeenDone, isVisible, hasShownLuigiThing_orSomething;

		u32 currentStateID;

		u32 _2E8;

		u32 countdownToEndabilityCopy, activePlayerCountMultBy4_maybe;
		u32 batteryLevels[4];
		u32 pgCountdown;

		void hijack_loadLevelNumber(); // replaces 80B6BDD0
};

extern char CurrentLevel;
extern char CurrentWorld;

void loadLevelNameAndNumber(m2d::EmbedLayout_c *layout) {
	nw4r::lyt::TextBox
		*T_levelName_00, *T_worldNum_00,
		*T_corseNum_00, *T_pictureFont_00;

	// Setup text panes
	T_levelName_00 = layout->findTextBoxByName("T_levelName_00");
	T_worldNum_00 = layout->findTextBoxByName("T_worldNum_00");
	T_corseNum_00 = layout->findTextBoxByName("T_corseNum_00");
	T_pictureFont_00 = layout->findTextBoxByName("T_pictureFont_00");

	// Work out the thing now
	dLevelInfo_c::entry_s *level = dLevelInfo_c::s_info.searchBySlot(CurrentWorld, CurrentLevel);
	if (level) {
		// Level Name
		wchar_t convLevelName[160];
		const char *srcLevelName = dLevelInfo_c::s_info.getNameForLevel(level);
		int i = 0;
		while (i < 159 && srcLevelName[i]) {
			convLevelName[i] = srcLevelName[i];
			i++;
		}
		convLevelName[i] = 0;
		T_levelName_00->SetString(convLevelName);

		// Level Number
		wchar_t worldNumber[16];
		getWorldNumber(level->displayWorld, worldNumber);
		T_worldNum_00->SetString(worldNumber);
		
		
		wchar_t levelNumber[16];
		getLevelNumber(level->displayLevel, levelNumber);
		if (showCoursePic) {
			T_corseNum_00->SetVisible(false);
			T_pictureFont_00->SetVisible(true);
			T_pictureFont_00->SetString(levelNumber);
		} else {
			T_pictureFont_00->SetVisible(false);
			T_corseNum_00->SetVisible(true);
			T_corseNum_00->SetString(levelNumber);
		}

	} else {
		T_levelName_00->SetString(L"Unknown Level Name!");
		T_worldNum_00->SetString(L"?");
		T_corseNum_00->SetString(L"?");
	}
}

#include "fileload.h"
void PregameLytHandler::hijack_loadLevelNumber() {
	loadLevelNameAndNumber(&layout);
	WriteBMGToTextBox(layout.findTextBoxByName("T_world_00"), GetBMG(), 102, 31, 0); // World text

	nw4r::lyt::Picture *P_worldSample_00;
	P_worldSample_00 = layout.findPictureByName("P_worldSample_00");
	
	// Make it visible in-case it wasn't
	P_worldSample_00->SetVisible(true);

	// This is not the greatest way to read a file but I suppose it works in a pinch
	char tplName[64];
	sprintf(tplName, "/WorldSample/%02d-%02d.tpl", CurrentWorld+1, CurrentLevel+1);
	
	static File tpl;
	if (tpl.open(tplName)) {
		// Apply the background image to our pane
		P_worldSample_00->material->texMaps[0].ReplaceImage((TPLPalette*)tpl.ptr(), 0);
	} else {
		// Hide the pane, the fail background pane that's underneath will be seen
		P_worldSample_00->SetVisible(false);
	}
	
}




