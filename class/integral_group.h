#pragma once
#include "reference.h"
#include "any_iterator.hpp"
#include "integral.h"
#include "lazy_ref_storage.h"
#include "member_container.h"

namespace up {

	struct group_member_ref_iterator;
	struct group_member_iterator;
	
	struct group {

		using internal_group_iterator = util::any_iterator<up::member, std::forward_iterator_tag>;
		
		using mem_iterator = internal_group_iterator;
		using ref_iterator = group_member_ref_iterator;

		using ref_sequence_type = up::sequence<ref_iterator>;
		using mem_sequence_type = up::sequence<mem_iterator>;

		virtual mem_sequence_type members() const = 0;
		virtual ref_sequence_type refs() const = 0;

		template<typename Interface, typename Callable>
		void for_each(Callable&& cl) const
		{
			for (auto ref : refs()) {
				if (ref.is<Interface>()) {
					cl(ref.as<Interface>());
				}
			}
		}
		
	private:

	};

	struct group_member_ref_iterator : public boost::iterator_facade<group_member_ref_iterator, ref<void>, std::forward_iterator_tag, ref<void>> {
	public:
		group_member_ref_iterator(group::mem_iterator iter, ref<void> container)
			: iter_(std::move(iter)), container_(std::move(container))
		{
		
		}

		ref<void> dereference() const
		{
			auto result = iter_->bind(container_);
			
			return result;
		}

		void increment() {
			++iter_;
		}

		bool equal(const group_member_ref_iterator& other) const
		{
			return iter_ == other.iter_;
		}
		
	private:
		group::internal_group_iterator iter_;
		ref<void> container_;
	};

	namespace adapters {

		namespace detail {

			template< typename Class, unsigned int Disambigue >
			struct static_group_proxy : public group {
				static_group_proxy(Class& obj, ref<Class> cref, const member_container* members)
					: container_(cref), members_(members)
				{}

				mem_sequence_type members() const override
				{
					return members_->members();
				}

				ref_sequence_type refs() const override
				{
					return to_sequence(ref_iterator(members().begin(), container_), ref_iterator(members().end(), container_));
				}

			private:
				ref<Class> container_;
				const member_container* members_;
			};


			template< typename ClassType, unsigned int Disambigue >
			struct group_proxy_binder : public up::detail::static_proxy_binder<ClassType, group, static_group_proxy<ClassType, Disambigue> >
			{
				using proxy_type = static_group_proxy<ClassType, Disambigue>;
				using static_proxy_binder::static_proxy_binder;

				template<typename... Members>
				group_proxy_binder(std::string id, up::member::visibility_types vis, Members&&... members)
					: static_proxy_binder(std::move(id), vis), group_members_(std::forward<Members>(members)...)
				{}

				group_proxy_binder(group_proxy_binder&&) = default;

				// called by static_member
				ref<const void> bind(const ref<const void>& obj) const override {
					auto& storage = up::detail::lazy_ref_storage::get();
					auto* objptr = const_cast<void*>(obj.get_ptr());
					auto mref = up::cast<ClassType>(up::ref_const_cast(obj));

					return storage.get<proxy_type, group>( up::detail::lazy_id<proxy_type>(objptr), *static_cast<ClassType*>(objptr), mref, &group_members_ );
				}

				template< typename T >
				std::enable_if_t<std::is_convertible<T, static_member<ClassType>>::value, group_proxy_binder&&> member(T&& mem) &&
				{
					group_members_.declare_members(std::forward<T>(mem));
					return std::move(*this);
				}

				static_member_container group_members_;
			};

		}

		template< typename Class, unsigned int Disambigue = 0 >
		using static_group = detail::group_proxy_binder<Class, Disambigue>;
		
	}

	namespace types {
		using group_type = static_class<group>;
	}

	const types::group_type&    get_meta_type(const group*);

}
