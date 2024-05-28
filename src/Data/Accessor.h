#ifndef DATA_ACCESSOR_H
#define DATA_ACCESSOR_H

#include "Mission/IFF.h"

#include "ActorAccessor.h"
#include "SWVRAccessor.h"

#include <vector>
#include <map>

namespace Data {

// These are prototypes, so I do not have to include each of them. This is to save compilation time.
namespace Mission {
    class BMPResource;
    class DCSResource;
    class FUNResource;
    class FontResource;
    class MSICResource;
    class NetResource;
    class ObjResource;
    class PTCResource;
    class PYRResource;
    class RPNSResource;
    class TilResource;
    class TOSResource;
    class WAVResource;
}

class Accessor {
public:
    struct SearchValue {
        uint32_t type;
        uint32_t resource_id;

        bool operator< ( const SearchValue & operand ) const;
    };
    struct DataValue {
        Mission::Resource*       changable_r;
        const Mission::Resource* constant_r;
    };

private:
    std::map<SearchValue, DataValue> search;
    ActorAccessor actor_accessor;
    SWVRAccessor swvr_accessor;

public:
    Accessor();
    virtual ~Accessor();

    void loadConstant( const Mission::IFF &resource_r );
    void load( Mission::IFF &resource_r );
    void clear();

    ActorAccessor& getActorAccessor() { return actor_accessor; }
    const ActorAccessor& getActorAccessor() const { return actor_accessor; }

    SWVRAccessor& getSWVRAccessor() { return swvr_accessor; }
    const SWVRAccessor& getSWVRAccessor() const { return swvr_accessor; }

    Mission::BMPResource* getBMP( uint32_t resource_id );
    const Mission::BMPResource* getConstBMP( uint32_t resource_id ) const;
    std::vector<Mission::BMPResource*> getAllBMP();
    std::vector<const Mission::BMPResource*> getAllConstBMP() const;

    Mission::DCSResource* getDCS( uint32_t resource_id );
    const Mission::DCSResource* getConstDCS( uint32_t resource_id ) const;
    std::vector<Mission::DCSResource*> getAllDCS();
    std::vector<const Mission::DCSResource*> getAllConstDCS() const;

    Mission::FUNResource* getFUN( uint32_t resource_id );
    const Mission::FUNResource* getConstFUN( uint32_t resource_id ) const;
    std::vector<Mission::FUNResource*> getAllFUN();
    std::vector<const Mission::FUNResource*> getAllConstFUN() const;

    Mission::FontResource* getFNT( uint32_t resource_id );
    const Mission::FontResource* getConstFNT( uint32_t resource_id ) const;
    std::vector<Mission::FontResource*> getAllFNT();
    std::vector<const Mission::FontResource*> getAllConstFNT() const;

    Mission::MSICResource* getMISC( uint32_t resource_id );
    const Mission::MSICResource* getConstMISC( uint32_t resource_id ) const;
    std::vector<Mission::MSICResource*> getAllMISC();
    std::vector<const Mission::MSICResource*> getAllConstMISC() const;

    Mission::NetResource* getNET( uint32_t resource_id );
    const Mission::NetResource* getConstNET( uint32_t resource_id ) const;
    std::vector<Mission::NetResource*> getAllNET();
    std::vector<const Mission::NetResource*> getAllConstNET() const;

    Mission::ObjResource* getOBJ( uint32_t resource_id );
    const Mission::ObjResource* getConstOBJ( uint32_t resource_id ) const;
    std::vector<Mission::ObjResource*> getAllOBJ();
    std::vector<const Mission::ObjResource*> getAllConstOBJ() const;

    Mission::PTCResource* getPTC( uint32_t resource_id );
    const Mission::PTCResource* getConstPTC( uint32_t resource_id ) const;
    std::vector<Mission::PTCResource*> getAllPTC();
    std::vector<const Mission::PTCResource*> getAllConstPTC() const;

    Mission::PYRResource* getPYR( uint32_t resource_id );
    const Mission::PYRResource* getConstPYR( uint32_t resource_id ) const;
    std::vector<Mission::PYRResource*> getAllPYR();
    std::vector<const Mission::PYRResource*> getAllConstPYR() const;

    Mission::RPNSResource* getRPNS( uint32_t resource_id );
    const Mission::RPNSResource* getConstRPNS( uint32_t resource_id ) const;
    std::vector<Mission::RPNSResource*> getAllRPNS();
    std::vector<const Mission::RPNSResource*> getAllConstRPNS() const;

    Mission::TilResource* getTIL( uint32_t resource_id );
    const Mission::TilResource* getConstTIL( uint32_t resource_id ) const;
    std::vector<Mission::TilResource*> getAllTIL();
    std::vector<const Mission::TilResource*> getAllConstTIL() const;

    Mission::TOSResource* getTOS( uint32_t resource_id );
    const Mission::TOSResource* getConstTOS( uint32_t resource_id ) const;
    std::vector<Mission::TOSResource*> getAllTOS();
    std::vector<const Mission::TOSResource*> getAllConstTOS() const;

    Mission::WAVResource* getWAV( uint32_t resource_id );
    const Mission::WAVResource* getConstWAV( uint32_t resource_id ) const;
    std::vector<Mission::WAVResource*> getAllWAV();
    std::vector<const Mission::WAVResource*> getAllConstWAV() const;
};

} // Data

#endif // DATA_ACCESSOR_H
