#include "Hash.h"

#include "../Aircraft.h"
#include "../Elevator.h"
#include "../DynamicProp.h"
#include "../Turret.h"
#include "../ItemPickup.h"
#include "../NeutralTurret.h"
#include "../Prop.h"
#include "../SkyCaptain.h"
#include "../Trigger.h"
#include "../WalkableProp.h"
#include "../X1Alpha.h"

namespace {

class Declaration {
private:
    const static size_t TABLE_SIZE = 0x100;

    Data::Mission::ACTResource* table_p[ TABLE_SIZE ];
    
    // Type ID 32 - 32 Unknown bytes - Police Locker Unit Contianing Powerups.

    void setupTable() {
        table_p[ Data::Mission::ACT::Aircraft::TYPE_ID ] = new Data::Mission::ACT::Aircraft();
        table_p[ Data::Mission::ACT::Elevator::TYPE_ID ] = new Data::Mission::ACT::Elevator();
        table_p[ Data::Mission::ACT::DynamicProp::TYPE_ID ] = new Data::Mission::ACT::DynamicProp();
        table_p[ Data::Mission::ACT::Turret::TYPE_ID ] = new Data::Mission::ACT::Turret();
        table_p[ Data::Mission::ACT::ItemPickup::TYPE_ID ] = new Data::Mission::ACT::ItemPickup();
        table_p[ Data::Mission::ACT::NeutralTurret::TYPE_ID ] = new Data::Mission::ACT::NeutralTurret();
        table_p[ Data::Mission::ACT::Prop::TYPE_ID ] = new Data::Mission::ACT::Prop();
        table_p[ Data::Mission::ACT::SkyCaptain::TYPE_ID ] = new Data::Mission::ACT::SkyCaptain();
        table_p[ Data::Mission::ACT::Trigger::TYPE_ID ] = new Data::Mission::ACT::Trigger();
        table_p[ Data::Mission::ACT::WalkableProp::TYPE_ID ] = new Data::Mission::ACT::WalkableProp();
        table_p[ Data::Mission::ACT::X1Alpha::TYPE_ID ] = new Data::Mission::ACT::X1Alpha();
    }
public:
    Declaration() {
        for( unsigned i = 0; i < TABLE_SIZE; i++ )
            table_p[ i ] = nullptr;

        setupTable();
    }

    ~Declaration() {
        for( unsigned i = 0; i < TABLE_SIZE; i++ ) {
            if( table_p[ i ] != nullptr )
                delete table_p[ i ];
        }
    }

    Data::Mission::ACTResource* getHash( uint_fast8_t key_type_ID ) const {
        if( key_type_ID < TABLE_SIZE )
            return table_p[ key_type_ID ];
        else
            return nullptr;
    }
} hash;

};

Data::Mission::ACTResource* Data::Mission::ACT::Hash::generateAct( const ACTResource *const obj_r, uint_fast8_t Type_ID ) {
    return hash.getHash( Type_ID )->duplicate( *obj_r );
}

bool Data::Mission::ACT::Hash::isActValid( uint_fast8_t Type_ID ) {
    return hash.getHash( Type_ID ) != nullptr;
}
