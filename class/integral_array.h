#pragma once

#include "static_type.h"
#include "events.h"

namespace up {

	struct array {
		virtual const meta_type& type() const = 0;
		virtual size_t size() const = 0;
		virtual ref<const void> operator[](size_t idx) const = 0;
		virtual ref<void> operator[](size_t idx) = 0;
	};

}