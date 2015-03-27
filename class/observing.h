#pragma once
#include <unordered_map>
#include "observable.h"

namespace up {

	using subobject_member_id = std::pair< const void* /* Class Pointer */, const void* /* Tag */ >;

	namespace detail {

		template< typename T >
		struct tag_struct {
			static char tag;
		};

		template< typename T >
		char tag_struct<T>::tag;
		
		


		struct pairhash {
		public:
			template <typename T, typename U>
			std::size_t operator()(const std::pair<T, U> &x) const
			{
				return std::hash<uintptr_t>()(reinterpret_cast<uintptr_t>(x.first))^std::hash<uintptr_t>()(reinterpret_cast<uintptr_t>(x.second));
			}
		};

		using storage_ptr = std::shared_ptr< util::detail::observable3_storage >;
		using storage_weak_ptr = std::weak_ptr< util::detail::observable3_storage >;

		struct tagged_observables {
			storage_ptr get_storage(const subobject_member_id& id);
			storage_ptr query_storage(const subobject_member_id& id);
			void release(const subobject_member_id&);
			
			static tagged_observables& get();
	
		private:
			std::unordered_map< subobject_member_id, storage_weak_ptr, pairhash > storages_;
		};

		class tagged_access
		{
		public:
			tagged_access(subobject_member_id id);

			storage_ptr get_storage();
			util::detail::iteration_ref try_get_storage();

			util::link subscribe(void* ptr);
			void unsubscribe(void* ptr);

		private:
			subobject_member_id id_;
		};

	}

	template< typename TagType, typename T >
	subobject_member_id make_tag(const T* obj)
	{
		return subobject_member_id(obj, &detail::tag_struct<TagType>::tag);
	}

	template< typename Iface >
	using tagged_observable = util::observable3<Iface, detail::tagged_access>;

}