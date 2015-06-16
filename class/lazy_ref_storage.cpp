#include "lazy_ref_storage.h"
#include "events.h"

namespace up {

	namespace detail {

		
		lazy_storage_ocb::lazy_storage_ocb(const meta_type* type, const void* objptr, lazy_ref_storage* store, id_type id, bool register_events_holder )
			: ocb( type, objptr ), store_( store ), id_(id)
		{
			if (register_events_holder) {
				detail::set_last_constructed_holder(this);
			}
		}

		lazy_storage_ocb::~lazy_storage_ocb()
		{
			store_->remove( id_ );
		}


	}

}