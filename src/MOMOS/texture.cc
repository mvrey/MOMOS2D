#include <iostream>

#include <MOMOS/texture.h>


Texture2D::Texture2D()
    : Width(0)
    , Height(0)
    , Internal_Format(GL_RGB)
    , Image_Format(GL_RGB)
    , Wrap_S(GL_REPEAT)
    , Wrap_T(GL_REPEAT)
    , Filter_Min(GL_LINEAR)
    , Filter_Max(GL_LINEAR)
	, bytes_per_pixel_(0)
{
    glGenTextures(1, &this->ID);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

	// Cache CPU copy so sprite sampling functions can read pixels later on
	switch (this->Image_Format) {
	case GL_RGBA:
	case GL_BGRA:
		bytes_per_pixel_ = 4;
		break;
	case GL_RGB:
	case GL_BGR:
		bytes_per_pixel_ = 3;
		break;
	case GL_RED:
	case GL_ALPHA:
		bytes_per_pixel_ = 1;
		break;
	default:
		bytes_per_pixel_ = 4;
		break;
	}

	const size_t pixel_count = static_cast<size_t>(width) * static_cast<size_t>(height);
	const size_t byte_count = pixel_count * bytes_per_pixel_;
	if (data != nullptr && byte_count > 0) {
		pixel_data_.assign(data, data + byte_count);
	} else {
		pixel_data_.clear();
		bytes_per_pixel_ = 0;
	}
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}