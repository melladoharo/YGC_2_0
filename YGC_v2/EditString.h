#ifndef _EDITSTRING_H__
#define _EDITSTRING_H__

class EditString
{
public:
	EditString(const Ogre::String& text);
	~EditString();

	void injectKeyPress(const OIS::KeyEvent arg);

	void setText(const Ogre::String& newText)
	{
		mText = newText;
		mPosition = mText.end();
		mCaret = (int)mText.length();
	}

	Ogre::String& getText()
	{
		return mText;
	}

	int getCursorPosition()
	{
		return mCaret;
	}
	void setCursorPosition(int newPos)
	{
		if (newPos < mText.length())
		{
			mCaret = newPos;
			mPosition = mText.begin() + newPos;
		}
	}

	void moveCursorLeft()
	{
		if (mPosition != mText.begin())
		{
			--mPosition;
			mCaret--;
		}
	}
	void moveCursorRight()
	{
		if (mPosition != mText.end())
		{
			++mPosition;
			mCaret++;
		}
	}

private:
	Ogre::String mText; // The text for editing
	bool mInsert; // Overwrite or insert
	Ogre::String::iterator mPosition; // Position for insert / overwrite
	int mCaret; // Caret Position - for positioning the cursor.
};

#endif // #ifndef _EDITSTRING_H__

