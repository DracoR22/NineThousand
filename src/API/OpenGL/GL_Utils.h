#pragma once

#include <glad/glad.h>

namespace OpenGLUtils {
	GLenum GLInternalFormatToGLFormat(GLenum internalFormat);
	GLenum GLInternalFormatToGLType(GLenum internalFormat);
}