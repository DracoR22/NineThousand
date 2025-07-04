#pragma once

namespace RendererCommon {
    constexpr float POSTPROCESS_QUAD_VERTICES[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

   constexpr float UI_VERTICES[] = {
        // positions    // tex coords
         0.0f, 0.0f,     0.0f, 0.0f,
         1.0f, 0.0f,     1.0f, 0.0f,
         1.0f, 1.0f,     1.0f, 1.0f,

         0.0f, 0.0f,     0.0f, 0.0f,
         1.0f, 1.0f,     1.0f, 1.0f,
         0.0f, 1.0f,     0.0f, 1.0f
    };

    enum class PostProcessMode {
        NONE,
        SHARPEN
    };
}