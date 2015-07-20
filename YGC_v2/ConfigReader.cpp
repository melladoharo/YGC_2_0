#include "Stdafx.h"
#include "ConfigReader.h"

ConfigReader* ConfigReader::mConfigReader = NULL;
SI_Error ConfigReader::mRC = SI_FAIL;

ConfigReader::ConfigReader()
{
	mReader = new CSimpleIniA();
	mRC = mReader->LoadFile("./ygcConfig.ini");
}

ConfigReader::~ConfigReader()
{
}

ConfigReader* ConfigReader::getSingletonPtr()
{
	return mConfigReader ? mConfigReader : (mConfigReader = new ConfigReader);
}

