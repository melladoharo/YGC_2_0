#include "stdafx.h"
#include "FormSelectorGame.h"
#include "FormOverview.h"
#include "FormReview.h"
#include "FormModels.h"
#include "FormImages.h"
#include "FormVideos.h"
#include "FormMusic.h"
#include "FormCollector.h"

FormSelectorGame::FormSelectorGame(GameInfo* gInfo, GuiManager* tray, GuiListener* oldListener /*= 0*/) :
mTrayMgr(tray),
mGameInfo(gInfo),
mFormCurrent(0),
mFormOverview(0),
mFormReview(0),
mFormModels(0),
mFormImages(0),
mFormVideos(0),
mFormMusic(0),
mFormCollector(0)
{
	// assing this listener to menubar
	mTrayMgr->assignListenerToMenuBar(this);
	mTrayMgr->loadMenuBarGame();
	mTrayMgr->hideMenuBar();
	mTrayMgr->hideCursor();

	// create default resource group for the game
	mGameInfo->createResourceGroup();

	// load a ramdon wallpaper (if the game has one)
	_loadRandomWallpaper();
	
	mTrayMgr->showProgressBar();
	mTrayMgr->setProgressBarProgress(1.0f / 7.0f);
	mTrayMgr->setProgressBarCaption("Loading overview...");
	mFormOverview = new FormOverview(mGameInfo, mTrayMgr);
	mFormOverview->hide();
	mFormOverview->disableForm();
	mTrayMgr->setProgressBarProgress(2.0f / 7.0f);
	mTrayMgr->setProgressBarCaption("Loading review...");
	mFormReview = new FormReview(mGameInfo, mTrayMgr);
	mFormReview->hide();
	mFormReview->disableForm();
	mTrayMgr->setProgressBarProgress(3.0f / 7.0f);
	mTrayMgr->setProgressBarCaption("Loading images...");
	mFormImages = new FormImages(mGameInfo, mTrayMgr);
	mFormImages->hide();
	mFormImages->disableForm();
	mTrayMgr->setProgressBarProgress(4.0f / 7.0f);
	mTrayMgr->setProgressBarCaption("Loading videos...");
	mFormVideos = new FormVideos(mGameInfo, mTrayMgr);
	mFormVideos->hide();
	mFormVideos->disableForm();
	mTrayMgr->setProgressBarProgress(5.0f / 7.0f);
	mTrayMgr->setProgressBarCaption("Loading music...");
	mFormMusic = new FormMusic(mGameInfo, mTrayMgr);
	mFormMusic->hide();
	mFormMusic->disableForm();
	mTrayMgr->setProgressBarProgress(6.0f / 7.0f);
	mTrayMgr->setProgressBarCaption("Loading models...");
	mFormModels = new FormModels(mGameInfo, mTrayMgr);
	mFormModels->hide();
	mFormModels->disableForm();
	mTrayMgr->setProgressBarProgress(7.0f / 7.0f);
	mTrayMgr->setProgressBarCaption("Loading collectors edition...");
	mFormCollector = new FormCollector(mGameInfo, mTrayMgr);
	mFormCollector->hide();
	mFormCollector->disableForm();
	mFormCurrent = mFormOverview;
	mFormCurrent->enableForm();
	mFormCurrent->show();
	mTrayMgr->hideProgressBar();
	mTrayMgr->enableFadeEffect();
	mTrayMgr->showMenuBar();
	mTrayMgr->showCursor();
}


FormSelectorGame::~FormSelectorGame()
{
	if (mFormOverview) delete mFormOverview;
	if (mFormReview) delete mFormReview;
	if (mFormModels) delete mFormModels;
	if (mFormImages) delete mFormImages;
	if (mFormVideos) delete mFormVideos;
	if (mFormMusic) delete mFormMusic;
	if (mFormCollector) delete mFormCollector;

	mGameInfo->destroyResourceGroup();
}

void FormSelectorGame::menuBarItemHit(MenuBar* menu)
{
	FormBase* previousForm = mFormCurrent;

	if (menu->getSelectedItem() == "mbw_game_home") // :) (trick) [returns control to 'FormGames']
	{
		mFormCurrent = previousForm = 0;
	}
	else if (menu->getSelectedItem() == "mbw_game_overview")
	{
		if (!mFormOverview) mFormOverview = new FormOverview(mGameInfo, mTrayMgr);
		mFormCurrent = mFormOverview;
	}
	else if (menu->getSelectedItem() == "mbw_game_review")
	{
		if (!mFormReview) mFormReview = new FormReview(mGameInfo, mTrayMgr);
		mFormCurrent = mFormReview;
	}
	else if (menu->getSelectedItem() == "mbw_game_models")
	{
		if (!mFormModels) mFormModels = new FormModels(mGameInfo, mTrayMgr);
		mFormCurrent = mFormModels;
	}
	else if (menu->getSelectedItem() == "mbw_game_images")
	{
		if (!mFormImages) mFormImages = new FormImages(mGameInfo, mTrayMgr);
		mFormCurrent = mFormImages;
	}
	else if (menu->getSelectedItem() == "mbw_game_videos")
	{
		if (!mFormVideos) mFormVideos = new FormVideos(mGameInfo, mTrayMgr);
		mFormCurrent = mFormVideos;
	}
	else if (menu->getSelectedItem() == "mbw_game_music")
	{
		if (!mFormMusic) mFormMusic = new FormMusic(mGameInfo, mTrayMgr);
		mFormCurrent = mFormMusic;
	}
	else if (menu->getSelectedItem() == "mbw_game_collector")
	{
		if (!mFormCollector) mFormCollector = new FormCollector(mGameInfo, mTrayMgr);
		mFormCurrent = mFormCollector;
	}
	
	// Apply the new form
	if (mFormCurrent)
	{
		mTrayMgr->enableFadeEffect();
		previousForm->hide();
		//previousForm->disableForm(); // some forms need to be update every frame
		mFormCurrent->enableForm();
		mFormCurrent->show();
	}
}

void FormSelectorGame::_loadRandomWallpaper()
{
	std::vector<sInfoResource> infoWalls;
	mGameInfo->findWallResource(infoWalls);

	unsigned int index = rand() % infoWalls.size();
	if (!Ogre::TextureManager::getSingleton().resourceExists(infoWalls[index].name))
		GameInfo::loadImageFromDisk(infoWalls[index].path, infoWalls[index].name, mGameInfo->getGroupName());
	Ogre::MaterialPtr matImage = Ogre::MaterialManager::getSingleton().create(
		"/Mat/Wallpaper", mGameInfo->getGroupName());
	matImage->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	matImage->getTechnique(0)->getPass(0)->createTextureUnitState(infoWalls[index].name);
	mTrayMgr->showBackdrop(matImage->getName());
}
