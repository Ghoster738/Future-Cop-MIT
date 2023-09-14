#include "Utilities/Options/Parameters.h"
#include "Utilities/Options/Paths.h"
#include "Utilities/Options/Options.h"
#include "Utilities/Logger.h"

#include "Data/Mission/ACT/Unknown.h"
#include "Data/Manager.h"

#include <iostream>

namespace {

std::string header( std::string camel_case, std::string snake_case, const std::vector<std::string> &structure ) {
    std::stringstream stream;

    stream << "#ifndef DATA_MISSION_" << snake_case << "_HEADER\n";
    stream << "#define DATA_MISSION_" << snake_case << "_HEADER\n\n";

    stream << "#include \"../ACTManager.h\"\n";
    stream << "#include <json/json.h>\n\n";

    stream << "namespace Data {\n\n";

    stream << "namespace Mission {\n\n";

    stream << "namespace ACT {\n\n";

    stream << "class " << camel_case << " : public ACTResource {\n";
    stream << "public:\n";
    stream << "    static uint_fast8_t TYPE_ID;\n\n";

    stream << "    struct Internal {\n";

    for( std::string data_type : structure ) {
        size_t end = data_type.find( ';' );

        if( end != std::string::npos )
            data_type = data_type.substr( 0, end + 1 );

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

    stream << "    virtual ACTResource* duplicate( const ACTResource &original ) const;\n\n";

    stream << "    Internal getInternal() const;\n\n";

    stream << "    static std::vector<" << camel_case << "*> getVector( Data::Mission::ACTManager& act_manager );\n\n";
    stream << "    static const std::vector<" << camel_case << "*> getVector( const Data::Mission::ACTManager& act_manager );\n\n";

    stream << "};\n";

    stream << "}\n\n"; // ACT

    stream << "}\n\n"; // Mission

    stream << "}\n\n"; // Data

    stream << "#endif // DATA_MISSION_" << snake_case << "_HEADER\n";

    return stream.str();
}

std::string code( uint16_t type_id, std::string camel_case, const std::vector<std::string> &structure ) {
    std::stringstream stream;

    stream << "#include \"" << camel_case << ".h\"\n\n";

    stream << "#include <cassert>\n\n";

    stream << "uint_fast8_t Data::Mission::ACT::" << camel_case << "::TYPE_ID = " << type_id << ";\n\n";

    stream << "Json::Value Data::Mission::ACT::" << camel_case << "::makeJson() const {\n";
    stream << "    Json::Value root = Data::Mission::ACTResource::makeJson();\n";
    stream << "    const std::string NAME = getTypeIDName();\n\n";

    for( std::string data_type : structure ) {
        size_t begin = data_type.find( ' ' ) + 1;
        size_t end   = data_type.find( ';' );
        const std::string data_name = data_type.substr( begin, end - begin );

        stream << "    root[\"ACT\"][NAME][\"" << data_name << "\"] = internal." << data_name << ";\n";
    }

    stream << "\n";
    stream << "    return root;\n";
    stream << "}\n\n";

    stream << "bool Data::Mission::ACT::" << camel_case << "::readACTType( uint_fast8_t act_type, Utilities::Buffer::Reader &data_reader, Utilities::Buffer::Endian endian ) {\n";
    stream << "    assert(act_type == this->getTypeID());\n\n";

    stream << "    if( data_reader.totalSize() != this->getSize() )\n";
    stream << "        return false;\n\n";

    size_t data_size = 0;

    for( std::string data_type : structure ) {
        size_t begin = data_type.find( ' ' ) + 1;
        size_t end   = data_type.find( ';' );
        const std::string data_name = data_type.substr(begin, end - begin);

        std::string comment = "";

        if( data_type.find( "//" ) != std::string::npos )
            comment = " " + data_type.substr(data_type.find( "//" ));

        std::string type_name = data_type.substr(0, begin - 1);
        std::string read_name = "error";
        if( type_name == "uint8_t" ) {
            read_name = "readU8()";
            data_size += sizeof( uint8_t );
        }
        else
        if( type_name == "uint16_t" ) {
            read_name = "readU16( endian )";
            data_size += sizeof( uint16_t );
        }
        else
        if( type_name == "uint32_t" ) {
            read_name = "readU32( endian )";
            data_size += sizeof( uint32_t );
        }
        stream << "    internal." << data_name << " = data_reader." << read_name << ";" << comment << "\n";
    }

    stream << "\n";
    stream << "    return true;\n";
    stream << "}\n\n";

    stream << "Data::Mission::ACT::" << camel_case << "::" << camel_case << "() {}\n\n";

    stream << "Data::Mission::ACT::" << camel_case << "::" << camel_case << "( const ACTResource& obj ) : ACTResource( obj ) {}\n\n";

    stream << "Data::Mission::ACT::" << camel_case << "::" << camel_case << "( const " << camel_case << "& obj ) : ACTResource( obj ), internal( obj.internal ) {}\n\n";

    stream << "uint_fast8_t Data::Mission::ACT::" << camel_case << "::getTypeID() const {\n";
    stream << "    return TYPE_ID;\n";
    stream << "}\n\n";

    stream << "std::string Data::Mission::ACT::" << camel_case << "::getTypeIDName() const {\n";
    stream << "    return \"" << camel_case << "\";\n";
    stream << "}\n\n";

    stream << "size_t Data::Mission::ACT::" << camel_case << "::getSize() const {\n";
    stream << "    return " << data_size << "; // bytes\n";
    stream << "}\n\n";

    stream << "bool Data::Mission::ACT::" << camel_case << "::checkRSL() const { return false; }\n\n";

    stream << "Data::Mission::Resource* Data::Mission::ACT::" << camel_case << "::duplicate() const {\n";
    stream << "    return new Data::Mission::ACT::" << camel_case << "( *this );\n";
    stream << "}\n\n";

    stream << "Data::Mission::ACTResource* Data::Mission::ACT::" << camel_case << "::duplicate( const ACTResource &original ) const {\n";
    stream << "    auto copy_r = dynamic_cast<const " << camel_case << "*>( &original );\n\n";

    stream << "    if( copy_r != nullptr )\n";
    stream << "        return new Data::Mission::ACT::" << camel_case << "( *copy_r );\n";
    stream << "    else\n";
    stream << "        return new Data::Mission::ACT::" << camel_case << "( original );\n";
    stream << "}\n\n";

    stream << "Data::Mission::ACT::" << camel_case << "::Internal Data::Mission::ACT::" << camel_case << "::getInternal() const {\n";
    stream << "    return internal;\n";
    stream << "}\n\n";

    stream << "std::vector<Data::Mission::ACT::" << camel_case << "*> Data::Mission::ACT::" << camel_case << "::getVector( Data::Mission::ACTManager& act_manager ) {\n";
    stream << "    std::vector<ACTResource*> to_copy = act_manager.getACTs( Data::Mission::ACT::" << camel_case << "::TYPE_ID );\n\n";

    stream << "    std::vector<" << camel_case << "*> copy;\n\n";

    stream << "    copy.reserve( to_copy.size() );\n\n";

    stream << "    for( auto it = to_copy.begin(); it != to_copy.end(); it++ )\n";
    stream << "        copy.push_back( dynamic_cast<" << camel_case << "*>( (*it) ) );\n\n";

    stream << "    return copy;\n";
    stream << "}\n\n";

    stream << "const std::vector<Data::Mission::ACT::" << camel_case << "*> Data::Mission::ACT::" << camel_case << "::getVector( const Data::Mission::ACTManager& act_manager ) {\n";
    stream << "    return Data::Mission::ACT::" << camel_case << "::getVector( const_cast< Data::Mission::ACTManager& >( act_manager ) );\n";
    stream << "}";

    return stream.str();
}

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

    const size_t MAP_AMOUNT = Data::Manager::AMOUNT_OF_IFF_IDS;
    const auto MAPS = Data::Manager::map_iffs;

    for( size_t i = 0; i < MAP_AMOUNT; i++ ) {
        const std::string resource_identifier = *MAPS[ i ];

        auto entry = manager.getIFFEntry( resource_identifier );
        entry.importance = Data::Manager::Importance::NEEDED;

        if( !manager.setIFFEntry( resource_identifier, entry ) ) {
            {
                auto log = Utilities::logger.getLog( Utilities::Logger::ERROR );
                log.output << "Set IFF Entry has failed for \"" + resource_identifier + "\".";
            }
            return 0;
        }
    }

    manager.setLoad( Data::Manager::Importance::NEEDED );

    std::vector<const Data::Mission::IFF*> little_endian;
    std::vector<const Data::Mission::IFF*>    big_endian;

    for( size_t i = 0; i < MAP_AMOUNT; i++ ) {
        const std::string resource_identifier = *MAPS[ i ];

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

        little_endian.push_back( little_endian_r );
        big_endian.push_back(       big_endian_r );
    }


    uint16_t number = 1;

    while( number != 0 ) {
        std::cout << "Please enter the actor identifier number. Enter 0 to exit" << std::endl;
        std::cin >> number;

        if( number != 0 ) {
            std::cout << "Using number " << number << "\n";

            std::string snake_case_name = "ACTOR_ID_";
            snake_case_name += std::to_string( number );

            std::cout << "snake_case_name = " << snake_case_name << "\n";

            std::cout << "Please enter the the class name for this actor." << std::endl;
            std::string camel_case_name;
            std::cin >> camel_case_name;

            auto structure = Data::Mission::ACT::Unknown::getStructure( number, little_endian, big_endian );

            if( structure.empty() || structure.back().find( "ERROR:" ) == 0 )
                std::cout << "Cannot create file " << structure.back() << "\n";
            else {

                std::ofstream header_file;
                header_file.open( camel_case_name + ".h", std::ios::out );

                if( header_file.is_open() ) {
                    header_file << header( camel_case_name, snake_case_name, structure);

                    header_file.close();

                    std::cout << "Created header file \"" << camel_case_name << ".h\"\n";
                }
                else
                    std::cout << "Failed to create header file \"" << camel_case_name << ".h\"\n";

                std::ofstream code_file;
                code_file.open( camel_case_name + ".cpp", std::ios::out );

                if( code_file.is_open() ) {
                    code_file << code( number, camel_case_name, structure);

                    code_file.close();

                    std::cout << "Created code file \"" << camel_case_name << ".cpp\"\n";
                }
                else
                    std::cout << "Failed to create code file \"" << camel_case_name << ".cpp\"\n";
            }
        }
    }

    return 0;
}
