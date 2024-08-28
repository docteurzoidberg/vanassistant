#pragma once

#include <fb/fbgraphics.h>
#include <fb/fbg_fbdev.h>

#include <iostream>
#include <sys/types.h>
#include <cstdint>

#define OLC_GFX_CUSTOM_EX
#define OLC_PLATFORM_CUSTOM_EX

#include "olcPixelGameEngine.h"

// Platform class using FBG library
class LinuxFBGPlatform : public olc::Platform {
private:
	struct _fbg *fbg_instance;
	int width, height;
	const char* fbdev = "/dev/fb0";

	olc::rcode CreateFrameBuffer() {
		if(!fbg_fbdevSetup((char *)fbdev, 0)) {
      return olc::rcode::FAIL;
    }
		fbg_instance = fbg_fbdevInit();
		if (fbg_instance == NULL) {
			return olc::rcode::FAIL;
		}
		width = fbg_instance->width;
		height = fbg_instance->height;
		return olc::rcode::OK;
	}

public:
	olc::rcode ThreadStartUp() override { return olc::rcode::OK; }
	olc::rcode ThreadCleanUp() override { return olc::rcode::OK; }
	olc::rcode CreateGraphics(bool bFullScreen, bool bEnableVSYNC, const olc::vi2d& vViewPos, const olc::vi2d& vViewSize) override { return olc::rcode::OK; }
	olc::rcode CreateWindowPane(const olc::vi2d& vWindowPos, olc::vi2d& vWindowSize, bool bFullScreen) override { return olc::rcode::OK; }
	olc::rcode SetWindowTitle(const std::string& s) override { return olc::rcode::OK; }
	olc::rcode StartSystemEventLoop() override { return olc::rcode::OK; }

	olc::rcode ApplicationStartUp() override {
		return CreateFrameBuffer();
	}

	olc::rcode ApplicationCleanUp() override {
		if (fbg_instance) {
			fbg_close(fbg_instance);
		}
		return olc::rcode::OK;
	}

	olc::rcode HandleSystemEvent() override {
		// Implement any input handling here if necessary
		return olc::rcode::OK;
	}

	struct _fbg* GetFrameBufferInstance() {
		return fbg_instance;
	}

	int GetScreenWidth() {
		return width;
	}

	int GetScreenHeight() {
		return height;
	}
};

// Renderer class using FBG library
class LinuxFBGRenderer : public olc::Renderer {
private:
	LinuxFBGPlatform* platform;
	struct _fbg *fbg_instance;

public:

	void PrepareDrawing() override {
		std::cout << "PrepareDrawing" << std::endl;
		//CreateDevice({platform}, false, false);

		if (!fbg_instance) return;
		fbg_clear(fbg_instance, 0);
	}

	void SetDecalMode(const olc::DecalMode& mode) override {
		// Not implemented
		std::cout << "SetDecalMode: not implemented" << std::endl;
	}

	uint32_t CreateTexture(uint32_t textureIndex, uint32_t h, bool a, bool b) override {
		// Not implemented
		std::cout << "CreateTexture: not implemented" << std::endl;
		//TODO: Implement texture creation
		return 0;
	}

	void UpdateTexture(uint32_t textureIndex, olc::Sprite* sprite) override {
		// Not implemented
		std::cout << "UpdateTexture: not implemented" << std::endl;
		//TODO: Implement texture update
	}

	void ReadTexture(uint32_t textureIndex, olc::Sprite* sprite) override {
		// Not implemented
		std::cout << "ReadTexture: not implemented" << std::endl;
		//TODO: Implement texture read
	}

	uint32_t DeleteTexture(uint32_t textureIndex) override {
		// Not implemented
		std::cout << "DeleteTexture: not implemented" << std::endl;
		//TODO: Implement texture deletion
		return 0;
	}

	void ApplyTexture(uint32_t textureIndex) override {
		// Not implemented
		std::cout << "ApplyTexture (#" << textureIndex << "): not implemented" << std::endl;
	}

	void UpdateViewport(const olc::vi2d& pos, const olc::vi2d& size) override {
		// Not implemented
		std::cout << "UpdateViewport: not implemented" << std::endl;
	}

	void ClearBuffer(olc::Pixel p, bool bDepth) override {
		std::cout << "ClearBuffer: not implemented" << std::endl;
		//wip
		//fbg_clear(fbg_instance, 0);
	}

	olc::rcode CreateDevice(std::vector<void*> params, bool bFullScreen, bool bVSYNC) override {
		std::cout << "CreateDevice" << std::endl;
		platform = static_cast<LinuxFBGPlatform*>(params[0]);
		fbg_instance = platform->GetFrameBufferInstance();
		return olc::rcode::OK;
	}

	olc::rcode DestroyDevice() override {
		return olc::rcode::OK;
	}

	void PrepareDevice() override {
		if (!fbg_instance) return;
		// Clear the back buffer
		fbg_clear(fbg_instance, 0);
	}

	void DisplayFrame() override {
		std::cout << "DisplayFrame" << std::endl;
		if (!fbg_instance) return;
		// Swap buffers
		fbg_draw(fbg_instance);
		fbg_flip(fbg_instance);
	}

	void DrawDecal(const olc::DecalInstance& decal) override {
		std::cout << "DrawDecal" << std::endl;
		if (!fbg_instance) return;
		olc::Sprite* spr = decal.decal->sprite;
		if (!spr) return;
		for (int y = 0; y < spr->height; y++) {
			for (int x = 0; x < spr->width; x++) {
				olc::Pixel p = spr->GetPixel(x, y);
				if (p.a == 0) continue; // Skip fully transparent pixels
				fbg_pixela(fbg_instance, x + decal.pos[0].x, y + decal.pos[0].y, p.r, p.g, p.b, p.a);
			}
		}
	}

	void DrawLayerQuad(const olc::vf2d& offset, const olc::vf2d& scale, olc::Pixel tint) override {
  
  		//TODO: help me implement this and other drwing methods
		
		// Not implemented
		std::cout << "DrawLayerQuad: not implemented" << std::endl;

		if (!fbg_instance) return;

		// Calculate the size of the quad
		int width = scale.x * fbg_instance->width;
		int height = scale.y * fbg_instance->height;

		fbg_fill(fbg_instance, tint.r, tint.g, tint.b);

		// Draw a filled rectangle representing the layer quad with the specified tint
		fbg_frect(fbg_instance, offset.x, offset.y, width, height);
	}

};
