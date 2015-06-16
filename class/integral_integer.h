#pragma once

#include "static_type.h"
#include "events.h"

namespace up {

	struct integer : public up::events_holder_t<integer> {
		virtual int get() const = 0;
		virtual void set(int value) = 0;
		virtual bool validate(int value) const = 0;
		virtual std::pair<int, int> limits() const = 0;

		up::holder_signal<void(int)> changing;
		up::holder_signal<void(int)> changed;
		up::holder_signal<void(std::pair<int, int>)> limits_changing;
		up::holder_signal<void(std::pair<int, int>)> limits_changed;
	};

	namespace types {
		using integer_type = static_class<integer>;
	}

	const types::integer_type& get_meta_type(const integer*);


}