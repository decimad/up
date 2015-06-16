#pragma once
#include "meta_type.h"
#include "meta.h"
#include "reference.h"

namespace up {

	//
	// static_meta_class: get the metaclass type of a data type
	//

	namespace detail {

		template< typename Class >
		struct static_meta_class {
			using type = std::decay_t< decltype(get_meta_type(&std::declval<const Class>())) >;
		};

	}

	template< typename Class >
	using static_meta_type_t = typename detail::static_meta_class<Class>::type;

	//
	// lookup_meta_class: lookup the runtime metaclass object of a data type
	//
	template< typename Class >
	type_ptr lookup_meta_type() {
		return &get_meta_type(static_cast<const Class*>(nullptr));
	}

	template< typename ClassType >
	struct static_member : public up::member
	{
		using member::member;
	};
	
	//
	// bases
	//
	template< typename... Bases >
	struct bases {};

	namespace detail {

		template< typename Types >
		struct to_meta_type {
			using type = static_meta_type_t<Types>;
		};

		template< typename StaticMetaType >
		struct get_all_bases {
			using type = typename StaticMetaType::all_bases;
		};

		template< typename BaseList >
		struct deep_bases;

		template< typename Base0, typename... Bases >
		struct deep_bases< bases<Base0, Bases...> >
		{
			using bases_meta_types = meta::transform_list_t<bases<Base0, Bases...>, to_meta_type, meta::list>;  // bases< A, ... > -> list< A_Class, ... >
			using all_bases_list   = meta::transform_list_t< bases_meta_types, get_all_bases, bases>; // classes< A_Class, ... > -> bases< bases< A_Class_Bases...>, ... >
			using flat_bases_list  = meta::apply_t< all_bases_list, meta::union_list >;               // bases< bases< A_Class_Bases...>, ... > -> bases< A_Class_Bases..., B_Class_Bases..., ... >

			using type = meta::union_list_t< bases< Base0, Bases... >, flat_bases_list >;
		};

		template< >
		struct deep_bases< bases<> >
		{
			using type = bases<>;
		};

	}

	namespace detail {

		template< typename Parent, typename Child >
		class static_type_fragment : public type_fragment {
		public:
			const meta_type& type() const override {
				return *lookup_meta_type<Child>();
			}

			const std::string& name() const {
				return type().id();
			}

			ref<void> bind(const ref<void> & obj) override
			{
				return ref<void>(obj, fragment_ptr<void>( lookup_meta_type<Child>(), static_cast<Child*>(static_cast<Parent*>(obj.get_ptr()))) );
			}

			ref<const void> bind(const ref<const void>& obj) override
			{
				return ref<const void>(obj, fragment_ptr<const void>( lookup_meta_type<Child>(), static_cast<const Child*>(static_cast<const Parent*>(obj.get_ptr()))) );
			}
		};
	
		template< typename Proxy, typename Interface >
		struct automatic_proxy_ocb : public ocb
		{
			template< typename... Args >
			automatic_proxy_ocb(object_reference container, Args&&... args)
				: ocb(lookup_meta_type<Interface>(), static_cast<Interface*>(proxy_)), container_(std::move(container)), proxy_(std::forward<Args>(args)...)
			{
			}

			object_reference container_;
			Proxy proxy_;
		};

	}
		
	namespace detail {

		template< typename ClassType, typename InterfaceType, typename ProxyType >
		struct default_proxy_binder;

	}

	//
	// static_class
	//
	template< typename ClassType >
	class static_class : public meta_type {
	public:

		template<typename... Args>
		static_class(std::string name, Args&&... args)
			: meta_type(std::move(name))
		{
			interpret(std::forward<Args>(args)...);
		}

		template<typename Arg0, typename... Args>
		void interpret(Arg0&& arg0, Args&&... args)
		{
			interpret1(std::forward<Arg0>(arg0));
			interpret(std::forward<Args>(args)...);
		}

		void interpret()
		{
		}
		
		// Binders must be movable/copy constructible
		template<typename T>
		std::enable_if_t<std::is_convertible<T, up::static_member<ClassType>>::value> interpret1(T&& binder)
		{
			declare_member(std::make_unique<std::decay_t<T>>(std::forward<T>(binder)));
		}

		template< typename... Bases >
		void interpret1(const up::bases<Bases...>& b)
		{
			make_fragments(up::bases<Bases...>());
			make_casts(up::bases<Bases...>());
		}
				
	private:
		template< typename Base0, typename... Bases >
		void make_casts(const bases<Base0, Bases...>&) {
			const auto *base_class = lookup_meta_type<Base0>();
	
			// splitting casts into down and upcasts avoids checks for circularity
			casts::declare_downcast(this, base_class, &casts::detail::cast_func<ClassType, Base0>);
			casts::declare_upcast(base_class, this, &casts::detail::cast_func<Base0, ClassType>);

			make_casts(bases<Bases...>());
		}

		void make_casts(bases<>)
		{
		}

		template< typename Base0, typename... Bases >
		void make_fragments(const bases<Base0, Bases...>&)
		{
			declare_fragment(std::make_unique<detail::static_type_fragment<ClassType,Base0>>());
			make_fragments(bases<Bases...>());
		}

		void make_fragments(const bases<>&)
		{
		}

		template< typename Member0, typename... Tail >
		void make_members(meta::list<Member0, Tail...>)
		{
			declare_member(std::make_unique<Member0>());

			make_members(meta::list<Tail...>());
		}

		void make_members( meta::list<> )
		{}

		/*
		template< typename T >
		static T construct() {
			return T();
		}
		*/

		std::string name_;
	};

	namespace detail {

		template< typename T >
		struct is_integer_type {
			static const bool value =
				std::is_same<T, unsigned int>::value ||
				std::is_same<T, int>::value ||
				std::is_same<T, unsigned short>::value ||
				std::is_same<T, short>::value;
		};

	}



	template< typename ClassType >
	class class_ {
	public:
		class_(const std::string& name) {
			get_instance().set_name(name);
			register_type(typeid(ClassType), &get_instance());
			register_type(name, &get_instance());
		}

		template< typename... Bases >
		class_& bases()
		{
			declare_bases( up::bases<Bases...>() );
			return *this;
		}
		
		template< typename T >
		std::enable_if_t<std::is_convertible<T, static_member<ClassType>>::value, class_&> member(T&& mem)
		{
			get_instance().declare_member(std::make_unique<std::decay_t<T>>(std::forward<T>(mem)));
			return *this;
		}

		template< typename T >
		std::enable_if_t<detail::is_integer_type<T>::value, class_&>
			member(std::string name, member::visibility_types vis, T(ClassType::*Getter)() const, void (ClassType::*Setter)(T), bool (ClassType::*Validator)(T) const = nullptr)
		{
			return *this;
		}

		static meta_type& get_instance() {
			static meta_type the_type;
			return the_type;
		}

	private:
		template< typename Base0, typename... Bases >
		void declare_bases( up::bases<Base0, Bases... > ) {
			get_instance().declare_fragment(std::make_unique<detail::static_type_fragment<ClassType, Base0>>());
			const auto *base_class = lookup_meta_type<Base0>();
			casts::declare_downcast(&get_instance(), base_class, &casts::detail::cast_func<ClassType, Base0>);
			casts::declare_upcast(base_class, &get_instance(), &casts::detail::cast_func<Base0, ClassType>);

			declare_bases( up::bases<Bases...>() );
		}

		void declare_bases( up::bases<> )
		{}

	};

#define EXPOSE_CLASS(Type) \
	const up::meta_type& get_meta_type(const Type*) { return up::class_<Type>::get_instance(); }
	
	/*
		Casting support
	*/

	//
	// make_ref
	//
	template< typename T >
	ref<T> make_ref(T& obj) {
		return ref<T>(&obj);
	}

	//
	// make_most_ref
	//
	template< typename T >
	ref<typename std::conditional<std::is_const<T>::value, const void, void>::type>
		make_most_ref(T& obj)
	{
		auto ptr = &obj;
		const type* cl = lookup_class(typeid(obj));
		const type* source = lookup_meta_class<T>();

		if(!cl) {
			cl = source;
		}

		return ref<typename std::conditional<std::is_const<T>::value, const void, void>::type>(cl, casts::do_upcast(source, ptr, cl));
	}

	//
	// to_mostref
	//
	template< typename T >
	ref<typename std::conditional<std::is_const<T>::value, const void, void>::type>
		to_mostref(const ref<T>& ref)
	{
		auto mostclass = lookup_class(ref->get_class()->instance_id(ref->get_ptr()));
		return ref<typename std::conditional<std::is_const<T>::value, const void, void>::type>(mostclass, casts::do_upcast(ref->get_class(), ref->get_ptr(), mostclass), ref.get_lifetime() );
	}
	
	//
	// cast: cheap cast which will not cast to the most derived type.
	//

	namespace detail {

		// If source type is non-const, allow const_cast, otherwise don't!
		template< typename SourceType, typename DestType >
		DestType* const_cast_helper( const void* ptr ) {
			static_assert(std::is_const<DestType>::value || !std::is_const<SourceType>::value, "Bad conversion!");
			return static_cast<DestType*>(const_cast<void*>(ptr));
		}

	}


	template<typename DestType, typename SrcType>
	ref<DestType> cast(const ref<SrcType>& src) {
		const auto* destclass = lookup_meta_type<DestType>();
		auto* ptr = casts::do_downcast(src.get_object_type(), src.get_object_ptr(), destclass);
		return ref<DestType>(src.get_ocb(), detail::fragment_ptr<DestType>(destclass, detail::const_cast_helper<SrcType, DestType>(ptr)));
	}

	//
	// cast_full: cast one object reference type to another by first casting to most derived type and then back.
	//
	template<typename DestType, typename SrcType>
	ref<DestType> cast_full(const ref<SrcType>& src) {
		const auto* destclass = lookup_meta_type<DestType>();
		auto* ptr = casts::do_downcast(src.get_object_type(), src.get_object_ptr(), destclass);
		
		return ref<DestType>(src.get_ocb(), static_cast<DestType*>(const_cast<void*>(ptr)));
	}
	
	namespace detail {

		//
		// type_to_id: get unique "const void*" per type
		//
		template< typename T >
		const void* type_to_id()
		{
			static char val;
			return &val;
		}

		//
		// static_proxy_binder: helper class for member binders which hand out proxies that only need an object reference (of ClassType) at runtime.
		//				 Note: There must be only one proxy instance per member of an object at any one time.
		//                     That's why we use lazy_proxy_storage to keep "weak" references around.
		//			     Note: If the proxy or its interface has events then active links will keep the proxy alive as well, even
		//                     if all object references expired.
		//
		template< typename ClassType, typename InterfaceType, typename ProxyType >
		struct static_proxy_binder : public up::static_member<ClassType> {

			using container_type = ClassType;
			using interface_type = InterfaceType;
			using proxy_type = ProxyType;
			using class_type = ClassType;

			static_proxy_binder(std::string id, up::member::visibility_types vis)
				: static_member(std::move(id), *lookup_meta_type<interface_type>(), vis)
			{}

			static_proxy_binder(const static_proxy_binder&) = default;
			static_proxy_binder(static_proxy_binder&&) = default;

			static ref<interface_type> query_ref(const class_type& obj)
			{
				auto& storage = up::detail::lazy_ref_storage::get();
				return storage.query<interface_type>( up::detail::lazy_id<proxy_type>(&obj) );
			}

			template< typename Signal, typename... Args >
			static void raise(class_type* obj, Signal(interface_type::*SignalPtr), Args&&... args)
			{
				auto iface_ref = query_ref(*obj);
				if (iface_ref) {
					((iface_ref.get_ptr())->*SignalPtr).raise(std::forward<Args>(args)...);
				}
			}
		
		};

		template< typename ClassType, typename InterfaceType, typename ProxyType >
		struct default_proxy_binder : public static_proxy_binder<ClassType, InterfaceType, ProxyType> {

			using static_proxy_binder::static_proxy_binder;

			// called by static_member
			ref<const void> bind(const ref<const void>& obj) const override {
				auto& storage = up::detail::lazy_ref_storage::get();
				auto* objptr = const_cast<void*>(obj.get_ptr());

				return storage.get<proxy_type, interface_type>( detail::lazy_id<proxy_type>(objptr), *static_cast<container_type*>(objptr) );
			}

		};

		template< typename ClassType, typename InterfaceType, typename ProxyType, typename FunctionalGetterType, typename FunctionalSetterType, typename FunctionalValidatorType >
		struct default_functional_binder : public static_member<ClassType> {
			default_functional_binder(FunctionalGetterType getter, FunctionalSetterType setter, FunctionalValidatorType validator)
				: getter_(getter), setter_(setter), validator_(validator)
			{}

		};

	}
}