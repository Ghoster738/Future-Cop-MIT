#include "../../../Data/Mission/IFFOptions.h"
#include <sstream>
#include <iostream>

namespace {

bool compareBooleans( bool e, bool r, std::string field_name, std::ostream *information_r ) {
    if( e != r ) {
        if( information_r != nullptr ) {
            *information_r << "  expected." << field_name << " (" << e << ") != result." << field_name << " (" << r << ")" << std::endl;
        }

        return false;
    }
    return true;
}

bool IFFOptionCompare( const Data::Mission::IFFOptions a, const Data::Mission::IFFOptions b, std::ostream *information_r = nullptr ) {
    bool status = true;

    if( !compareBooleans( a.enable_global_dry_default, b.enable_global_dry_default, "enable_global_dry_default",information_r ) )
        status = false;
    if( !compareBooleans( a.act.override_dry, b.act.override_dry, "act.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.anm.override_dry, b.anm.override_dry, "anm.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.anm.export_palette, b.anm.export_palette, "anm.export_palette", information_r ) )
        status = false;
    if( !compareBooleans( a.bmp.override_dry, b.bmp.override_dry, "bmp.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.bmp.export_palette, b.bmp.export_palette, "bmp.export_palette", information_r ) )
        status = false;
    if( !compareBooleans( a.dcs.override_dry, b.dcs.override_dry, "dcs.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.font.override_dry, b.font.override_dry, "font.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.fun.override_dry, b.fun.override_dry, "fun.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.msic.override_dry, b.msic.override_dry, "msic.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.net.override_dry, b.net.override_dry, "net.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.net.enable_obj, b.net.enable_obj, "net.enable_obj", information_r ) )
        status = false;
    if( !compareBooleans( a.obj.override_dry, b.obj.override_dry, "obj.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.ptc.override_dry, b.ptc.override_dry, "ptc.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.ptc.no_model, b.ptc.no_model, "ptc.no_model", information_r ) )
        status = false;
    if( !compareBooleans( a.ptc.entire_point_cloud, b.ptc.entire_point_cloud, "ptc.entire_point_cloud", information_r ) )
        status = false;
    if( !compareBooleans( a.ptc.entire_height_map, b.ptc.entire_height_map, "ptc.entire_height_map", information_r ) )
        status = false;
    if( !compareBooleans( a.ptc.enable_backface_culling, b.ptc.enable_backface_culling, "ptc.enable_backface_culling", information_r ) )
        status = false;
    if( !compareBooleans( a.pyr.override_dry, b.pyr.override_dry, "pyr.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.pyr.export_prime_bw, b.pyr.export_prime_bw, "pyr.export_prime_bw", information_r ) )
        status = false;
    if( !compareBooleans( a.rpns.override_dry, b.rpns.override_dry, "rpns.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.snds.override_dry, b.snds.override_dry, "snds.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.til.override_dry, b.til.override_dry, "til.override_dry", information_r ) )
        status = false;
    if( !compareBooleans( a.til.enable_til_backface_culling, b.til.enable_til_backface_culling, "til.enable_til_backface_culling", information_r ) )
        status = false;
    if( !compareBooleans( a.til.enable_point_cloud_export, b.til.enable_point_cloud_export, "til.enable_point_cloud_export", information_r ) )
        status = false;
    if( !compareBooleans( a.til.enable_height_map_export, b.til.enable_height_map_export, "til.enable_height_map_export", information_r ) )
        status = false;
    if( !compareBooleans( a.til.enable_til_export_model, b.til.enable_til_export_model, "til.enable_til_export_model", information_r ) )
        status = false;
    if( !compareBooleans( a.wav.override_dry, b.wav.override_dry, "wav.override_dry", information_r ) )
        status = false;

    return status;
}

void invalidParameterTest( std::vector<std::string> before, const std::string single_param,  std::vector<std::string> after, int &is_not_success, std::ostream *information_r = nullptr ) {
    Data::Mission::IFFOptions enabled_nothing;
    std::vector<std::string> parameters;

    for( auto i : before ) {
        parameters.push_back( i );
    }
    parameters.push_back( single_param );
    for( auto i : after ) {
        parameters.push_back( i );
    }

    bool is_valid = enabled_nothing.readParams( parameters, nullptr );

    if( is_valid ) {
        if( information_r != nullptr ) {
            *information_r << "Error: parameter cause failed to fail for {";

            for( auto i : parameters ) {
                *information_r << " \"" << i << "\"";
            }
            *information_r << std::endl;

            enabled_nothing.readParams( parameters, information_r );
        }
        is_not_success = true;
    }
}

void testMultipleCommands( const Data::Mission::IFFOptions expected, const std::vector<std::string> parameter_array, int &is_not_success, std::ostream &information ) {
    Data::Mission::IFFOptions enabled_nothing;
    Data::Mission::IFFOptions result;
    int found_failure = false;

    if( IFFOptionCompare( enabled_nothing, expected ) ) {
        if( parameter_array.size() == 1 )
            information << "Error: there is no corresponding boolean for " << parameter_array[0] << "." << std::endl;
        else
            information << "Error: there is no corresponding boolean for Parameters." << std::endl;

        found_failure = true;
    }

    const bool parameter_status = result.readParams( parameter_array, nullptr );

    if( !IFFOptionCompare( expected, result ) ) {
        if( parameter_array.size() == 1 )
            information << "Error: " << parameter_array[0] << " did not enable properly." << std::endl;
        else {
            information << "Error: Parameters did not enable properly." << std::endl;
        }

        IFFOptionCompare( expected, result, &information );
        Data::Mission::IFFOptions( parameter_array, &information );
        found_failure = true;
    }

    if( !parameter_status ) {
        if( parameter_array.size() == 1 )
            information << "Error: " << parameter_array[0] << " somehow invoked parameter errors." << std::endl;
        else {
            information << "Error: Parameters somehow invoked parameter errors." << std::endl;
        }
        result.readParams( parameter_array, &information );
        found_failure = true;
    }

    if( found_failure && parameter_array.size() != 1 ) {
        information << "  The given parameters are { ";
        for( auto i : parameter_array ) {
            information << " \"" << i << "\"";
        }
        information << "}" << std::endl;
    }

    is_not_success |= found_failure;
}

void testSingleCommand( const Data::Mission::IFFOptions expected, const std::string argument, int &is_not_success, std::ostream &information ) {
    const std::vector<std::string> parameter_array = { argument };

    testMultipleCommands( expected, parameter_array, is_not_success, information );
}

}

int main() {
    int is_not_success = false;

    { // Test against self.
        Data::Mission::IFFOptions enabled_nothing;

        if( !IFFOptionCompare( enabled_nothing, enabled_nothing ) ) {
            std::cout << "Error: self comparsion failed!" << std::endl;
            IFFOptionCompare( enabled_nothing, enabled_nothing, &std::cout );
            is_not_success = true;
        }
    }

    { // Test empty parameter cause.
        Data::Mission::IFFOptions enabled_nothing;

        bool is_valid = enabled_nothing.readParams( std::vector<std::string>(), nullptr );

        if( !is_valid ) {
            std::cout << "Error: empty parameter cause failed!" << std::endl;
            enabled_nothing.readParams( std::vector<std::string>(), &std::cout );
            is_not_success = true;
        }
    }

    const std::string DRY         =  "--DRY";
    const std::string ACT_ENABLE  =  "--ACT_ENABLE";
    const std::string ANM_ENABLE  =  "--ANM_ENABLE";
    const std::string ANM_PALETTE =  "--ANM_PALETTE";
    const std::string BMP_ENABLE  =  "--BMP_ENABLE";
    const std::string BMP_PALETTE =  "--BMP_PALETTE";
    const std::string DCS_ENABLE  =  "--DCS_ENABLE";
    const std::string FONT_ENABLE = "--FONT_ENABLE";
    const std::string FUN_ENABLE  =  "--FUN_ENABLE";
    const std::string NET_ENABLE  =  "--NET_ENABLE";
    const std::string NET_EXPORT_OBJ = "--NET_EXPORT_OBJ";
    const std::string OBJ_ENABLE  =  "--OBJ_ENABLE";
    const std::string PTC_ENABLE  =  "--PTC_ENABLE";
    const std::string PTC_NO_MODEL = "--PTC_NO_MODEL";
    const std::string PTC_ENTIRE_POINT_CLOUD = "--PTC_ENTIRE_POINT_CLOUD";
    const std::string PTC_ENTIRE_HEIGHT_MAP = "--PTC_ENTIRE_HEIGHT_MAP";
    const std::string PTC_EXPORT_CULL = "--PTC_EXPORT_CULL";
    const std::string PYR_ENABLE  =  "--PYR_ENABLE";
    const std::string PYR_PRIME_BLACK_WHITE = "--PYR_PRIME_BLACK_WHITE";
    const std::string RPNS_ENABLE =  "--RPNS_ENABLE";
    const std::string SNDS_ENABLE =  "--SNDS_ENABLE";
    const std::string TIL_ENABLE  =  "--TIL_ENABLE";
    const std::string TIL_EXPORT_POINT_CLOUD_MAP  =  "--TIL_EXPORT_POINT_CLOUD_MAP";
    const std::string TIL_EXPORT_HEIGHT_MAP  =  "--TIL_EXPORT_HEIGHT_MAP";
    const std::string TIL_EXPORT_MODEL  =  "--TIL_EXPORT_MODEL";
    const std::string TIL_EXPORT_CULL   =  "--TIL_EXPORT_CULL";
    const std::string WAV_ENABLE  =  "--WAV_ENABLE";

    { // Test expected.enable_global_dry_default. The dry command test.
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        testSingleCommand( expected, DRY, is_not_success, std::cout );
    }

    { // Test act.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.act.override_dry = true;
        testSingleCommand( expected, ACT_ENABLE, is_not_success, std::cout );
    }

    { // Test anm.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.anm.override_dry = true;
        testSingleCommand( expected, ANM_ENABLE, is_not_success, std::cout );
    }

    { // Test anm.export_palette
        Data::Mission::IFFOptions expected;
        expected.anm.export_palette = true;
        testSingleCommand( expected, ANM_PALETTE, is_not_success, std::cout );
    }

    { // Test bmp.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.bmp.override_dry = true;
        testSingleCommand( expected, BMP_ENABLE, is_not_success, std::cout );
    }

    { // Test bmp.export_palette
        Data::Mission::IFFOptions expected;
        expected.bmp.export_palette = true;
        testSingleCommand( expected, BMP_PALETTE, is_not_success, std::cout );
    }

    { // Test dcs.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.dcs.override_dry = true;
        testSingleCommand( expected, DCS_ENABLE, is_not_success, std::cout );
    }

    { // Test font.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.font.override_dry = true;
        testSingleCommand( expected, FONT_ENABLE, is_not_success, std::cout );
    }

    { // Test fun.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.fun.override_dry = true;
        testSingleCommand( expected, FUN_ENABLE, is_not_success, std::cout );
    }

    { // Test net.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.net.override_dry = true;
        testSingleCommand( expected, NET_ENABLE, is_not_success, std::cout );
    }

    { // Test net.enable_obj
        Data::Mission::IFFOptions expected;
        expected.net.enable_obj = true;
        testSingleCommand( expected, NET_EXPORT_OBJ, is_not_success, std::cout );
    }

    { // Test obj.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.obj.override_dry = true;
        testSingleCommand( expected, OBJ_ENABLE, is_not_success, std::cout );
    }

    { // Test ptc.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.ptc.override_dry = true;
        testSingleCommand( expected, PTC_ENABLE, is_not_success, std::cout );
    }

    { // Test ptc.no_model
        Data::Mission::IFFOptions expected;
        expected.ptc.no_model = true;
        testSingleCommand( expected, PTC_NO_MODEL, is_not_success, std::cout );
    }

    { // Test ptc.entire_point_cloud
        Data::Mission::IFFOptions expected;
        expected.ptc.entire_point_cloud = true;
        testSingleCommand( expected, PTC_ENTIRE_POINT_CLOUD, is_not_success, std::cout );
    }

    { // Test ptc.entire_height_map
        Data::Mission::IFFOptions expected;
        expected.ptc.entire_height_map = true;
        testSingleCommand( expected, PTC_ENTIRE_HEIGHT_MAP, is_not_success, std::cout );
    }

    { // Test ptc.enable_til_backface_culling
        Data::Mission::IFFOptions expected;
        expected.ptc.enable_backface_culling = true;
        testSingleCommand( expected, PTC_EXPORT_CULL, is_not_success, std::cout );
    }

    { // Test pyr.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.pyr.override_dry = true;
        testSingleCommand( expected, PYR_ENABLE, is_not_success, std::cout );
    }

    { // Test ptc.export_prime_bw
        Data::Mission::IFFOptions expected;
        expected.pyr.export_prime_bw = true;
        testSingleCommand( expected, PYR_PRIME_BLACK_WHITE, is_not_success, std::cout );
    }

    { // Test rpns.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.rpns.override_dry = true;
        testSingleCommand( expected, RPNS_ENABLE, is_not_success, std::cout );
    }

    { // Test snds.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.snds.override_dry = true;
        testSingleCommand( expected, SNDS_ENABLE, is_not_success, std::cout );
    }

    { // Test til.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.til.override_dry = true;
        testSingleCommand( expected, TIL_ENABLE, is_not_success, std::cout );
    }

    { // Test til.enable_point_cloud_export
        Data::Mission::IFFOptions expected;
        expected.til.enable_point_cloud_export = true;
        testSingleCommand( expected, TIL_EXPORT_POINT_CLOUD_MAP, is_not_success, std::cout );
    }

    { // Test til.enable_height_map_export
        Data::Mission::IFFOptions expected;
        expected.til.enable_height_map_export = true;
        testSingleCommand( expected, TIL_EXPORT_HEIGHT_MAP, is_not_success, std::cout );
    }

    { // Test til.enable_til_backface_culling
        Data::Mission::IFFOptions expected;
        expected.til.enable_til_backface_culling = true;
        testSingleCommand( expected, TIL_EXPORT_CULL, is_not_success, std::cout );
    }

    { // Test til.entire_height_map
        Data::Mission::IFFOptions expected;
        expected.til.enable_til_export_model = true;
        testSingleCommand( expected, TIL_EXPORT_MODEL, is_not_success, std::cout );
    }

    { // Test wav.override_dry
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.wav.override_dry = true;
        testSingleCommand( expected, WAV_ENABLE, is_not_success, std::cout );
    }

    { // Test multiple parameter inputs.
        Data::Mission::IFFOptions expected;
        expected.net.enable_obj = true;
        expected.ptc.no_model = true;
        expected.ptc.entire_height_map = true;
        expected.bmp.export_palette = true;
        expected.til.enable_til_export_model = true;
        const std::vector<std::string> parameter_array = { NET_EXPORT_OBJ, PTC_NO_MODEL, PTC_ENTIRE_HEIGHT_MAP, BMP_PALETTE, TIL_EXPORT_MODEL};

        testMultipleCommands( expected, parameter_array, is_not_success, std::cout );
    }

    { // Test multiple parameter inputs with dry parameter.
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.net.enable_obj = true;
        expected.ptc.no_model = true;
        expected.ptc.entire_height_map = true;
        expected.bmp.export_palette = true;
        expected.til.enable_til_export_model = true;
        const std::vector<std::string> parameter_array = { NET_EXPORT_OBJ, PTC_NO_MODEL, DRY, PTC_ENTIRE_HEIGHT_MAP, BMP_PALETTE, TIL_EXPORT_MODEL};

        testMultipleCommands( expected, parameter_array, is_not_success, std::cout );
    }

    { // Test multiple parameter inputs with ENABLE parameter.
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.ptc.override_dry = true;
        expected.net.enable_obj = true;
        expected.ptc.no_model = true;
        expected.ptc.entire_height_map = true;
        expected.bmp.export_palette = true;
        expected.til.enable_til_export_model = true;
        const std::vector<std::string> parameter_array = { NET_EXPORT_OBJ, PTC_NO_MODEL, PTC_ENABLE, PTC_ENTIRE_HEIGHT_MAP, BMP_PALETTE, TIL_EXPORT_MODEL};

        testMultipleCommands( expected, parameter_array, is_not_success, std::cout );
    }

    // DRY and ENABLE would conflict with each other. Thus, it is forbidden.
    // ENABLE should be used to make the program more specific on what it exports.
    // DRY should express when not to write to anything to the disk.
    {
        Data::Mission::IFFOptions expected;
        expected.enable_global_dry_default = true;
        expected.net.override_dry = true;
        expected.net.enable_obj = true;
        const std::vector<std::string> parameter_array = { NET_EXPORT_OBJ, DRY, NET_ENABLE };

        int should_fail = false;

        std::stringstream stream_empty;

        testMultipleCommands( expected, parameter_array, should_fail, stream_empty );

        if( !should_fail ) {
            is_not_success = true;

            std::cout << "Error: DRY and ENABLE do not mix. Output:\n";
            std::cout << stream_empty.str() << std::endl;
        }
    }

    // This makes sure the write operations would work well.
    {
        Data::Mission::IFFOptions expected;
        expected.net.override_dry = true;

        if( !expected.net.shouldWrite( true ) ) {
            is_not_success = true;
            std::cout << "Error: override_dry (1) with dry (1) should write." << std::endl;
        }
        if( !expected.net.shouldWrite( false ) ) {
            is_not_success = true;
            std::cout << "Error: override_dry (1) with dry (0) should write." << std::endl;
        }

        expected.net.override_dry = false;

        if( expected.net.shouldWrite( true ) ) {
            is_not_success = true;
            std::cout << "Error: override_dry (0) with dry (1) should not write." << std::endl;
        }
        if( !expected.net.shouldWrite( false ) ) {
            is_not_success = true;
            std::cout << "Error: override_dry (0) with dry (0) should write." << std::endl;
        }
    }

    {
        std::vector<std::string> invalid_commands = { "k", "ke", "kel", "kelp", "-\"", "-ke", "-kel", "-kelp", "--k", "--ke", "--kel", "--kelp" };

        { // Empty of valid test.
            const std::vector<std::string> before = {};
            const std::vector<std::string> after  = {};

            // Kelp is the most unlikely word to test hence it is being used.
            for( auto i : invalid_commands ) {
                invalidParameterTest( before, i, after, is_not_success, &std::cout );
            }
        }
        { // Empty of valid test.
            const std::vector<std::string> before = {};
            const std::vector<std::string> after  = { BMP_PALETTE };

            // Kelp is the most unlikely word to test hence it is being used.
            for( auto i : invalid_commands ) {
                invalidParameterTest( before, i, after, is_not_success, &std::cout );
            }
        }
        { // Empty of valid test.
            const std::vector<std::string> before = { BMP_PALETTE };
            const std::vector<std::string> after  = {};

            // Kelp is the most unlikely word to test hence it is being used.
            for( auto i : invalid_commands ) {
                invalidParameterTest( before, i, after, is_not_success, &std::cout );
            }
        }
        { // One occerence case enforcement.
            const std::vector<std::string> before = { BMP_PALETTE };
            const std::vector<std::string> after  = { BMP_PALETTE };

            // Kelp is the most unlikely word to test hence it is being used.
            for( auto i : invalid_commands ) {
                invalidParameterTest( before, i, after, is_not_success, &std::cout );
            }
        }
        { // One occerence case needed
            const std::vector<std::string> before = { ANM_PALETTE };
            const std::vector<std::string> after  = { BMP_PALETTE };

            // Kelp is the most unlikely word to test hence it is being used.
            for( auto i : invalid_commands ) {
                invalidParameterTest( before, i, after, is_not_success, &std::cout );
            }
        }
    }

    return is_not_success;
}
