#include "koopatlas/hud.h"
#include <newer.h>

dWMHud_c *dWMHud_c::instance = 0;

dWMHud_c *dWMHud_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dWMHud_c));
	dWMHud_c *c = new(buffer) dWMHud_c;

	instance = c;
	return c;
}

dWMHud_c::dWMHud_c() {
	layoutLoaded = false;
	displayedControllerType = -1;
}

enum WMHudAnimation {
	IN_INFO = 0,
	OUT_INFO,
	IN_COURSE_INFO,
	OUT_COURSE_INFO,
};


int dWMHud_c::onCreate() {
	if (!layoutLoaded) {
		bool gotFile = layout.loadArc("corseSelectUIGuide.arc", false);
		if (!gotFile)
			return false;

		bool output = layout.build("corseSelectUIGuide_37.brlyt");

		// Setup animations
		static const char *brlanNames[] = {
			"corseSelectUIGuide_37_inInfo.brlan",
			"corseSelectUIGuide_37_outInfo.brlan",
			"corseSelectUIGuide_37_inCourseInfo.brlan",
			"corseSelectUIGuide_37_outCourseInfo.brlan",
		};
		static const char *groupNames[] = {
			"A00_info",			// inInfo
			"A00_info",			// outInfo
			"A02_courseInfo",	// inCourseInfo
			"A02_courseInfo",	// outCourseInfo
		};
		static const int brlanIDs[] = {
			0, 1, 2, 3,
		};

		layout.loadAnimations(brlanNames, 4);
		layout.loadGroups(groupNames, brlanIDs, 4);
		layout.disableAllAnimations();


		// Now let's get some BMG strings
		// Should probably mention, the guide for Select World has the Items text printed in it
		static const char *tbNames[4] = {
			"T_guideViewR_00",	// Menu
			"T_guideViewC_00",	// Items, formerly Select World
			"T_guideViewL_01"	// Items (Retail)
			"T_guideViewLS_00"	// View Map
		};
		layout.setLangStrings(tbNames, (int[4]){12, 15, 11, 10}, 4, 2);
		
		// "World" text above the level number
		WriteBMGToTextBox(layout.findTextBoxByName("T_world_00"), GetBMG(), 4, 60, 0);


		// Now get a bunch of panes
		static const char *paneNames[] = {
			"N_IconPos1P_00", "N_IconPos2P_00",
			"N_IconPos3P_00", "N_IconPos4P_00"
		};
		layout.getPanes(paneNames, &N_IconPosXP_00[0], 4);

		static const char *pictureNames[] = {
			"P_flagClear_00", "P_flagClearS_00",
			"P_cCs_00", "P_cCs_01", "P_cCs_02",
			"P_cC_00", "P_cC_01", "P_cC_02",
			"P_marioFace_00", "P_luigiFace_00",
			"P_BkinoFace_00", "P_YkinoFace_00",
			"P_rankStar_00", "P_rankStar_01", "P_rankStar_02"
		};
		layout.getPictures(pictureNames, &P_flagClear_00, 15);

		static const char *textBoxNames[] = {
			"T_worldNum_00",
			"T_cSelect_00", "T_cSelect_pic",
			"T_worldName_00", "T_levelName_00",
			"T_lifeNumber_00", "T_lifeNumber_01",
			"T_lifeNumber_02", "T_lifeNumber_03"
		};
		layout.getTextBoxes(textBoxNames, &T_worldNum_00, 9);

		layoutLoaded = true;

		layout.drawOrder = 0;

		// Bring in our stuff
		layout.enableNonLoopAnim(IN_INFO);
		layout.enableNonLoopAnim(IN_COURSE_INFO);

		setupLives();
	}

	return true;
}


void dWMHud_c::loadInitially() {
	if (doneFirstShow)
		return;

	doneFirstShow = true;
	allowInAnimPlay = false;

	if (!dScKoopatlas_c::instance->pathManager.isMoving)
		enteredNode();
}


int dWMHud_c::onDelete() {
	dWMHud_c::instance = 0;

	if (!layoutLoaded)
		return true;

	return layout.free();
}


int dWMHud_c::onExecute() {
	if (!layoutLoaded)
		return true;
	
	loadWorldInfo();
	loadCourseInfo();
	//setupCompletionStars();

	setupLives(); // FUCK IT
	updatePressableButtonThingies();

	layout.execAnimations();
	layout.update();
	
	return true;
}


int dWMHud_c::onDraw() {
	if (!layoutLoaded)
		return true;

	layout.scheduleForDrawing();

	return true;
}


void dWMHud_c::hideAll() {
	layout.enableNonLoopAnim(OUT_INFO);
	
	dKPNode_s *node = dScKoopatlas_c::instance->pathManager.currentNode;
	layout.enableNonLoopAnim(OUT_COURSE_INFO);
}

void dWMHud_c::showAll() {
	layout.enableNonLoopAnim(IN_INFO);
	
	dKPNode_s *node = dScKoopatlas_c::instance->pathManager.currentNode;
	layout.enableNonLoopAnim(IN_COURSE_INFO);
}



void dWMHud_c::loadWorldInfo() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	
	wchar_t convertedWorldName[32];
	int i;
	for (i = 0; i < 32; i++) {
		convertedWorldName[i] = save->newerWorldName[i];
		if (convertedWorldName[i] == 0)
			break;
	}
	convertedWorldName[31] = 0;

	T_worldName_00->SetString(convertedWorldName);

	// Apply colors!
	T_worldName_00->colour1 = save->hudTextColours[0];
	T_worldName_00->colour2 = save->hudTextColours[1];
}


void dWMHud_c::loadCourseInfo() {
	dLevelInfo_c *levelInfo = &dLevelInfo_c::s_info;
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	
	// Fixes a bunch of issues with it not knowing the current node
	dKPNode_s *node;
	node = dScKoopatlas_c::instance->pathManager.currentNode;
	nodeForCourseInfo = node;
	
	// Return if it isn't a level
	if (node->type != dKPNode_s::LEVEL) {
		return;
	}
	
	// Info
	int world = nodeForCourseInfo->levelNumber[0] - 1;
	int level = nodeForCourseInfo->levelNumber[1] - 1;
	
// LEVEL NAME
	dLevelInfo_c::entry_s *infEntry = levelInfo->searchBySlot(world, level);
	
	// Return if no level data exists (and set some values to defaults as well)
	if (infEntry == 0) {
		// No level data exists
		displayLevelInfo(false);
		T_levelName_00->SetString(L"Unknown Level Name!");
		T_levelName_00->SetVisible(true); // Since displayLevelInfo makes it invisible
		T_worldNum_00->SetString(L"?");
		T_cSelect_00->SetString(L"?");
		// Just in-case this is the currently visible pane
		T_cSelect_pic->SetString(L"?");
		return;
	}

	wchar_t convertedLevelName[100];
	const char *sourceLevelName = levelInfo->getNameForLevel(infEntry);
	int charCount = 0;
	
	while (*sourceLevelName != 0 && charCount < 99) {
		convertedLevelName[charCount] = *sourceLevelName;
		sourceLevelName++;
		charCount++;
	}
	convertedLevelName[charCount] = 0;

	T_levelName_00->SetString(convertedLevelName);
	
	// Hide the level name for Start Nodes and Map Signs
	if ((level >= 96) && (level < 98)) {
		T_levelName_00->SetVisible(false);
	}
	
	// Enable it everywhere else
	else {
		T_levelName_00->SetVisible(true);
	}


// LEVEL NUMBER
	wchar_t worldNumber[16];
	getWorldNumber(infEntry->displayWorld, worldNumber);
	T_worldNum_00->SetString(worldNumber);
	
	
	wchar_t levelNumber[16];
	getLevelNumber(infEntry->displayLevel, levelNumber);
	// If the level ID is an icon, we hide the regular pane and write
	// it to a pane that fits it better
	if (showCoursePic) {
		T_cSelect_00->SetVisible(false);
		T_cSelect_pic->SetVisible(true);
		T_cSelect_pic->SetString(levelNumber);
	} else {
		T_cSelect_pic->SetVisible(false);
		T_cSelect_00->SetVisible(true);
		T_cSelect_00->SetString(levelNumber);
	}

	
// CLEAR FLAGS
	u32 conds = GetSaveFile()->GetBlock(-1)->GetLevelCondition(world, level);
	// States: 0 = Invisible, 1 = Visible, 2 = Semi-Visible
	int normalState = 0, secretState = 0;

	if ((conds & COND_BOTH_EXITS) && (infEntry->flags & 0x30) == 0x30) {
		// If this level has two exits and one of them is already collected,
		// then show the faded flags
		normalState = 2;
		secretState = 2;
	}

	if ((conds & COND_NORMAL) && (infEntry->flags & 0x10))
		normalState = 1;
	if ((conds & COND_SECRET) && (infEntry->flags & 0x20))
		secretState = 1;

	P_flagClear_00->alpha = (normalState == 2) ? 80 : 255;
	P_flagClear_00->SetVisible(normalState > 0);

	P_flagClearS_00->alpha = (secretState == 2) ? 80 : 255;
	P_flagClearS_00->SetVisible(secretState > 0);



// STAR COINS
	bool haveSC = (infEntry->flags & 2); // Are Star Coins enabled for this level or not?

	for (int i = 0; i < 3; i++) {
		bool flag = (conds & (COND_COIN1 << i));
		P_cC_0[i]->SetVisible(flag);
		P_cCs_0[i]->SetVisible(haveSC); // Coin outlines
	}

}


// World completion stars
/*void dWMHud_c::setupCompletionStars() {
	// These might be reintroduced down the line, but right now
	// I don't think they function, and also I don't know where to put them (in the layout)
	
	// Figure out if stars are needed:
	// Star 0: World is complete (last level beat)
	// Star 1: All exits complete
	// Star 2: All star coins obtained

	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	dLevelInfo_c *linfo = &dLevelInfo_c::s_info;
	dLevelInfo_c::entry_s *lastLevel = linfo->searchByDisplayNum(save->newerWorldID, lastLevelIDs[save->newerWorldID]);
	
	P_rankStar_00->SetVisible(false);
	
	// Check if the last level of the world is cleared
	if (lastLevel) {
		//bool isLastDone;
		bool isLastDone = (save->GetLevelCondition(lastLevel->worldSlot,lastLevel->levelSlot) & COND_NORMAL);
		P_rankStar_00->SetVisible(isLastDone);
	}

	// Now calculate the other two
	P_rankStar_01->SetVisible(true);
	P_rankStar_02->SetVisible(true);

	dLevelInfo_c::section_s *sect = linfo->getSectionByIndex(save->newerWorldID);

	for (int i = 0; i < sect->levelCount; i++) {
		dLevelInfo_c::entry_s *entry = &sect->levels[i];

		if (entry->flags & 2) {
			u32 conds = save->GetLevelCondition(entry->worldSlot, entry->levelSlot);

			// Check if all Exits are cleared
			if (((entry->flags & 0x10) && !(conds & COND_NORMAL)) || ((entry->flags & 0x20) && !(conds & COND_SECRET))) {
				P_rankStar_01->SetVisible(false);
			}

			// Check if we have all Star Coins
			if ((conds & COND_COIN_ALL) != COND_COIN_ALL) {
				P_rankStar_02->SetVisible(false);
			}
		}
	}

	// idk Cutland or something
	if (save->newerWorldID == 15) {
		P_rankStar_00->SetVisible(false);
		P_rankStar_01->SetVisible(false);
		P_rankStar_02->SetVisible(false);
	}

}*/


void dWMHud_c::displayLevelInfo(bool willItShow) {
	T_levelName_00->SetVisible(willItShow);
	// Coins and the outlines
	for (int i = 0; i < 3; i++) {
		P_cC_0[i]->SetVisible(willItShow);
		P_cCs_0[i]->SetVisible(willItShow);
	}
	P_flagClear_00->SetVisible(willItShow);
	P_flagClearS_00->SetVisible(willItShow);
}


void dWMHud_c::enteredNode(dKPNode_s *node) {
	// Stops the IN_INFO anim from playing twice on the initial load
	if (allowInAnimPlay)
		layout.enableNonLoopAnim(IN_INFO);
	
	if (node == 0)
		node = dScKoopatlas_c::instance->pathManager.currentNode;

	if (node->type == dKPNode_s::LEVEL && doneFirstShow) {
		nodeForCourseInfo = node;
		allowInAnimPlay = true;
	}
	
	// Stopping Points
	if (node->type == dKPNode_s::STOP) {
		T_cSelect_00->SetVisible(false);
		displayLevelInfo(false);
		
		// Prints the junction icon
		wchar_t levelNumberStop[16];
		getLevelNumber(44, levelNumberStop);
		T_cSelect_pic->SetVisible(true);
		T_cSelect_pic->SetString(levelNumberStop);
	}

	if (node->type == dKPNode_s::SIGN) {
		T_cSelect_00->SetVisible(false);
		displayLevelInfo(false);
		
		// Prints the junction icon
		wchar_t levelNumberSign[16];
		getLevelNumber(49, levelNumberSign);
		T_cSelect_pic->SetVisible(true);
		T_cSelect_pic->SetString(levelNumberSign);
	}
	
}

void dWMHud_c::leftNode(dKPNode_s *node) {
	if (node == 0)
		node = dScKoopatlas_c::instance->pathManager.currentNode;

	// Only play when exiting a level node, stopping point, or sign node
	if ((node->type == dKPNode_s::LEVEL) || (node->type == dKPNode_s::STOP) || (node->type == dKPNode_s::SIGN)) {
		layout.enableNonLoopAnim(OUT_INFO);
	}
	
}



// Replaces the showFooter and hideFooter functions
void dWMHud_c::shouldShowCourseInfo(bool value) {
	if (value) {
		layout.enableNonLoopAnim(IN_COURSE_INFO);
	}
	else {
		layout.enableNonLoopAnim(OUT_COURSE_INFO);
	}
}


void dWMHud_c::setupLives() {
	static const int LogicalPlayerIDs[] = {0,1,3,2};

	P_marioFace_00->SetVisible(false);
	P_luigiFace_00->SetVisible(false);
	P_BkinoFace_00->SetVisible(false);
	P_YkinoFace_00->SetVisible(false);

	int playerCount = 0;

	for (int i = 0; i < 4; i++) {
		// The part in setupLives()
		int playerID = LogicalPlayerIDs[i];
		int slotID = SearchForIndexOfPlayerID(playerID);
		int lives = Player_Lives[slotID];
		int length = 2;

		WriteNumberToTextBox(&lives, &length, T_lifeNumber[slotID], true);

		// The part in setupIconThings()
		if (QueryPlayerAvailability(slotID)) {
			playerCount++;

			nw4r::lyt::Pane *facePane = (&P_marioFace_00)[playerID];
			facePane->trans = N_IconPosXP_00[playerCount - 1]->trans;
			facePane->SetVisible(true);
		}
	}

	for (int i = 0; i < 4; i++) {
		N_IconPosXP_00[i]->SetVisible(false);
	}
	N_IconPosXP_00[playerCount - 1]->SetVisible(true);
}

// Changes button guides if P1 has a Nunchuk plugged in
void dWMHud_c::updatePressableButtonThingies() {
	int cntType = RemoconMng->controllers[0]->controllerType;

	if (cntType != displayedControllerType) {
		displayedControllerType = cntType;

		int beef = (cntType == 0) ? 0 : 1;
		GameMgrP->currentControllerType = beef;

		WriteBMGToTextBox(layout.findTextBoxByName("T_guideViewLS_00"), GetBMG(), 4, 10, 0); // (c) View Map
		WriteBMGToTextBox(layout.findTextBoxByName("T_guideViewC_00"), GetBMG(), 4, 15, 0);  // [B] Items
	}
}
