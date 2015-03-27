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
				return std::hash<uintptr_t>()(reinterpret_cast<const uintptr_t>(x.first))^std::hash<uintptr_t>()(reinterpret_cast<const uintptr_t>(x.second));
			}
		};
		
		//
		// lazy_proxy_storage: (object pointer, member identifier-pointer) -> refcounted_virtual
		//
		struct lazy_proxy_storage {
			using id_type = std::pair< const void* /* Object Pointer */, const void* /* Member Identifier */ >;
			using ptr_type = boost::intrusive_ptr< refcounted_virtual >;

		public:
			template< typename ProxyType, typename ObjectType >
			boost::intrusive_ptr<ProxyType> query(ObjectType& obj, const void* id) {
				auto it = storage_.find(id_type(&obj,id));
				if(it!=storage_.end()) {
					return static_cast<ProxyType*>(it->second);
				} else {
					return boost::intrusive_ptr<ProxyType>();
				}
			}
			
			template< typename ProxyType, typename ObjectType >
			boost::intrusive_ptr<ProxyType> get(ObjectType& obj, const void* id) {
				auto it = storage_.find(id_type(&obj, id));
				if(it!=storage_.end()) {
					return static_cast<ProxyType*>(it->second);
				} else {
					boost::intrusive_ptr<ProxyType> ptr(new ProxyType(obj));
					storage_.emplace(id_type(&obj, id), ptr.get());
					return ptr;
				}
			}

			void remove(const id_type& id) {
				auto it = storage_.find(id);
				storage_.erase(it);
			}

			static lazy_proxy_storage& get() {
				static lazy_proxy_storage storage;
				return storage;
			}

		private:
			std::unordered_map< id_type, refcounted_virtual*, pairhash > storage_;
		};

		template< typename T >
		struct lazy_refcount_storage {

			using ptr_type = boost::intrusive_ptr<T>;

			ptr_type query(uintptr_t id) {
				auto it = storage_.find(id);
				if(it!=storage_.end()) {
					return it->second;
				} else {
					return ptr_type();
				}
			}
			
			template< typename... Args >
			boost::intrusive_ptr<T> get(uintptr_t id, Args&&... args) {
				auto it = storage_.find(id);
				if(it!=storage_.end()) {
					return it->second;
				} else {
					boost::intrusive_ptr<T> ptr(new T(std::forward<Args>(args)...));
					storage_.emplace(id, ptr.get());
					return ptr;
				}
			}

			static lazy_refcount_storage& get() {
				static lazy_refcount_storage storage;
				return storage;
			}

			std::unordered_map< uintptr_t, T*> storage_;
		};



	}

}