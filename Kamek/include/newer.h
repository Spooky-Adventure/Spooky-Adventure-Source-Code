#ifndef NEWER_H
#define NEWER_H 

extern int lastLevelIDs[];

void WriteAsciiToTextBox(nw4r::lyt::TextBox *tb, const char *source);

int getUnspentStarCoinCount();
int getStarCoinCount();

// Can probably get rid of this when the other layouts that use it are updated
void getNewerLevelNumberString(int world, int level, wchar_t *dest);

void getWorldNumber(int world, wchar_t *dest);
void getLevelNumber(int level, wchar_t *dest);

// Used to decide if the icon pane is shown
bool showCoursePic;


#endif /* NEWER_H */
