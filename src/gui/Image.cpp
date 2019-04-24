#include <utility>

#include "GL/gl3w.h"
#include "Image.hpp" // IWYU pragma: associated
#include "util/StringBuilder.hpp"
#include "core/Exception.hpp"
#include "util/define_logger.hpp"
#include <limits>

DEFINE_LOGGER( aima::gui::Image );

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT( x )                       \
do {                                           \
    if( !(x) ) {                               \
        using namespace aima::core::exception; \
        using aima::util::StringBuilder;       \
        AIMA_THROW_EXCEPTION(                  \
            Exception{} << Because( StringBuilder( 512 ) << "STBI assertion failed: " << #x )); \
    }                                          \
} while ( false )

#include "stb_image.h"

using aima::gui::Image;

Image::Image( const std::filesystem::path& path ) : data( stbi_load( path.c_str(), &width_, &height_, &channels_, 0 )),
                                                    texture_( nullTexture ) {
    TRACE;

    if ( data == nullptr ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because(
                util::StringBuilder( 512 ) << "Failed to load image \"" << path << "\": " << stbi_failure_reason()));
    }

    LOG4CPLUS_DEBUG( GetLogger(), "Loaded image " << path << " at " << static_cast<void*>(data));
}

Image::Image( ImGuiWrapper& imGuiWrapper, const std::filesystem::path& path ) : Image( path ) {
    makeTexture( imGuiWrapper, false );
}

Image::~Image() {
    try {
        dispose();
    }
    catch ( ... ) {}
}

void Image::dispose() {
    if ( data == nullptr ) return;
    stbi_image_free( data );
    data = nullptr;
    glDeleteTextures( 1, &texture_ );
}

void Image::makeTexture( ImGuiWrapper&, bool remake ) {
    TRACE;

    if ( texture_ != nullTexture ) {
        if ( !remake ) return;
        glDeleteTextures( 1, &texture_ );
    }

    LOG4CPLUS_DEBUG( GetLogger(), "Making texture from image at " << static_cast<void*>(data));

    glGenTextures( 1, &texture_ );
    glBindTexture( GL_TEXTURE_2D, texture_ );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
    glGenerateMipmap( GL_TEXTURE_2D );
}

GLuint aima::gui::Image::texture() const {
    if ( texture_ != nullTexture ) return texture_;

    using namespace aima::core::exception;
    AIMA_THROW_EXCEPTION( Exception{} << Because( "Texture not created" ));
}
