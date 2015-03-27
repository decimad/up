#pragma once
#include "class.h"
#include "reference.h"

#include <memory>
#include <queue>


namespace up {

	class class_;

	namespace casts {

		namespace detail {

			template< typename Src, typename Dest >
			const void* cast_func(const void* src) {
				return static_cast<const Dest*>(static_cast<const Src*>(src));
			}

		}
		
		using cast_func_type = const void* (*)(const void*);

		void declare_upcast(const class_* a, const class_*b,  cast_func_type ab_cast);
		void declare_downcast(const class_* a, const class_*b, cast_func_type ab_cast);

		template< typename Derived, typename Base >
		void declare_static_base_cast()
		{
			declare_cast( lookup_meta_class<Src>(), lookup_meta_class<Dest>(), &detail::cast_func<Src,Dest>, &detail::cast_func<Dest,Src> );
		}

		void*       do_upcast(const class_* src_class,       void* src_ptr, const class_* dest_class);
		const void* do_upcast(const class_* src_class, const void* src_ptr, const class_* dest_class);

		void*       do_downcast(const class_* src_class,       void* src_ptr, const class_* dest_class);
		const void* do_downcast(const class_* src_class, const void* src_ptr, const class_* dest_class);

		template< typename Class >
		const Class* do_upcast(const class_* src_class, const void* src_ptr)
		{
			return static_cast<const Class*>(do_upcast(src_class, src_ptr, lookup_meta_class<Class>()));
		}

		template< typename Class >
		Class* do_downcast(const class_* src_class, const void* src_ptr)
		{
			return static_cast<Class*>(do_downcast(src_class, src_ptr, lookup_meta_class<Class>()));
		}
		
		std::pair<const class_*, void*> do_mostcast(const class_* cl, void* ptr);
		std::pair<const class_*, const void*> do_mostcast(const class_* cl, const void* ptr);
		
	}
	
}
