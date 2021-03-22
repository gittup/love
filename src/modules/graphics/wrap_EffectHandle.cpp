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
#include "wrap_EffectHandle.h"

namespace love
{
namespace graphics
{

EffectHandle *luax_checkeffecthandle(lua_State *L, int idx)
{
	return luax_checktype<EffectHandle>(L, idx);
}

int w_EffectHandle_exists(lua_State *L)
{
	EffectHandle *handle = luax_checkeffecthandle(L, 1);
	luax_pushboolean(L, handle->exists());
	return 1;
}

const luaL_Reg w_EffectHandle_functions[] =
{
	{ "exists", w_EffectHandle_exists },
	{ 0, 0 }
};

extern "C" int luaopen_effecthandle(lua_State *L)
{
	return luax_register_type(L, &EffectHandle::type, w_EffectHandle_functions, nullptr);
}

} // graphics
} // love
