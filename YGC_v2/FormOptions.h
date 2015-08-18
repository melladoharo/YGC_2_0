#ifndef _FORMOPTIONS_H__
#define _FORMOPTIONS_H__

#include "FormBase.h"

class FormOptions : public FormBase
{
public:
	FormOptions(GuiManager* tray, GuiListener* oldListener = 0);
	~FormOptions();

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// FormBase
	void hide();
	void show();
	void hideAllOptions();
	void hideOptions();
	void showOptions();
	void hideOptionsGeneral();
	void showOptionsGeneral();
	void hideOptionsGraphics();
	void showOptionsGraphics();
	void hideOptionsAdvancedGraphics();
	void showOptionsAdvancedGraphics();
	void hideEditPathGames();
	void showEditPathGames();

	// GuiListener
	void buttonHit(Button* button);
	void labelHit(Label* label);
	void itemChanged(ItemSelector* selector);

private:
	Ogre::StringVector _getResolutionModes(const Ogre::String& aspectRatio);
};

#endif // #ifndef _FORMOPTIONS_H__

