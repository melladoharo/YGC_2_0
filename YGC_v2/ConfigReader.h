#ifndef _CONFIGREADER_H__
#define _CONFIGREADER_H__

static const std::string pathConfig = "./YGC2.ini";

class ConfigReader
{
public:
	~ConfigReader();
	static ConfigReader* getSingletonPtr();

	CSimpleIniA* getReader(){ return mReader; }
	SI_Error getLastError() { return mRC; }
	SI_Error saveConfig() 
	{ 
		if (mReader)
			return mReader->SaveFile(pathConfig.c_str());
		return SI_FAIL;
	}

private:
	ConfigReader();
	ConfigReader(ConfigReader& C) {};
	ConfigReader& operator=(ConfigReader& C) {};
	SI_Error _createDefaultConfig();

	static ConfigReader* mConfigReader;
	CSimpleIniA* mReader;
	static SI_Error mRC;
};

#endif // #ifndef _CONFIGREADER_H__

