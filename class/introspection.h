#pragma once
#include "class.h"
#include <functional>

namespace up {

	void bases_breadth_first(const class_* cl, const std::function<void(const class_*)>& func);
	void bases_depth_first(const class_* cl, const std::function<void(const class_*)>& func);


}
