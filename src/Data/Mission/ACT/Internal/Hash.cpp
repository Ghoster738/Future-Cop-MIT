#include "Hash.h"

#include "../X1Alpha.h"
#include "../BaseTurret.h"
#include "../Aircraft.h"
#include "../Skycaptin.h"
#include "../Prop.h"
#include "../Trigger.h"

namespace {

class Declaration {
private:
    const static size_t TABLE_SIZE = 0x100;

    Data::Mission::ACTResource* table_p[ TABLE_SIZE ];
    
    // Type ID 16 - 36 Unknown bytes - Precinct Assault Powerups.
    // Type ID 32 - 32 Unknown bytes - Police Locker Unit Contianing Powerups.

    void setupTable() {
        table_p[ Data::Mission::ACT::X1Alpha::TYPE_ID ] = new Data::Mission::ACT::X1Alpha();
        table_p[ Data::Mission::ACT::BaseTurret::TYPE_ID ] = new Data::Mission::ACT::BaseTurret();
        table_p[ Data::Mission::ACT::Aircraft::TYPE_ID ] = new Data::Mission::ACT::Aircraft();
        table_p[ Data::Mission::ACT::Skycaptin::TYPE_ID ] = new Data::Mission::ACT::Skycaptin();
        table_p[ Data::Mission::ACT::Prop::TYPE_ID ] = new Data::Mission::ACT::Prop();
        table_p[ Data::Mission::ACT::Trigger::TYPE_ID ] = new Data::Mission::ACT::Trigger();
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
