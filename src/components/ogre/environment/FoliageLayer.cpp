//
// C++ Implementation: FoliageLayer
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

#include "FoliageLayer.h"
#include "pagedgeometry/include/PagedGeometry.h"
#include "pagedgeometry/include/PropertyMaps.h"
#include "../MathConverter.h"
#include "../EmberOgre.h"
#include "../terrain/TerrainGenerator.h"
#include "../terrain/TerrainPageFoliage.h"
#include "../terrain/TerrainPage.h"
#include "../terrain/TerrainPageShadow.h"
#include "services/EmberServices.h"
#include "services/logging/LoggingService.h"
#include <wfmath/intersect.h>

using namespace PagedGeometry;
using namespace Ogre;
using namespace EmberOgre::Terrain;
namespace EmberOgre {

namespace Environment {

FoliageLayer::FoliageLayer(::PagedGeometry::PagedGeometry *geom, GrassLoader<FoliageLayer> *ldr)
{
	FoliageLayer::geom = geom;
	FoliageLayer::parent = ldr;

	minWidth = 1.0f; maxWidth = 1.0f;
	minHeight = 1.0f; maxHeight = 1.0f;
// 	minY = 0; maxY = 0;
	renderTechnique = GRASSTECH_QUAD;
	fadeTechnique = FADETECH_ALPHA;
	animMag = 1.0f;
	animSpeed = 1.0f;
	animFreq = 1.0f;
	waveCount = 0.0f;
	animate = false;
	blend = false;
	shaderNeedsUpdate = true;
}


FoliageLayer::~FoliageLayer()
{
}

unsigned int FoliageLayer::calculateMaxGrassCount(float densityFactor, float volume)
{
	return static_cast<unsigned int>(densityFactor * volume);
}

unsigned int FoliageLayer::_populateGrassList(PageInfo page, float *posBuff, unsigned int grassCount)
{
	unsigned int finalGrassCount = 0;
	TerrainPosition wfPos(Ogre2Atlas_TerrainPosition(page.centerPoint));
	TerrainPage* terrainPage = EmberOgre::getSingleton().getTerrainGenerator()->getTerrainPage(wfPos);
	if (terrainPage) {
		Ogre::TRect<float> ogrePageExtent = Atlas2Ogre(terrainPage->getExtent());
		Ogre::TRect<float> adjustedBounds = Ogre::TRect<float>(page.bounds.left - ogrePageExtent.left, page.bounds.top - ogrePageExtent.top, page.bounds.right - ogrePageExtent.left, page.bounds.bottom - ogrePageExtent.top);
		TerrainPageFoliage::PlantStore plants;
		terrainPage->getPageFoliage()->getPlantsForArea(adjustedBounds, plants);
// 		WFMath::AxisBox<2> wfBounds = Ogre2Atlas(page.bounds);
		for (TerrainPageFoliage::PlantStore::const_iterator I = plants.begin(); I != plants.end(); ++I) {
// 			TerrainPosition posInWorld;
/*			posInWorld.x() = (*I).x() + (terrainPage->getExtent().lowCorner().x());
			posInWorld.y() = (*I).y() + (terrainPage->getExtent().lowCorner().y());
			if (WFMath::Contains<2>(wfBounds, posInWorld, true)) {
				Ogre::Vector2 ogrePos = Atlas2Ogre_Vector2(posInWorld);*/
				*posBuff++ = I->x + ogrePageExtent.left;
				*posBuff++ = I->y + ogrePageExtent.top;
				finalGrassCount++;
// 			}
		}
	}
	return finalGrassCount;
}

Ogre::uint32 FoliageLayer::getColorAt(float x, float z)
{
	//TODO: add caching of the last fetched terrain page and first check if the position isn't at that page, since we'll in most cass will call this method with positions that are close to eachother
	TerrainPosition wfPos(x, -z);
	TerrainPage* terrainPage = EmberOgre::getSingleton().getTerrainGenerator()->getTerrainPage(wfPos);
	TerrainPageShadow& terrainShadow = terrainPage->getPageShadow();
	Ogre::Image* image = terrainShadow.getImage();
	if (image) {
		Ogre::TRect<float> ogrePageExtent = Atlas2Ogre(terrainPage->getExtent());
		unsigned int adjustedX = static_cast<unsigned int>(x - ogrePageExtent.left);
		unsigned int adjustedZ = static_cast<unsigned int>(z - ogrePageExtent.top);
		unsigned char val(image->getData()[static_cast<size_t>((image->getWidth() * adjustedZ) + adjustedX)]);
		
		///construct a 32 bit value from an array of four 8 chars
		Ogre::uint8 aVal[4] = {val, val, val, 0xFF}; ///use full alpha
		return *((Ogre::uint32*)aVal);
	}
	return 0;

}


}

}