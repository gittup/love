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
#include "common/Module.h"
#include "EffectManager.h"
#include "Effect.h"
#include "EffectHandle.h"
#include "Graphics.h"
#include "filesystem/Filesystem.h"
#include "filesystem/FileData.h"
#include "image/Image.h"

#include "Effekseer.h"
#include "EffekseerRendererGL.h"

static ::EffekseerRendererGL::Renderer *renderer;
static ::Effekseer::Manager *manager;

class LoveTextureLoader : public ::Effekseer::TextureLoader
{
public:
	LoveTextureLoader()
	{
	}
	virtual ~LoveTextureLoader() = default;

	Effekseer::TextureData* Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType) override
	{
		using namespace love;
		using namespace love::image;
		using namespace love::filesystem;
		auto fs = Module::getInstance<Filesystem>(Module::M_FILESYSTEM);
		auto imagemodule = Module::getInstance<Image>(Module::M_IMAGE);
		auto gfx = Module::getInstance<love::graphics::Graphics>(Module::M_GRAPHICS);
		int8_t path8[256];
		::Effekseer::ConvertUtf16ToUtf8(path8, 256, (const int16_t*)path);
		FileData *fd = fs->read((char*)path8);
		ImageData *id = imagemodule->newImageData(fd);
		love::graphics::Image::Slices slices(love::graphics::TEXTURE_2D);
		love::graphics::Image::Settings settings;

		slices.set(0, 0, id);
		love::graphics::Image *image = gfx->newImage(slices, settings);

		auto textureData = new Effekseer::TextureData();
		textureData->UserPtr = nullptr;
		textureData->UserID = image->getHandle();
		textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
		textureData->Width = image->getWidth();
		textureData->Height = image->getHeight();
		textureData->HasMipmap = false;

		return textureData;
	}

	void Unload(Effekseer::TextureData* data) override
	{
		if (data != nullptr && data->UserPtr != nullptr)
		{
			GLuint texture = static_cast<GLuint>(data->UserID);
			glDeleteTextures(1, &texture);
		}

		if (data != nullptr)
		{
			delete data;
		}
	}
};

class LoveEffectLoader : public ::Effekseer::EffectLoader
{
public:
	LoveEffectLoader()
	{
	}

	~LoveEffectLoader()
	{
	}

	bool Load(const EFK_CHAR* path, void*& data, int32_t& size)
	{
		data = NULL;
		size = 0;

		using namespace love;
		using namespace love::filesystem;
		auto fs = Module::getInstance<Filesystem>(Module::M_FILESYSTEM);
		int8_t path8[256];
		::Effekseer::ConvertUtf16ToUtf8(path8, 256, (const int16_t*)path);
		FileData *fd = fs->read((char*)path8);

		size = (int32_t)fd->getSize();
		data = new uint8_t[size];
		memcpy(data, fd->getData(), size);

		return true;
	}

	void Unload(void* data, int32_t size)
	{
		uint8_t* data8 = (uint8_t*)data;
		ES_SAFE_DELETE_ARRAY(data8);
	}
};

namespace love
{
namespace graphics
{

// During init, enable debug output

love::Type EffectManager::type("EffectManager", &Drawable::type);

EffectManager::EffectManager()
{
#ifdef LOVE_EMSCRIPTEN
	renderer = ::EffekseerRendererGL::Renderer::Create(8000, EffekseerRendererGL::OpenGLDeviceType::Emscripten);
#else
	renderer = ::EffekseerRendererGL::Renderer::Create(8000, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
#endif

	if(!renderer) {
		throw love::Exception("Unable to create Effekseer Renderer.\n");
	}

	manager = ::Effekseer::Manager::Create(8000);

	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	manager->SetEffectLoader(new LoveEffectLoader());
	manager->SetTextureLoader(new LoveTextureLoader());
	manager->SetModelLoader(renderer->CreateModelLoader());
	manager->SetMaterialLoader(renderer->CreateMaterialLoader());

	updateCounter = 0.0f;
}

EffectHandle *EffectManager::play(Effect *effect)
{
	EffectHandle *handle = new EffectHandle(manager->Play(effect->getEffect(), ::Effekseer::Vector3D(0, 0, 0)), this);
	// Scale x/y to make effects large enough to be seen, inverting y axis
	manager->SetScale(handle->getHandle(), 10.0, -10.0, 1.0);
	return handle;
}

void EffectManager::stop(EffectHandle *handle)
{
	manager->StopEffect(handle->getHandle());
}

void EffectManager::stopAll()
{
	manager->StopAllEffects();
}

::Effekseer::Manager *EffectManager::getManager()
{
	return this->manager;
}

::EffekseerRendererGL::Renderer *EffectManager::getRenderer()
{
	return this->renderer;
}

void EffectManager::update(float dt)
{
	/* Effekseer's manager update is in frames, which is 60fps. Only call
	 * Update once our internal counter is larger than one frame.
	 */
	updateCounter += dt * 60.0;
	if(updateCounter >= 1.0f) {
		manager->Update(updateCounter);
		updateCounter = 0.0f;
	}
}

void EffectManager::setProjection(Graphics *gfx)
{
	float windowWidth = (float)gfx->getWidth();
	float windowHeight = (float)gfx->getHeight();
	::Effekseer::Matrix44 proj = ::Effekseer::Matrix44().OrthographicRH((float)windowWidth, (float)windowHeight, -128.0, 128.0);

	// Invert y axis
	proj.Values[1][1] = -proj.Values[1][1];

	// And move 0, 0 to top-left
	proj.Values[3][0] = -1;
	proj.Values[3][1] = 1;

	renderer->SetProjectionMatrix(proj);
}

void EffectManager::draw(Graphics *gfx, const Matrix4 &m)
{
	GLint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
	gfx->flushStreamDraws();

	setProjection(gfx);

	// TODO: Inverse of this?
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

	renderer->SetCameraMatrix(matrix);

	renderer->BeginRendering();
	manager->Draw();
	renderer->EndRendering();
	glUseProgram(prog); // TODO Effekseer changes the glUseProgram to something else, so we need to restore it, otherwise everything done in love.draw() doesn't show up.
}

} // graphics
} // love
