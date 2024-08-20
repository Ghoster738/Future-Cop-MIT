#ifndef DATA_MISSION_IFF_OPTIONS_HEADER
#define DATA_MISSION_IFF_OPTIONS_HEADER

#include <string>
#include <vector>
#include <ostream>
#include <map>

namespace Data::Mission {

struct IFFOptions {
    IFFOptions();
    IFFOptions( const std::vector<std::string> & arguments, std::ostream *output_r );
    virtual ~IFFOptions();

    bool readParams( const std::vector<std::string> &arguments, std::ostream *output_r );

    static bool hasUnexpectedParams( const std::string &name, const std::vector<std::string> &arguments, std::ostream *output_r );

    static bool singleArgument( std::map<std::string, std::vector<std::string>> &argument, const std::string actual_name, std::ostream *output_r, bool &is_found );

    std::string getOptions() const;

    struct ResourceOption {
        bool override_dry;

        ResourceOption() : override_dry( false ) {}
        virtual ~ResourceOption() {}

        virtual std::string getNameSpace() const = 0;

        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );

        std::string getBuiltInOptions( unsigned number_of_spaces = 0 ) const;
        virtual std::string getOptions() const = 0;

        bool shouldWrite( bool enable_global_dry_default ) const;
    };

    // This area stores the actual options.
    bool enable_global_dry_default;

    // This is the the option to other resources.

    struct AIFFOption : public ResourceOption {
        bool to_wav;

        AIFFOption() : ResourceOption(), to_wav( false ) {}
        std::string getNameSpace() const { return "AIFF"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } aiff;

    struct ACTOption : public ResourceOption {

        ACTOption() : ResourceOption() {}
        std::string getNameSpace() const { return "ACT"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } act;

    struct ANMOption : public ResourceOption {
        bool export_palette;

        ANMOption() : ResourceOption(), export_palette( false ) {}
        std::string getNameSpace() const { return "ANM"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } anm;

    struct BMPOption : public ResourceOption {
        bool export_palette;

        BMPOption() : ResourceOption(), export_palette( false ) {}
        std::string getNameSpace() const { return "BMP"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } bmp;

    struct DCSOption : public ResourceOption {

        DCSOption() : ResourceOption() {}
        std::string getNameSpace() const { return "DCS"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } dcs;

    struct FontOption : public ResourceOption {

        FontOption() : ResourceOption() {}
        std::string getNameSpace() const { return "FONT"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } font;

    struct FUNOption : public ResourceOption {

        FUNOption() : ResourceOption() {}
        std::string getNameSpace() const { return "FUN"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } fun;

    struct MSICOption : public ResourceOption {

        MSICOption() : ResourceOption() {}
        std::string getNameSpace() const { return "MSIC"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } msic;

    struct NETOption : public ResourceOption {
        bool enable_obj;

        NETOption() : ResourceOption(), enable_obj( false ) {}
        std::string getNameSpace() const { return "NET"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } net;

    struct ObjOption : public ResourceOption {
        bool no_model;
        bool export_metadata;
        bool export_bounding_box;

        ObjOption() : ResourceOption(), no_model(false), export_metadata( false ), export_bounding_box( false ) {}
        std::string getNameSpace() const { return "OBJ"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } obj;

    struct PTCOption : public ResourceOption {
        bool no_model;
        bool entire_point_cloud;
        bool entire_height_map;
        bool enable_backface_culling;

        PTCOption() : ResourceOption(), no_model( false ), entire_point_cloud( false ), entire_height_map( false ), enable_backface_culling( false ) {}
        std::string getNameSpace() const { return "PTC"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } ptc;

    struct PYROption : public ResourceOption {
        bool export_prime_bw;
        bool export_palettless_atlas;

        PYROption() : ResourceOption(), export_prime_bw( false ), export_palettless_atlas( false ) {}
        std::string getNameSpace() const { return "PYR"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } pyr;

    struct RPNSOption : public ResourceOption {

        RPNSOption() : ResourceOption() {}
        std::string getNameSpace() const { return "RPNS"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } rpns;

    struct SNDSOption : public ResourceOption {

        SNDSOption() : ResourceOption() {}
        std::string getNameSpace() const { return "SNDS"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } snds;

    struct TilOption : public ResourceOption {
        bool export_metadata;
        bool enable_point_cloud_export;
        bool enable_height_map_export;
        bool enable_til_export_model;
        bool enable_til_backface_culling;

        TilOption() : ResourceOption(), export_metadata( false ), enable_point_cloud_export( false ), enable_height_map_export( false ), enable_til_export_model( false ), enable_til_backface_culling(false) {}
        std::string getNameSpace() const { return "TIL"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } til;

    struct WavOption : public ResourceOption {
        bool reencode_wav;

        WavOption() : ResourceOption(), reencode_wav( false ) {}
        std::string getNameSpace() const { return "WAV"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
        virtual std::string getOptions() const;
    } wav;
};

}
#endif // DATA_MISSION_IFF_OPTION_HEADER
