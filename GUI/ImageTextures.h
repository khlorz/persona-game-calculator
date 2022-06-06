#pragma once

#ifndef _ImageTextures_
#define _ImageTextures_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <array>

class ImgTextures;

template<size_t T>
using TextureArray = std::array<ImgTextures*, T>;
using ImgTexVec    = std::vector<ImgTextures*>;

class ImgTextures
{
	private:
		GLuint      iTexture;
		int         iwidth;
		int         iheight;
		float       iAspectRatio;
	public:
		ImgTextures();
		~ImgTextures();

		// Load an image texture from the filepath
		bool   LoadTextures(const char* path);
		// Copy a texture from an existing texture
		void   CopyTexture(ImgTextures img);
		// Sets the aspect ratio of the image_texture
		void   SetAspectRatio();
		// Clear the texture
		void   ClearTextures();
		// Get the texture height
		int	   GetHeight();
		// Get the texture width
		int	   GetWidth();
		// Get the texture aspect ratio
		float  GetAspectRatio();
		// Get the texture
		GLuint GetTexture();
};


#endif