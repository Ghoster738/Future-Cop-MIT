#include "Data/Mission/IFF.h"
#include "Utilities/DataHandler.h"
#include <iostream>
#include <fstream>
#include "Config.h"
#include "Data/Mission/BMPResource.h"
#include "Data/Mission/Til/Mesh.h"

namespace {
    std::string INPUT_OPERATION = "-i";
    std::string OUTPUT_OPERATION = "-o";
    std::string RAW_OUTPUT_OPERATION = "-r";
    std::string DECODE_OUTPUT_OPERATION = "-d";
    std::string COMPARE_OUTPUT_OPERATION = "-c";
    std::string OUTPUT_HELP_OPERATION = "-h";

    void help_output( std::ostream& stream ) {
        
        stream << "\n";
        stream << "Future Cop: MIT - Mission reader (version " << FUTURE_COP_MIT_VERSION << ")\n";
        stream << "\n";
        stream << "Usage:" << "\n";
        stream << "  FCMissionReader [-h] [-i <path>] [-o <path>] [-c] [-d] [-r] " << "\n";
        stream << "\n";
        stream << "Options:" << "\n";
        stream << "  -h         Display this help screen" << "\n";
        stream << "  -i <path>  File path for input, up to two inputs are supported" << "\n";
        stream << "  -o <path>  Path to the folder or file for the outputs" << "\n";
        stream << "  -r         Export raws" << "\n";
        stream << "  -d         Export supported and decoded files" << "\n";
        stream << "  -c         Determine and write the similarities between two inputs" << "\n";
        stream << "Decoding [ -d ] Options:" << "\n";
        stream << "  --DRY      Do not export any decoded files. Do not use with ENABLE commands." << "\n";
        stream << "  --*_ENABLE This sets sepecific resources to be exported rather than decoding them all." << "\n";
        stream << " ACT Options:" << "\n";
        stream << "  --ACT_ENABLE  Set these resources to be explicitly exported." << "\n";
        stream << " ANM Options:" << "\n";
        stream << "  --ANM_ENABLE  Set these resources to be explicitly exported." << "\n";
        stream << "  --ANM_PALETTE Display this help screen" << "\n";
    }
}

int main( int argc, char *argv[] ) {
    // Data::Mission::Til::Mesh::loadMeshScript( "./tile_set.json", nullptr );

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
                    std::cout << "Input Error: you exceeded the limit of two input Mission files." << std::endl;
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
                    mission_file[0].exportAllResources( output_folder_path, false, extra_commands );
                else
                {
                    // int resource_index = std::stoi( input.substr( 3, input.length() - 3 ) );

                    // mission_file[0].exportSingleResource( resource_index, output_folder_path.c_str(), false, extra_commands );
                }
            }
            else
            if( input.find(DECODE_OUTPUT_OPERATION) == 0 ) {
                if( input.length() == 2 )
                    mission_file[0].exportAllResources( output_folder_path, true, extra_commands );
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
