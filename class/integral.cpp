#include "integral.h"

namespace up {

	classes::integer_class integer_class("up.integral.integer");
	classes::real_class    real_class("up.integral.real");

	const classes::integer_class& get_meta_class(const integer*) { return integer_class; }
	const classes::real_class& get_meta_class(const real*) { return real_class; }

}