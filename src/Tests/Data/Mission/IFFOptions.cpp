#include "../../../Data/Mission/IFFOptions.h"
#include <iostream>

namespace {

bool compareBooleans( bool a, bool b, std::string field_name, std::ostream *information_r ) {
    if( a != b ) {
        if( information_r != nullptr ) {
            *information_r << "a " << field_name << " != b." << field_name << std::endl;
        }

        return false;
    }
    return true;
}

bool IFFOptionCompare( const Data::Mission::IFFOptions a, const Data::Mission::IFFOptions b, std::ostream *information_r = nullptr ) {
    if( compareBooleans( a.enable_global_dry_default, b.enable_global_dry_default, "enable_global_dry_default",information_r ) )
        return false;
    if( compareBooleans( a.act.override_dry, b.act.override_dry, "act.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.anm.override_dry, b.anm.override_dry, "anm.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.anm.export_palette, b.anm.export_palette, "anm.export_palette", information_r ) )
        return false;
    if( compareBooleans( a.bmp.override_dry, b.bmp.override_dry, "bmp.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.bmp.export_palette, b.bmp.export_palette, "bmp.export_palette", information_r ) )
        return false;
    if( compareBooleans( a.dcs.override_dry, b.dcs.override_dry, "dcs.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.font.override_dry, b.font.override_dry, "font.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.fun.override_dry, b.fun.override_dry, "fun.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.msic.override_dry, b.msic.override_dry, "msic.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.net.override_dry, b.net.override_dry, "net.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.net.enable_obj, b.net.enable_obj, "net.enable_obj", information_r ) )
        return false;
    if( compareBooleans( a.obj.override_dry, b.obj.override_dry, "obj.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.ptc.override_dry, b.ptc.override_dry, "ptc.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.ptc.no_model, b.ptc.no_model, "ptc.no_model", information_r ) )
        return false;
    if( compareBooleans( a.ptc.entire_point_cloud, b.ptc.entire_point_cloud, "ptc.entire_point_cloud", information_r ) )
        return false;
    if( compareBooleans( a.ptc.entire_height_map, b.ptc.entire_height_map, "ptc.entire_height_map", information_r ) )
        return false;
    if( compareBooleans( a.pyr.override_dry, b.pyr.override_dry, "pyr.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.pyr.export_prime_bw, b.pyr.export_prime_bw, "pyr.export_prime_bw", information_r ) )
        return false;
    if( compareBooleans( a.rpns.override_dry, b.rpns.override_dry, "rpns.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.snds.override_dry, b.snds.override_dry, "snds.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.til.override_dry, b.til.override_dry, "til.override_dry", information_r ) )
        return false;
    if( compareBooleans( a.til.enable_point_cloud_export, b.til.enable_point_cloud_export, "til.enable_point_cloud_export", information_r ) )
        return false;
    if( compareBooleans( a.til.enable_height_map_export, b.til.enable_height_map_export, "til.enable_height_map_export", information_r ) )
        return false;
    if( compareBooleans( a.til.enable_til_export_model, b.til.enable_til_export_model, "til.enable_til_export_model", information_r ) )
        return false;
    if( compareBooleans( a.wav.override_dry, b.wav.override_dry, "wav.override_dry", information_r ) )
        return false;
    return true;
}

}

int main() {
    int is_not_success = false;

    { // Test against self.
        Data::Mission::IFFOptions enabled_nothing;

        if( !IFFOptionCompare( enabled_nothing, enabled_nothing ) ) {
            std::cout << "Error: self comparsion failed!" << std::endl;
            is_not_success = true;
        }
    }

    { // Test anm.override_dry
        Data::Mission::IFFOptions enabled_nothing;
        Data::Mission::IFFOptions enabled_something;

        enabled_something.anm.override_dry = true;

        if( IFFOptionCompare( enabled_nothing, enabled_something ) ) {
            std::cout << "Error: enabled_something.anm.override_dry is not in comparision." << std::endl;
            IFFOptionCompare( enabled_nothing, enabled_something, &std::cout );
            is_not_success = true;
        }

        Data::Mission::IFFOptions enabled_parameter( std::vector<std::string>({ "--ACT_ENABLE" }), nullptr );

        if( !IFFOptionCompare( enabled_parameter, enabled_something ) ) {
            std::cout << "Error: --ACT_ENABLE did not enable properly." << std::endl;
            IFFOptionCompare( enabled_parameter, enabled_something, &std::cout );
            is_not_success = true;
        }
    }

    return is_not_success;
}
