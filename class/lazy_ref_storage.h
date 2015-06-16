#pragma once
#include <unordered_map>
#include <boost/intrusive_ptr.hpp>
#include "reference.h"

namespace up {

	namespace detail {

		//
		// pairhash: borrowed from http://stackoverflow.com/questions/20590656/error-for-hash-function-of-pair-of-ints
		//           Note: We'll not be storing equal pairs nor will we store swapped pairs, so the pitfalls don't count here.
		struct pairhash {
		public:
			template <typename T, typename U>
			std::size_t operator()(const std::pair<T, U> &x) const
			{
				return std::hash<uintptr_t>()(x.first^x.second);
			}
		};
		

		struct lazy_ref_storage;

		struct lazy_storage_ocb : public ocb {
			using id_type = std::pair< uintptr_t, uintptr_t >;

			lazy_storage_ocb(const meta_type* type, const void* objptr, lazy_ref_storage* store, id_type id, bool register_as_holder);
			~lazy_storage_ocb();

		private:
			id_type id_;
			lazy_ref_storage* store_;
		};
		
		template< typename ProxyType, typename InterfaceType >
		struct lazy_proxy_ocb : public lazy_storage_ocb {			
			template< typename... Args >
			lazy_proxy_ocb( lazy_ref_storage* store, id_type id, Args&&... args )
				: lazy_storage_ocb( lookup_meta_type<InterfaceType>(), static_cast<const InterfaceType*>(&proxy_), store, id, std::is_convertible<InterfaceType, events_holder >::value ), proxy_( std::forward<Args>(args)... )
			{}

			ProxyType* get_proxy()
			{
				return &proxy_;
			}

			~lazy_proxy_ocb()
			{
			}

			ProxyType proxy_;
		};
		
		template<typename T>
		std::pair<uintptr_t, uintptr_t> lazy_id(const void* a)
		{
			return std::pair<uintptr_t, uintptr_t>(reinterpret_cast<uintptr_t>(a), reinterpret_cast<uintptr_t>(type_to_id<T>()));
		}

		inline std::pair<uintptr_t, uintptr_t> lazy_id(const void* a, const void* b)
		{
			return std::pair<uintptr_t, uintptr_t>(reinterpret_cast<uintptr_t>(a), reinterpret_cast<uintptr_t>(b));
		}
		
		//
		// lazy_proxy_storage: (object pointer, member identifier-pointer) -> lazy_storage_ocb
		//
		struct lazy_ref_storage {
			using id_type = std::pair< uintptr_t, uintptr_t >;
			using ptr_type = boost::intrusive_ptr< lazy_storage_ocb >;

		public:
			template< typename InterfaceType >
			ref<InterfaceType> query(const id_type& id) {
				auto it = storage_.find(id);
				if(it!=storage_.end()) {
					return ref<InterfaceType>(it->second, static_cast<InterfaceType*>(const_cast<void*>(it->second->get_ptr())));
				} else {
					return ref<InterfaceType>();
				}
			}
			
			template< typename ProxyType, typename InterfaceType, typename... Args >
			ref<InterfaceType> get(const id_type& id, Args&&... args) {
				auto it = storage_.find(id);
				if(it!=storage_.end()) {
					return ref<InterfaceType>(it->second, const_cast<InterfaceType*>(static_cast<const InterfaceType*>(it->second->get_ptr())));
				} else {
					boost::intrusive_ptr<lazy_proxy_ocb<ProxyType, InterfaceType>> ptr(new lazy_proxy_ocb<ProxyType, InterfaceType>(this, id, std::forward<Args>(args)...));
					auto* objptr = ptr.get();
					storage_.emplace(id, objptr);
					return ref<InterfaceType>(std::move(ptr), const_cast<InterfaceType*>(static_cast<const InterfaceType*>(objptr->get_proxy())) );
				}
			}

			void remove(const id_type& id) {
				auto it = storage_.find(id);
				storage_.erase(it);
			}

			static lazy_ref_storage& get() {
				static lazy_ref_storage storage;
				return storage;
			}

		private:
			std::unordered_map< id_type, lazy_storage_ocb*, pairhash > storage_;
		};

	}

}