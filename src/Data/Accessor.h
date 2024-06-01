#ifndef DATA_ACCESSOR_H
#define DATA_ACCESSOR_H

#include "Mission/IFF.h"

#include "ActorAccessor.h"

#include <vector>
#include <map>

namespace Data {

// These are prototypes, so I do not have to include each of them. This is to save compilation time.
namespace Mission {
    class ANMResource;
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
    class SNDSResource;
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

    std::map<uint32_t, Accessor> swvr_files;

    void emplaceConstant( const Mission::Resource *resource_r );
    void emplace( Mission::Resource *resource_r );

public:
    Accessor();
    virtual ~Accessor();

    void loadConstant( const Mission::IFF &resource_r );
    void load( Mission::IFF &resource_r );
    void clear();

    Accessor* getSWVRAccessor(uint32_t tos_offset);
    const Accessor* getSWVRAccessor(uint32_t tos_offset) const;

    ActorAccessor& getActorAccessor() { return actor_accessor; }
    const ActorAccessor& getActorAccessor() const { return actor_accessor; }

    Mission::ANMResource* getANM( uint32_t resource_id );
    const Mission::ANMResource* getConstANM( uint32_t resource_id ) const;
    std::vector<Mission::ANMResource*> getAllANM();
    std::vector<const Mission::ANMResource*> getAllConstANM() const;

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

    Mission::NetResource* getNET( uint32_t resource_id );
    const Mission::NetResource* getConstNET( uint32_t resource_id ) const;
    std::vector<Mission::NetResource*> getAllNET();
    std::vector<const Mission::NetResource*> getAllConstNET() const;

    Mission::MSICResource* getMSIC( uint32_t resource_id );
    const Mission::MSICResource* getConstMSIC( uint32_t resource_id ) const;
    std::vector<Mission::MSICResource*> getAllMSIC();
    std::vector<const Mission::MSICResource*> getAllConstMSIC() const;

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

    Mission::SNDSResource* getSNDS( uint32_t resource_id );
    const Mission::SNDSResource* getConstSNDS( uint32_t resource_id ) const;
    std::vector<Mission::SNDSResource*> getAllSNDS();
    std::vector<const Mission::SNDSResource*> getAllConstSNDS() const;

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
