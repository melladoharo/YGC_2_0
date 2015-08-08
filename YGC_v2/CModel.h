#ifndef _CMODEL_H__
#define _CMODEL_H__

class CModel
{
public:
	CModel(const Ogre::String& nameModel, Ogre::SceneNode* parent = NULL);
	~CModel();

	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void setVisible(bool visible) { mVisible = visible; if (mNode) mNode->setVisible(mVisible); }
	void hide() { mVisible = false; if (mNode) mNode->setVisible(false); }
	void show() { mVisible = true; if (mNode) mNode->setVisible(true); }
	bool isVisible() const { return mVisible; }
	Ogre::SceneNode* getNode() const { return mNode; }
	Ogre::Entity* getEntity() const { return mEnt; }
	Ogre::Vector3 getPosition() const { return (mNode) ? mNode->getPosition() : Ogre::Vector3::ZERO; }
	void setPosition(const Ogre::Vector3& newPos) { if (mNode) mNode->setPosition(newPos); }
	Ogre::Quaternion getOrientation() const { return (mNode) ? mNode->getOrientation() : Ogre::Quaternion::IDENTITY; }

	void applyNewText(const Ogre::String& newText, unsigned int subEnt=0);
	void restoreMaterial(unsigned int subEnt);

private:
	Ogre::SceneManager* mSceneMgr;		// Default scene manager 
	Ogre::Entity* mEnt;
	Ogre::SceneNode* mNode;
	Ogre::SceneNode* mParentNode;
	Ogre::MaterialPtr mMat;
	std::vector<Ogre::MaterialPtr> mMatOriginal;
	bool mVisible;
};

#endif // _CMODEL_H__

