#include "meta_type.h"
#include "reference.h"

namespace up {

	ref<void> ref_const_cast(const ref<const void>& src) {
		return ref<void>(src.get_ocb(), detail::fragment_ptr<void>(src.type(), const_cast<void*>(src.get_ptr())));
	}

	ref<void> ref_const_cast(ref<const void>&& src) {
		return ref<void>(std::move(src.get_ocb()), detail::fragment_ptr<void>(src.type(), const_cast<void*>(src.get_ptr())));
	}

}