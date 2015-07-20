#ifndef _FORMREVIEW_H__
#define _FORMREVIEW_H__

#include "FormBase.h"
#include "GameInfo.h"
#include "Widget.h"

enum eFRstatus{ FR_VIEW, FR_EDIT, FR_MOVE };

struct sInfoReview
{
	SimpleText* review;
	SimpleText* score;
	SimpleText* author;
	Ogre::Real left;
	Ogre::Real top;
	Ogre::Real fontSize;
};

class FormReview : public FormBase
{
public:
	FormReview(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener = 0);
	~FormReview();

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// GuiListener
	void buttonHit(Button* button);

	// FormBase
	void hide();
	void show();
	void showOptions();
	void hideOptions();

private:
	void _createReview(const Ogre::String& name, const Ogre::String& textReview, const Ogre::String& textScore,
		const Ogre::String& textAuthor, Ogre::Real left, Ogre::Real top, Ogre::Real sizeFont);
	bool _loadReviews(const Ogre::String& pathIni);
	bool _saveReviews(const Ogre::String& pathIni);
	void _moveReview(unsigned int currentReview, Ogre::Vector2 newPos);
	void _spacingReview(unsigned int currentReview);
	void _increaseSize(unsigned int currentReview);
	void _drecreaseSize(unsigned int currentReview);

	GameInfo* mGameInfo;
	std::vector<sInfoReview> mReviews;
	Ogre::String mPathIni;
	unsigned int mCurrentReview;
	eFRstatus mStatus;
};

#endif // #ifndef _FORMREVIEW_H__

