#include "stdafx.h"
#include "LineEdit.h"

LineEdit::LineEdit(const Ogre::String& name, const Ogre::String& caption, const Ogre::String& message, 
	Ogre::Real left, Ogre::Real top, Ogre::Real width) : 
mEditString(new EditString(message)),
mLeftIndexMessage(0), mLeftIndexCursor(0),
mRightIndexMessage(0), mRightIndexCursor(0),
mMaxCharacters(0)
{
	mElement = Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("YgcGui/LineEdit", "Panel", name);
	Ogre::OverlayContainer* c = (Ogre::OverlayContainer*)mElement;
	mTextAreaCaption = (Ogre::TextAreaOverlayElement*)c->getChild(c->getName() + "/LineEditCaption");
	mMessage = (Ogre::OverlayElement*)c->getChild(c->getName() + "/LineEditMessage");
	c = (Ogre::OverlayContainer*)mMessage;
	mTextAreaMessage = (Ogre::TextAreaOverlayElement*)c->getChild(c->getName() + "/LineEditMessageCaption");
	mCursor = (Ogre::OverlayElement*)c->getChild(c->getName() + "/LineEditCursor");
	mCursor->hide();

	// spacing elements
	mElement->setLeft(left);
	mElement->setTop(top);
	mElement->setWidth(width);
	mTextAreaCaption->setCaption(caption);
	mTextAreaCaption->setLeft(12);
	Ogre::Real sizeCaption = sizeInPixels(mTextAreaCaption->getCaption(), mTextAreaCaption->getFontName(), 
		mTextAreaCaption->getCharHeight(), mTextAreaCaption->getSpaceWidth());
	mMessage->setLeft(sizeCaption + ((mElement->getWidth() - sizeCaption) / 4));
	mMessage->setWidth(mElement->getWidth() - mMessage->getLeft() - 12);
	mTextAreaMessage->setWidth(mMessage->getWidth() - 5);

	// how many characters fit in the textarea
	Ogre::Font* f = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(mTextAreaMessage->getFontName()).getPointer();
	Ogre::Real charWidth = f->getGlyphAspectRatio('Z') * mTextAreaMessage->getCharHeight();
	mMaxCharacters = mTextAreaMessage->getWidth() / charWidth;

	setText(message);
}

LineEdit::~LineEdit()
{
	if (mEditString) delete mEditString;
}




void LineEdit::_cursorMoved(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mElement, cursorPos)) mElement->setParameter("transparent", "false");
	else mElement->setParameter("transparent", "true");
}

void LineEdit::_cursorPressed(const Ogre::Vector2& cursorPos)
{
	if (isCursorOver(mElement, cursorPos))
	{
		mCursor->show();
	}
	else
	{
		mCursor->hide();
	}
}

void LineEdit::setText(const Ogre::String& text)
{
	mEditString->setText(text);
	mEditString->setCursorPosition(0);
	mLeftIndexCursor = mLeftIndexMessage = mRightIndexMessage = 0;
	// right index cursor 
	mRightIndexCursor = std::min(mEditString->getText().size(), mMaxCharacters);
	// set caption
	mTextAreaMessage->setCaption(text.substr(mLeftIndexMessage, (mRightIndexCursor - mLeftIndexMessage)));
}

void LineEdit::injectKeyPress(const OIS::KeyEvent &arg)
{
	if (mCursor->isVisible() && mEditString)
	{
		mEditString->injectKeyPress(arg);
		Ogre::String message = mEditString->getText();

		if (std::isgraph(arg.text) || std::isspace(arg.text)) // is alphanumeric or white-space?
		{
			if (mRightIndexMessage < mEditString->getText().size())
			{
				mRightIndexMessage++;
				
				if (mRightIndexCursor < mMaxCharacters)
				{
					mRightIndexCursor++;
				}
				if (mEditString->getCursorPosition() > mMaxCharacters)
				{
					mRightIndexCursor++;
					mLeftIndexMessage++;
					mLeftIndexCursor++;
				}
				mTextAreaMessage->setCaption(message.substr(mLeftIndexMessage, (mRightIndexCursor - mLeftIndexMessage)));
			}
		}
		else if (arg.key == OIS::KC_LEFT) 
		{
			if (mRightIndexMessage> 0)
			{
				mRightIndexMessage--;
				if (mEditString->getCursorPosition() < mLeftIndexCursor)
				{
					mLeftIndexMessage--;
					mLeftIndexCursor--;
					mRightIndexCursor--;
					mTextAreaMessage->setCaption(message.substr(mLeftIndexMessage, (mRightIndexCursor - mLeftIndexMessage)));
				}
			}
		}
		else if (arg.key == OIS::KC_RIGHT)
		{
			if (mRightIndexMessage < mEditString->getText().size())
			{
				mRightIndexMessage++;
				if (mEditString->getCursorPosition() > mRightIndexCursor)
				{
					mLeftIndexMessage++;
					mRightIndexCursor++;
					mLeftIndexCursor++;
					mTextAreaMessage->setCaption(message.substr(mLeftIndexMessage, (mRightIndexMessage - mLeftIndexMessage)));
				}
			}
		}
		else if (arg.key == OIS::KC_BACK)
		{
			if (mRightIndexMessage > 0)
			{
				mRightIndexMessage--;
				if (mLeftIndexMessage > 0)
				{
					mLeftIndexMessage--;
					mLeftIndexCursor--;
					mRightIndexCursor--;
				}
				mTextAreaMessage->setCaption(message.substr(mLeftIndexMessage, (mRightIndexCursor - mLeftIndexMessage)));
			}
		}
		else if (arg.key == OIS::KC_DELETE)
		{
			mTextAreaMessage->setCaption(message.substr(mLeftIndexMessage, (mRightIndexCursor - mLeftIndexMessage)));
		}
		else if (arg.key == OIS::KC_HOME)
		{
			mEditString->setCursorPosition(0);
			mLeftIndexCursor = mLeftIndexMessage = mRightIndexMessage = 0;
			mRightIndexCursor = std::min(mEditString->getText().size(), mMaxCharacters);
			mTextAreaMessage->setCaption(message.substr(mLeftIndexMessage, (mRightIndexCursor - mLeftIndexMessage)));
		}
		else if (arg.key == OIS::KC_END)
		{
			mEditString->setCursorPosition(mEditString->getText().size());
			mRightIndexCursor = mRightIndexMessage = mEditString->getText().size();
			mLeftIndexCursor = mLeftIndexMessage = (mRightIndexMessage > mMaxCharacters)
				? mRightIndexMessage - mMaxCharacters
				: 0;
			mTextAreaMessage->setCaption(message.substr(mLeftIndexMessage, (mRightIndexMessage - mLeftIndexMessage)));
		}
		else if (arg.key == OIS::KC_PGUP)
		{
			for (unsigned int i = 0; i < 10; i++)
			{
				if (mRightIndexMessage > 0)
				{
					mRightIndexMessage--;
					if (mEditString->getCursorPosition() < mLeftIndexCursor)
					{
						mLeftIndexMessage--;
						mLeftIndexCursor--;
						mRightIndexCursor--;
					}
				}
			}
			mTextAreaMessage->setCaption(message.substr(mLeftIndexMessage, (mRightIndexCursor - mLeftIndexMessage)));
		}
		else if (arg.key == OIS::KC_PGDOWN)
		{
			for (unsigned int i = 0; i < 10; i++)
			{
				if (mRightIndexMessage < mEditString->getText().size())
				{
					mRightIndexMessage++;
					if (mEditString->getCursorPosition() >= mRightIndexCursor)
					{
						mLeftIndexMessage++;
						mRightIndexCursor++;
						mLeftIndexCursor++;
					}
				}
			}
			mTextAreaMessage->setCaption(message.substr(mLeftIndexMessage, (mRightIndexCursor - mLeftIndexMessage)));
		}

		// update cursor position
		Ogre::Real sizeMessage = sizeInPixels(message.substr(mLeftIndexMessage, (mRightIndexMessage - mLeftIndexMessage)),
			mTextAreaMessage->getFontName(), mTextAreaMessage->getCharHeight(), mTextAreaMessage->getSpaceWidth());
		mCursor->setLeft(sizeMessage);
	}
}

