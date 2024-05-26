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

    const Mission::ANMResource*  getConstANM( uint32_t resource_id ) const;
    std::vector<const Mission::ANMResource*> getAllConstANM() const;

    const Mission::BMPResource*  getConstBMP( uint32_t resource_id ) const;
    std::vector<const Mission::BMPResource*> getAllConstBMP() const;

    const Mission::DCSResource*  getConstDCS( uint32_t resource_id ) const;
    std::vector<const Mission::DCSResource*> getAllConstDCS() const;

    const Mission::FUNResource*  getConstFUN( uint32_t resource_id ) const;
    std::vector<const Mission::FUNResource*> getAllConstFUN() const;

    const Mission::FontResource* getConstFNT( uint32_t resource_id ) const;
    std::vector<const Mission::FontResource*> getAllConstFNT() const;

    const Mission::MSICResource* getConstMISC( uint32_t resource_id ) const;
    std::vector<const Mission::MSICResource*> getAllConstMISC() const;

    const Mission::NetResource*  getConstNET( uint32_t resource_id ) const;
    std::vector<const Mission::NetResource*> getAllConstNET() const;

    const Mission::ObjResource*  getConstOBJ( uint32_t resource_id ) const;
    std::vector<const Mission::ObjResource*> getAllConstOBJ() const;

    const Mission::PTCResource*  getConstPTC( uint32_t resource_id ) const;
    std::vector<const Mission::PTCResource*> getAllConstPTC() const;

    const Mission::PYRResource*  getConstPYR( uint32_t resource_id ) const;
    std::vector<const Mission::PYRResource*> getAllConstPYR() const;

    const Mission::RPNSResource* getConstRPNS( uint32_t resource_id ) const;
    std::vector<const Mission::RPNSResource*> getAllConstRPNS() const;

    const Mission::SNDSResource* getConstSNDS( uint32_t resource_id ) const;
    std::vector<const Mission::SNDSResource*> getAllConstSNDS() const;

    const Mission::TilResource*  getConstTIL( uint32_t resource_id ) const;
    std::vector<const Mission::TilResource*> getAllConstTIL() const;

    const Mission::TOSResource*  getConstTOS( uint32_t resource_id ) const;
    std::vector<const Mission::TOSResource*> getAllConstTOS() const;

    const Mission::WAVResource*  getConstWAV( uint32_t resource_id ) const;
    std::vector<const Mission::WAVResource*> getAllConstWAV() const;
};

} // Data

#endif // DATA_ACCESSOR_H
