#include "Data/Mission/IFF.h"
#include "Utilities/DataHandler.h"
#include "Utilities/QuiteOkImage.h"
#include <iostream>
#include <fstream>
#include "Data/Mission/BMPResource.h"
#include "Data/Mission/Til/Mesh.h"

namespace {
    std::string  INPUT_OPERATION = "-i";
    std::string OUTPUT_OPERATION = "-o";
    std::string RAW_OUTPUT_OPERATION = "-r";
    std::string DECODE_OUTPUT_OPERATION = "-d";
    std::string COMPARE_OUTPUT_OPERATION = "-c";
    std::string OUTPUT_HELP_OPERATION = "-h";

    void help_output( std::ostream& out ) {
        out << std::endl
            << "This program was created to read and decode mission files" << std::endl
            << "\t" << INPUT_OPERATION  << " - file_path for the input for almost one case you will need one."
            << std::endl
            << "\t" << OUTPUT_OPERATION << " - The path to the folder or file for the outputs." << std::endl
            << "\t" << RAW_OUTPUT_OPERATION << " - An export command for the raws." << std::endl
            << "\t" << DECODE_OUTPUT_OPERATION << " - An export command for the supported and decoded files."
            << std::endl
            << "\t" << COMPARE_OUTPUT_OPERATION << " - This tells or writes the simularities between two inputs."
            << std::endl
            << "\t" << OUTPUT_HELP_OPERATION << " - Displays this very list." << std::endl << std::endl;
    }
}

int main( int argc, char *argv[] ) {
    Data::Mission::Til::Mesh::loadMeshScript( "./tile_set.json", nullptr );

    Data::Mission::IFF mission_file[2]; // Two mission files at a time for now.
    std::string output_folder_path = "./output/";
    bool custom_output = false;
    int number_of_inputs = 0;
    std::vector<std::string> extra_commands;

    // Setup the data handler
    Utilities::DataHandler::init();

    if( argc == 1 )
    {
        help_output( std::cout );
    }
    else
    {
        for( int i = 1; i < argc; i++ )
        {
            std::string input( argv[ i ] );

            if( INPUT_OPERATION.compare( input ) == 0 )
                if( number_of_inputs < 2 )
                    mission_file[ number_of_inputs++ ].open( argv[++i] );
                else
                {
                    std::cout << "Input Error: you exceed the limit of two input Mission files." << std::endl;
                    i = argc;
                }
            else
            if( OUTPUT_OPERATION.compare( input ) == 0 ) {
                output_folder_path = argv[ ++i ];
                custom_output = true;
            }
            else
            if( input.find(RAW_OUTPUT_OPERATION) == 0 ) {
                if( input.length() == 2 )
                    mission_file[0].exportAllResources( output_folder_path.c_str(), false, extra_commands );
                else
                {
                    // int resource_index = std::stoi( input.substr( 3, input.length() - 3 ) );

                    // mission_file[0].exportSingleResource( resource_index, output_folder_path.c_str(), false, extra_commands );
                }
            }
            else
            if( input.find(DECODE_OUTPUT_OPERATION) == 0 ) {
                if( input.length() == 2 )
                    mission_file[0].exportAllResources( output_folder_path.c_str(), true, extra_commands );
                else
                {
                    // int resource_index = std::stoi( input.substr( 3, input.length() - 3 ) );

                    // mission_file[0].exportSingleResource( resource_index, output_folder_path.c_str(), true, extra_commands );
                }
            }
            else
            if( COMPARE_OUTPUT_OPERATION.compare( input ) == 0 ) {
                if( number_of_inputs < 2 ) {
                    std::cout << "Input Error: only two inputs are allowed not three more." << std::endl;
                    i = argc;
                }
                else
                if( custom_output ) {
                    std::ofstream resource;

                    resource.open( output_folder_path, std::ios::out );

                    if( resource.is_open() )
                    {
                        mission_file[0].compare( mission_file[1], resource );

                        resource.close();
                    }
                }
                else
                    mission_file[0].compare( mission_file[1], std::cout );
            }
            else
            if( OUTPUT_HELP_OPERATION.compare( input ) == 0 ) {
                help_output( std::cout );
            }
            else
            {
                extra_commands.push_back( input );
            }
        }
    }

    return 0;
}
