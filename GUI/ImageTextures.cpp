#include "ImageTextures.h"
#include "stb_image.h"

ImgTextures::ImgTextures() :
    iTexture(0),
    iwidth(0),
    iheight(0),
    iAspectRatio(0)
{}

ImgTextures::~ImgTextures()
{
    ClearTextures();
}

bool ImgTextures::LoadTextures(const char* path)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(path, &iwidth, &iheight, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    glGenTextures(1, &iTexture);
    glBindTexture(GL_TEXTURE_2D, iTexture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    //glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, sizeof(image_data)/ sizeof(image_data[0]), image_data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image_data);

    SetAspectRatio();

    return true;
}

void ImgTextures::ClearTextures()
{
    glDeleteTextures(1, &iTexture);
    iTexture = 0;
}

void ImgTextures::SetAspectRatio()
{
    iAspectRatio = float(iwidth) / float(iheight);
}

GLuint ImgTextures::GetTexture()
{
    return iTexture;
}

float ImgTextures::GetAspectRatio()
{
    return iAspectRatio;
}

int ImgTextures::GetWidth()
{
    return iwidth;
}

int ImgTextures::GetHeight()
{
    return iheight;
}

void ImgTextures::CopyTexture(ImgTextures img)
{
    GLubyte* image_data = new GLubyte[img.GetWidth() * img.GetHeight() * 4];
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, img.GetTexture());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    glGenTextures(1, &iTexture);
    glBindTexture(GL_TEXTURE_2D, iTexture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

    delete[] image_data;

    iwidth  = img.GetWidth();
    iheight = img.GetHeight();
}