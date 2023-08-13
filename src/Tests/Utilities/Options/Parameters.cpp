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

}

int main( int argc, char *argv[] ) {
    int problem = 0;

    std::string name = "No Name";

    if( argc > 0 )
        name = argv[0];

    {
        std::string param[] = {"LOL", "lolip"};

        ParametersGiven parameters( {name, param[0], param[1]} );

        if( parameters.getParamPointers() == nullptr ) {
            std::cout << "parameters.getParamPointers() should not be null." << std::endl;
            problem = 1;
        }
        if( parameters.getParamAmount() != 3 ) {
            std::cout << "parameters.getParamAmount() should be " << 3 << " not " << parameters.getParamAmount() << "." << std::endl;
            problem = 1;
        }

        if( parameters.getParamPointers() != nullptr ) {
            if( std::string( parameters.getParamPointers()[0] ) != name ) {
                std::cout << "parameters.getParamPointers()[0] should be \"" << name << "\" not \"" << parameters.getParamPointers()[0] << "\"." << std::endl;
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
    ParametersGiven no_name_parameters();

    Utilities::Options::Parameters default_parameters( no_name_parameters.getParamAmount(), no_name_parameters.getParamPointers() );
    
    return problem;
}
