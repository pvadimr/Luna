/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*******************************************************************************/
#include "../../../../../config.h"
#ifdef POLYVOX
#ifndef __PolyVox_MarchingCubeTables_H__
#define __PolyVox_MarchingCubeTables_H__

#include "PolyVoxCore/Impl/TypeDef.h"

namespace PolyVox
{
	extern const POLYVOX_API int edgeTable[256];
	extern const POLYVOX_API int triTable[256][16];
}

#endif
#endif
