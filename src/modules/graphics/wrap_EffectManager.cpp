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
#include "EffectManager.h"
#include "EffectHandle.h"
#include "wrap_Effect.h"
#include "wrap_EffectHandle.h"

namespace love
{
namespace graphics
{

EffectManager *luax_checkeffectmanager(lua_State *L, int idx)
{
	return luax_checktype<EffectManager>(L, idx);
}

int w_EffectManager_play(lua_State *L)
{
	EffectManager *manager = luax_checkeffectmanager(L, 1);
	Effect *effect = luax_checkeffect(L, 2);
	EffectHandle *handle = nullptr;
	luax_catchexcept(L, [&](){ handle = manager->play(effect); });
	luax_pushtype(L, handle);
	handle->release();
	return 1;
}

int w_EffectManager_stop(lua_State *L)
{
	EffectManager *manager = luax_checkeffectmanager(L, 1);
	EffectHandle *handle = luax_checkeffecthandle(L, 2);
	luax_catchexcept(L, [&](){ manager->stop(handle); });
	return 0;
}

int w_EffectManager_stopAll(lua_State *L)
{
	EffectManager *manager = luax_checkeffectmanager(L, 1);
	luax_catchexcept(L, [&](){ manager->stopAll(); });
	return 0;
}

int w_EffectManager_update(lua_State *L)
{
	EffectManager *manager = luax_checkeffectmanager(L, 1);
	float dt = luax_checkfloat(L, 2);
	luax_catchexcept(L, [&](){ manager->update(dt); });
	return 0;
}

const luaL_Reg w_EffectManager_functions[] =
{
	{ "play", w_EffectManager_play },
	{ "stop", w_EffectManager_stop },
	{ "stopAll", w_EffectManager_stopAll },
	{ "update", w_EffectManager_update },
	{ 0, 0 }
};

extern "C" int luaopen_effectmanager(lua_State *L)
{
	return luax_register_type(L, &EffectManager::type, w_EffectManager_functions, nullptr);
}

} // graphics
} // love
