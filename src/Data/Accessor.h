#ifndef DATA_ACCESSOR_H
#define DATA_ACCESSOR_H

#include "Mission/IFF.h"

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
    class WAVResource;
}

class Accessor {
public:
    struct SearchValue {
        uint32_t type;
        uint32_t resource_id;

        bool operator< ( const SearchValue & operand ) const;
    };

private:
    std::map<SearchValue, Mission::Resource*> search;

public:
    Accessor( std::vector<Mission::IFF*> resources );
    virtual ~Accessor();

    Mission::ANMResource*  getANM(  uint32_t resource_id ) const;
    Mission::BMPResource*  getBMP(  uint32_t resource_id ) const;
    Mission::DCSResource*  getDCS(  uint32_t resource_id ) const;
    Mission::FUNResource*  getFUN(  uint32_t resource_id ) const;
    Mission::FontResource* getFNT(  uint32_t resource_id ) const;
    Mission::MSICResource* getMISC( uint32_t resource_id ) const;
    Mission::NetResource*  getNET(  uint32_t resource_id ) const;
    Mission::ObjResource*  getOBJ(  uint32_t resource_id ) const;
    Mission::PTCResource*  getPTC(  uint32_t resource_id ) const;
    Mission::PYRResource*  getPYR(  uint32_t resource_id ) const;
    Mission::RPNSResource* getRPNS( uint32_t resource_id ) const;
    Mission::SNDSResource* getSNDS( uint32_t resource_id ) const;
    Mission::TilResource*  getTIL(  uint32_t resource_id ) const;
    Mission::WAVResource*  getWAV(  uint32_t resource_id ) const;
};

} // Data

#endif // DATA_ACCESSOR_H
