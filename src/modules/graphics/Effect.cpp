/**
 * Copyright (c) 2006-2020 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

// LOVE
#include "Effect.h"
#include "opengl/EffectManager.h"

namespace love
{
namespace graphics
{

love::Type Effect::type("Effect", &Object::type);

Effect::Effect(EffectManager *manager, std::string &filename)
{
	EFK_CHAR filename16[256];
	::Effekseer::ConvertUtf8ToUtf16(filename16, 256, filename.c_str());
	effect = Effekseer::Effect::Create(manager->getManager(), filename16);
}

::Effekseer::Effect *Effect::getEffect()
{
	return this->effect;
}

} // graphics
} // love
