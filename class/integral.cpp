#include "integral.h"

namespace up {

	types::integer_type integer_type("up.integral.integer");
	types::real_type    real_type("up.integral.real");
	types::enum_type    enum_type("up.integral.enumeration");
	types::null_type    null_type("up.null");
	types::group_type   group_type("up.integral.group");

	const types::integer_type& get_meta_type(const integer*) { return integer_type; }
	const types::real_type& get_meta_type(const real*) { return real_type; }
	const types::enum_type& get_meta_type(const enumeration*) { return enum_type; }
	const types::group_type& get_meta_type(const group*) { return group_type; }
	const types::null_type& get_meta_type(const void*) { return null_type; }

}