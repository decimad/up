#pragma once

#include "static_class.h"
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
	
	namespace classes {
		using integer_class = static_class<integer>;
		using real_class    = static_class<real>;
		using enum_class    = static_class<enumeration>;
		using boolean_class = static_class<boolean>;
	}

	const classes::integer_class& get_meta_class(const integer*);
	const classes::real_class&    get_meta_class(const real*);
	const classes::enum_class&    get_meta_class(const enumeration*);
	const classes::boolean_class& get_meta_class(const boolean*);

}