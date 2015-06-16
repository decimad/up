#pragma once
#include <limits>
#include "meta_type.h"
#include "integral.h"
#include "lazy_ref_storage.h"

namespace up {

	namespace adapters {
		
		namespace detail {

			template< typename Class, typename T, T(Class::*Getter)() const, void (Class::*Setter)(T value), bool (Class::*Validator)(T value) const, bool AutoEvents >
			struct static_integer_proxy : public integer
			{
			public:
				static_integer_proxy(Class& ref)
					: ref_(ref)
				{}

				int get() const override {
					return static_cast<int>((ref_.*Getter)());
				}

				void set(int value) override {
					if (AutoEvents) changing.raise(value);
					(ref_.*Setter)(static_cast<T>(value));
					if (AutoEvents) changed.raise(value);
				}

				bool validate(int value) const override {
					if (Validator) {
						return (ref_.*Validator)(static_cast<T>(value));
					}
					else {
						return true;
					}
				}

				std::pair<int, int> limits() const override {
					return std::pair<int, int>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
				}

				~static_integer_proxy() {
					std::cout << "deleting integer proxy...\n";
				}

				using interface_type = integer;
				Class& ref_;
			};

			template< typename Class, typename T, bool AutoEvents >
			struct functional_integer_proxy : public integer
			{
			public:
				using getter_type = std::function<T()>;
				using setter_type = std::function<void(T)>;
				using validator_type = std::function<bool(T)>;
				
				functional_integer_proxy(getter_type getter, setter_type setter, validator_type validator)
					: getter_(getter), setter_(setter), validator_(validator)
				{}

				int get() const override {
					return static_cast<int>(getter_());
				}

				void set(int value) override {
					if (AutoEvents) changing.raise(value);
					setter_(static_cast<T>(value));
					if (AutoEvents) changed.raise(value);
				}

				bool validate(int value) const override {
					if (validator_) {
						return validator_(static_cast<T>(value));
					}
					else {
						return true;
					}
				}

				std::pair<int, int> limits() const override {
					return std::pair<int, int>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
				}

				~functional_integer_proxy() {
					std::cout << "deleting integer proxy...\n";
				}

				std::function<T()> getter_;
				std::function<void(T)> setter_;
				std::function<bool(T)> validator_;

				using interface_type = integer;
			};



		}

		template< typename Class, typename T, T(Class::*Getter)() const, void (Class::*Setter)(T value), bool (Class::*Validator)(T value) const = nullptr, bool AutoEvents = false >
		using static_method_integer = up::detail::default_proxy_binder< Class, integer, detail::static_integer_proxy<Class, T, Getter, Setter, Validator, AutoEvents> >;


	}

}