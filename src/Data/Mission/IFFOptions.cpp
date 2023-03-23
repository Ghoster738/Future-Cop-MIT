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

    std::string key_value = "";
    std::vector<std::string> parameters;
    for( const auto &i : raw_arguments ) {
        if( i.size() >= 2 ) {
            if( (i.at(0) == '-' && i.at(1) == '-') || // if it is a full option.
                (i.at(0) == '-' && (i.at(1) < '0' || i.at(1) > '9')) // If this is a partial option, and partial options should not be confussed with negative numbers.
            ) {
                // Submit previous command.
                if( !key_value.empty() ) {
                    arguments[ key_value ] = parameters;
                    parameters.clear();
                }

                key_value = i;
            }
            else
                parameters.push_back( i );
        }
        else
            parameters.push_back( i );
    }

    if( !key_value.empty() || !parameters.empty() ) {
        arguments[ key_value ] = parameters;
        parameters.clear();
    }

    valid_parameters          |= act.readParams( arguments, output_r );
    enable_global_dry_default |= act.override_dry;

    valid_parameters          |= anm.readParams( arguments, output_r );
    enable_global_dry_default |= anm.override_dry;

    valid_parameters          |= bmp.readParams( arguments, output_r );
    enable_global_dry_default |= bmp.override_dry;

    valid_parameters          |= dcs.readParams( arguments, output_r );
    enable_global_dry_default |= dcs.override_dry;

    valid_parameters          |= font.readParams( arguments, output_r );
    enable_global_dry_default |= font.override_dry;

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

    if( !arguments.empty() ) {
        if( output_r != nullptr ) {
            *output_r << "Unrecognized IFF Options\n";

            for( auto a : arguments) {
                *output_r << "  " << a.first << ":";

                for( auto i : a.second ) {
                    *output_r << " " << i;
                }
            }
            *output_r << std::endl;
        }

        valid_parameters = false;
    }

    return valid_parameters;
}

bool IFFOptions::hasUnexpectedParams( const std::string &name, const std::vector<std::string> &arguments, std::ostream *output_r ) {
    if( arguments.size() != 0 ) {
        if( output_r != nullptr ) {
            *output_r << name << " should not have arguments.\n";

            for( auto i : arguments ) {
                *output_r << "  " << i << "\n";
            }
            *output_r << std::endl;
        }
        return true;
    }
    return false;
}

bool IFFOptions::singleArgument( std::map<std::string, std::vector<std::string>> &arguments, const std::string actual_name, std::ostream *output_r, bool &is_found ) {
    auto argument = arguments.find( actual_name );

    if( argument != arguments.end() ) {

        if( hasUnexpectedParams( argument->first, argument->second, output_r ) ) {
            return false;
        }

        arguments.erase( argument );

        is_found = true;
    }

    return true; // Successfully found.
}

bool IFFOptions::ResourceOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    auto enable_option = arguments.find("--" + getNameSpace() + "_ENABLE");

    if( enable_option != arguments.end() ) {

        if( hasUnexpectedParams( enable_option->first, enable_option->second, output_r ) ) {
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
