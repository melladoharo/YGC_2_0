#ifndef _FORMNEWGAME_H__
#define _FORMNEWGAME_H__

#include "FormBase.h"

class FormNewGame : public FormBase
{
public:
	FormNewGame(GuiManager* tray, GuiListener* oldListener = 0);
	~FormNewGame();

	// Ogre::FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// FormBase
	void hideAllOptions();
	void hideOptions();
	void showOptions();

	// GuiListener
	void buttonHit(Button* button);
	void okDialogClosed(const Ogre::DisplayString& message);

	Ogre::String getName() { return mNameGame; }
	bool isFinished() { return mIsFinish; }

private:
	bool _isValidGame(const Ogre::String& nameGame);
	bool _createNewGame(const Ogre::String& nameGame);

	Ogre::String mNameGame;
	bool mIsFinish;
};

#endif // #ifndef _FORMNEWGAME_H__

