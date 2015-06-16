#pragma once
#include <limits>
#include "meta_type.h"
#include "integral.h"
#include "lazy_ref_storage.h"

namespace up {

	namespace adapters {

		template< typename EnumType >
		std::pair< EnumType, std::string > enum_field(EnumType value, std::string label)
		{
			return std::pair< EnumType, std::string >(value, std::move(label));
		}

		template< typename EnumType >
		struct static_enumeration_set : public enumeration_set
		{
		public:
			template< typename... Members >
			static_enumeration_set(Members&&... members)
			{
				declare_members(std::forward<Members>(members)...);
			}

			EnumType to_value(const enumeration_member& val) const
			{
				return member_values_[val.index()];
			}

			const std::string& to_label(EnumType val) const
			{
				return (*(begin() + value_lookups_[val])).label();
			}

			const size_t& to_index() const
			{
				return (*(begin() + value_lookups_[val])).index();
			}

			const enumeration_member& to_member(EnumType val) const
			{
				auto it = value_lookups_.find(val);
				return *(begin() + it->second);
			}

		private:
			// getting pair<EnumValue, std::string>'s
			template< typename Arg0, typename... Args >
			void declare_members(Arg0&& arg0, Args&&... args)
			{
				auto index = member_values_.size();
				member_values_.emplace_back(arg0.first);
				value_lookups_.emplace(arg0.first, index);
				enumeration_set::declare_member(std::move(arg0.second));
				declare_members(std::forward<Args>(args)...);
			}

			void declare_members()
			{}

			using values_type = std::vector< EnumType >;
			values_type member_values_;

			using lookup_type = std::unordered_map<EnumType, size_t>;
			lookup_type value_lookups_;
		};

		namespace detail {

			template< typename ClassType, typename EnumType, typename ProxyType >
			struct enum_proxy_binder : public up::detail::static_proxy_binder<ClassType, enumeration, ProxyType >
			{
				using enum_set_type = static_enumeration_set<EnumType>;

				using static_proxy_binder::static_proxy_binder;

				enum_proxy_binder(std::string id, up::member::visibility_types vis, const enum_set_type* type)
					: static_proxy_binder(std::move(id), vis), type_(type)
				{}

				// called by static_member
				ref<const void> bind(const ref<const void>& obj) const override {
					auto& storage = up::detail::lazy_ref_storage::get();
					auto* objptr = const_cast<void*>(obj.get_ptr());

					return storage.get<proxy_type, interface_type>( up::detail::lazy_id<proxy_type>(objptr), *static_cast<container_type*>(objptr), type_);
				}

				const enum_set_type* type_;
			};

			template< typename Class, typename EnumType, EnumType(Class::*Getter)() const, void (Class::*Setter)(EnumType value), bool AutoEvents >
			struct enum_proxy : public enumeration
			{
			public:
				using enum_set_type = static_enumeration_set<EnumType>;

				enum_proxy(Class& ref, const enum_set_type* members)
					: ref_(ref), members_(members)
				{}

				const enumeration_member& get() const override {
					return members_->to_member((ref_.*Getter)());
				}

				void set(const enumeration_member& value) {

					auto val = members_->to_value(value);

					if (AutoEvents) changing.raise(value);
					(ref_.*Setter)(val);
					if (AutoEvents) changed.raise(value);
				}

				const enumeration_set& values() const override {
					return *members_;
				}

				~enum_proxy() {
					std::cout << "deleting enum proxy...\n";
				}

				using interface_type = enumeration;
				const enum_set_type* members_;
				Class& ref_;
			};

		}

		template< typename Class, typename EnumType, EnumType(Class::*Getter)() const, void (Class::*Setter)(EnumType value), bool AutoEvents = false >
		using method_enum = detail::enum_proxy_binder<Class, EnumType, detail::enum_proxy<Class, EnumType, Getter, Setter, AutoEvents > >;
		
	}
}