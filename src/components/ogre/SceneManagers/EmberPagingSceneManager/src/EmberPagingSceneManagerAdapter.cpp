//
// C++ Implementation: EmberPagingSceneManagerAdapter
//
// Description:
//
//
// Author: Erik Hjortsberg <erik.hjortsberg@gmail.com>, (C) 2006
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.//
//

#include "OgrePagingLandScapePrecompiledHeaders.h"

#include "EmberPagingSceneManagerAdapter.h"

#include "EmberPagingSceneManager.h"
#include "EmberPagingTerrainObserver.h"
#include "OgrePagingLandScapeOptions.h"
#include "OgrePagingLandScapePageManager.h"
#include "OgrePagingLandScapePage.h"
#include "OgrePagingLandScapeData2DManager.h"
#include "OgreMemoryAllocatorConfig.h"
#include "OgrePagingLandScapeRenderableManager.h"
#include "OgrePagingLandScapeTileManager.h"
#include "OgrePagingLandScapeListenerManager.h"

namespace Ember {
namespace OgreView {

	Ogre::PagingLandScapeOptions* EmberPagingSceneManagerAdapter::getOptions()
	{
		return mSceneManager.getOptions();
	}

	EmberPagingSceneManagerAdapter::EmberPagingSceneManagerAdapter(EmberPagingSceneManager& scenemanager) : mSceneManager(scenemanager)
	{
	}

	EmberPagingSceneManagerAdapter::~EmberPagingSceneManagerAdapter()
	{
		if (mTerrainObservers.size()) {
			S_LOG_WARNING("All terrain observers weren't deregistered from the EmberPagingSceneManagerAdapter. This will lead to memory leaks.");
		}
	}


	int EmberPagingSceneManagerAdapter::getPageSize()
	{
		return getOptions()->PageSize;
	}

	Ogre::Real EmberPagingSceneManagerAdapter::getHeightAt(const Ogre::Real x, const Ogre::Real z)
	{
		return mSceneManager.getHeightAt(x, z);
	}


	void EmberPagingSceneManagerAdapter::setWorldPagesDimensions(int numberOfPagesWidth, int numberOfPagesHeight, int widthOffsetInPages, int heightOffsetInPages)
	{
		//we don't want to shift the terrain half a page, so we have to make sure that the we have an even amount of pages
		if ((numberOfPagesWidth % 2) != 0) {
			numberOfPagesWidth++;
		}
		if ((numberOfPagesHeight % 2) != 0) {
			numberOfPagesHeight++;
		}

		Ogre::PagingLandScapeOptions* options(getOptions());
		//in order position (0,0) to be aligned to the centre of the terrain we must offset the position of the terrain a bit
		options->position.x = ((numberOfPagesHeight * 0.5f) - heightOffsetInPages) * options->PageSize;
		options->position.z = ((numberOfPagesWidth * 0.5f) - widthOffsetInPages) * options->PageSize;

		options->world_height = numberOfPagesHeight;
		options->world_width = numberOfPagesWidth;

		//update the options
		options->NumPages = options->world_height * options->world_width;
		options->maxUnScaledZ = options->world_height * (options->PageSize - 1) * 0.5f;
		options->maxUnScaledX = options->world_width  * (options->PageSize - 1) * 0.5f;

		options->maxScaledZ = options->scale.z * options->maxUnScaledZ;
		options->maxScaledX = options->scale.x * options->maxUnScaledX;

	}

	void EmberPagingSceneManagerAdapter::resize(Ogre::AxisAlignedBox newSize, int levels)
	{
		mSceneManager.resize(newSize, levels);
	}

	void EmberPagingSceneManagerAdapter::setCamera(Ogre::Camera* camera)
	{
		mSceneManager.setOption("primaryCamera", camera);
	}

	void EmberPagingSceneManagerAdapter::setResourceGroupName(const std::string& groupName)
	{
		mSceneManager.setOption("GroupName", &groupName);
		mSceneManager.getOptions()->groupName = groupName;
		mSceneManager.getOptions()->cfgGroupName = groupName;
	}

	void EmberPagingSceneManagerAdapter::loadOptions(const std::string& filePath)
	{
	    S_LOG_INFO("Trying to load terrain options from file " << filePath << ".");
		struct stat theStat;
   		int ret = stat(filePath.c_str(), &theStat);
   		if (ret) {
			S_LOG_FAILURE("Could not find file "<< filePath);
			return;
   		}
		std::ifstream *filestream = OGRE_NEW_T(std::ifstream, Ogre::MEMCATEGORY_GENERAL)();
		//We need to open the file in binary mode for Ogre to handle line endings on both unix and windows systems.
		filestream->open(filePath.c_str(), std::ios::in | std::ios::binary);

		if (filestream->fail())
		{
			S_LOG_FAILURE("Could not open file "<< filePath);
			delete filestream;
			return;
		}

		//this will envelope the file stream pointer and delete it when it's destroyed itself
		Ogre::FileStreamDataStream* stream = OGRE_NEW Ogre::FileStreamDataStream(filePath, filestream, theStat.st_size, true);
		Ogre::DataStreamPtr dataPtr(stream);

        if (stream->size() == 0) {
			S_LOG_FAILURE("Zero size file found at "<< filePath);
			return;
        }

		mSceneManager.getOptions()->loadMapOptions(dataPtr);

		mSceneManager.getOptions()->setTextureFormat("EmberTexture");
	}

	void EmberPagingSceneManagerAdapter::loadScene()
	{
		mSceneManager.loadScene();
		mSceneManager.getOptions()->setOption("LoadNow", 0);
	}

	void EmberPagingSceneManagerAdapter::reset()
	{
		mSceneManager.resetScene();
	}


	void EmberPagingSceneManagerAdapter::setOption(const std::string& strKey, const void* pValue)
	{
		mSceneManager.setOption(strKey, pValue);
	}

	void EmberPagingSceneManagerAdapter::getOption(const std::string& strKey, void* pDestValue)
	{
		mSceneManager.getOption(strKey, pDestValue);
	}

	void EmberPagingSceneManagerAdapter::setUninitializedHeight(float height)
	{
		mSceneManager.getOptions()->uninitializedHeight = height;
	}


	Ogre::SceneManager& EmberPagingSceneManagerAdapter::getSceneManager() const
	{
		return mSceneManager;
	}

	void EmberPagingSceneManagerAdapter::reloadAllPages()
	{
		mSceneManager.getPageManager()->load();
	}

	void EmberPagingSceneManagerAdapter::reloadPage(unsigned int x, unsigned int z)
	{
// 		Ogre::Vector2 position(x,z);
// 		setOption("PageUpdate", &position);
		Ogre::PagingLandScapePage* page=  mSceneManager.getPageManager()->getPage(x, z, false);
		if (page) {
//			page->reload();
//			mSceneManager.getData2DManager()->load();
 			//mSceneManager.getData2DManager()->reload(x, z);
			page->updateTerrain();
/*			page->getSceneNode()->_update(true, true);*/
/*			page->unload();
			page->load();
			page->_updateLod();*/
		} else {
			S_LOG_WARNING("Trying to reload page which doesn't exist at position x: " << x << " y: " << z << ".");
		}
// 		mSceneManager.getRootSceneNode()->_update(true, true);
// 		mSceneManager.PagingLandScapeOctreeResize();
	}

	void EmberPagingSceneManagerAdapter::loadFirstPage()
	{
		if (mSceneManager.getOptions()->primaryCamera) {
			mSceneManager.getPageManager()->LoadFirstPage(mSceneManager.getOptions()->primaryCamera);
		}
	}

	std::string EmberPagingSceneManagerAdapter::getDebugInfo()
	{
		std::stringstream ss;

		Ogre::PagingLandScapeRenderableManager* renderableManager(mSceneManager.getRenderableManager());
		ss << "Renderables: " << renderableManager->numRenderables() << "\n Free: " << renderableManager->numFree() << "\n Loading: " << renderableManager->numLoading() << "\n Visible: " << renderableManager->numVisibles() << "\nTiles: " << mSceneManager.getTileManager()->numTiles() << "\n Free: " << mSceneManager.getTileManager()->numFree() << "\nPages loaded: " << mSceneManager.getPageManager()->getLoadedPageSize() << "\n Unloaded: " << mSceneManager.getPageManager()->getUnloadedPagesSize();
		return ss.str();
	}


	Terrain::ITerrainObserver* EmberPagingSceneManagerAdapter::createObserver()
	{
		EmberPagingTerrainObserver* observer = new EmberPagingTerrainObserver();
		Ogre::PagingLandscapeDelegate* delegate = new Ogre::PagingLandscapeDelegate(observer, &EmberPagingTerrainObserver::tileShow);
		mSceneManager.getListenerManager()->addShowTileListener(delegate);
		mTerrainObservers.insert(TerrainObserverStore::value_type(observer, delegate));
		return observer;
	}

	void EmberPagingSceneManagerAdapter::destroyObserver(Terrain::ITerrainObserver* observer)
	{
		EmberPagingTerrainObserver* observerImpl = static_cast<EmberPagingTerrainObserver*>(observer);
		TerrainObserverStore::iterator I = mTerrainObservers.find(observerImpl);
		if (I != mTerrainObservers.end()) {
			mTerrainObservers.erase(I);
			delete observerImpl;
			Ogre::PagingLandscapeDelegate* delegate = I->second;
			mSceneManager.getListenerManager()->removeShowTileListener(delegate);
			delete delegate;
		}

	}

}
}
