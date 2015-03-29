#include "integral.h"

namespace up {

	types::integer_type integer_type("up.integral.integer");
	types::real_type    real_type("up.integral.real");

	const types::integer_type& get_meta_type(const integer*) { return integer_type; }
	const types::real_type& get_meta_type(const real*) { return real_type; }

}