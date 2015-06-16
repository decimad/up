#pragma once

#include "integral_array.h"
#include "integral_bool.h"
#include "integral_enum.h"
#include "integral_integer.h"
#include "integral_real.h"
#include "integral_group.h"

namespace up {	

	namespace types {
		struct void_type {};
		using null_type = static_class<void_type>;
	}

	const types::null_type& get_meta_type(const void*);

}