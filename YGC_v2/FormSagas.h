#ifndef _FORMSAGAS_H__
#define _FORMSAGAS_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "Thumbnail3D.h"
#include "FormSagaOverview.h"

class FormSagas : public FormBase
{
public:
	FormSagas(const Ogre::String& pathSagas, GuiManager* tray, GuiListener* oldListener = 0);
	~FormSagas();

	// Ogre::FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// Visibility
	void hide();
	void show();

	// Menu options
	void hideAllOptions();
	void hideOptions();
	void showOptions();
	void hideOptionsThumbs();
	void showOptionsThumbs();

	// GuiListener
	void sliderMoved(Slider* slider);
	void buttonHit(Button* button);
	void labelHit(Label* label);
	void itemChanged(ItemSelector* selector);
	void sliderOptionsMoved(SliderOptions* slider);

	// Forms control
	void removeSaga();

private:
	FormSagaOverview* mFormSagaOverview;
	Ogre::SceneNode* mParentThumbs;
	std::vector<GameInfo*> mGameInfo;
	std::vector<Thumbnail3D*> mThumbs;
	sGridThumbs mGridThumbs;
	Thumbnail3D* mLastThumbOver;
};

#endif // #ifndef _FORMSAGAS_H__
