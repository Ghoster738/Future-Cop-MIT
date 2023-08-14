#include "../../../Utilities/Options/Parameters.h"
#include <vector>
#include <string>

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
    output << "help       =  " << parameters.help.getValue()       << ", modified = " << parameters.help.wasModified() << "\n";
    output << "fullScreen =  " << parameters.fullScreen.getValue() << ", modified = " << parameters.fullScreen.wasModified() << "\n";
    output << "resWidth   =  " << parameters.resWidth.getValue()   << ", modified = " << parameters.resWidth.wasModified() << "\n";
    output << "resHeight  =  " << parameters.resHeight.getValue()  << ", modified = " << parameters.resHeight.wasModified() << "\n";
    output << "configPath =  " << parameters.configPath.getValue() << ", modified = " << parameters.configPath.wasModified() << "\n";
    output << "userDir    =  " << parameters.userDir.getValue()    << ", modified = " << parameters.userDir.wasModified() << "\n";
    output << "dataDir    =  " << parameters.dataDir.getValue()    << ", modified = " << parameters.dataDir.wasModified() << std::endl;
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

        found_problem |= testModParam( found_problem, default_parameters.help,       "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.fullScreen, "FullScreen", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.resWidth,   "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.resHeight,  "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.configPath, "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.userDir,    "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.dataDir,    "DataDir",    test_name, std::cout );

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

        found_problem |= testModParam( found_problem, default_parameters.help,       "Help",       test_name, std::cout );
        if( found_problem == 0 && default_parameters.fullScreen.wasModified() && default_parameters.fullScreen.getValue() != true) {
            std::cout << "Error: FullScreen was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.resWidth,   "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.resHeight,  "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.configPath, "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.userDir,    "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.dataDir,    "DataDir",    test_name, std::cout );

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

        found_problem |= testModParam( found_problem, default_parameters.help,       "Help",       test_name, std::cout );
        if( found_problem == 0 && default_parameters.fullScreen.wasModified() && default_parameters.fullScreen.getValue() != false) {
            std::cout << "Error: FullScreen was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.resWidth,   "Width",      test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.resHeight,  "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.configPath, "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.userDir,    "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.dataDir,    "DataDir",    test_name, std::cout );

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

        found_problem |= testModParam( found_problem, default_parameters.help,       "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.fullScreen, "FullScreen", test_name, std::cout );
        if( found_problem == 0 && default_parameters.resWidth.wasModified() && default_parameters.resWidth.getValue() != 720) {
            std::cout << "Error: Width was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.resHeight,  "Height",     test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.configPath, "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.userDir,    "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.dataDir,    "DataDir",    test_name, std::cout );

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

        found_problem |= testModParam( found_problem, default_parameters.help,       "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.fullScreen, "FullScreen", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.resWidth,   "Width",      test_name, std::cout );
        if( found_problem == 0 && default_parameters.resHeight.wasModified() && default_parameters.resHeight.getValue() != 480) {
            std::cout << "Error: Width was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.configPath, "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.userDir,    "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.dataDir,    "DataDir",    test_name, std::cout );

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

        found_problem |= testModParam( found_problem, default_parameters.help,       "Help",       test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.fullScreen, "FullScreen", test_name, std::cout );
        if( found_problem == 0 && default_parameters.resWidth.wasModified() && default_parameters.resWidth.getValue() != 720) {
            std::cout << "Error: Width was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        if( found_problem == 0 && default_parameters.resHeight.wasModified() && default_parameters.resHeight.getValue() != 480) {
            std::cout << "Error: Height was not modified or set properly in \"" << test_name << "\" case when it should of.\n";

            found_problem |= 1;
        }
        found_problem |= testModParam( found_problem, default_parameters.configPath, "ConfigPath", test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.userDir,    "UserDir",    test_name, std::cout );
        found_problem |= testModParam( found_problem, default_parameters.dataDir,    "DataDir",    test_name, std::cout );

        if( found_problem ) {
            displayParameters( default_parameters, std::cout );
            problem |= 1;
        }
    }
    
    return problem;
}
