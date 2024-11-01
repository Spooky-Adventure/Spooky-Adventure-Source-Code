#include "koopatlas/starCoinData.h"

dWMStarCoinData_c *dWMStarCoinData_c::instance = 0;

dWMStarCoinData_c *dWMStarCoinData_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dWMStarCoinData_c));
	dWMStarCoinData_c *c = new(buffer) dWMStarCoinData_c;

	instance = c;
	return c;
}

int dWMStarCoinData_c::onCreate() {
    bool gotFile = layout.loadArc("worldCollectionCoinDate.arc", false);
    if (!gotFile)
        return false;
    
    layout.build("worldCollectionCoinDate_12.brlyt");

    layout.drawOrder = 3;

    // This is messy, clean this up later
    N_coinBarDate_00 = layout.findPaneByName("N_coinBarDate_00");
    W_fileter_00 = layout.findPaneByName("W_fileter_00");

    T_worldNum_00 = layout.findTextBoxByName("T_worldNum_00");
	T_corseNum_00 = layout.findTextBoxByName("T_corseNum_00");
	T_pictureFont_00 = layout.findTextBoxByName("T_pictureFont_00");

    P_coinBase1_00 = layout.findPictureByName("P_coinBase1_00");
    P_coinBase2_00 = layout.findPictureByName("P_coinBase2_00");
    P_coinBase3_00 = layout.findPictureByName("P_coinBase3_00");

    P_coinFileter_00 = layout.findPictureByName("P_coinFileter_00");
    P_coinFileter_01 = layout.findPictureByName("P_coinFileter_01");
    P_coinFileter_02 = layout.findPictureByName("P_coinFileter_02");

    P_coin_1_00 = layout.findPictureByName("P_coin_1_00");
    P_coin_2_00 = layout.findPictureByName("P_coin_2_00");
    P_coin_3_00 = layout.findPictureByName("P_coin_3_00");

    P_topFileter = layout.findPictureByName("P_topFileter");

    return true;
}

int dWMStarCoinData_c::onExecute() {
    layout.execAnimations();
	layout.update();

	return true;
}

int dWMStarCoinData_c::onDraw() {
    layout.scheduleForDrawing();
}

int dWMStarCoinData_c::onDelete() {
    return layout.free();
}


// Controls visibility of the "filter" panes
void dWMStarCoinData_c::filtersVisible(bool value) {
    W_fileter_00->SetVisible(value); // Blue bar behind coins
    P_topFileter->SetVisible(value); // Yellow thing

    // Coin outline overlays
    P_coinFileter_00->SetVisible(value);
    P_coinFileter_01->SetVisible(value);
    P_coinFileter_02->SetVisible(value);
}