#include "observing.h"

namespace up {

	namespace detail {

		struct deleter {
			deleter(const subobject_member_id& id)
				: id_(id)
			{
			}

			void operator()( util::detail::observable3_storage* ptr ) {
				tagged_observables::get().release(id_);
				delete ptr;
			}

			subobject_member_id id_;

		};

		tagged_observables& tagged_observables::get()
		{
			static tagged_observables instance;
			return instance;
		}

		void tagged_observables::release(const subobject_member_id& id)
		{
			storages_.erase(storages_.find(id));
		}

		storage_ptr tagged_observables::get_storage(const subobject_member_id& id)
		{
			auto it = storages_.find(id);
			if(it==storages_.end()) {
				storage_ptr ptr = storage_ptr( new util::detail::observable3_storage, deleter(id) );
				storages_.emplace(id, ptr);
				return ptr;
			} else {
				return it->second.lock();
			}
		}

		storage_ptr tagged_observables::query_storage(const subobject_member_id& id)
		{
			auto it = storages_.find(id);
			if(it==storages_.end()) {
				return storage_ptr();
			} else {
				return it->second.lock();
			}
		}

		//
		// tagged_access
		//

		tagged_access::tagged_access(subobject_member_id id)
			: id_(id)
		{
		}
		
		storage_ptr tagged_access::get_storage()
		{
			return tagged_observables::get().get_storage(id_);
		}

		util::detail::iteration_ref tagged_access::try_get_storage()
		{
			return tagged_observables::get().query_storage(id_);
		}

		util::link tagged_access::subscribe(void* ptr)
		{
			auto storage = tagged_observables::get().get_storage(id_);
			storage->subscribe(ptr);
			return util::link(storage,ptr);
		}

		void tagged_access::unsubscribe(void* ptr)
		{
			tagged_observables::get().get_storage(id_)->unsubscribe(ptr);
		}

	}

}