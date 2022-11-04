#include "Chooser.h"
#include "PortableNetworkGraphics.h"
#include "QuiteOkImage.h"

Utilities::ImageFormat::Chooser::Chooser() {
    formats.push_back( new Utilities::ImageFormat::PortableNetworkGraphics() );
    formats.push_back( new Utilities::ImageFormat::QuiteOkImage() );
    
    for( auto x : formats ) {
        if( x->canWrite() )
            writer_references.push_back( x );
        if( x->canRead() )
            reader_references.push_back( x );
    }
}

Utilities::ImageFormat::Chooser::~Chooser() {
    for( auto x : formats ) {
        delete x;
    }
}

Utilities::ImageFormat::ImageFormat* Utilities::ImageFormat::Chooser::getWriterReference( const ImageBase2D<Grid2DPlacementNormal>& image_data, bool first ) {
    if( first )
        return getWriterReference( *image_data.getPixelFormat() );
    else {
        size_t size_of_smallest_image = SIZE_MAX;
        size_t size_of_image;
        Utilities::ImageFormat::ImageFormat *imageFormat_p = nullptr;
        
        // This operation is not fast, but it is intended to find the best format that would
        // take the least disk space.
        for( auto x : writer_references ) {
            if( x->supports( *image_data.getPixelFormat() ) ) {
                size_of_image = x->getSpace( image_data );
                
                if( size_of_image == 0 ) {
                    // Do nothing! I am pretty sure that no image file is zero bytes.
                }
                else if( size_of_image < size_of_smallest_image ) {
                    // Choose the format that does not shirnk the image file.
                    size_of_smallest_image = size_of_image;
                    imageFormat_p = x;
                }
            }
        }
        
        return imageFormat_p;
    }
}

Utilities::ImageFormat::ImageFormat* Utilities::ImageFormat::Chooser::getWriterReference( const PixelFormatColor& pixel_format ) {
    for( auto x : writer_references ) {
        if( x->supports( pixel_format ) )
            return x;
    }
    return nullptr;
}

Utilities::ImageFormat::ImageFormat* Utilities::ImageFormat::Chooser::getReaderReference( const Buffer& readerForImage ) {
    for( auto x : reader_references ) {
        if( x->isFormat( readerForImage ) )
            return x;
    }
    return nullptr;
}

Utilities::ImageFormat::ImageFormat* Utilities::ImageFormat::Chooser::getWriterCopy(  const ImageBase2D<Grid2DPlacementNormal>& image_data, bool first ) const {
    ImageFormat *ref = const_cast<Chooser*>( this )->getWriterReference( image_data, first );
    
    if( ref != nullptr )
        return ref->duplicate();
    else
        return nullptr;
}

Utilities::ImageFormat::ImageFormat* Utilities::ImageFormat::Chooser::getWriterCopy(  const PixelFormatColor& pixel_format ) const {
    ImageFormat *ref = const_cast<Chooser*>( this )->getWriterReference( pixel_format );
    
    if( ref != nullptr )
        return ref->duplicate();
    else
        return nullptr;
}

Utilities::ImageFormat::ImageFormat* Utilities::ImageFormat::Chooser::getReaderCopy( const Buffer& readerForImage ) const {
    ImageFormat *ref = const_cast<Chooser*>( this )->getReaderReference( readerForImage );
    
    if( ref != nullptr )
        return ref->duplicate();
    else
        return nullptr;
}
