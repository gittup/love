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
#include "common/config.h"
#include "EffectHandle.h"
#include "Graphics.h"

#include "Effekseer.h"
#include "EffekseerRendererGL.h"

namespace love
{
namespace graphics
{

love::Type EffectHandle::type("EffectHandle", &Drawable::type);

EffectHandle::EffectHandle(::Effekseer::Handle handle, EffectManager *manager)
{
	this->handle = handle;
	this->manager = manager;
}

::Effekseer::Handle EffectHandle::getHandle()
{
	return this->handle;
}

bool EffectHandle::exists()
{
	return manager->getManager()->Exists(handle);
}

void EffectHandle::draw(Graphics *gfx, const Matrix4 &m)
{
	GLint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
	gfx->flushStreamDraws();

	manager->setProjection(gfx);

	Matrix4 t(gfx->getTransform(), m);
	int i, j;
	const float *e = t.getElements();
	::Effekseer::Matrix44 matrix;

	// Convert Love matrix to Effekseer matrix
	for(j=0; j<4; j++) {
		for(i=0; i<4; i++) {
			matrix.Values[i][j] = e[i*4 + j];
		}
	}

	::EffekseerRendererGL::Renderer *renderer = manager->getRenderer();
	renderer->SetCameraMatrix(matrix);

	renderer->BeginRendering();
	manager->getManager()->DrawHandle(handle);
	renderer->EndRendering();
	glUseProgram(prog); // TODO Effekseer changes the glUseProgram to something else, so we need to restore it, otherwise everything done in love.draw() doesn't show up.
}
// TODO: Provide lua access to functions like StopEffect()

} // graphics
} // love
