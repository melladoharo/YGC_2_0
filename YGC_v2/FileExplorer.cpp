#include "stdafx.h"
#include "FileExplorer.h"
#include "ConfigReader.h"

FileExplorer::FileExplorer(const Ogre::String& name, const Ogre::String& path, Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height) :
mPath(path),
mDisplayIndex(0),
mSelectionIndex(-1),
mDragging(false),
mDragOffset(0.0f)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/FileExplorer", "Panel", name);
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	mHeader = (Ogre::OverlayContainer*)c->getChild(c->getName() + "/YgcGui/FileExplorerHeader");
	mPrevButton = (Ogre::PanelOverlayElement*)mHeader->getChild(mHeader->getName() + "/PreviousButton");
	mNextButton = (Ogre::PanelOverlayElement*)mHeader->getChild(mHeader->getName() + "/NextButton");
	mHomeButton = (Ogre::PanelOverlayElement*)mHeader->getChild(mHeader->getName() + "/HomeButton");
	Ogre::OverlayContainer* hc = (Ogre::OverlayContainer*)mHeader->getChild(mHeader->getName() + "/HeaderContainerTextArea");
	mTextAreaFullPath = (Ogre::TextAreaOverlayElement*)hc->getChild(hc->getName() + "/FileExplorerCaption");
	mBody = (Ogre::OverlayContainer*)c->getChild(c->getName() + "/YgcGui/FileExplorerBody");
	Ogre::OverlayContainer* bc = (Ogre::OverlayContainer*)mBody->getChild(mBody->getName() + "/BodyContainerTextArea");
	mTextAreaParentPath = (Ogre::TextAreaOverlayElement*)bc->getChild(bc->getName() + "/ItemsCaption");
	mTrackFront = (Ogre::BorderPanelOverlayElement*)mBody->getChild(mBody->getName() + "/ExplorerTrackFront");
	mHandle = (Ogre::PanelOverlayElement*)mTrackFront->getChild(mTrackFront->getName() + "/ExplorerHandle");

	// spacing elements
	mElement->setLeft(left);
	mElement->setTop(top);
	mElement->setWidth(width);
	mPrevButton->setLeft(12);
	mNextButton->setLeft(mPrevButton->getLeft() + mPrevButton->getWidth() + 10);
	mHomeButton->setLeft(mNextButton->getLeft() + mNextButton->getWidth() + 10);
	hc->setWidth(mElement->getWidth() - mHomeButton->getLeft() - mHomeButton->getWidth() - 10 - 12);
	hc->setLeft(mElement->getWidth() - hc->getWidth() - 12);
	mTextAreaFullPath->setWidth(hc->getWidth() - 7);
	mBody->setWidth(mElement->getWidth());
	mBody->setHeight(height - mHeader->getHeight() - 40);
	mBody->setTop(mHeader->getHeight() + 20);
	mTextAreaParentPath->setWidth(mElement->getWidth());
	mTrackFront->setLeft(mElement->getWidth() - mTrackFront->getWidth() - 16);
	mTrackFront->setHeight(mBody->getHeight() - bc->getHeight() - 20);
	mTrackFront->setTop(bc->getHeight()); 
	mHandle->setWidth(mTrackFront->getWidth() + 6);
	mHandle->setHeight(20);
	mHandle->setLeft(-3);
	// max items shown
	mMaxItemsShown = mBody->getHeight() / 35; // ...mBody->getHeight() / item->getHeight();
	setPath(path);
}

FileExplorer::~FileExplorer()
{
}



void FileExplorer::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (mDragging)
	{
		Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);
		Ogre::Real newTop = mHandle->getTop() + co.y - mDragOffset;
		Ogre::Real lowerBoundary = mTrackFront->getHeight() - mHandle->getHeight();
		mHandle->setTop(Ogre::Math::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

		Ogre::Real scrollPercentage = Ogre::Math::Clamp<Ogre::Real>(newTop / lowerBoundary, 0, 1);
		int newIndex = (int)(scrollPercentage * (mItems.size() - mItemElements.size()) + 0.5);
		if (newIndex != mDisplayIndex) _setDisplayIndex(newIndex);
		return;
	}

	for (unsigned int i = 0; i < mItemsShown; ++i)
	{
		if (i != mSelectionIndex)
		{
			if (isCursorOver(mItemElements[i], cursorPos))
				mItemElements[i]->setParameter("transparent", "false");
			else
				mItemElements[i]->setParameter("transparent", "true");
		}	
	}
}

void FileExplorer::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);

	if (co.squaredLength() <= 81)
	{
		mDragging = true;
		mDragOffset = co.y;
		return;
	}
	else if (Widget::isCursorOver(mTrackFront, cursorPos, -10))
	{
		Ogre::Real newTop = mHandle->getTop() + co.y;
		Ogre::Real lowerBoundary = mTrackFront->getHeight() - mHandle->getHeight();
		mHandle->setTop(Ogre::Math::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

		Ogre::Real scrollPercentage = Ogre::Math::Clamp<Ogre::Real>(newTop / lowerBoundary, 0, 1);
		_setDisplayIndex((unsigned int)(scrollPercentage * (mItems.size() - mItemElements.size()) + 0.5));
		return;
	}
}

void FileExplorer::_cursorReleased(const Ogre::Vector2& cursorPos)
{
	mDragging = false;

	if (isCursorOver(mPrevButton, cursorPos))
	{ 
		if (mPath.has_parent_path() && mPath.parent_path().is_absolute())
			setPath(mPath.parent_path().generic_string());
		else
			_setPathHome();
		return;
	}
	else if (Widget::isCursorOver(mHomeButton, cursorPos))
	{
		_setPathHome();
		return;
	}
	for (unsigned int i = 0; i < mItemsShown; ++i)
	{
		if (mItemElements[i]->getParameter("transparent") == "false" && i != mSelectionIndex)
		{
			// deselect previous item selected
			if (mSelectionIndex != -1)
				mItemElements[mSelectionIndex]->setParameter("transparent", "true");
			
			Ogre::OverlayContainer* ic = (Ogre::OverlayContainer*)mItemElements[i];
			Ogre::TextAreaOverlayElement* textArea = (Ogre::TextAreaOverlayElement*)ic->getChild(ic->getName() + "/ItemFileExplorerCaption");
			Ogre::String newPath = textArea->getCaption().substr(3); // removes the " + " from caption
			if (!boost::filesystem::path(newPath).is_absolute())
			{
				if (mPath.generic_string()[mPath.generic_string().size() - 1] != '/') // no repeat character '/' 
					newPath = "/" + newPath;
				newPath = mPath.generic_string() + newPath;
			}

			// if new path is directory, go to new directory
			if (boost::filesystem::is_directory(newPath))
			{
				setPath(newPath);
			}
			// if it is a file, mark as selected
			else if (boost::filesystem::is_regular_file(newPath))
			{
				mSelectionIndex = i;
				mSelectedPath = newPath;
			}
			return;
		}
	}
}



void FileExplorer::setPath(const Ogre::String& path)
{
	mPath = mSelectedPath = boost::filesystem::path(path);
	mSelectionIndex = -1;
	mHandle->setTop(0);
	_destroyItems();
	_getPathItems();
	fitCaptionToArea(mPath.generic_string(), mTextAreaFullPath, mTextAreaFullPath->getWidth());
	fitCaptionToArea(mPath.has_leaf() ? " - " + mPath.leaf().generic_string() : " - " + mPath.generic_string(), mTextAreaParentPath, mTextAreaParentPath->getWidth());

	mItemsShown = std::max<int>(0, std::min<int>(mMaxItemsShown, mItems.size()));

	for (unsigned int i = 0; i < mItemsShown; i++)   // create all the item elements
	{
		Ogre::String nameItem = "ItemFileExplorer_n" + Ogre::StringConverter::toString(i + 1);
		_createTextAreaItems(nameItem, mItems[i], (i + 1) * 30);
	}
}



void FileExplorer::scrollUp()
{
	if (mItems.size() > mMaxItemsShown)
	{
		Ogre::Real lowerBoundary = mTrackFront->getHeight() - mHandle->getHeight();
		Ogre::Real sizeDesp = (3.0f * lowerBoundary) / mItems.size();
		mHandle->setTop(mHandle->getTop() - sizeDesp);
		Ogre::Real newTop = mHandle->getTop();
		mHandle->setTop(Ogre::Math::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

		Ogre::Real scrollPercentage = Ogre::Math::Clamp<Ogre::Real>(newTop / lowerBoundary, 0, 1);
		_setDisplayIndex((unsigned int)(scrollPercentage * (mItems.size() - mItemElements.size()) + 0.5));
	}
}

void FileExplorer::scrollDown()
{
	if (mItems.size() > mMaxItemsShown)
	{
		Ogre::Real lowerBoundary = mTrackFront->getHeight() - mHandle->getHeight();
		Ogre::Real sizeDesp = (3.0f * lowerBoundary) / mItems.size();
		mHandle->setTop(mHandle->getTop() + sizeDesp);
		Ogre::Real newTop = mHandle->getTop();
		mHandle->setTop(Ogre::Math::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

		Ogre::Real scrollPercentage = Ogre::Math::Clamp<Ogre::Real>(newTop / lowerBoundary, 0, 1);
		_setDisplayIndex((unsigned int)(scrollPercentage * (mItems.size() - mItemElements.size()) + 0.5));
	}
}



void FileExplorer::_getPathItems()
{
	mItems.clear();
	// first directory items
	for (boost::filesystem::directory_iterator it(mPath), end; it != end; ++it)
		if (boost::filesystem::is_directory(it->path()))
			mItems.push_back(" + " + it->path().leaf().generic_string());
	// then, files
	for (boost::filesystem::directory_iterator it(mPath), end; it != end; ++it)
		if (!boost::filesystem::is_directory(it->path()))
			mItems.push_back("   " + it->path().leaf().generic_string());
}

void FileExplorer::_destroyItems()
{
	for (unsigned int i = 0; i < mItemElements.size(); ++i)
	{
		mBody->removeChild(mItemElements[i]->getName());
		Widget::nukeOverlayElement(mItemElements[i]);
	}
	mItemElements.clear();
}

void FileExplorer::_createTextAreaItems(const Ogre::String& name, const Ogre::String& caption, Ogre::Real top)
{
	mItemElements.push_back(Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("ItemFileExplorer", "Panel", name));
	mBody->addChild(mItemElements.back());
	mItemElements.back()->setTop(top);
	mItemElements.back()->setWidth(mElement->getWidth() - mTrackFront->getWidth() - 12 - 16);
	Ogre::OverlayContainer* ic = (Ogre::OverlayContainer*)mItemElements.back();
	Ogre::TextAreaOverlayElement* textArea = (Ogre::TextAreaOverlayElement*)ic->getChild(ic->getName() + "/ItemFileExplorerCaption");
	textArea->setLeft(40);
	fitCaptionToArea(caption, textArea, mItemElements.back()->getWidth() - 40);
}

void FileExplorer::_setDisplayIndex(unsigned int index)
{
	index = std::min<int>(index, mItems.size() - mItemElements.size());
	mDisplayIndex = index;

	Ogre::OverlayContainer* ie;
	Ogre::TextAreaOverlayElement* ta;
	for (int i = 0; i < (int)mItemElements.size(); i++)
	{
		ie = (Ogre::OverlayContainer*)mItemElements[i];
		ta = (Ogre::TextAreaOverlayElement*)ie->getChild(ie->getName() + "/ItemFileExplorerCaption");
		fitCaptionToArea(mItems[mDisplayIndex + i], ta, mItemElements.back()->getWidth() - 40);
	}
}

void FileExplorer::_setPathHome()
{
	mItems.clear();
	mPath = mSelectedPath = Ogre::StringUtil::BLANK;
	mSelectionIndex = -1;
	mHandle->setTop(0);
	_destroyItems();
	
	fitCaptionToArea("System", mTextAreaFullPath, mTextAreaFullPath->getWidth());
	fitCaptionToArea(" - System", mTextAreaParentPath, mTextAreaParentPath->getWidth());

	// explore system units availables
	for (char i = 'A'; i <= 'Z'; ++i)
	{
		Ogre::String systemUnit = { i, ':', '/'};
		if (boost::filesystem::is_directory(systemUnit))
		{
			mItems.push_back(" + " + systemUnit);
		}
	}

	// search for my documents (windows only)
	for (unsigned int i = 0; i < mItems.size(); ++i)
	{
		if (boost::filesystem::is_directory(mItems[i] + "Users"))
		{
			for (boost::filesystem::directory_iterator it(mItems[i] + "Users"), end; it != end; ++it)
			{
				if (boost::filesystem::is_directory(*it) &&
					it->path().filename().generic_string() != "All Users" &&
					it->path().filename().generic_string() != "Default" &&
					it->path().filename().generic_string() != "Public")
				{
					mItems.push_back(" + " + it->path().generic_string());
				}
			}
		}
	}
	
	// path games
	Ogre::String mainPathGames = ConfigReader::getSingletonPtr()->getReader()->GetValue("SYSTEM", "Path_Games", "./Games");
	if (boost::filesystem::is_directory(mainPathGames))
		mItems.push_back(" + " + mainPathGames);

	mItemsShown = std::max<int>(0, std::min<int>(mMaxItemsShown, mItems.size()));

	for (unsigned int i = 0; i < mItemsShown; i++)   // create all the item elements
	{
		Ogre::String nameItem = "ItemFileExplorer_n" + Ogre::StringConverter::toString(i + 1);
		_createTextAreaItems(nameItem, mItems[i], (i + 1) * 30);
	}
}


