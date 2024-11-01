#ifndef __KOOPATLAS_HUD_H
#define __KOOPATLAS_HUD_H

#include "koopatlas/core.h"

class dWMHud_c : public dBase_c {
	public:
		dWMHud_c();

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		bool layoutLoaded;
		m2d::EmbedLayout_c layout;

		static dWMHud_c *build();
		static dWMHud_c *instance;

		bool doneFirstShow;
		bool allowInAnimPlay;
		
		void loadInitially();

		void enteredNode(dKPNode_s *node = 0);
		void leftNode(dKPNode_s *node = 0);

		void setupLives();

		void hideAll();
		void showAll();
		
		void shouldShowCourseInfo(bool value); // Plays the in/out animation for course info
		void displayLevelInfo(bool willItShow); // Hides the level data (name, coins, flags)

	private:
		void loadCourseInfo();
		void loadWorldInfo();

		dKPNode_s *nodeForCourseInfo;

		int displayedControllerType;
		void updatePressableButtonThingies();

		void setupCompletionStars();

		nw4r::lyt::Pane
			*N_IconPosXP_00[4];

		nw4r::lyt::Picture
			*P_flagClear_00, *P_flagClearS_00,
			*P_cCs_0[3], *P_cC_0[3],
			*P_marioFace_00, *P_luigiFace_00,
			*P_BkinoFace_00, *P_YkinoFace_00,
			*P_rankStar_00, *P_rankStar_01, *P_rankStar_02;

		nw4r::lyt::TextBox
			*T_worldNum_00,
			*T_cSelect_00, *T_cSelect_pic,
			*T_worldName_00, *T_levelName_00,
			*T_lifeNumber[4];
};

#endif

