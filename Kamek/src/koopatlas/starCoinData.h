#ifndef __KOOPATLAS_STARCOINDATA_H
#define __KOOPATLAS_STARCOINDATA_H

#include "koopatlas/core.h"

class dWMStarCoinData_c : public dActor_c {
	public:
        static dWMStarCoinData_c *build();
		static dWMStarCoinData_c *instance;

		dWMStarCoinData_c();

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		bool layoutLoaded;
		m2d::EmbedLayout_c layout;

        void filtersVisible(bool value);


		nw4r::lyt::Pane
            *N_coinBarDate_00, *W_fileter_00;

        nw4r::lyt::Picture
			*P_coinBase1_00, *P_coinBase2_00, *P_coinBase3_00,       // Coin outlines
            *P_coinFileter_00, *P_coinFileter_01, *P_coinFileter_02, // Coin outline overlays
            *P_coin_1_00, *P_coin_2_00, *P_coin_3_00,                // Coin images
            *P_topFileter;

		nw4r::lyt::TextBox
			*T_worldNum_00, *T_corseNum_00, *T_pictureFont_00;
};

#endif
