#include "stdafx.h"
#include "FormReview.h"

FormReview::FormReview(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
FormBase(tray, oldListener),
mGameInfo(gInfo),
mPathIni(Ogre::StringUtil::BLANK),
mStatus(FR_VIEW)
{
	// find the .ini config file
	sInfoResource infoIni;
	mGameInfo->findIniReviews(infoIni);
	mPathIni = infoIni.path;

	_loadReviews(mPathIni);
}

FormReview::~FormReview()
{
	hideOptions();
}



bool FormReview::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_A)
	{
		_increaseSize(mCurrentReview);
	}
	else if (arg.key == OIS::KC_R)
	{
		_drecreaseSize(mCurrentReview);
	}
	
	return FormBase::keyPressed(arg);
}

bool FormReview::keyReleased(const OIS::KeyEvent &arg)
{
	return FormBase::keyReleased(arg);
}

bool FormReview::mouseMoved(const OIS::MouseEvent &arg)
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	if (mStatus == FR_MOVE)
	{
		Ogre::Vector2 cursorPos(arg.state.X.abs, arg.state.Y.abs);
		_moveReview(mCurrentReview, cursorPos);
	}

	return FormBase::mouseMoved(arg);
}

bool FormReview::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	if (mStatus == FR_EDIT)
	{
		Ogre::Vector2 cursorPos(arg.state.X.abs, arg.state.Y.abs);
		for (unsigned int i = 0; i < mReviews.size(); ++i)
		{
			if (Widget::isCursorOver(mReviews[i].review->getOverlayElement(), cursorPos))
			{
				mCurrentReview = i;
				mStatus = FR_MOVE;
				break;
			}
		}
	}

	return FormBase::mousePressed(arg, id);
}

bool FormReview::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if (mStatus == FR_MOVE) mStatus = FR_EDIT;

	return FormBase::mouseReleased(arg, id);
}

void FormReview::buttonHit(Button* button)
{
	if (button->getName() == "FormReview/Button/EditReviews")
	{
		mStatus = FR_EDIT;
	}
	else if (button->getName() == "FormReview/Button/CloseOptions")
	{
		if (mStatus == FR_EDIT) 
			_saveReviews(mPathIni);
		mStatus = FR_VIEW;
		hideOptions();
	}
}



bool FormReview::_loadReviews(const Ogre::String& pathIni)
{
	if (boost::filesystem::is_regular_file(pathIni))
	{
		CSimpleIniA* simpleIni = new CSimpleIniA;
		SI_Error iniStatus = simpleIni->LoadFile(pathIni.c_str());
		if (iniStatus == SI_OK)
		{
			// get all sections 
			CSimpleIniA::TNamesDepend sections;
			simpleIni->GetAllSections(sections);
			CSimpleIniA::TNamesDepend::const_iterator i;
			for (i = sections.begin(); i != sections.end(); ++i)
			{
				Ogre::String sectionName = i->pItem;
				if (sectionName.find("GAME.REVIEW") != Ogre::String::npos)
				{
					Ogre::String review = simpleIni->GetValue(sectionName.c_str(), "Review", "");
					_correctCaption(review);
					Ogre::String score = simpleIni->GetValue(sectionName.c_str(), "Score", "");
					_correctCaption(score);
					Ogre::String author = simpleIni->GetValue(sectionName.c_str(), "Author", "");
					_correctCaption(author);
					Ogre::Real left = simpleIni->GetDoubleValue(sectionName.c_str(), "Position_x", 50);
					Ogre::Real top = simpleIni->GetDoubleValue(sectionName.c_str(), "Position_y", 50);
					Ogre::Real charHeight = simpleIni->GetDoubleValue(sectionName.c_str(), "Font_Size", 20);
					_createReview(sectionName, review, score, author, left, top, charHeight);
				}
			}
			delete simpleIni;
			return true; // ok
		}
		delete simpleIni;
		return false; // error
	}
	return false; // error
}

bool FormReview::_saveReviews(const Ogre::String& pathIni)
{
	if (boost::filesystem::is_regular_file(pathIni))
	{
		CSimpleIniA* simpleIni = new CSimpleIniA;
		SI_Error iniStatus = simpleIni->LoadFile(pathIni.c_str());
		simpleIni->Reset(); // remove the previous content

		if (iniStatus == SI_OK)
		{
			for (unsigned int i = 0; i < mReviews.size(); ++i)
			{
				Ogre::String sectionName = "GAME.REVIEW." + Ogre::StringConverter::toString(i);

				simpleIni->SetValue(sectionName.c_str(), "Review", mReviews[i].review->getText().c_str());
				simpleIni->SetValue(sectionName.c_str(), "Score", mReviews[i].score->getText().c_str());
				simpleIni->SetValue(sectionName.c_str(), "Author", mReviews[i].author->getText().c_str());
				simpleIni->SetDoubleValue(sectionName.c_str(), "Position_x", mReviews[i].left);
				simpleIni->SetDoubleValue(sectionName.c_str(), "Position_y", mReviews[i].top);
				simpleIni->SetDoubleValue(sectionName.c_str(), "Font_Size", mReviews[i].fontSize);
			}
			SI_Error saveStatus = simpleIni->SaveFile(pathIni.c_str());
			delete simpleIni;
			if (saveStatus == SI_OK)
				return true; // ok
		}
		delete simpleIni;
		return false; // error
	}
	return false; // error
}


void FormReview::_createReview(const Ogre::String& name, const Ogre::String& textReview, const Ogre::String& textScore, const Ogre::String& textAuthor, Ogre::Real left, Ogre::Real top, Ogre::Real sizeFont)
{
	mReviews.push_back(sInfoReview());
	mReviews.back().left = left;
	mReviews.back().top = top;
	mReviews.back().fontSize = sizeFont;

	mReviews.back().review = mTrayMgr->createSimpleText("FormReview/Review/" + name,
		textReview, "YgcFont/SemiBold/21", 350, 90, mReviews.back().fontSize, 6);
	mReviews.back().score = mTrayMgr->createSimpleText("FormReview/Score/" + name,
		textScore, "YgcFont/SemiboldItalic/16", 250, 90, mReviews.back().fontSize, 6);
	mReviews.back().author = mTrayMgr->createSimpleText("FormReview/Author/" + name,
		textAuthor, "YgcFont/SemiboldItalic/16", 250, 90, mReviews.back().fontSize - 6, 6);
	mReviews.back().author->setTextColor(Ogre::ColourValue(0.79f, 0.79f, 0.80f));

	addWidgetToForm(mReviews.back().review);
	addWidgetToForm(mReviews.back().score);
	addWidgetToForm(mReviews.back().author);

	_spacingReview(mReviews.size() - 1);
}


void FormReview::_increaseSize(unsigned int currentReview)
{
	if (currentReview >= 0 && currentReview < mReviews.size())
	{
		mReviews[currentReview].fontSize = mReviews[currentReview].fontSize + 2;
		mReviews[currentReview].review->setCaptionHeight(mReviews[currentReview].fontSize);
		mReviews[currentReview].score->setCaptionHeight(mReviews[currentReview].fontSize);
		mReviews[currentReview].author->setCaptionHeight(mReviews[currentReview].fontSize - 6);
		_spacingReview(currentReview);
		
	}
}

void FormReview::_drecreaseSize(unsigned int currentReview)
{
	if (currentReview >= 0 && currentReview < mReviews.size())
	{
		mReviews[currentReview].fontSize = mReviews[currentReview].fontSize - 2;
		mReviews[currentReview].review->setCaptionHeight(mReviews[currentReview].fontSize);
		mReviews[currentReview].score->setCaptionHeight(mReviews[currentReview].fontSize);
		mReviews[currentReview].author->setCaptionHeight(mReviews[currentReview].fontSize - 6);
		_spacingReview(currentReview);
	}
}

void FormReview::_moveReview(unsigned int currentReview, Ogre::Vector2 newPos)
{
	if (currentReview >= 0 && currentReview < mReviews.size())
	{
		mReviews[currentReview].left = newPos.x / mScreenSize.x;
		mReviews[currentReview].top = newPos.y / mScreenSize.y;
		_spacingReview(currentReview);
	}
}

void FormReview::_spacingReview(unsigned int currentReview)
{
	if (currentReview >= 0 && currentReview < mReviews.size())
	{
		mReviews[currentReview].review->setLeft((mScreenSize.x * mReviews[currentReview].left) -
			(mReviews[currentReview].review->getMaxSizeLine() / 2));
		mReviews[currentReview].review->setTop(mScreenSize.y * mReviews[currentReview].top);

		mReviews[currentReview].score->setLeft(mReviews[currentReview].review->getLeft() +
			(mReviews[currentReview].review->getMaxSizeLine() / 2) -
			(mReviews[currentReview].score->getMaxSizeLine()) / 2);
		mReviews[currentReview].score->setTop(mReviews[currentReview].review->getTop() +
			(mReviews[currentReview].review->getNumLines() * mReviews[currentReview].fontSize) + 5);

		mReviews[currentReview].author->setLeft(mReviews[currentReview].review->getLeft() +
			(mReviews[currentReview].review->getMaxSizeLine() / 2) -
			(mReviews[currentReview].author->getMaxSizeLine()) / 2);
		mReviews[currentReview].author->setTop(
			(mReviews[currentReview].score->getText() == Ogre::StringUtil::BLANK)
			? mReviews[currentReview].review->getTop() + (mReviews[currentReview].review->getNumLines() * mReviews[currentReview].fontSize) + 15
			: mReviews[currentReview].score->getTop() + mReviews[currentReview].fontSize + 8);
	}
}



void FormReview::hide()
{
	FormBase::hide();
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->hide();
}

void FormReview::show()
{
	FormBase::show();
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->show();
}

void FormReview::showOptions()
{
	if (!mTrayMgr->getWidget("FormReview/Button/EditReviews")) // is visible? 
	{
		//topButton=screenHeight - (num_buttons*sep_buttons) - set_buttons;
		Ogre::Real sizeButton = 170, leftButton = 35, topButton = 0, sepButton = 40;
		topButton = mScreenSize.y - (2 * sepButton) - sepButton;

		mTrayMgr->createButton("FormReview/Button/EditReviews", "Edit Reviews", leftButton, topButton, sizeButton);
		topButton += sepButton;
		mTrayMgr->createButton("FormReview/Button/CloseOptions", "Close", leftButton, topButton, sizeButton);
	}
}

void FormReview::hideOptions()
{
	if (mTrayMgr->getWidget("FormReview/Button/EditReviews"))
	{
		mTrayMgr->destroyWidget("FormReview/Button/EditReviews");
		mTrayMgr->destroyWidget("FormReview/Button/CloseOptions");
	}
}

