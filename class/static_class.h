#pragma once
#include "type.h"

namespace up {

	//
	// static_meta_class: get the metaclass type of a data type
	//
	template< typename Class >
	using static_meta_class = typename std::remove_const<typename std::remove_reference<decltype(get_meta_class(static_cast<const Class*>(nullptr)))>::type>::type;

	//
	// lookup_meta_class: lookup the runtime metaclass object of a data type
	//
	template< typename Class >
	const type* lookup_meta_class() {
		return &get_meta_class(static_cast<const Class*>(nullptr));
	}

	namespace detail {

		//
		// member
		//
		template< typename T >
		struct member {
			virtual const char* id() = 0;
			virtual const type* cl() = 0;
			virtual object_ref<void> make_ref(T& obj) = 0;
			virtual ~member() {}
		};

		//
		// member_t
		//
		template< typename T, typename Member >
		struct member_t : public member<T> {
			const char* id() {
				return Member::member_id();
			}

			const type* cl()
			{
				return Member::get_class();
			}

			object_ref<void> make_ref(T& obj) {
				return Member::make_ref(obj);
			}
		};

	}

	//
	// static_member
	//
	template< const char* Id, typename Binder >
	struct static_member : public Binder {
		static const char* member_id() {
			return Id;
		}

		static const type* member_class() {
			return Binder::get_class();
		}
	};

	//
	// bases
	//
	template< typename... Bases >
	struct bases {};

	//
	// static_class
	//
	template< typename T, typename BaseList = up::bases<>, typename... Members >
	class static_class : public type {
	public:
		const char* get_name() const override {
			return name_;
		}

		const type_info& type_id() const override {
			return typeid(T);
		}

		const type_info& instance_id(const void* instance) const {
			return typeid(*const_cast<T*>(static_cast<const T*>(instance)));
		}

		std::size_t num_bases() const override
		{
			return bases_.size();	// ...
		}

		const type* get_base(std::size_t idx) const override
		{
			return bases_[idx];
		}

		const char* member_id(std::size_t idx) const override {
			return index_lookup_[idx]->id();
		}

		const type* member_class(std::size_t idx) const override {
			return index_lookup_[idx]->cl();
		}

		std::size_t num_members() const override {
			return sizeof...(Members);
		}

		object_ref<void> get_member(void* ptr, std::size_t idx) const override {
			return index_lookup_[idx]->make_ref(*static_cast<T*>(ptr));
		}

		object_ref<void> get_member(void* ptr, const std::string& id) const override {
			auto it = id_lookup_.find(id);
			if(it!=id_lookup_.end()) {
				return it->second->make_ref(*static_cast<T*>(ptr));
			} else {
				return object_ref<detail::internal_tag>(nullptr, nullptr);
			}
		}

		static_class(const char* name)
			: name_(name)
		{
			register_type(name, this);
			register_type(typeid(T), this);
			add_lookups(construct<Members>()...);
			make_casts(BaseList());
		}

	private:
		template< typename Base0, typename... Bases >
		void make_casts(const bases<Base0, Bases...>&) {
			const auto *base_class = lookup_meta_class<Base0>();
			bases_.push_back(base_class);

			// splitting casts into down and upcasts avoids checks for circularity
			casts::declare_downcast(this, base_class, &casts::detail::cast_func<T, Base0>);
			casts::declare_upcast(base_class, this, &casts::detail::cast_func<Base0, T>);

			make_casts(bases<Bases...>());
		}

		void make_casts(bases<>) {}

		template< typename T >
		static T construct() {
			return T();
		}

		template< typename Arg0, typename... Args >
		void add_lookups(Arg0&& arg0, Args&&... args) {
			index_lookup_.emplace_back(std::make_unique<detail::member_t<T, Arg0>>());
			id_lookup_.emplace(
				std::make_pair(std::string(arg0.member_id()), std::make_unique<detail::member_t<T, Arg0>>())
				);

			add_lookups(std::forward<Args>(args)...);
		}

		static void add_lookups()
		{}

		std::map< std::string, std::unique_ptr<detail::member<T>> > id_lookup_;
		std::vector< std::unique_ptr<detail::member<T>> > index_lookup_;
		std::vector< const type* > bases_;
		const char* name_;
	};

	/*
		Casting support
	*/

	//
	// make_ref
	//
	template< typename T >
	object_ref<T> make_ref(T& obj) {
		return object_ref<T>(lookup_meta_class<typename std::remove_const<T>::type>(), &obj);
	}

	//
	// make_most_ref
	//
	template< typename T >
	object_ref<typename std::conditional<std::is_const<T>::value, const void, void>::type>
		make_most_ref(T& obj)
	{
		auto ptr = &obj;
		const type* cl = lookup_class(typeid(obj));
		const type* source = lookup_meta_class<T>();

		if(!cl) {
			cl = source;
		}

		return object_ref<typename std::conditional<std::is_const<T>::value, const void, void>::type>(cl, casts::do_upcast(source, ptr, cl));
	}

	//
	// to_mostref
	//
	template< typename T >
	object_ref<typename std::conditional<std::is_const<T>::value, const void, void>::type>
		to_mostref(const object_ref<T>& ref)
	{
		auto mostclass = lookup_class(ref->get_class()->instance_id(ref->get_ptr()));
		return object_ref<typename std::conditional<std::is_const<T>::value, const void, void>::type>(mostclass, casts::do_upcast(ref->get_class(), ref->get_ptr(), mostclass), ref.get_lifetime() );
	}
	
	//
	// cast: cheap cast which will not cast to the most derived type.
	//
	template<typename DestType, typename SrcType>
	object_ref<DestType> cast(const object_ref<SrcType>& src) {
		const auto* destclass = lookup_meta_class<DestType>();
		auto* ptr = casts::do_downcast(src.get_class(), src.get_ptr(), destclass);
		return object_ref<DestType>(destclass, static_cast<DestType*>(ptr), src.get_lifetime());
	}

	//
	// cast_full: cast one object reference type to another by first casting to most derived type and then back.
	//
	template<typename DestType, typename SrcType>
	object_ref<DestType> cast_full(const object_ref<SrcType>& src) {
		const auto* destclass = lookup_meta_class<DestType>();
		auto pair = casts::do_mostcast(src.get_class(), src.get_ptr());
		auto* ptr = casts::do_downcast(pair.first, pair.second, destclass);
		
		return object_ref<DestType>(destclass, static_cast<DestType*>(const_cast<void*>(ptr)), src.get_lifetime());
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
		// proxy_member: helper class for member binders which hand out proxies that only need an object reference (of ClassType) at runtime.
		//				 Note: There must be only one proxy instance per member of an object at any one time.
		//                     That's why we use lazy_proxy_storage to keep "weak" references around.
		//			     Note: If the proxy or its interface has events then active links will keep the proxy alive as well, even
		//                     if all object references expired.
		//
		template< typename ClassType, typename InterfaceType, typename ProxyType >
		struct static_proxy_member {

			using interface_type = InterfaceType;
			using proxy_type = ProxyType;
			using typetype = ClassType;

			static const type* get_class()
			{
				return lookup_meta_class<InterfaceType>();
			}

			static boost::intrusive_ptr<proxy_type> query_ref(typetype& obj)
			{
				return up::detail::lazy_proxy_storage::get().query<proxy_type>(obj, up::detail::type_to_id<proxy_type>());
			}

			template< typename Signal, typename... Args >
			static void raise(typename typetype* obj, Signal(interface_type::*SignalPtr), Args&&... args)
			{
				auto ptr = query_ref(*obj);
				if(ptr) {
					(static_cast<interface_type*>(ptr.get())->*SignalPtr).raise(std::forward<Args>(args)...);
				}
			}

			//
			// called by static_class
			//
			static object_ref<void> make_ref(typename typetype& obj) {
				auto ptr = up::detail::lazy_proxy_storage::get().get<proxy_type>(obj, up::detail::type_to_id<proxy_type>());
				return object_ref<interface_type>(lookup_meta_class<interface_type>(), ptr.get(), ptr);
			}
		};

	}
}