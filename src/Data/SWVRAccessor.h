#ifndef DATA_SWVR_ACCESSOR_H
#define DATA_SWVR_ACCESSOR_H

#include "Mission/IFF.h"

#include <vector>
#include <map>

namespace Data {

// These are prototypes, so I do not have to include each of them. This is to save compilation time.
namespace Mission {
    class ANMResource;
    class MSICResource;
    class SNDSResource;
}

class SWVRAccessor {
public:
    struct SearchValue {
        uint32_t type;
        uint32_t tos_offset;

        bool operator< ( const SearchValue & operand ) const;
    };
    template <class resource>
    struct DataValue {
        resource*       changable_r;
        const resource* constant_r;
    };

private:
    std::map<SearchValue, DataValue<Mission::Resource>> search;

    DataValue<Mission::MSICResource> msic_resource;

public:
    SWVRAccessor();
    virtual ~SWVRAccessor();

    // These three methods should only be called by Accessor.
    void emplaceConstant( const Mission::Resource *resource_r );
    void emplace( Mission::Resource *resource_r );
    void clear();

    std::vector<Mission::Resource*> getAll();
    std::vector<const Mission::Resource*> getAllConst() const;

    std::vector<Mission::Resource*> getAllType( uint32_t type );
    std::vector<const Mission::Resource*> getAllConstType( uint32_t type ) const;

    Mission::ANMResource* getANM( uint32_t resource_id );
    const Mission::ANMResource* getConstANM( uint32_t resource_id ) const;
    std::vector<Mission::ANMResource*> getAllANM();
    std::vector<const Mission::ANMResource*> getAllConstANM() const;

    Mission::MSICResource* getMSIC() { return msic_resource.changable_r; }
    const Mission::MSICResource* getConstMSIC() const { return msic_resource.constant_r; }

    Mission::SNDSResource* getSNDS( uint32_t resource_id );
    const Mission::SNDSResource* getConstSNDS( uint32_t resource_id ) const;
    std::vector<Mission::SNDSResource*> getAllSNDS();
    std::vector<const Mission::SNDSResource*> getAllConstSNDS() const;
};

} // Data

#endif // DATA_SWVR_ACCESSOR_H