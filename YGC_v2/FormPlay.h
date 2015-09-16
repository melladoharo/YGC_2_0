#ifndef _FORMPLAY_H__
#define _FORMPLAY_H__

#include "FormBase.h"
#include "GameInfo.h"

class FormPlay : public FormBase
{
public:
	FormPlay(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener = 0);
	~FormPlay();

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// FormBase - visibility
	void hide();
	void show();

	// GuiListener
	void buttonHit(Button* button);
	void labelHit(Label* label);

	// FormBase - options
	void hideAllOptions();
	void hideOptions();
	void showOptions();
	void hideOptionsEditPath();
	void showOptionsEditPath();

private:
	GameInfo* mGameInfo;
};

#endif // #ifndef _FORMPLAY_H__

