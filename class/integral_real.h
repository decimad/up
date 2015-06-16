#pragma once

#include "static_type.h"
#include "events.h"

namespace up {

	struct real : public up::events_holder_t<real> {
		virtual double get() const = 0;
		virtual void set(double value) = 0;
		virtual std::pair<double, double> limits() const = 0;

		up::holder_signal<void(double)> changing;
		up::holder_signal<void(double)> changed;
		up::holder_signal<void(std::pair<double, double>)> limits_changing;
		up::holder_signal<void(std::pair<double, double>)> limits_changed;
	};

	namespace types {
		using real_type = static_class<real>;
	}

	const types::real_type&    get_meta_type(const real*);

}