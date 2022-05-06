#ifndef FORMAT_CHOOSER_HEADER
#define FORMAT_CHOOSER_HEADER

#include "ImageFormat.h"

namespace Utilities {

namespace ImageFormat {

/**
 * This class is to provide automatic format choosing for both read and write operations.
 */
class Chooser {
protected:
    std::vector<ImageFormat*> formats;
    
    std::vector<ImageFormat*> writer_references;
    std::vector<ImageFormat*> reader_references;
public:
    Chooser();
    virtual ~Chooser();
    
    /**
     * This gets the writer reference for memory effiency.
     * However, thread safety for using this is unsafe!
     * @param image_data This is the image that will be encoded.
     * @param first If true the first valid format that has no compromises in image quality or at least a writer. If this is false then it would choose the format that both has no compromises in quality and has the smallest file size.
     * @return A valid ImageFormat for success, or a nullptr if there is no format that supports the image_data.
     */
    ImageFormat* getWriterReference( const ImageData& image_data, bool first = true );
    
    /**
     * This gets the writer reference for memory effiency.
     * However, thread safety for using this is unsafe!
     * @param type This is holds the type of image data that would be encoded.
     * @param bytes_per_channel This is holds the color depth of the data.
     * @return A valid ImageFormat for success, or a nullptr if there is no format that supports the image_data.
     */
    ImageFormat* getWriterReference( ImageData::Type type, unsigned int bytes_per_channel );
    /**
     * This gets the reader reference for memory effiency.
     * However, thread safety for using this is unsafe!
     * @param image_data This is the magic number to be read for data.
     * @return A valid ImageFormat for success, or a nullptr if there is no format that supports the image_data.
     */
    ImageFormat* getReaderReference( const Buffer& readerForImage );
    
    ImageFormat* getWriterCopy( const ImageData& image_data, bool first = true ) const;
    ImageFormat* getWriterCopy( ImageData::Type type, unsigned int bytes_per_channel ) const;
    ImageFormat* getReaderCopy( const Buffer& readerForImage ) const;
};

};

};

#endif // FORMAT_CHOOSER_HEADER

