#pragma once
#include "type.h"
#include <functional>

namespace up {

	void bases_breadth_first(const type* cl, const std::function<void(const type*)>& func);
	void bases_depth_first(const type* cl, const std::function<void(const type*)>& func);


}
