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

    static bool noAdditionalParams(  const std::string &name, const std::vector<std::string> &arguments, std::ostream *output_r );

    static bool singleArgument( std::map<std::string, std::vector<std::string>> &argument, const std::string actual_name, std::ostream *output_r, bool &is_found );

    struct ResourceOption {
        bool override_dry;

        ResourceOption() : override_dry( false ) {}
        virtual ~ResourceOption() {}

        virtual std::string getNameSpace() const = 0;

        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );

        bool shouldWrite( bool enable_global_dry_default ) const;
    };

    // This area stores the actual options.
    bool enable_global_dry_default;

    // This is the the option to other resources.

    struct ACTOption : public ResourceOption {

        ACTOption() : ResourceOption() {}
        std::string getNameSpace() const { return "ACT"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } act;

    struct ANMOption : public ResourceOption {
        bool export_palette;

        ANMOption() : ResourceOption(), export_palette( false ) {}
        std::string getNameSpace() const { return "ANM"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } anm;

    struct BMPOption : public ResourceOption {
        bool export_palette;

        BMPOption() : ResourceOption() {}
        std::string getNameSpace() const { return "BMP"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } bmp;

    struct DCSOption : public ResourceOption {

        DCSOption() : ResourceOption() {}
        std::string getNameSpace() const { return "DCS"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } dcs;

    struct FontOption : public ResourceOption {

        FontOption() : ResourceOption() {}
        std::string getNameSpace() const { return "FONT"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } font;

    struct FUNOption : public ResourceOption {

        FUNOption() : ResourceOption() {}
        std::string getNameSpace() const { return "FUN"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } fun;

    struct MSICOption : public ResourceOption {

        MSICOption() : ResourceOption() {}
        std::string getNameSpace() const { return "MSIC"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } msic;

    struct NETOption : public ResourceOption {
        bool enable_obj;

        NETOption() : ResourceOption(), enable_obj( false ) {}
        std::string getNameSpace() const { return "NET"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } net;

    struct ObjOption : public ResourceOption {

        ObjOption() : ResourceOption() {}
        std::string getNameSpace() const { return "OBJ"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } obj;

    struct PTCOption : public ResourceOption {
        bool no_model;
        bool entire_point_cloud;
        bool entire_height_map;

        PTCOption() : ResourceOption(), no_model( false ), entire_point_cloud( false ), entire_height_map( false ) {}
        std::string getNameSpace() const { return "PTC"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } ptc;

    struct PYROption : public ResourceOption {
        bool export_prime_bw;

        PYROption() : ResourceOption(), export_prime_bw( false ) {}
        std::string getNameSpace() const { return "PYR"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } pyr;

    struct RPNSOption : public ResourceOption {

        RPNSOption() : ResourceOption() {}
        std::string getNameSpace() const { return "RPNS"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } rpns;

    struct SNDSOption : public ResourceOption {

        SNDSOption() : ResourceOption() {}
        std::string getNameSpace() const { return "SNDS"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } snds;

    struct TilOption : public ResourceOption {
        bool enable_point_cloud_export;
        bool enable_height_map_export;
        bool enable_til_export_model;

        TilOption() : ResourceOption(), enable_point_cloud_export( false ), enable_height_map_export( false ), enable_til_export_model( false ) {}
        std::string getNameSpace() const { return "TIL"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } til;

    struct WavOption : public ResourceOption {

        WavOption() : ResourceOption() {}
        std::string getNameSpace() const { return "WAV"; }
        virtual bool readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r );
    } wav;
};

}
#endif // DATA_MISSION_IFF_OPTION_HEADER
