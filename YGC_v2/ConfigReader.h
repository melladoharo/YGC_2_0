#ifndef _CONFIGREADER_H__
#define _CONFIGREADER_H__

class ConfigReader
{
public:
	~ConfigReader();
	static ConfigReader* getSingletonPtr();

	CSimpleIniA* getReader(){ return mReader; }

	SI_Error getLastError() { return mRC; }

private:
	ConfigReader();
	ConfigReader(ConfigReader& C) {};
	ConfigReader& operator=(ConfigReader& C) {};
	static ConfigReader* mConfigReader;
	static SI_Error mRC;
	CSimpleIniA* mReader;
};

#endif // _CONFIGREADER_H__

