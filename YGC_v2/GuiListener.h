#ifndef _GUILISTENER_H__
#define _GUILISTENER_H__

// enumerator values for button states
enum ButtonState
{
	BS_UP,
	BS_OVER,
	BS_DOWN,
	BS_SELECTED // added for menubar
};


struct sMiniButton
{
	Ogre::PanelOverlayElement* panel;
	Ogre::MaterialPtr matUp, matOver, matDown;
	ButtonState currentState;
	Ogre::String action;
};

class Button;
class MenuBar;
class Thumbnail;
class Slider;
class SelectMenu;
class TrackList;
class MediaPlayer;
class Label;
class CheckBox;
class SliderOptions;
class ItemSelector;

/*=============================================================================
| Listener class for responding to tray events.
=============================================================================*/
class GuiListener
{
public:
	virtual ~GuiListener() {}
	virtual void buttonHit(Button* button) {}
	virtual void menuBarItemHit(MenuBar* menu) {}
	virtual void thumbnailHit(Thumbnail* thumbnail) {}
	virtual void sliderMoved(Slider* slider) {}
	virtual void itemSelected(SelectMenu* menu) {}
	virtual void trackListHit(TrackList* track) {}
	virtual void medialPlayerHit(MediaPlayer* player) {}
	virtual void labelHit(Label* label) {}
	virtual void checkBoxToggled(CheckBox* box) {}
	virtual void sliderOptionsMoved(SliderOptions* slider) {}
	virtual void itemChanged(ItemSelector* selector) {}
	virtual void okDialogClosed(const Ogre::DisplayString& message) {}
	virtual void yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit) {}
};

#endif // #ifndef _GUILISTENER_H__

