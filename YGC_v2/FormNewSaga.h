#ifndef _FORMNEWSAGA_H__
#define _FORMNEWSAGA_H__

#include "FormBase.h"

class FormNewSaga : public FormBase
{
public:
	FormNewSaga(GuiManager* tray, GuiListener* oldListener = 0);
	~FormNewSaga();

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
	void hideSelectGames();
	void showSelectGames();

	// GuiListener
	void buttonHit(Button* button);
	void okDialogClosed(const Ogre::DisplayString& message);

private:
	bool _isValidSaga(const Ogre::String& nameSaga);
	bool _createNewSaga(const Ogre::String& nameSaga);

	Ogre::String mNameSaga;
	bool mNewSagaAdded;
};

#endif // #ifndef _FORMNEWSAGA_H__

