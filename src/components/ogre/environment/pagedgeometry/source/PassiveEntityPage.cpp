/*-------------------------------------------------------------------------------------
Copyright (c) 2008 Erik Hjortsberg

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

//PassiveEntityPage.cpp
//PassiveEntityPage is an extension to PagedGeometry which displays entities as entities (no optimization techniques).
//-------------------------------------------------------------------------------------

#define __PassiveEntityPage_CPP__
#include "PassiveEntityPage.h"

#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreVector3.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreString.h>
#include <OgreStringConverter.h>
using namespace Ogre;

namespace PagedGeometry {

//-------------------------------------------------------------------------------------

unsigned long PassiveEntityPage::GUID = 0;


void PassiveEntityPage::init(::PagedGeometry::PagedGeometry  *geom)
{
	sceneMgr = geom->getSceneManager();
}

PassiveEntityPage::PassiveEntityPage()
	: sceneMgr(NULL)
{}

PassiveEntityPage::~PassiveEntityPage()
{
}


void PassiveEntityPage::addEntity(Ogre::Entity *ent, const Ogre::Vector3 &position, const Ogre::Quaternion &rotation, const Ogre::Vector3 &scale, const Ogre::ColourValue &color)
{
	mEntities.push_back(std::pair<SceneNode*, Entity*>(ent->getParentSceneNode(), ent));
/*	ent->setVisible(true);
	ent->getParentSceneNode()->detachObject(ent);*/
}

void PassiveEntityPage::removeEntities()
{
// 	for (EntityStore::iterator I(mEntities.begin()); I != mEntities.end(); ++I) {
// 		if (!I->second->getParentSceneNode()) {
// 			I->first->attachObject(I->second);
// 		}
// 	}
	mEntities.clear();
}


void PassiveEntityPage::setVisible(bool visible)
{
	if (visible) {
		for (EntityStore::iterator I(mEntities.begin()); I != mEntities.end(); ++I) {
			I->second->setVisible(true);
/*			if (!I->second->getParentSceneNode()) {
				I->first->attachObject(I->second);
			}*/
		}
	} else {
		for (EntityStore::iterator I(mEntities.begin()); I != mEntities.end(); ++I) {
			I->second->setVisible(false);
/*			if (I->second->getParentSceneNode()) {
				I->first->detachObject(I->second);
			}*/
		}
	}
}

void PassiveEntityPage::setFade(bool enabled, Ogre::Real visibleDist, Ogre::Real invisibleDist)
{
}
}