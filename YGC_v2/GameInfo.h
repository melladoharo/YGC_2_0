#ifndef _GAMEINFO_H__
#define _GAMEINFO_H__

/*=============================================================================
| sInfoResource struct - find a load textures, images, videos, models, ...
=============================================================================*/
struct sInfoResource
{
	Ogre::String path;		// full path (X:/Games/Game_x/.../resource.res)
	Ogre::String name;		// name for ogre resource manager (unique)
	Ogre::String pathThumb;	// full path for the thumbnail resource (if needed, image resource for example)
	Ogre::String nameThumb;	// name thumbnail for ogre resource manager (unique)
	Ogre::String filename;	// filename resource [resource.res, image.jpg, video.mp4, model.zip, ...]
	Ogre::String caption;	// visible name for resource [resource.res --> resource, image.jpg --> image, ...]
};

// FileType enum (checking the extension of a file)
enum eFileType{ FT_IMAGE, FT_VIDEO, FT_SOUND, FT_INI, FT_ZIP };

/*=============================================================================
| GameInfo class - managment the resources of the game selected
=============================================================================*/
class GameInfo
{
public:
	GameInfo(const Ogre::String& pathGame);
	~GameInfo();

	// find resources at runtime
	void findHeaderResource(sInfoResource& infoHeader)
	{
		_findGenericResource(infoHeader, getPathGame() + "/Images/Header/", FT_IMAGE, "FormGames/Header/", "sdk_default_header.png");
	}
	void findCoverResource(sInfoResource& infoCover)
	{		
		_findGenericResource(infoCover, getPathGame() + "/Images/Cover/", FT_IMAGE, "Overview/Cover/", "sdk_default_cover.png");
	}
	void findLogoResource(sInfoResource& infoLogo)
	{
		_findGenericResource(infoLogo, getPathGame() + "/Images/Logo/", FT_IMAGE, "Overview/Logo/", "sdk_default_logo.png");
	}
	void findDiscResource(std::vector<sInfoResource>& infoDisc)
	{
		_findGenericResource(infoDisc, getPathGame() + "/Images/Discs/", FT_IMAGE, "Overview/Discs/", "sdk_default_disc.png");
	}
	void findWallResource(std::vector<sInfoResource>& infoWall)
	{
		_findGenericResource(infoWall, getPathGame() + "/Images/Wallpapers/", FT_IMAGE, "Overview/Wallpaper/", "sdk_default_wall.png");
	}
	void findBoxArtResource(sInfoResource& infoBoxArt)
	{
		_findGenericResource(infoBoxArt, getPathGame() + "/Images/BoxArt/", FT_IMAGE, "Overview/BoxArt/", "sdk_default_boxart.png");
	}
	void findConceptArtResources(std::vector<sInfoResource>& infoImages)
	{
		_findGenericResource(infoImages, getPathGame() + "/Images/Concept Art/", FT_IMAGE, "ConceptArt/");
	}
	void findScreenshotsResources(std::vector<sInfoResource>& infoImages)
	{
		_findGenericResource(infoImages, getPathGame() + "/Images/Screenshots/", FT_IMAGE, "Screenshot/");
	}
	void findSoundtrackResources(std::vector<sInfoResource>& infoTracks)
	{
		_findGenericResource(infoTracks, getPathGame() + "/Sounds/Soundtrack", FT_SOUND, "Sound/Soundtrack/");
	}
	void findTrailersResources(std::vector<sInfoResource>& infoTrailers)
	{
		_findGenericResource(infoTrailers, getPathGame() + "/Videos/Trailers/", FT_VIDEO, "Video/Trailer/");
	}
	void findGameplaysResources(std::vector<sInfoResource>& infoGameplays)
	{
		_findGenericResource(infoGameplays, getPathGame() + "/Videos/Gameplays/", FT_VIDEO, "Video/Gameplay/");
	}
	void findModelsResources(std::vector<sInfoResource>& infoModels)
	{
		_findGenericResource(infoModels, getPathGame() + "/Models/Characters/", FT_ZIP, "Model/Character/");
	}
	void findIniReviews(sInfoResource& infoIni)
	{
		_findGenericResource(infoIni, getPathGame() + "/Reviews/", FT_INI);
	}
	void findIniCollector(sInfoResource& infoIni)
	{
		_findGenericResource(infoIni, getPathGame() + "/Models/Collectors Edition/", FT_INI);
	}

	void createResourceGroup()
	{

		// import all zip files [3d models]
		for (boost::filesystem::recursive_directory_iterator it(getPathGame()), end; it != end; ++it)
		{
			if (boost::filesystem::is_regular_file(it->path()) && _checkExtension(it->path().extension().generic_string(), FT_ZIP))
			{
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(it->path().generic_string(), "Zip", getGroupName());
			}
		}
		//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPathGame(), "FileSystem", getGroupName(), true);
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPathGame() + "/Models/Characters/", "FileSystem", getGroupName());
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPathGame() + "/Models/Collector Edition/", "FileSystem", getGroupName());
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPathGame() + "/Models/Objects/", "FileSystem", getGroupName());
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPathGame() + "/Videos/Gameplays/", "FileSystem", getGroupName());
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPathGame() + "/Videos/Trailers/", "FileSystem", getGroupName());
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPathGame() + "/Sounds/Soundtrack/", "FileSystem", getGroupName());
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPathGame() + "/Sounds/Voices/", "FileSystem", getGroupName());
		Ogre::ResourceGroupManager::getSingletonPtr()->initialiseResourceGroup(getGroupName());
	}
	void destroyResourceGroup()
	{	
		if (Ogre::ResourceGroupManager::getSingleton().resourceGroupExists(getGroupName()))
			Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(getGroupName());	
	}

	Ogre::String getValueOption(const Ogre::String& section, const Ogre::String& key, const Ogre::String& defaultvalue)
	{
		if (mIniError == SI_OK)
			return mIniGame.GetValue(section.c_str(), key.c_str(), defaultvalue.c_str());
		return defaultvalue;
	}

	Ogre::String getPathGame() const { return mPathGame.generic_string(); }
	Ogre::String getId() const { return mId; }
	Ogre::String getName() const { return mName; }
	Ogre::String getGroupName() const { return "Group/" + mId; }
	SI_Error getIniError() const { return mIniError; }

	// Image extras
	static bool loadImageFromDisk(const Ogre::String& pathImg, const Ogre::String& nameImg, const Ogre::String& nameGroup, int numMipMaps = 0);
	static bool createThumbnail(const boost::filesystem::path& pathImg, const boost::filesystem::path& pathThumb, unsigned int sizeThumb);

private:
	Ogre::String _generateHash(const Ogre::String text);
	bool _checkExtension(const Ogre::String& fileExtension, eFileType fileType);
	void _findGenericResource(sInfoResource& infoImage, const Ogre::String& pathFind, eFileType typeFile,
		const Ogre::String& nameRes = Ogre::StringUtil::BLANK, const Ogre::String& textureRes = Ogre::StringUtil::BLANK);
	void _findGenericResource(std::vector<sInfoResource>& infoRes, const Ogre::String& pathFind, eFileType typeFile,
		const Ogre::String& nameRes = Ogre::StringUtil::BLANK, const Ogre::String& textureRes = Ogre::StringUtil::BLANK);

	boost::filesystem::path mPathGame; // full path of the game
	Ogre::String mId; // unique identifier for the game
	Ogre::String mName; // name game
	CSimpleIniA mIniGame; // .ini file
	SI_Error mIniError; // .ini file error
};

#endif // #ifndef _GAMEINFO_H__

