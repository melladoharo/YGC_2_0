#include "stdafx.h"
#include "GameInfo.h"

GameInfo::GameInfo(const Ogre::String& pathGame) :
mPathGame(pathGame),
mId(_generateHash(mPathGame.leaf().generic_string())),
mName(boost::filesystem::path(pathGame).leaf().generic_string()),
mIniError(SI_Error::SI_FAIL)
{
	// load .ini config
	sInfoResource infoIni;
	_findGenericResource(infoIni, getPathGame(), FT_INI);
	mIniError = mIniGame.LoadFile(infoIni.path.c_str());
}

GameInfo::~GameInfo()
{
	destroyResourceGroup();
}



bool GameInfo::loadImageFromDisk(const Ogre::String& pathImg, const Ogre::String& nameImg, const Ogre::String& nameGroup, int numMipMaps)
{
	if (boost::filesystem::exists(pathImg))
	{
		std::ifstream ifs(pathImg.c_str(), std::ios::binary | std::ios::in);

		if (ifs.is_open())
		{
			Ogre::String tex_ext;
			Ogre::String::size_type index_of_extension = pathImg.find_last_of('.');
			if (index_of_extension != Ogre::String::npos)
			{
				tex_ext = pathImg.substr(index_of_extension + 1);
				Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(pathImg, &ifs, false));
				Ogre::Image img;
				img.load(data_stream, tex_ext);
				Ogre::TextureManager::getSingleton().loadImage(nameImg, nameGroup, img, Ogre::TEX_TYPE_2D, numMipMaps);
			}
			ifs.close();
			return true;
		}
	}
	return false;
}

bool GameInfo::createThumbnail(const boost::filesystem::path& pathImg, const boost::filesystem::path& pathThumb, unsigned int sizeThumb)
{
	// load the original image
	if (FIBITMAP *bitmap = FreeImage_Load(FreeImage_GetFileType(pathImg.generic_string().c_str()), pathImg.generic_string().c_str()))
	{
		// create the new thumb of the image
		if (FIBITMAP* newThumb = FreeImage_MakeThumbnail(bitmap, sizeThumb))
		{
			//FreeImage_Save(FreeImage_GetFileType(pathImg.generic_string().c_str()), newThumb, pathThumb.generic_string().c_str());
			FreeImage_Save(FIF_PNG, newThumb, pathThumb.generic_string().c_str());
			FreeImage_Unload(newThumb);
			FreeImage_Unload(bitmap);
			return true;
		}
		FreeImage_Unload(bitmap);
	}
	return false;
}

Ogre::String GameInfo::_generateHash(const Ogre::String text)
{
	boost::hash<std::string> string_hash;
	return Ogre::StringConverter::toString(string_hash(text));
}

void GameInfo::_findGenericResource(sInfoResource& infoRes, const Ogre::String& pathFind, eFileType typeFile,
	const Ogre::String& nameRes /*= Ogre::StringUtil::BLANK*/, const Ogre::String& textureRes /*= Ogre::StringUtil::BLANK*/)
{
	// default info for resource
	infoRes.path = Ogre::StringUtil::BLANK;
	infoRes.name = textureRes;
	infoRes.pathThumb = Ogre::StringUtil::BLANK;
	infoRes.nameThumb = textureRes;
	infoRes.filename = Ogre::StringUtil::BLANK;
	infoRes.caption = Ogre::StringUtil::BLANK;

	// find resource in path
	if (boost::filesystem::is_directory(pathFind))
	{
		for (boost::filesystem::directory_iterator it(pathFind), end; it != end; ++it)
		{
			if (boost::filesystem::is_regular_file(it->path()) &&
				_checkExtension(it->path().extension().generic_string(), typeFile))
			{
				Ogre::String hashInfo = _generateHash(it->path().generic_string());

				infoRes.path = it->path().generic_string();
				infoRes.name = nameRes + hashInfo;
				infoRes.pathThumb = getPathGame() + "/Images/Thumbs/" + hashInfo + ".png";
				infoRes.nameThumb = nameRes + "Thumb/" + hashInfo;
				infoRes.filename = it->path().filename().generic_string();
				infoRes.caption = it->path().stem().generic_string();
				break; // only one resource (only one cover, logo, ...)
			}
		}
	}
}

void GameInfo::_findGenericResource(std::vector<sInfoResource>& infoRes, const Ogre::String& pathFind, eFileType typeFile, 
	const Ogre::String& nameRes /*= Ogre::StringUtil::BLANK*/, const Ogre::String& textureRes /*= Ogre::StringUtil::BLANK*/)
{
	//infoRes.clear(); // if not clear, add resources

	// find resource
	if (boost::filesystem::is_directory(pathFind))
	{
		for (boost::filesystem::directory_iterator it(pathFind), end; it != end; ++it)
		{
			if (boost::filesystem::is_regular_file(it->path()) &&
				_checkExtension(it->path().extension().generic_string(), typeFile))
			{
				Ogre::String hashInfo = _generateHash(it->path().generic_string());
				Ogre::String resIndex = Ogre::StringConverter::toString(infoRes.size());

				infoRes.push_back(sInfoResource());
				infoRes.back().path = it->path().generic_string();
				infoRes.back().name = nameRes + resIndex + hashInfo;
				infoRes.back().pathThumb = getPathGame() + "/Images/Thumbs/" + hashInfo + ".png";
				infoRes.back().nameThumb = nameRes + "Thumb/" + resIndex + hashInfo;
				infoRes.back().filename = it->path().filename().generic_string();
				infoRes.back().caption = it->path().stem().generic_string();
			}
		}
	}

	// no resource found, create default one (necesary for resources must have a texture, like disc or wallpapers)
	if (infoRes.empty() && textureRes != Ogre::StringUtil::BLANK)
	{
		infoRes.push_back(sInfoResource());
		infoRes.back().path = Ogre::StringUtil::BLANK;
		infoRes.back().name = textureRes;
		infoRes.back().pathThumb = Ogre::StringUtil::BLANK;
		infoRes.back().nameThumb = textureRes;
		infoRes.back().filename = Ogre::StringUtil::BLANK;
		infoRes.back().caption = Ogre::StringUtil::BLANK;
	}
}

bool GameInfo::_checkExtension(const Ogre::String& fileExtension, eFileType fileType)
{
	static const std::vector<Ogre::String> imageExtension{ 
		".jpg", ".png", ".bmp", ".jpeg", ".dds", ".tga", ".targa", ".tif", ".tiff", ".ico", ".iff" };
	static const std::vector<Ogre::String> videoExtension{ 
		".avi", ".divx", ".flv", ".mpg", ".mka", ".mp4", ".mkv", ".mpeg", ".mov", ".vob", ".wmv" };
	static const std::vector<Ogre::String> soundExtension{ ".flac", ".m4a", ".mp3", ".mid", ".wav", ".wma" };
	static const std::vector<Ogre::String> iniExtension{ ".ini", ".Ini", ".INI" };
	static const std::vector<Ogre::String> zipExtension{ ".zip", ".Zip", ".ZIP" };

	switch (fileType)
	{
	case FT_IMAGE:
		if (std::find(std::begin(imageExtension), std::end(imageExtension), fileExtension) != std::end(imageExtension))
			return true;
		break;
	case FT_VIDEO:
		if (std::find(std::begin(videoExtension), std::end(videoExtension), fileExtension) != std::end(videoExtension))
			return true;
		break;
	case FT_SOUND:
		if (std::find(std::begin(soundExtension), std::end(soundExtension), fileExtension) != std::end(soundExtension))
			return true;
		break;
	case FT_INI:
		if (std::find(std::begin(iniExtension), std::end(iniExtension), fileExtension) != std::end(iniExtension))
			return true;
		break;
	case FT_ZIP:
		if (std::find(std::begin(zipExtension), std::end(zipExtension), fileExtension) != std::end(zipExtension))
			return true;
		break;
	default:
		return false;
		break;
	}
	
	return false;
}

