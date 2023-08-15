#include "../../../Utilities/Options/Parameters.h"
#include <vector>
#include <string>
#include <iostream>

namespace {

class ParametersGiven {
private:
    std::string argument_table;
    std::vector<size_t> argument_offsets;

    bool regenerate;
    std::vector<char*> argument_pointers;
public:
    ParametersGiven( const std::vector<std::string> &arguments = {} ) {
        addArguments( arguments );
    }

    void addArgument( const std::string &argument ) {
        this->argument_offsets.push_back( this->argument_table.size() );
        argument_table += argument;
        argument_table += '\0';
        regenerate = true;
    }

    void addArguments( const std::vector<std::string> &arguments ) {
        for( auto i : arguments ) {
            addArgument( i );
        }
    }

    int getParamAmount() const {
        return this->argument_offsets.size();
    }

    char** getParamPointers() {
        if( regenerate ) {
            argument_pointers.clear();

            for( auto offset : argument_offsets ) {
                argument_pointers.push_back( const_cast<char*>( argument_table.c_str() ) + offset );
            }

            regenerate = false;
        }

        return argument_pointers.data();
    }
};

void displayParameters( const Utilities::Options::Parameters &parameters, std::ostream &output ) {
    output << "help         = " << parameters.help.getValue()         << ", modified = " << parameters.help.wasModified() << "\n";
    output << "full_screen  = " << parameters.full_screen.getValue()  << ", modified = " << parameters.full_screen.wasModified() << "\n";
    output << "res_width    = " << parameters.res_width.getValue()    << ", modified = " << parameters.res_width.wasModified() << "\n";
    output << "res_height   = " << parameters.res_height.getValue()   << ", modified = " << parameters.res_height.wasModified() << "\n";
    output << "config_path  = " << parameters.config_path.getValue()  << ", modified = " << parameters.config_path.wasModified() << "\n";
    output << "user_dir     = " << parameters.user_dir.getValue()     << ", modified = " << parameters.user_dir.wasModified() << "\n";
    output << "win_data_dir = " << parameters.win_data_dir.getValue() << ", modified = " << parameters.win_data_dir.wasModified() << "\n";
    output << "mac_data_dir = " << parameters.mac_data_dir.getValue() << ", modified = " << parameters.mac_data_dir.wasModified() << "\n";
    output << "psx_data_dir = " << parameters.psx_data_dir.getValue() << ", modified = " << parameters.psx_data_dir.wasModified() << std::endl;
}

template<class I>
int testModParam( int status, const I &param, const std::string &name, const std::string &test_name, std::ostream &output ) {
    if( status == 0 && param.wasModified() ) {
        output << "Error: " << name << " was modified in \"" << test_name << "\" case when it should not of.\n";

        return 1;
    }
    else
        return 0;
}

}

int main( int argc, char *argv[] ) {
    int problem = 0;

    std::string program_name = "No Name";

    if( argc > 0 )
        program_name = argv[0];

    {
        std::string param[] = {"LOL", "lolip"};

        ParametersGiven parameters( {program_name, param[0], param[1]} );

        if( parameters.getParamPointers() == nullptr ) {
            std::cout << "parameters.getParamPointers() should not be null." << std::endl;
            problem = 1;
        }
        if( parameters.getParamAmount() != 3 ) {
            std::cout << "parameters.getParamAmount() should be " << 3 << " not " << parameters.getParamAmount() << "." << std::endl;
            problem = 1;
        }

        if( parameters.getParamPointers() != nullptr ) {
            if( std::string( parameters.getParamPointers()[0] ) != program_name ) {
                std::cout << "parameters.getParamPointers()[0] should be \"" << program_name << "\" not \"" << parameters.getParamPointers()[0] << "\"." << std::endl;
                problem = 1;
            }
            if( std::string( parameters.getParamPointers()[1] ) != param[0] ) {
                std::cout << "parameters.getParamPointers()[1] should be \"" << param[0] << "\" not \"" << parameters.getParamPointers()[1] << "\"." << std::endl;
                problem = 1;
            }
            if( std::string( parameters.getParamPointers()[2] ) != param[1] ) {
                std::cout << "parameters.getParamPointers()[2] should be \"" << param[1] << "\" not \"" << parameters.getParamPointers()[2] << "\"." << std::endl;
                problem = 1;
            }
        }
    }

    {
        std::string test_name = "no parameters";
        ParametersGiven no_name_parameters;

        Utilities::Options::Parameters default_parameters;
        default_parameters.getParameters( no_name_parameters.getParamAmount(), no_name_parameters.getParamPointers(), std::cout );

        int found_problem = 0;

        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.full_screen,  "FullScreen", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_width,    "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_height,   "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.config_path,  "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.user_dir,     "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.win_data_dir, "WinDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "valid fullscreen";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--fullscreen" );

        Utilities::Options::Parameters default_parameters;
        default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );

        int found_problem = 0;

        if( found_problem == 0 && default_parameters.full_screen.wasModified() && default_parameters.full_screen.getValue() != true) {
            std::cout << "Error: FullScreen was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_width,    "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_height,   "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.config_path,  "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.user_dir,     "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.win_data_dir, "WinDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "valid windowed";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--window" );

        Utilities::Options::Parameters default_parameters;
        default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );

        int found_problem = 0;

        if( found_problem == 0 && default_parameters.full_screen.wasModified() && default_parameters.full_screen.getValue() != false) {
            std::cout << "Error: FullScreen was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_width,    "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_height,   "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.config_path,  "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.user_dir,     "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.win_data_dir, "WinDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "--width valid test";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--width" );
        parameters.addArgument( "720" );

        Utilities::Options::Parameters default_parameters( parameters.getParamAmount(), parameters.getParamPointers() );

        int found_problem = 0;

        if( found_problem == 0 && default_parameters.res_width.wasModified() && default_parameters.res_width.getValue() != 720) {
            std::cout << "Error: Width was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.full_screen,  "FullScreen", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_height,   "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.config_path,  "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.user_dir,     "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.win_data_dir, "WinDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "--height valid test";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--height" );
        parameters.addArgument( "480" );

        Utilities::Options::Parameters default_parameters( parameters.getParamAmount(), parameters.getParamPointers() );

        int found_problem = 0;

        if( found_problem == 0 && default_parameters.res_height.wasModified() && default_parameters.res_height.getValue() != 480) {
            std::cout << "Error: Width was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.full_screen,  "FullScreen", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_width,    "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.config_path,  "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.user_dir,     "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.win_data_dir, "WinDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "--res valid test";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--res" );
        parameters.addArgument( "720x480" );

        Utilities::Options::Parameters default_parameters( parameters.getParamAmount(), parameters.getParamPointers() );

        int found_problem = 0;

        if( found_problem == 0 && default_parameters.res_width.wasModified() && default_parameters.res_width.getValue() != 720) {
            std::cout << "Error: Width was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        if( found_problem == 0 && default_parameters.res_height.wasModified() && default_parameters.res_height.getValue() != 480) {
            std::cout << "Error: Height was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.full_screen,  "FullScreen", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.config_path,  "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.user_dir,     "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.win_data_dir, "WinDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "--res valid test";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--res" );
        parameters.addArgument( "720x480" );
        parameters.addArgument( "--fullscreen" );

        Utilities::Options::Parameters default_parameters( parameters.getParamAmount(), parameters.getParamPointers() );

        int found_problem = 0;

        if( found_problem == 0 && default_parameters.res_width.wasModified() && default_parameters.res_width.getValue() != 720) {
            std::cout << "Error: Width was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        if( found_problem == 0 && default_parameters.res_height.wasModified() && default_parameters.res_height.getValue() != 480) {
            std::cout << "Error: Height was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        if( found_problem == 0 && default_parameters.full_screen.wasModified() && default_parameters.full_screen.getValue() != true) {
            std::cout << "Error: FullScreen was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.config_path,  "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.user_dir,     "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.win_data_dir, "WinDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "--config valid";
        std::string path = "./";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--config" );
        parameters.addArgument( path ); // In linux this will always be valid.

        Utilities::Options::Parameters default_parameters( parameters.getParamAmount(), parameters.getParamPointers() );

        int found_problem = 0;

        if( found_problem == 0 && default_parameters.config_path.wasModified() && default_parameters.config_path.getValue() != path ) {
            std::cout << "Error: ConfigPath was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.full_screen,  "FullScreen", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_width,    "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_height,   "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.user_dir,     "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.win_data_dir, "WinDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "--user valid";
        std::string path = "./";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--user" );
        parameters.addArgument( path ); // In linux this will always be valid.

        Utilities::Options::Parameters default_parameters( parameters.getParamAmount(), parameters.getParamPointers() );

        int found_problem = 0;

        if( found_problem == 0 && default_parameters.user_dir.wasModified() && default_parameters.user_dir.getValue() != path ) {
            std::cout << "Error: UserDir was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.full_screen,  "FullScreen", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_width,    "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_height,   "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.config_path,  "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.win_data_dir, "WinDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "--data valid";
        std::string path = "./";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--data" );
        parameters.addArgument( path ); // In linux this will always be valid.

        Utilities::Options::Parameters default_parameters( parameters.getParamAmount(), parameters.getParamPointers() );

        int found_problem = 0;

        if( found_problem == 0 && default_parameters.win_data_dir.wasModified() && default_parameters.win_data_dir.getValue() != path ) {
            std::cout << "Error: WinDataDir was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.help,         "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.full_screen,  "FullScreen", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_width,    "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.res_height,   "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.config_path,  "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.user_dir,     "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.mac_data_dir, "MacDataDir", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.psx_data_dir, "PSXDataDir", test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }

    {
        std::string test_name = "missing name case";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--missingname" );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ) != "unsupported option specified in commandline, use --help to list valid options" ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "multi fullscreen";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--fullscreen" );
        parameters.addArgument( "--fullscreen" );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ) != "multiple fullscreen and/or window mode parameters specified in commandline" ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "multi window";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--window" );
        parameters.addArgument( "--window" );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ) != "multiple fullscreen and/or window mode parameters specified in commandline" ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "multi fullscreen to window";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--fullscreen" );
        parameters.addArgument( "--window" );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ) != "multiple fullscreen and/or window mode parameters specified in commandline" ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "multi window to fullscreen";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--window" );
        parameters.addArgument( "--fullscreen" );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ) != "multiple fullscreen and/or window mode parameters specified in commandline" ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "lesser width";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--width" );
        parameters.addArgument( "10" );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ).find( "resolution width must be at least" ) != 0 ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "multi width";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--width" );
        parameters.addArgument( "600" );
        parameters.addArgument( "--width" );
        parameters.addArgument( "600" );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ).find( "multiple width/resolution parameters specified in commandline" ) != 0 ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "bad width";
        ParametersGiven parameters;

        std::string parameter_value = "10$";

        parameters.addArgument( program_name );
        parameters.addArgument( "--width" );
        parameters.addArgument( parameter_value );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ) != "invalid width value \"" + parameter_value + "\" specified in commandline" ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "lesser height";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--height" );
        parameters.addArgument( "10" );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ).find( "resolution height must be at least" ) != 0 ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "multi height";
        ParametersGiven parameters;

        parameters.addArgument( program_name );
        parameters.addArgument( "--height" );
        parameters.addArgument( "600" );
        parameters.addArgument( "--height" );
        parameters.addArgument( "600" );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ).find( "multiple height/resolution parameters specified in commandline" ) != 0 ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string test_name = "bad height";
        ParametersGiven parameters;

        std::string parameter_value = "10$";

        parameters.addArgument( program_name );
        parameters.addArgument( "--height" );
        parameters.addArgument( parameter_value );

        try {
            Utilities::Options::Parameters default_parameters;
            default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
            problem |= 1;
            std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
        }
        catch( std::invalid_argument arg )
        {
            if( std::string( arg.what() ) != "invalid height value \"" + parameter_value + "\" specified in commandline" ) {
                std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                problem |= 1;
            }
        }
    }

    {
        std::string header_test_name = "bad dim ";

        std::vector<std::string> combo = { "A720x600", "720Ax600", "720xA600", "720x600A", "720X600"};

        for( auto i : combo ) {
            std::string test_name = header_test_name + i;

            ParametersGiven parameters;

            parameters.addArgument( program_name );
            parameters.addArgument( "--res" );
            parameters.addArgument( i );

            try {
                Utilities::Options::Parameters default_parameters;
                default_parameters.getParameters( parameters.getParamAmount(), parameters.getParamPointers(), std::cout );
                problem |= 1;
                std::cout << "\"" << test_name << "\" should of caught the error by now." << std::endl;
            }
            catch( std::invalid_argument arg )
            {
                if( std::string( arg.what() ) != "invalid resolution parameter specified in commandline" ) {
                    std::cout << arg.what() << "\n This output is invalid for \"" << test_name << "\"" << std::endl;
                    problem |= 1;
                }
            }
        }
    }
    
    return problem;
}
