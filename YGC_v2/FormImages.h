#ifndef _FORMIMAGES_H__
#define _FORMIMAGES_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "Thumbnail3D.h"

class FormImages : public FormBase
{
public:
	FormImages(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener = 0);
	~FormImages();

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
	void hideOptionsThumbs();
	void showOptionsThumbs();

	// GuiListener
	void buttonHit(Button* button);
	void sliderMoved(Slider* slider);
	void labelHit(Label* label);
	void itemSelected(SelectMenu* menu);
	void itemChanged(ItemSelector* selector);
	void sliderOptionsMoved(SliderOptions* slider);

private:
	void _filterImageByIndex(std::pair<unsigned int, unsigned int> indexFilter);

	GameInfo* mGameInfo;
	Ogre::SceneNode* mParentThumbs;
	std::vector<sInfoResource> mImages;
	std::vector<Thumbnail3D*> mThumbs;
	Thumbnail3D* mLastThumbOver;
	sGridThumbs mGridThumbs;
	std::pair<unsigned int, unsigned int> mIndexScreenshots,
		mIndexWallpapers, mIndexConceptArt;
};

#endif // #ifndef _FORMIMAGES_H__

