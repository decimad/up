#pragma once
#include <limits>
#include "class.h"
#include "integral.h"
#include "lazy_proxy.h"

#include <iostream>

namespace up {

	namespace adapters {

		namespace detail {
			
			template< typename Class, typename T, T(Class::*Getter)() const, void (Class::*Setter)(T value), bool AutoEvents >
			struct integer_proxy : public integer
			{
			public:
				integer_proxy(Class& ref)
					: ref_(ref)
				{}

				int get() const override {
					return static_cast<int>((ref_.*Getter)());
				}

				void set(int value) override {
					if(AutoEvents) changing.raise(value);
					(ref_.*Setter)(static_cast<T>(value));
					if(AutoEvents) changed.raise(value);
				}

				std::pair<int, int> limits() const override {
					return std::pair<int, int>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
				}

				~integer_proxy() {
					up::detail::lazy_proxy_storage::get().remove(up::detail::lazy_proxy_storage::id_type(&ref_, up::detail::type_to_id<integer_proxy>()));
					std::cout<<"deleting integer proxy...\n";
				}

				Class& ref_;
			};

		}

		template< typename Class, typename T, T(Class::*Getter)() const, void (Class::*Setter)(T value), bool AutoEvents = false >
		using method_integer = up::detail::static_proxy_member< Class, integer, detail::integer_proxy<Class, T, Getter, Setter, AutoEvents> >; 

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
					if(AutoEvents) changing.raise(value);
					(ref_.*Setter)(static_cast<T>(value));
					if(AutoEvents) changed.raise(value);
				}

				std::pair<double, double> limits() const override {
					return std::pair<double, double>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
				}

				~real_proxy() {
					up::detail::lazy_proxy_storage::get().remove(up::detail::lazy_proxy_storage::id_type(&ref_, up::detail::type_to_id<real_proxy>()));
					std::cout<<"deleting real proxy...\n";
				}

				Class& ref_;
			};


		}
		
		template< typename Class, typename T, T(Class::*Getter)() const, void (Class::*Setter)(T value), bool AutoEvents = false >
		using method_real = up::detail::static_proxy_member<Class, real, detail::real_proxy<Class, T, Getter, Setter, AutoEvents > >; // {

	}

}
