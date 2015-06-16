#pragma once

#include "static_type.h"
#include "events.h"

namespace up {

	struct boolean : public up::events_holder_t<boolean> {
		virtual bool get() const = 0;
		virtual void set(bool) const = 0;

		up::holder_signal<void(bool)> changing;
		up::holder_signal<void(bool)> changed;
	};

	namespace types {
		using boolean_type = static_class<boolean>;
	}

	const types::boolean_type& get_meta_type(const boolean*);

}