#include "IFFOptions.h"

namespace Data::Mission {

IFFOptions::IFFOptions() : enable_global_dry_default( false ) {
}

IFFOptions::IFFOptions( const std::vector<std::string> & arguments, std::ostream *output_r ) {
    readParams( arguments, output_r );
}

IFFOptions::~IFFOptions() {
}

bool IFFOptions::readParams( const std::vector<std::string> &raw_arguments, std::ostream *output_r ) {
    bool valid_parameters = 0;

    std::map<std::string, std::vector<std::string>> arguments;

    valid_parameters          |= act.readParams( arguments, output_r );
    enable_global_dry_default |= act.override_dry;

    valid_parameters          |= anm.readParams( arguments, output_r );
    enable_global_dry_default |= anm.override_dry;

    valid_parameters          |= bmp.readParams( arguments, output_r );
    enable_global_dry_default |= bmp.override_dry;

    valid_parameters          |= dcs.readParams( arguments, output_r );
    enable_global_dry_default |= dcs.override_dry;

    valid_parameters          |= fun.readParams( arguments, output_r );
    enable_global_dry_default |= fun.override_dry;

    valid_parameters          |= msic.readParams( arguments, output_r );
    enable_global_dry_default |= msic.override_dry;

    valid_parameters          |= net.readParams( arguments, output_r );
    enable_global_dry_default |= net.override_dry;

    valid_parameters          |= obj.readParams( arguments, output_r );
    enable_global_dry_default |= obj.override_dry;

    valid_parameters          |= ptc.readParams( arguments, output_r );
    enable_global_dry_default |= ptc.override_dry;

    valid_parameters          |= pyr.readParams( arguments, output_r );
    enable_global_dry_default |= pyr.override_dry;

    valid_parameters          |= rpns.readParams( arguments, output_r );
    enable_global_dry_default |= rpns.override_dry;

    valid_parameters          |= til.readParams( arguments, output_r );
    enable_global_dry_default |= til.override_dry;

    valid_parameters          |= wav.readParams( arguments, output_r );
    enable_global_dry_default |= wav.override_dry;

    return valid_parameters;
}

bool IFFOptions::ResourceOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    auto enable_option = arguments.find("--" + getNameSpace() + "_ENABLE");

    if( enable_option != arguments.end() ) {

        if( enable_option->second.size() != 0 ) {

            if( output_r != nullptr ) {
                *output_r << enable_option->first << " should not have arguments next to it!\n";

                for( auto i : enable_option->second ) {
                    *output_r << "  " << i << "\n";
                }
                *output_r << std::endl;
            }
            return false; // Cancel due to irrecoverable error.
        }

        // Do not want to make the program think there is an unrecognized command.
        arguments.erase( enable_option );

        override_dry = true;
    }

    return true;
}

bool IFFOptions::ResourceOption::shouldWrite( bool enable_global_dry_default ) const {
    if( enable_global_dry_default )
        if( override_dry )
            return true;
        else
            return false;
    else
        return true;
}

}
