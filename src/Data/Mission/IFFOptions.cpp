#include "IFFOptions.h"

namespace {

bool hasExtraOf( std::string key_value, const std::map<std::string, std::vector<std::string>> arguments, std::ostream *output_r ) {
    if( arguments.find( key_value ) != arguments.end() ) {
        if( output_r != nullptr ) {
            *output_r << "The number of \"" << key_value << "\"'s should only be one or zero!" << std::endl;
        }

        return true;
    }
    return false;
}

}

namespace Data::Mission {

IFFOptions::IFFOptions() : enable_global_dry_default( false ) {
}

IFFOptions::IFFOptions( const std::vector<std::string> & arguments, std::ostream *output_r ) : enable_global_dry_default( false ) {
    readParams( arguments, output_r );
}

IFFOptions::~IFFOptions() {
}

bool IFFOptions::readParams( const std::vector<std::string> &raw_arguments, std::ostream *output_r ) {
    std::map<std::string, std::vector<std::string>> arguments;
    bool invalid_parameters = false;

    std::string key_value = "";
    std::vector<std::string> parameters;
    for( const auto &i : raw_arguments ) {
        if( i.size() >= 2 ) {
            if( (i.at(0) == '-' && i.at(1) == '-') || // if it is a full option.
                (i.at(0) == '-' && (i.at(1) < '0' || i.at(1) > '9')) // If this is a partial option, and partial options should not be confussed with negative numbers.
            ) {
                // Submit previous command.
                if( !key_value.empty() ) {
                    if( hasExtraOf( key_value, arguments, output_r ) ) {
                        invalid_parameters |= true;
                    }

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
        if( hasExtraOf( key_value, arguments, output_r ) ) {
            invalid_parameters |= true;
        }
        arguments[ key_value ] = parameters;
        parameters.clear();
    }

    enable_global_dry_default = false;

    invalid_parameters        |= act.readParams( arguments, output_r );
    enable_global_dry_default |= act.override_dry;

    invalid_parameters        |= anm.readParams( arguments, output_r );
    enable_global_dry_default |= anm.override_dry;

    invalid_parameters        |= bmp.readParams( arguments, output_r );
    enable_global_dry_default |= bmp.override_dry;

    invalid_parameters        |= dcs.readParams( arguments, output_r );
    enable_global_dry_default |= dcs.override_dry;

    invalid_parameters        |= font.readParams( arguments, output_r );
    enable_global_dry_default |= font.override_dry;

    invalid_parameters        |= fun.readParams( arguments, output_r );
    enable_global_dry_default |= fun.override_dry;

    invalid_parameters        |= msic.readParams( arguments, output_r );
    enable_global_dry_default |= msic.override_dry;

    invalid_parameters        |= net.readParams( arguments, output_r );
    enable_global_dry_default |= net.override_dry;

    invalid_parameters        |= obj.readParams( arguments, output_r );
    enable_global_dry_default |= obj.override_dry;

    invalid_parameters        |= ptc.readParams( arguments, output_r );
    enable_global_dry_default |= ptc.override_dry;

    invalid_parameters        |= pyr.readParams( arguments, output_r );
    enable_global_dry_default |= pyr.override_dry;

    invalid_parameters        |= rpns.readParams( arguments, output_r );
    enable_global_dry_default |= rpns.override_dry;

    invalid_parameters        |= snds.readParams( arguments, output_r );
    enable_global_dry_default |= snds.override_dry;

    invalid_parameters        |= til.readParams( arguments, output_r );
    enable_global_dry_default |= til.override_dry;

    invalid_parameters        |= wav.readParams( arguments, output_r );
    enable_global_dry_default |= wav.override_dry;

    bool has_dry;

    if( singleArgument( arguments, "--dry", output_r, has_dry ) ) {
        if( has_dry ) {
            // If enable_global_dry_default is turned on by resource.override_dry. Then, give
            // out an error.
            if( enable_global_dry_default ) {
                invalid_parameters |= true;

                if( output_r != nullptr ) {
                    *output_r << "--dry is not allowed to be used with *_ENABLE parameter!" << std::endl;
                }
            }

            enable_global_dry_default = true;
        }
    }


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

        invalid_parameters |= true;
    }

    return !invalid_parameters;
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

    return true; // Successfully executed.
}

bool IFFOptions::ResourceOption::readParams( std::map<std::string, std::vector<std::string>> &arguments, std::ostream *output_r ) {
    auto enable_option = arguments.find("--" + getNameSpace() + "_ENABLE");

    if( enable_option != arguments.end() ) {

        if( hasUnexpectedParams( enable_option->first, enable_option->second, output_r ) ) {
            return true; // Cancel due to irrecoverable error.
        }

        // Do not want to make the program think there is an unrecognized command.
        arguments.erase( enable_option );

        override_dry = true;
    }

    return false;
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
