//
// C++ Implementation: ShrubberyFoliage
//
// Description: 
//
//
// Author: Erik Hjortsberg <erik.hjortsberg@iteam.se>, (C) 2008
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ShrubberyFoliage.h"

#include "services/EmberServices.h"
#include "services/logging/LoggingService.h"
#include "services/config/ConfigService.h"

#include "FoliageLayer.h"
#include "FoliageArea.h"
#include "FoliageLoader.h"

#include "components/ogre/EmberOgre.h"
#include "components/ogre/terrain/TerrainGenerator.h"


#include "../AvatarCamera.h"
#include "../EmberOgre.h"
#include "../terrain/TerrainGenerator.h"
#include "../terrain/TerrainLayerDefinition.h"
#include "../terrain/TerrainArea.h"
#include "../terrain/TerrainShader.h"

#include "../terrain/ISceneManagerAdapter.h"

#include "pagedgeometry/include/PagedGeometry.h"
#include "pagedgeometry/include/BatchPage.h"
#include "pagedgeometry/include/TreeLoader3D.h"

namespace EmberOgre {

namespace Environment {

ShrubberyFoliage::ShrubberyFoliage(const Terrain::TerrainLayerDefinition& terrainLayerDefinition, const Terrain::TerrainFoliageDefinition& foliageDefinition)
: FoliageBase(terrainLayerDefinition, foliageDefinition)
, mLoader(0)
{
}

ShrubberyFoliage::~ShrubberyFoliage()
{
	delete mLoader;
}

void ShrubberyFoliage::initialize()
{
	Ogre::Camera* camera = EmberOgre::getSingleton().getMainCamera()->getCamera();
	mPagedGeometry = new ::PagedGeometry::PagedGeometry(camera, 32);
	const WFMath::AxisBox<2>& worldSize = EmberOgre::getSingleton().getTerrainGenerator()->getTerrainInfo().getWorldSizeInIndices();	mPagedGeometry->setBounds(Atlas2Ogre(worldSize));
	
	mPagedGeometry->addDetailLevel<PagedGeometry::BatchPage>(64, 32);
	
	mLoader = new FoliageLoader(mTerrainLayerDefinition, mFoliageDefinition);
 	mPagedGeometry->setPageLoader(mLoader);
}

void ShrubberyFoliage::frameStarted(const Ogre::FrameEvent & evt)
{	
	
	if (mPagedGeometry) {
		try {
			mPagedGeometry->update();
		} catch (const Ogre::Exception& ex)
		{
			S_LOG_FAILURE("Error when updating grass. Will disable grass.\n"<< ex.what());
			delete mPagedGeometry;
			delete mLoader;
			mPagedGeometry = 0;
			mLoader = 0;
		}
	}	
}



}

}