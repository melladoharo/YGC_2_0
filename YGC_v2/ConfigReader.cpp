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
	mReader->SetDoubleValue("FORM_GAMES", "Thumbs_Rows", 3);
	mReader->SetDoubleValue("FORM_GAMES", "Thumbs_Size", 2.77f);
	mReader->SetDoubleValue("FORM_GAMES", "Thumbs_Top", 3);
	mReader->SetDoubleValue("FORM_GAMES", "Thumbs_Left", -21);
	mReader->SetDoubleValue("FORM_GAMES", "Thumbs_Horizontal_Sep", 11.5f);
	mReader->SetDoubleValue("FORM_GAMES", "Thumbs_Vertial_Sep", 6.25f);
	mReader->SetDoubleValue("FORM_IMAGES", "Thumbs_Rows", 3);
	mReader->SetDoubleValue("FORM_IMAGES", "Thumbs_Size", 2.77f);
	mReader->SetDoubleValue("FORM_IMAGES", "Thumbs_Top", 3);
	mReader->SetDoubleValue("FORM_IMAGES", "Thumbs_Left", -21);
	mReader->SetDoubleValue("FORM_IMAGES", "Thumbs_Horizontal_Sep", 11.5f);
	mReader->SetDoubleValue("FORM_IMAGES", "Thumbs_Vertial_Sep", 6.25f);
	mReader->SetDoubleValue("FORM_VIDEOS", "Thumbs_Rows", 3);
	mReader->SetDoubleValue("FORM_VIDEOS", "Thumbs_Size", 2.77f);
	mReader->SetDoubleValue("FORM_VIDEOS", "Thumbs_Top", 3);
	mReader->SetDoubleValue("FORM_VIDEOS", "Thumbs_Left", -21);
	mReader->SetDoubleValue("FORM_VIDEOS", "Thumbs_Horizontal_Sep", 11.5f);
	mReader->SetDoubleValue("FORM_VIDEOS", "Thumbs_Vertial_Sep", 6.25f);
	return mReader->SaveFile(pathConfig.c_str());
}

