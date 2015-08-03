#include "stdafx.h"
#include "FormReview.h"

FormReview::FormReview(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) : 
FormBase(tray, oldListener),
mGameInfo(gInfo),
mPathIni(Ogre::StringUtil::BLANK),
mCurrentReview(0)
{
	// find the .ini config file
	sInfoResource infoIni;
	mGameInfo->findIniReviews(infoIni);
	mPathIni = infoIni.path;

	_loadReviews(mPathIni);
}

FormReview::~FormReview()
{
	hideAllOptions();
	for (unsigned int i = 0; i < mReviews.size(); ++i)
	{
		mTrayMgr->destroyWidget(mReviews[i].review);
		mTrayMgr->destroyWidget(mReviews[i].score);
		mTrayMgr->destroyWidget(mReviews[i].author);
	}
}



bool FormReview::keyPressed(const OIS::KeyEvent &arg)
{
	if (!mReviews.empty())
	{
		LineEdit* leReview = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormReview/LineEdit/Review"));
		if (leReview)
		{
			leReview->injectKeyPress(arg);
			mReviews[mCurrentReview].review->setText(leReview->getText());
			_spacingReview(mCurrentReview);
		}
		LineEdit* leScore = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormReview/LineEdit/Score"));
		if (leScore)
		{
			leScore->injectKeyPress(arg);
			mReviews[mCurrentReview].score->setText(leScore->getText());
			_spacingReview(mCurrentReview);
		}
		LineEdit* leAuthor = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormReview/LineEdit/Author"));
		if (leAuthor)
		{
			leAuthor->injectKeyPress(arg);
			mReviews[mCurrentReview].author->setText(leAuthor->getText());
			_spacingReview(mCurrentReview);
		}
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

	return FormBase::mouseMoved(arg);
}

bool FormReview::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	return FormBase::mousePressed(arg, id);
}

bool FormReview::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	return FormBase::mouseReleased(arg, id);
}

void FormReview::buttonHit(Button* button)
{
	if (button->getName() == "FormReview/Button/CloseOptions")
	{
		hideOptions();
	}
	else if (button->getName() == "FormReview/Button/CloseOptionsReviews")
	{
		_saveReviews(mPathIni);
		hideOptionsReviews();
		showOptions();
	}
}

void FormReview::labelHit(Label* label)
{
	if (label->getName() == "FormReview/Label/EditReviews")
	{
		hideOptions();
		showOptionsReviews();
	}
	else if (label->getName() == "FormReview/Label/NewReview")
	{
		Ogre::String nameReview = "Review_" + Ogre::StringConverter::toString(mReviews.size());
		_createReview(nameReview, "Review", "-/-", "Author", 0.5f, 0.5f, 21);
		mCurrentReview = mReviews.size() - 1;
		_loadConfigWindowsReview(mCurrentReview, true);
	}
	else if (label->getName() == "FormReview/Label/DeleteReview")
	{
		if (mCurrentReview >= 0 && mCurrentReview < mReviews.size())
		{
			mTrayMgr->destroyWidget(mReviews[mCurrentReview].review->getName());
			mTrayMgr->destroyWidget(mReviews[mCurrentReview].score->getName());
			mTrayMgr->destroyWidget(mReviews[mCurrentReview].author->getName());
			mReviews.erase(mReviews.begin() + mCurrentReview);
			_saveReviews(mPathIni);
			mCurrentReview = 0;
			_loadConfigWindowsReview(mCurrentReview);
		}
	}
}

void FormReview::itemChanged(ItemSelector* selector)
{
	if (selector->getName() == "FormReview/Selector/Review")
	{
		// saves the changes before pass to the next (or previous) review
		_saveReviews(mPathIni);

		// pass to next (or previous) review, and load its parameters in the windows configuration
		Ogre::String reviewString = selector->getSelectedOption();
		mCurrentReview = Ogre::StringConverter::parseInt(reviewString);
		_loadConfigWindowsReview(mCurrentReview);
	}
}

void FormReview::sliderOptionsMoved(SliderOptions* slider)
{
	if (slider->getName() == "FormReview/Slider/Size")
	{
		_setFontSize(mCurrentReview, slider->getValue());
	}
	else if (slider->getName() == "FormReview/Slider/PosX")
	{
		if (!mReviews.empty())
			_moveReview(mCurrentReview, Ogre::Vector2(slider->getValue() / 100.0f, mReviews[mCurrentReview].top));
	}
	else if (slider->getName() == "FormReview/Slider/PosY")
	{
		if (!mReviews.empty())
			_moveReview(mCurrentReview, Ogre::Vector2(mReviews[mCurrentReview].left, slider->getValue() / 100.0f));
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

	_spacingReview(mReviews.size() - 1);
}

void FormReview::_setFontSize(unsigned int currentReview, unsigned int fontSize)
{
	if (currentReview >= 0 && currentReview < mReviews.size())
	{
		mReviews[currentReview].fontSize = fontSize;
		mReviews[currentReview].review->setCaptionHeight(fontSize);
		mReviews[currentReview].score->setCaptionHeight(fontSize);
		mReviews[currentReview].author->setCaptionHeight(fontSize - 6);
		_spacingReview(currentReview);
	}
}

void FormReview::_moveReview(unsigned int currentReview, Ogre::Vector2 newPos)
{
	if (currentReview >= 0 && currentReview < mReviews.size())
	{
		mReviews[currentReview].left = newPos.x;
		mReviews[currentReview].top = newPos.y;
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

void FormReview::_loadConfigWindowsReview(unsigned int currentReview, bool newReview /*= false*/)
{
	LineEdit* leReview = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormReview/LineEdit/Review"));
	LineEdit* leScore = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormReview/LineEdit/Score"));
	LineEdit* leAuthor = dynamic_cast<LineEdit*>(mTrayMgr->getWidget("FormReview/LineEdit/Author"));
	ItemSelector* selReview = dynamic_cast<ItemSelector*>(mTrayMgr->getWidget("FormReview/Selector/Review"));
	SliderOptions* sdSize = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormReview/Slider/Size"));
	SliderOptions* sdPosX = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormReview/Slider/PosX"));
	SliderOptions* sdPosY = dynamic_cast<SliderOptions*>(mTrayMgr->getWidget("FormReview/Slider/PosY"));
	Ogre::StringVector itemsReviews;
	for (unsigned int i = 0; i < mReviews.size(); ++i)
		itemsReviews.push_back(Ogre::StringConverter::toString(i));

	if (mReviews.empty())
	{
		leReview->setText("");
		leScore->setText("");
		leAuthor->setText("");
		itemsReviews.push_back("No reviews");
		selReview->setItems(itemsReviews);
		sdSize->setValue(12, false);
		sdPosX->setValue(0, false);
		sdPosY->setValue(0, false);
	}
	else if (newReview)
	{
		leReview->setText("Review");
		leScore->setText("-/-");
		leAuthor->setText("Author");
		selReview->setItems(itemsReviews);
		selReview->selectOption(Ogre::StringConverter::toString(mCurrentReview), false);
		sdSize->setValue(21, false);
		sdPosX->setValue(50, false); 
		sdPosY->setValue(50, false);
	}
	else if (currentReview >= 0 && currentReview < mReviews.size())
	{
		leReview->setText(mReviews[currentReview].review->getText());
		leScore->setText(mReviews[currentReview].score->getText());
		leAuthor->setText(mReviews[currentReview].author->getText());
		selReview->setItems(itemsReviews);
		selReview->selectOption(Ogre::StringConverter::toString(mCurrentReview), false);
		sdSize->setValue(mReviews[currentReview].fontSize, false);
		sdPosX->setValue(mReviews[currentReview].left * 100.0f, false);
		sdPosY->setValue(mReviews[currentReview].top * 100.0f, false);
	}
}



void FormReview::hide()
{
	for (unsigned int i = 0; i < mReviews.size(); ++i)
	{
		mReviews[i].review->hide();
		mReviews[i].score->hide();
		mReviews[i].author->hide();
	}
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->hide();
	hideAllOptions();
}

void FormReview::show()
{
	for (unsigned int i = 0; i < mReviews.size(); ++i)
	{
		mReviews[i].review->show();
		mReviews[i].score->show();
		mReviews[i].author->show();
	}
	Widget* logoGame = mTrayMgr->getWidget("FormOverview/Logo");
	if (logoGame) logoGame->show();
}



void FormReview::hideAllOptions()
{
	hideOptions();
	hideOptionsReviews();
}

void FormReview::hideOptions()
{
	if (mTrayMgr->getWidget("FormReview/Window/Options"))
	{
		mTrayMgr->destroyDialogWindow("FormReview/Window/Options");
		mTrayMgr->destroyWidget("FormReview/Label/EditReviews");
		mTrayMgr->destroyWidget("FormReview/Label/Help");
		mTrayMgr->destroyWidget("FormReview/Button/CloseOptions");
	}
}

void FormReview::showOptions()
{
	if (!mTrayMgr->getWidget("FormReview/Window/Options")) // menu is hidden
	{
		unsigned int numOptions = 3;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 350;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;

		mTrayMgr->createDialogWindow("FormReview/Window/Options", "REVIEWS OPTIONS", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createLabel("FormReview/Label/EditReviews", "EDIT REVIEWS", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormReview/Label/Help", "HELP", left, top, width, 23); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormReview/Button/CloseOptions", "BACK", left, top, 60);
	}
}

void FormReview::hideOptionsReviews()
{
	if (mTrayMgr->getWidget("FormReview/Window/OptionsReview"))
	{
		mTrayMgr->destroyDialogWindow("FormReview/Window/OptionsReview");
		mTrayMgr->destroyWidget("FormReview/LineEdit/Review");
		mTrayMgr->destroyWidget("FormReview/LineEdit/Score");
		mTrayMgr->destroyWidget("FormReview/LineEdit/Author");
		mTrayMgr->destroyWidget("FormReview/Selector/Review");
		mTrayMgr->destroyWidget("FormReview/Slider/Size");
		mTrayMgr->destroyWidget("FormReview/Slider/PosX");
		mTrayMgr->destroyWidget("FormReview/Slider/PosY");
		mTrayMgr->destroyWidget("FormReview/Label/NewReview");
		mTrayMgr->destroyWidget("FormReview/Label/DeleteReview");
		mTrayMgr->destroyWidget("FormReview/Button/CloseOptionsReviews");
	}
}

void FormReview::showOptionsReviews()
{
	if (!mTrayMgr->getWidget("FormReview/Window/OptionsReview")) // menu is hidden
	{
		unsigned int numOptions = 10;
		Ogre::Real sepOptions = 40, sepButton = 30, sepWindow = 50;
		Ogre::Real left = 50;
		Ogre::Real width = 425;
		Ogre::Real height = numOptions * sepOptions;
		Ogre::Real top = mScreenSize.y - height - sepWindow - sepButton;
		Ogre::StringVector itemsReviews;

		mTrayMgr->createDialogWindow("FormReview/Window/OptionsReview", "EDIT REVIEWS", left, top, width, height);	 top += sepOptions / 2;
		mTrayMgr->createLineEdit("FormReview/LineEdit/Review", "Review", "", left, top, width); top += sepOptions;
		mTrayMgr->createLineEdit("FormReview/LineEdit/Score", "Score  ", "", left, top, width); top += sepOptions;
		mTrayMgr->createLineEdit("FormReview/LineEdit/Author", "Author", "", left, top, width); top += sepOptions;
		mTrayMgr->createItemSelector("FormReview/Selector/Review", "Review", itemsReviews, left, top, width); top += sepOptions;
		mTrayMgr->createSliderOptions("FormReview/Slider/Size", "Size", left, top, width, 12, 32, 100);	top += sepOptions;
		mTrayMgr->createSliderOptions("FormReview/Slider/PosX", "Position X", left, top, width, 0, 100, 100);	top += sepOptions;
		mTrayMgr->createSliderOptions("FormReview/Slider/PosY", "Position Y", left, top, width, 0, 100, 100);	top += sepOptions;
		mTrayMgr->createLabel("FormReview/Label/NewReview", "NEW REVIEW", left, top, width, 23); top += sepOptions;
		mTrayMgr->createLabel("FormReview/Label/DeleteReview", "DELETE REVIEW", left, top, width, 23); top = mScreenSize.y - sepWindow - sepButton;
		mTrayMgr->createButton("FormReview/Button/CloseOptionsReviews", "BACK", left, top, 60);
		
		_loadConfigWindowsReview(0);
	}
}

