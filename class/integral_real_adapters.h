#pragma once
#include <limits>
#include "meta_type.h"
#include "integral.h"
#include "lazy_ref_storage.h"

namespace up {

	namespace adapters {
		
		namespace detail {

			template< typename Class, typename T, T(Class::*Getter)() const, void (Class::*Setter)(T value), bool AutoEvents >
			struct real_proxy : public real
			{
			public:
				real_proxy(Class& ref)
					: ref_(ref)
				{}

				double get() const override {
					return static_cast<double>((ref_.*Getter)());
				}

				void set(double value) {
					if (AutoEvents) changing.raise(value);
					(ref_.*Setter)(static_cast<T>(value));
					if (AutoEvents) changed.raise(value);
				}

				std::pair<double, double> limits() const override {
					return std::pair<double, double>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
				}

				~real_proxy() {
					std::cout << "deleting real proxy...\n";
				}

				using interface_type = real;
				Class& ref_;
			};


		}

		template< typename Class, typename T, T(Class::*Getter)() const, void (Class::*Setter)(T value), bool AutoEvents = false >
		using method_real = up::detail::default_proxy_binder<Class, real, detail::real_proxy<Class, T, Getter, Setter, AutoEvents > >;

	}

}
