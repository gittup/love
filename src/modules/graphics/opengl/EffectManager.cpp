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
#include "common/Module.h"
#include "EffectManager.h"
#include "graphics/Effect.h"
#include "graphics/EffectHandle.h"
#include "Graphics.h"
#include "filesystem/Filesystem.h"
#include "filesystem/FileData.h"
#include "image/Image.h"

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

love::Type EffectManager::type("EffectManager", &Object::type);

EffectManager::EffectManager()
{
	printf("Start Effekseer\n");

	// TODO: Pull this from Love and adjust when the window is adjusted?
	// Or grab the projection matrix directly from love?
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;
	renderer = ::EffekseerRendererGL::Renderer::Create(8000, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

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

	auto g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH(
		90.0f / 180.0f * 3.14f, (float)windowWidth / (float)windowHeight, 1.0f, 500.0f));

	renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));
}

EffectHandle *EffectManager::play(Effect *effect, float x, float y, float z)
{
	EffectHandle *handle = new EffectHandle(manager->Play(effect->getEffect(), x, y, z));
	return handle;
}

::Effekseer::Manager *EffectManager::getManager()
{
	return this->manager;
}

void EffectManager::update(float dt)
{
	/* Effekseer's manager update is in frames, which is 60fps */
	manager->Update(dt * 60.0);
}

void EffectManager::draw()
{
	renderer->BeginRendering();
	manager->Draw();
	renderer->EndRendering();
	glUseProgram(3); // TODO Effekseer changes the glUseProgram to something else, so we need to restore it, otherwise everything done in love.draw() doesn't show up.
}

} // graphics
} // love
