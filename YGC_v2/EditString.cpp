#include "stdafx.h"
#include "EditString.h"

EditString::EditString(const Ogre::String& text) :
mInsert(true), 
mPosition(mText.begin()), 
mCaret(0)
{
	setText(text);
}

EditString::~EditString()
{
}



void EditString::injectKeyPress(const OIS::KeyEvent arg)
{
	bool keyUsed = true;

	if (std::isgraph(arg.text) || std::isspace(arg.text))
	{
		if (mInsert || mPosition == mText.end())
		{
			mPosition = mText.insert(mPosition, arg.text);
		}
		else
		{
			*mPosition = arg.text;
		}
		++mPosition;
		mCaret++;
	}
	else
	{
		switch (arg.key)
		{
		case OIS::KC_BACK:
			if (mPosition != mText.begin())
			{
				mPosition = mText.erase(--mPosition);
				--mCaret;
			}
			break;

		case OIS::KC_INSERT:
			mInsert = !mInsert;
			break;

		case OIS::KC_HOME:
			mPosition = mText.begin();
			mCaret = 0;
			break;

		case OIS::KC_END:
			mPosition = mText.end();
			mCaret = (int)mText.length();
			break;

		case OIS::KC_LEFT:
			if (mPosition != mText.begin())
			{
				--mPosition;
				mCaret--;
			}
			break;

		case OIS::KC_RIGHT:
			if (mPosition != mText.end())
			{
				++mPosition;
				mCaret++;
			}
			break;

		case OIS::KC_DELETE:
			if (mPosition != mText.end())
				mPosition = mText.erase(mPosition);
			break;

		case OIS::KC_PGDOWN:
			for (unsigned int i = 0; i < 10; ++i)
				moveCursorRight();
			break;

		case OIS::KC_PGUP:
			for (unsigned int i = 0; i < 10; ++i)
				moveCursorLeft();
			break;

		default:
			keyUsed = false;
			break;
		}
	}
}

