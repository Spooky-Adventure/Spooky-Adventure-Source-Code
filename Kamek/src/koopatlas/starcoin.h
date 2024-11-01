#ifndef __KOOPATLAS_STARCOIN_H
#define __KOOPATLAS_STARCOIN_H

#include "koopatlas/core.h"

class dWMStarCoin_c : public dActor_c {
	public:
		dWMStarCoin_c();

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		bool layoutLoaded;
		m2d::EmbedLayout_c layout;

		bool visible;
		void show();

		static dWMStarCoin_c *build();
		static dWMStarCoin_c *instance;

		enum Animation {
			IN_WINDOW = 0,
			OUT_WINDOW,
			LOOP_CHECK,
			SCREEN_BEFORE,
			SCREEN_NEXT,
			SECRET_SHOW,
			SECRET_HIDE,
			SHOW_SECTION,
			HIDE_SECTION,
			SHOW_LEFT_ARROW,
			HIDE_LEFT_ARROW,
			SHOW_RIGHT_ARROW,
			HIDE_RIGHT_ARROW
		};

		enum _Constants {
			ROW_COUNT = 9,
			COLUMN_COUNT = 2,
			SHINE_COUNT = 5,
			MAX_SECTION_COUNT = 16,
		};

		int currentSection;
		int currentSectionIndex;
		int sectionIndices[MAX_SECTION_COUNT];
		int availableSectionCount;
		int direction;

		bool isLeftArrowVisible, isRightArrowVisible;
		bool willExit;

		bool canScrollLeft() const;
		bool canScrollRight() const;
		void loadInfo();
		void loadSectionInfo();

		void showLeftArrow();
		void showRightArrow();
		void hideLeftArrow();
		void hideRightArrow();
		void setLeftArrowVisible(bool value);
		void setRightArrowVisible(bool value);

		nw4r::lyt::Pane
			*N_coinBarPos_0[10], *W_fileter_0[10];
		
		nw4r::lyt::Picture
			*P_topFileter_0[10],
			*P_coinFileter_1_0[10], *P_coinFileter_2_0[10], *P_coinFileter_3_0[10],
			*P_coin_1_0[10], *P_coin_2_0[10], *P_coin_3_0[10];

		nw4r::lyt::TextBox
			*T_titleCoin_00, *T_back_00,
			*T_messageS_00, *T_message_00,
			*T_world_00, *T_world_01,
			*T_coinCurrent_00, *T_coinTotal_00,
			*T_worldNum_0[10], *T_corseNum_0[10], *T_pictureFont_0[10];
			

		dStateWrapper_c<dWMStarCoin_c> state;

		USING_STATES(dWMStarCoin_c);
		DECLARE_STATE(Hidden);
		DECLARE_STATE(ShowWait);
		DECLARE_STATE(Wait);
		DECLARE_STATE(ChangeWait);
		DECLARE_STATE(HideWait);
};

#endif

