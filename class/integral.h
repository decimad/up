#pragma once

#include "static_type.h"
#include "events.h"

namespace up {

	struct integer : public up::events_holder_t<integer> {
		virtual int get() const = 0;
		virtual void set(int value) = 0;
		virtual std::pair<int, int> limits() const = 0;

		up::holder_signal<void(int)> changing;
		up::holder_signal<void(int)> changed;
		up::holder_signal<void(std::pair<int, int>)> limits_changing;
		up::holder_signal<void(std::pair<int, int>)> limits_changed;
	};
	
	struct real : public up::events_holder_t<real> {
		virtual double get() const = 0;
		virtual void set(double value) = 0;
		virtual std::pair<double, double> limits() const = 0;

		up::holder_signal<void(double)> changing;
		up::holder_signal<void(double)> changed;
		up::holder_signal<void(std::pair<double, double>)> limits_changing;
		up::holder_signal<void(std::pair<double, double>)> limits_changed;
	};

	struct enumeration {
		virtual int get() const = 0;
		virtual void set(int index) = 0;

		virtual std::size_t num_enumerators() const = 0;
		virtual std::string get_id(int index) = 0;
		virtual std::string get_name(int index) = 0;
		virtual std::string get_description(int index) = 0;
	};

	struct boolean {
		virtual bool get() const = 0;
		virtual void set(bool) const = 0;
	};
	
	namespace types {
		using integer_type = static_class<integer>;
		using real_type    = static_class<real>;
		using enum_type    = static_class<enumeration>;
		using boolean_type = static_class<boolean>;
	}

	const types::integer_type& get_meta_type(const integer*);
	const types::real_type&    get_meta_type(const real*);
	const types::enum_type&    get_meta_type(const enumeration*);
	const types::boolean_type& get_meta_type(const boolean*);

}