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

#include "common/runtime.h"
#include "opengl/Graphics.h"
#include "Effect.h"
#include "EffectHandle.h"
#include "opengl/EffectManager.h"

namespace love
{
namespace graphics
{

Effect *luax_checkeffect(lua_State *L, int idx)
{
	return luax_checktype<Effect>(L, idx);
}

int w_Effect_play(lua_State *L)
{
	Graphics *gfx = Module::getInstance<Graphics>(Module::M_GRAPHICS);
	Effect *effect = luax_checkeffect(L, 1);
	float x = luax_checkfloat(L, 2);
	float y = luax_checkfloat(L, 3);
	float z = luax_checkfloat(L, 4);
	EffectHandle *handle = gfx->getEffectManager()->play(effect, x, y, z);
	luax_pushtype(L, handle);
	handle->release();
	return 1;
}

const luaL_Reg w_Effect_functions[] =
{
	{ "play", w_Effect_play},
	{ 0, 0}
};

extern "C" int luaopen_effect(lua_State *L)
{
	return luax_register_type(L, &Effect::type, w_Effect_functions, nullptr);
}

} // graphics
} // love
