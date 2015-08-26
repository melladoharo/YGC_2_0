#include "stdafx.h"
#include "MenuBar.h"

MenuBar::MenuBar() :
mFont(Ogre::FontManager::getSingletonPtr()->getByName("YgcFont/SemiBold/21")),
mIndexOver(0),
mIndexSelected(0),
mMouseOverBar(false)
{
	// Colours for font selected or not
	Ogre::MaterialPtr mataux = Ogre::MaterialManager::getSingletonPtr()->getByName("YgcGui/MenuBar/Colours");
	mColourSelect = mataux->getTechnique(0)->getPass(0)->getAmbient();
	mColourDeselect = mataux->getTechnique(0)->getPass(0)->getDiffuse();

	// Create main bar
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mElement = om.createOverlayElementFromTemplate("YgcGui/MenuBar", "BorderPanel", "MenuBarWidget");
	mElement->setWidth(Ogre::Root::getSingleton().getAutoCreatedWindow()->getViewport(0)->getActualWidth());
	mBar = (Ogre::OverlayContainer*)mElement;

	// Default create the main menu
	createMenuMain();
}

MenuBar::~MenuBar()
{
	_destroyAllItems();
}

void MenuBar::_destroyAllItems()
{
	for (unsigned int i = 0; i < mItems.size(); ++i)
	{
		mBar->removeChild(mItems[i].panel->getName());
		Widget::nukeOverlayElement(mItems[i].panel);
	}
	mItems.clear();
}



void MenuBar::createMenuMain()
{
	_destroyAllItems();

	Ogre::Real currLeft = sizeInPixels("       ", mFont->getName(), 21, 7);
	_createNewOption("mbw_main_games", "Games", currLeft);
	_createNewOption("mbw_main_sagas", "Sagas", currLeft);
	_createNewOption("mbw_main_options", "Options", currLeft);
	currLeft = Ogre::Real(mElement->getWidth() - sizeInPixels(" Exit        ", mFont->getName(), 21, 7));
	_createNewOption("mbw_main_exit", " Exit ", currLeft);

	mIndexSelected = mIndexOver = 0;
	setState(BS_SELECTED, mIndexSelected);
}

void MenuBar::createMenuGame()
{
	_destroyAllItems();

	Ogre::Real currLeft = sizeInPixels("       ", mFont->getName(), 21, 7);
	_createNewOption("mbw_game_overview", "Overview", currLeft);
	_createNewOption("mbw_game_review", "Review", currLeft);
	_createNewOption("mbw_game_images", "Images", currLeft);
	_createNewOption("mbw_game_videos", "Videos", currLeft);
	//_createNewOption("mbw_game_music", "Music", currLeft);
	_createNewOption("mbw_game_models", "Models", currLeft);
	_createNewOption("mbw_game_collector", "Collector", currLeft);
	_createNewOption("mbw_game_play", "Play", currLeft);
	currLeft = Ogre::Real(mElement->getWidth() - sizeInPixels("Games       ", mFont->getName(), 21, 7));
	_createNewOption("mbw_game_home", "Games", currLeft);

	mIndexSelected = mIndexOver = 0;
	setState(BS_SELECTED, mIndexSelected);
}

void MenuBar::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	mMouseOverBar = isCursorOver(mBar, cursorPos);

	if (isCursorOver(mItems[mIndexOver].panel, cursorPos))
		setState(BS_DOWN, mIndexOver);
}

void MenuBar::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	mMouseOverBar = isCursorOver(mBar, cursorPos);

	if (mItems[mIndexOver].currentState == BS_DOWN)
	{
		setState(BS_SELECTED, mIndexOver);
		if (mListener)
			mListener->menuBarItemHit(this);		
	}
}

void MenuBar::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	mMouseOverBar = isCursorOver(mBar, cursorPos);

	if (cursorPos.y < mElement->getHeight() + 10) // is over the main bar? (+10 margen to deselect item)
	{
		for (unsigned int i = 0; i < mItems.size(); ++i) // look for the item cursor is over it
		{
			if (mItems[i].currentState != BS_SELECTED && mItems[i].panel->isVisible())
			{
				if (isCursorOver(mItems[i].panel, cursorPos))
				{
					if (mItems[i].currentState == BS_UP)
					{
						mIndexOver = i;
						setState(BS_OVER, i);
					}
					break;
				}
				else
				{
					if (mItems[i].currentState != BS_UP) setState(BS_UP, i);
				}
			}
		}
	}
	else
	{
		if (mItems[mIndexOver].currentState != BS_SELECTED)
			setState(BS_UP, mIndexOver);
	}
}

void MenuBar::_forceCursorReleased()
{
	setState(BS_SELECTED, mIndexOver);
	if (mListener)
		mListener->menuBarItemHit(this);
	mMouseOverBar = false;
}


void MenuBar::setState(const ButtonState& bs, int index)
{
	if (bs == BS_OVER)
	{
		mItems[index].panel->setMaterialName("YgcGui/MenuBar/Over");
		mItems[index].textArea->setColour(mColourSelect);
	}
	else if (bs == BS_UP)
	{
		mItems[index].panel->setMaterialName("YgcGui/MenuBar");
		mItems[index].textArea->setColour(mColourDeselect);
	}
	else if (bs == BS_DOWN)
	{
		mItems[mIndexSelected].panel->setMaterialName("YgcGui/MenuBar");
		mItems[mIndexSelected].textArea->setColour(mColourDeselect);
		mItems[mIndexSelected].currentState = BS_UP;
		mItems[index].panel->setMaterialName("YgcGui/MenuBar/Selected");
		mItems[index].textArea->setColour(mColourSelect);
		mIndexSelected = index;
	}
	else // if (bs==BS_SELECTED)
	{
		mItems[index].panel->setMaterialName("YgcGui/MenuBar/Selected");
		mItems[index].textArea->setColour(mColourSelect);
	}

	mItems[index].currentState = bs;
}

void MenuBar::_createNewOption(const Ogre::String& namePanel, const Ogre::String& textOption, Ogre::Real& sepLeft)
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

	mItems.push_back(sItem());
	mItems.back().panel = (Ogre::BorderPanelOverlayElement*)om.createOverlayElementFromTemplate("YgcGui/MenuBar/Item", "BorderPanel", namePanel);
	mItems.back().textArea = (Ogre::TextAreaOverlayElement*)mItems.back().panel->getChild(mItems.back().panel->getName() + "/ItemCaption");
	mItems.back().currentState = BS_UP;
	mBar->addChild(mItems.back().panel);

	Ogre::Real optWidth = sizeInPixels(textOption, mFont->getName(), 21, 7);
	mItems.back().panel->setLeft(sepLeft);
	mItems.back().panel->setWidth(optWidth);
	mItems.back().textArea->setCaption(textOption);
	mItems.back().textArea->setColour(mColourDeselect);
	sepLeft += optWidth + sizeInPixels("       ", mFont->getName(), 21, 7);
}

