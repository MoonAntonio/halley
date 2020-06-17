#include "halley/concurrency/concurrent.h"
#include "halley/support/exception.h"
#include "halley_gl.h"
#include "texture_opengl.h"
#include "halley/core/graphics/texture_descriptor.h"
#include <gsl/gsl_assert>
#include "video_opengl.h"
#include "halley/support/logger.h"
#include "halley/text/string_converter.h"

using namespace Halley;

TextureOpenGL::TextureOpenGL(VideoOpenGL& parent, Vector2i size)
	: Texture(size)
	, parent(parent)
{
	glGenTextures(1, &textureId);
	startLoading();
}

TextureOpenGL::~TextureOpenGL()
{
	waitForOpenGLLoad();
	if (textureId != 0) {
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
}

TextureOpenGL& TextureOpenGL::operator=(TextureOpenGL&& other) noexcept
{
	other.waitForOpenGLLoad();

	size = other.size;
	textureId = other.textureId;
	texSize = other.texSize;

	doneLoading();

	return *this;
}

void TextureOpenGL::load(TextureDescriptor&& d)
{
	GLUtils glUtils;
	glUtils.bindTexture(textureId);
	
	if (texSize != d.size) {
		create(d.size, d.format, d.useMipMap, d.useFiltering, d.addressMode, d.pixelData);
	} else if (!d.pixelData.empty()) {
		updateImage(d.pixelData, d.format, d.useMipMap);
	}
	finishLoading();
}

void TextureOpenGL::reload(Resource&& resource)
{
	*this = std::move(dynamic_cast<TextureOpenGL&>(resource));
}

unsigned TextureOpenGL::getNativeId() const
{
	return textureId;
}

void TextureOpenGL::finishLoading()
{
	if (parent.isLoaderThread()) {
#if defined(WITH_OPENGL) || defined(WITH_OPENGL_ES3)
		fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
#endif
		glFlush();
	}

	doneLoading();
}

void TextureOpenGL::waitForOpenGLLoad() const
{
	waitForLoad();

#if defined (WITH_OPENGL) || defined(WITH_OPENGL_ES3)
	if (fence) {
		GLenum result = glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(10000000000));
		if (result == GL_TIMEOUT_EXPIRED) {
			//throw Exception("Timeout waiting for texture to load.");
			Logger::logError("Timeout waiting for texture fence to sync. Graphics might be corrupted.");
		}
		else if (result == GL_WAIT_FAILED) {
			glCheckError();
		}
		else if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
			glDeleteSync(fence);
			fence = nullptr;
		}
	}
#endif
}

// Do not use this method inside this class, due to fence
void TextureOpenGL::bind(int textureUnit) const
{
	waitForOpenGLLoad();
	GLUtils glUtils;
	glUtils.setTextureUnit(textureUnit);
	glUtils.bindTexture(textureId);
}

void TextureOpenGL::create(Vector2i size, TextureFormat format, bool useMipMap, bool useFiltering, TextureAddressMode addressMode, TextureDescriptorImageData& pixelData)
{
	Expects(size.x > 0);
	Expects(size.y > 0);
	Expects(size.x <= 4096);
	Expects(size.y <= 4096);
	glCheckError();

#if defined (WITH_OPENGL)
	if (useMipMap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1);
	}
#endif

#if defined (WITH_OPENGL) || defined(WITH_OPENGL_ES3)
	GLuint wrap = getGLAddressMode(addressMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
#endif

	int filtering = useFiltering ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMipMap ? GL_LINEAR_MIPMAP_LINEAR : filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

#if defined (WITH_OPENGL) || defined(WITH_OPENGL_ES3)
	if (format == TextureFormat::DEPTH) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
#endif

	GLuint internalFormat = getGLInternalFormat(format);
	GLuint pixelFormat = getGLPixelFormat(format);
	int stride = pixelData.empty() ? size.x : pixelData.getStrideOr(size.x);
	glPixelStorei(GL_UNPACK_ALIGNMENT, TextureDescriptor::getBitsPerPixel(format));
	glPixelStorei(GL_PACK_ROW_LENGTH, stride);

	if (pixelData.empty()) {
		Vector<char> blank;
		blank.resize(size.x * size.y * TextureDescriptor::getBitsPerPixel(format));
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, pixelFormat, GL_UNSIGNED_BYTE, blank.data());
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, pixelFormat, GL_UNSIGNED_BYTE, pixelData.getBytes());
	}
	glCheckError();

	texSize = size;
}

void TextureOpenGL::updateImage(TextureDescriptorImageData& pixelData, TextureFormat format, bool useMipMap)
{
	int stride = pixelData.getStrideOr(size.x);

#if defined (WITH_OPENGL) || defined(WITH_OPENGL_ES3)
	glPixelStorei(GL_UNPACK_ALIGNMENT, TextureDescriptor::getBitsPerPixel(format));
	glPixelStorei(GL_PACK_ROW_LENGTH, stride);
#endif
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, getGLPixelFormat(format), GL_UNSIGNED_BYTE, pixelData.getBytes());
	glCheckError();

#if defined (WITH_OPENGL) || defined(WITH_OPENGL_ES3)
	// Generate mipmap
	if (useMipMap) {
		glGenerateMipmap(GL_TEXTURE_2D);
		glCheckError();
	}
#endif
}

unsigned TextureOpenGL::getGLInternalFormat(TextureFormat format)
{
	switch (format) {
	case TextureFormat::Indexed:
		return GL_R8;
	case TextureFormat::RGB:
		return GL_RGB;
	case TextureFormat::RGBA:
		return GL_RGBA;
	case TextureFormat::DEPTH:
		return GL_DEPTH_COMPONENT24;
	default:
		throw Exception("Unknown texture format: " + toString(static_cast<int>(format)), HalleyExceptions::VideoPlugin);
	}
}

unsigned TextureOpenGL::getGLPixelFormat(TextureFormat format)
{
	switch (format) {
	case TextureFormat::Indexed:
		return GL_RED;
	case TextureFormat::RGB:
		return GL_RGB;
	case TextureFormat::RGBA:
		return GL_RGBA;
	case TextureFormat::DEPTH:
		return GL_DEPTH_COMPONENT;
	default:
		throw Exception("Unknown texture format: " + toString(static_cast<int>(format)), HalleyExceptions::VideoPlugin);
	}
}

unsigned TextureOpenGL::getGLAddressMode(TextureAddressMode addressMode)
{
	switch (addressMode) {
	case TextureAddressMode::Clamp:
		return GL_CLAMP_TO_EDGE;
	case TextureAddressMode::Mirror:
		return GL_MIRRORED_REPEAT;
	case TextureAddressMode::Repeat:
		return GL_REPEAT;
	default:
		throw Exception("Unknown texture address mode: " + toString(addressMode), HalleyExceptions::VideoPlugin);
	}
}
