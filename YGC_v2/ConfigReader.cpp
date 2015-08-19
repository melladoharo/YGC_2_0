#include "Stdafx.h"
#include "ConfigReader.h"

ConfigReader* ConfigReader::mConfigReader = NULL;
SI_Error ConfigReader::mRC = SI_FAIL;

ConfigReader::ConfigReader()
{
	mReader = new CSimpleIniA();
	mRC = mReader->LoadFile(pathConfig.c_str());
	if (mReader->IsEmpty()) _createDefaultConfig();
}

ConfigReader::~ConfigReader()
{
}

ConfigReader* ConfigReader::getSingletonPtr()
{
	return mConfigReader ? mConfigReader : (mConfigReader = new ConfigReader);
}



SI_Error ConfigReader::_createDefaultConfig()
{
	mReader->Reset();
	mReader->SetValue("SYSTEM", "Path_Games", "./Games");
	mReader->SetValue("SYSTEM", "Path_Sagas", "./Sagas");
	mReader->SetValue("SYSTEM.GRAPHICS", "Fullscreen", "Yes");
	mReader->SetValue("SYSTEM.GRAPHICS", "VSync", "Yes");
	mReader->SetValue("SYSTEM.GRAPHICS", "Aspect_Ratio", "Auto");
	mReader->SetValue("SYSTEM.GRAPHICS", "Resolution", "1024 x 768 @ 32-bit colour");
	mReader->SetValue("SYSTEM.GRAPHICS", "FSAA", "0");
	mReader->SetValue("SYSTEM.GRAPHICS", "Shadows", "Yes");
	mReader->SetValue("SYSTEM.GRAPHICS", "Shadows_resolution", "1024");
	mReader->SetValue("SYSTEM.GRAPHICS", "Blur_shadows", "Yes");
	mReader->SetValue("SYSTEM.GRAPHICS", "Filtering_mode", "Anisotropic");
	mReader->SetValue("SYSTEM.GRAPHICS", "Anisotropic_filter", "2");
	mReader->SetValue("FORM.OVERVIEW", "Cover_Resolution", "2048");
	mReader->SetValue("FORM.OVERVIEW", "Disc_Resolution", "1024");
	mReader->SetValue("FORM.OVERVIEW", "Logo_Resolution", "256");
	mReader->SetValue("FORM.OVERVIEW", "BoxArt_Resolution", "1024");
	mReader->SetValue("FORM.GAMES", "Thumbs_Resolution", "256");
	mReader->SetDoubleValue("FORM.GAMES", "Thumbs_Rows", 3);
	mReader->SetDoubleValue("FORM.GAMES", "Thumbs_Size", 2.77f);
	mReader->SetDoubleValue("FORM.GAMES", "Thumbs_Top", 3);
	mReader->SetDoubleValue("FORM.GAMES", "Thumbs_Left", -21);
	mReader->SetDoubleValue("FORM.GAMES", "Thumbs_Horizontal_Sep", 11.5f);
	mReader->SetDoubleValue("FORM.GAMES", "Thumbs_Vertial_Sep", 6.25f);
	mReader->SetValue("FORM.IMAGES", "Thumbs_Resolution", "128");
	mReader->SetDoubleValue("FORM.IMAGES", "Thumbs_Rows", 3);
	mReader->SetDoubleValue("FORM.IMAGES", "Thumbs_Size", 2.77f);
	mReader->SetDoubleValue("FORM.IMAGES", "Thumbs_Top", 3);
	mReader->SetDoubleValue("FORM.IMAGES", "Thumbs_Left", -21);
	mReader->SetDoubleValue("FORM.IMAGES", "Thumbs_Horizontal_Sep", 11.5f);
	mReader->SetDoubleValue("FORM.IMAGES", "Thumbs_Vertial_Sep", 6.25f);
	mReader->SetValue("FORM.VIDEOS", "Thumbs_Resolution", "256");
	mReader->SetDoubleValue("FORM.VIDEOS", "Thumbs_Rows", 3);
	mReader->SetDoubleValue("FORM.VIDEOS", "Thumbs_Size", 2.77f);
	mReader->SetDoubleValue("FORM.VIDEOS", "Thumbs_Top", 3);
	mReader->SetDoubleValue("FORM.VIDEOS", "Thumbs_Left", -21);
	mReader->SetDoubleValue("FORM.VIDEOS", "Thumbs_Horizontal_Sep", 11.5f);
	mReader->SetDoubleValue("FORM.VIDEOS", "Thumbs_Vertial_Sep", 6.25f);
	return mReader->SaveFile(pathConfig.c_str());
}

