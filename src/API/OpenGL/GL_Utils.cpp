#include "./GL_Utils.h"
#include <iostream>

namespace OpenGLUtils {
	GLenum GLInternalFormatToGLFormat(GLenum internalFormat) {
        switch (internalFormat) {
            // Red channel formats
        case GL_R8:
        case GL_R8_SNORM:
        case GL_R16:
        case GL_R16_SNORM:
        case GL_R16F:
        case GL_R32F:
            return GL_RED;
        case GL_R8UI:
        case GL_R8I:
        case GL_R16UI:
        case GL_R16I:
        case GL_R32UI:
        case GL_R32I:
            return GL_RED_INTEGER;

            // Red-Green channel formats
        case GL_RG8:
        case GL_RG8_SNORM:
        case GL_RG16:
        case GL_RG16_SNORM:
        case GL_RG16F:
        case GL_RG32F:
            return GL_RG;
        case GL_RG8UI:
        case GL_RG8I:
        case GL_RG16UI:
        case GL_RG16I:
        case GL_RG32UI:
        case GL_RG32I:
            return GL_RG_INTEGER;

            // RGB channel formats
        case GL_RGB8:
        case GL_RGB8_SNORM:
        case GL_RGB16:
        case GL_RGB16_SNORM:
        case GL_RGB16F:
        case GL_RGB32F:
        case GL_SRGB8:
            return GL_RGB;
        case GL_RGB8UI:
        case GL_RGB8I:
        case GL_RGB16UI:
        case GL_RGB16I:
        case GL_RGB32UI:
        case GL_RGB32I:
            return GL_RGB_INTEGER;

            // RGBA channel formats
        case GL_RGBA8:
        case GL_RGBA8_SNORM:
        case GL_RGBA16:
        case GL_RGBA16_SNORM:
        case GL_RGBA16F:
        case GL_RGBA32F:
        case GL_SRGB8_ALPHA8:
            return GL_RGBA;
        case GL_RGBA8UI:
        case GL_RGBA8I:
        case GL_RGBA16UI:
        case GL_RGBA16I:
        case GL_RGBA32UI:
        case GL_RGBA32I:
            return GL_RGBA_INTEGER;

            // Special packed formats
        case GL_RGB10_A2:
        case GL_RGB10_A2UI:
            return GL_RGBA;

            // Depth formats
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            return GL_DEPTH_COMPONENT;

            // Depth-stencil formats
        case GL_DEPTH24_STENCIL8:
        case GL_DEPTH32F_STENCIL8:
            return GL_DEPTH_STENCIL;

        default:
            std::cout << "OpenGLUtils::GLInternalFormatToGLFormat(): Unsupported internal format\n";
            return 0;
        }
	}

    GLenum GLInternalFormatToGLType(GLenum internalFormat) {
        switch (internalFormat) {
            // 8-bit unsigned and signed integer formats
        case GL_R8UI:      return GL_UNSIGNED_BYTE;
        case GL_R8I:       return GL_BYTE;
        case GL_RG8UI:     return GL_UNSIGNED_BYTE;
        case GL_RG8I:      return GL_BYTE;
        case GL_RGBA8UI:   return GL_UNSIGNED_BYTE;
        case GL_RGBA8I:    return GL_BYTE;

            // 16-bit unsigned and signed integer formats
        case GL_R16UI:     return GL_UNSIGNED_SHORT;
        case GL_R16I:      return GL_SHORT;
        case GL_RG16UI:    return GL_UNSIGNED_SHORT;
        case GL_RG16I:     return GL_SHORT;
        case GL_RGBA16UI:  return GL_UNSIGNED_SHORT;
        case GL_RGBA16I:   return GL_SHORT;

            // 32-bit unsigned and signed integer formats
        case GL_R32UI:     return GL_UNSIGNED_INT;
        case GL_R32I:      return GL_INT;
        case GL_RG32UI:    return GL_UNSIGNED_INT;
        case GL_RG32I:     return GL_INT;
        case GL_RGBA32UI:  return GL_UNSIGNED_INT;
        case GL_RGBA32I:   return GL_INT;

            // Special packed integer format
        case GL_RGB10_A2UI: return GL_UNSIGNED_INT_2_10_10_10_REV;

            // Normalized unsigned formats (non-integer)
        case GL_R8:        return GL_UNSIGNED_BYTE;
        case GL_RG8:       return GL_UNSIGNED_BYTE;
        case GL_RGBA8:     return GL_UNSIGNED_BYTE;
        case GL_SRGB8:     return GL_UNSIGNED_BYTE;
        case GL_SRGB8_ALPHA8: return GL_UNSIGNED_BYTE;
        case GL_R16:       return GL_UNSIGNED_SHORT;
        case GL_RG16:      return GL_UNSIGNED_SHORT;
        case GL_RGBA16:    return GL_UNSIGNED_SHORT;

            // Floating point formats
        case GL_R16F:      return GL_FLOAT;
        case GL_RG16F:     return GL_FLOAT;
        case GL_RGBA16F:   return GL_FLOAT;
        case GL_R32F:      return GL_FLOAT;
        case GL_RG32F:     return GL_FLOAT;
        case GL_RGBA32F:   return GL_FLOAT;
            // Depth and depth-stencil formats
        case GL_DEPTH_COMPONENT16:       return GL_UNSIGNED_SHORT;
        case GL_DEPTH_COMPONENT24:       return GL_UNSIGNED_INT;
        case GL_DEPTH_COMPONENT32F:      return GL_FLOAT;
        case GL_DEPTH24_STENCIL8:        return GL_UNSIGNED_INT_24_8;
        case GL_DEPTH32F_STENCIL8:       return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        default:
            std::cout << "OpenGLUtils::InternalFormatToType(GLenum internalFormat) failed Unsupported internal format\n";
            return 0;
        }
    }
}