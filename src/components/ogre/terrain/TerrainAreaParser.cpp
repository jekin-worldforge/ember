//
// C++ Implementation: TerrainAreaParser
//
// Description:
//
//
// Author: Erik Hjortsberg <erik.hjortsberg@gmail.com>, (C) 2009
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

#include "TerrainAreaParser.h"
#include "TerrainArea.h"
#include "framework/LoggingInstance.h"
#include <Mercator/Area.h>
#include <wfmath/atlasconv.h>

namespace Ember
{
namespace OgreView
{

namespace Terrain
{

bool TerrainAreaParser::parseArea(const Atlas::Message::MapType& areaData, WFMath::Polygon<2>& poly, int& layer)
{
	try {
		WFMath::Polygon<2> newPoly(areaData);
		poly = newPoly;
	} catch (const WFMath::_AtlasBadParse& ex) {
		S_LOG_WARNING("Error when parsing polygon data from atlas." << ex);
		return false;
	}

	Atlas::Message::MapType::const_iterator it = areaData.find("layer");
	if ((it != areaData.end()) && it->second.isInt()) {
		layer = it->second.asInt();
	} else {
		layer = 0;
	}

	return true;
}

const Atlas::Message::Element TerrainAreaParser::createElement(const WFMath::Polygon<2>& poly, int layer)
{
	Atlas::Message::Element map(poly.toAtlas());
	if (layer != 0) {
		if (map.isMap()) {
			map.asMap()["layer"] = layer;
		} else {
			S_LOG_WARNING("A polygon should be serialized into a map.");
		}
	}
	return map;

}

}

}
}
