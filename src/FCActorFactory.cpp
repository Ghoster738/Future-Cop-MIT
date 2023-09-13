#include "Utilities/Options/Parameters.h"
#include "Utilities/Options/Paths.h"
#include "Utilities/Options/Options.h"
#include "Utilities/Logger.h"

#include "Data/Mission/ACT/Unknown.h"
#include "Data/Manager.h"

#include <iostream>

std::string header( std::string camel_case, std::string snake_case, const std::vector<std::string> &structure ) {
    std::stringstream stream;

    stream << "#ifndef MISSION_ACT_TYPE_" << snake_case << "_HEADER\n";
    stream << "#define MISSION_ACT_TYPE_" << snake_case << "_HEADER\n\n";

    stream << "#include \"../ACTManager.h\"\n";
    stream << "#include <json/json.h>\n";

    stream << "namespace Data {\n\n";

    stream << "namespace Mission {\n\n";

    stream << "namespace ACT {\n\n";

    stream << "class " << camel_case << " : public ACTResource {\n";
    stream << "public:\n";
    stream << "    static uint_fast8_t TYPE_ID;\n\n";

    stream << "    struct Internal {\n";

    for( std::string data_type : structure ) {
        stream << "        " << data_type << "\n";
    }

    stream << "    } internal;\n\n";

    stream << "protected:\n";
    stream << "    virtual Json::Value makeJson() const;\n";
    stream << "    virtual bool readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian );\n\n";

    stream << "public:\n";
    stream << "    " << camel_case << "();\n";
    stream << "    " << camel_case << "( const ACTResource& obj );\n";
    stream << "    " << camel_case << "( const " << camel_case << "& obj );\n\n";

    stream << "    virtual uint_fast8_t getTypeID() const;\n";
    stream << "    virtual std::string getTypeIDName() const;\n\n";

    stream << "    virtual size_t getSize() const;\n\n";

    stream << "    virtual bool checkRSL() const;\n\n";

    stream << "    virtual Resource* duplicate() const;\n\n";

    stream << "    ACTResource* duplicate( const ACTResource &original ) const;\n\n";

    stream << "    Internal getInternal() const;\n\n";

    stream << "    static std::vector<" << camel_case << "*> getVector( Data::Mission::ACTManager& act_manager );\n\n";
    stream << "    static const std::vector<" << camel_case << "*> getVector( const Data::Mission::ACTManager& act_manager );\n\n";

    stream << "};\n";

    stream << "}\n\n"; // ACT

    stream << "}\n\n"; // Mission

    stream << "}\n\n"; // Data

    stream << "#endif\n";

    return stream.str();
}

int main(int argc, char** argv)
{
    // Read the parameter system.
    Utilities::Options::Parameters parameters( 1, argv );
    Utilities::Options::Paths      paths( parameters );
    Utilities::Options::Options    options( paths, parameters );

    Utilities::logger.setOutputLog( &std::cout, 0, Utilities::Logger::INFO );

    {
        auto initialize_log = Utilities::logger.getLog( Utilities::Logger::ALL );
        initialize_log.output << parameters.getBinaryName() << " started at " << Utilities::Logger::getTime();
    }

    Utilities::logger.setTimeStampMode( true );

    Data::Manager manager;
    manager.autoSetEntries( options.getWindowsDataDirectory(),     Data::Manager::Platform::WINDOWS );
    manager.autoSetEntries( options.getMacintoshDataDirectory(),   Data::Manager::Platform::MACINTOSH );
    manager.autoSetEntries( options.getPlaystationDataDirectory(), Data::Manager::Platform::PLAYSTATION );

    manager.togglePlatform( Data::Manager::Platform::WINDOWS,   true );
    manager.togglePlatform( Data::Manager::Platform::MACINTOSH, true );

    const std::string resource_identifier = *Data::Manager::precinct_assault_iffs[0];

    auto entry = manager.getIFFEntry( resource_identifier );
    entry.importance = Data::Manager::Importance::NEEDED;

    if( !manager.setIFFEntry( resource_identifier, entry ) ) {
        {
            auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
            log.output << "Set IFF Entry has failed for \"" + resource_identifier + "\".";
        }
        return 0;
    }

    manager.setLoad( Data::Manager::Importance::NEEDED );

    auto little_endian_r = manager.getIFFEntry( resource_identifier ).getIFF( Data::Manager::Platform::WINDOWS );
    if( little_endian_r == nullptr ) {
        {
            auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
            log.output << "The Windows mission IFF " << resource_identifier << " did not load.";
        }
        return 0;
    }

    auto big_endian_r = manager.getIFFEntry( resource_identifier ).getIFF( Data::Manager::Platform::MACINTOSH );
    if( big_endian_r == nullptr ) {
        {
            auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
            log.output << "The Macintosh mission IFF " << resource_identifier << " did not load.";
        }
        return 0;
    }

    uint16_t number = 1;

    while( number != 0 ) {
        std::cout << "Please enter the actor identifier number. Enter 0 to exit" << std::endl;
        std::cin >> number;

        if( number != 0 ) {
            std::cout << "Please enter the actor name in CamelCase." << std::endl;
            std::string camel_case_name;
            std::cin >> camel_case_name;

            std::string captialized_snake_case_name;

            if( !camel_case_name.empty() ) {
                captialized_snake_case_name += toupper( camel_case_name[0] );

                for( size_t i = 1; i < camel_case_name.size(); i++ ) {
                    if( isupper( camel_case_name[i] ) )
                        captialized_snake_case_name += "_";

                    captialized_snake_case_name += toupper( camel_case_name[i] );
                }
            }

            std::cout << "captialized_snake_case_name = " << captialized_snake_case_name << "\n";

            std::cout << "Using number " << number << "\n";

            auto structure = Data::Mission::ACT::Unknown::getStructure( number, *little_endian_r, *big_endian_r );

            std::cout << header( camel_case_name, captialized_snake_case_name, structure) << std::endl;
        }
    }

    return 0;
}
