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

private:
    std::map<SearchValue, Mission::Resource*> search;

public:
    Accessor();
    virtual ~Accessor();

    void load( Mission::IFF &resource_r );
    void clear();

    Mission::ANMResource*  getANM( uint32_t resource_id ) const;
    std::vector<const Mission::ANMResource*> getAllConstANM() const;

    Mission::BMPResource*  getBMP( uint32_t resource_id ) const;
    std::vector<const Mission::BMPResource*> getAllConstBMP() const;

    Mission::DCSResource*  getDCS( uint32_t resource_id ) const;
    std::vector<const Mission::DCSResource*> getAllConstDCS() const;

    Mission::FUNResource*  getFUN( uint32_t resource_id ) const;
    std::vector<const Mission::FUNResource*> getAllConstFUN() const;

    Mission::FontResource* getFNT( uint32_t resource_id ) const;
    std::vector<const Mission::FontResource*> getAllConstFNT() const;

    Mission::MSICResource* getMISC( uint32_t resource_id ) const;
    std::vector<const Mission::MSICResource*> getAllConstMISC() const;

    Mission::NetResource*  getNET( uint32_t resource_id ) const;
    std::vector<const Mission::NetResource*> getAllConstNET() const;

    Mission::ObjResource*  getOBJ( uint32_t resource_id ) const;
    std::vector<const Mission::ObjResource*> getAllConstOBJ() const;

    Mission::PTCResource*  getPTC( uint32_t resource_id ) const;
    std::vector<const Mission::PTCResource*> getAllConstPTC() const;

    Mission::PYRResource*  getPYR( uint32_t resource_id ) const;
    std::vector<const Mission::PYRResource*> getAllConstPYR() const;

    Mission::RPNSResource* getRPNS( uint32_t resource_id ) const;
    std::vector<const Mission::RPNSResource*> getAllConstRPNS() const;

    Mission::SNDSResource* getSNDS( uint32_t resource_id ) const;
    std::vector<const Mission::SNDSResource*> getAllConstSNDS() const;

    Mission::TilResource*  getTIL( uint32_t resource_id ) const;
    std::vector<const Mission::TilResource*> getAllConstTIL() const;

    Mission::TOSResource*  getTOS( uint32_t resource_id ) const;
    std::vector<const Mission::TOSResource*> getAllConstTOS() const;

    Mission::WAVResource*  getWAV( uint32_t resource_id ) const;
    std::vector<const Mission::WAVResource*> getAllConstWAV() const;
};

} // Data

#endif // DATA_ACCESSOR_H
