#pragma once
#include <type_traits>
#include <boost/intrusive_ptr.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include "sequence.h"
#include "meta_type.h"
#include "refcounted.h"

namespace up {

	class meta_type;
	class object_reference;
	class type_member_iterator;

	namespace detail {

		template< typename Interface >
		class fragment_ptr {
		public:
			fragment_ptr()
				: iface_(nullptr)
			{}

			fragment_ptr(Interface* iface)
				: iface_(iface)
			{
			}

			fragment_ptr(const meta_type* type, Interface* iface)
				: iface_(iface)
			{
			}

			Interface* get_ptr() const {
				return iface_;
			}

			const meta_type* type() const {
				return lookup_meta_type<std::decay_t<Interface>>();
			}

			Interface* operator->() const {
				return iface_;
			}

			explicit operator bool() const
			{
				return iface_ != nullptr;
			}

			void reset()
			{
				iface_ = nullptr;
			}


		private:
			Interface* iface_;
		};

		template< >
		class fragment_ptr< void > {
		public:
			fragment_ptr()
				: iface_(nullptr), type_(nullptr)
			{}

			fragment_ptr(const meta_type* type, void* iface)
				: type_(type), iface_(iface)
			{
			}

			template< typename Other >
			fragment_ptr(const fragment_ptr<Other>& ptr)
				: type_(ptr.type()), iface_(ptr.get_ptr())
			{}


			const meta_type* type() const
			{
				return type_;
			}

			void* get_ptr() const
			{
				return iface_;
			}

			void reset()
			{
				iface_ = nullptr;
				type_ = nullptr;
			}


		private:
			const meta_type* type_;
			void* iface_;
		};

		template< >
		class fragment_ptr< const void > {
		public:
			fragment_ptr()
				: iface_(nullptr), type_(nullptr)
			{}

			template< typename Other >
			fragment_ptr(const fragment_ptr<Other>& ptr)
				: type_(ptr.type()), iface_(ptr.get_ptr())
			{}

			fragment_ptr(const meta_type* type, const void* iface)
				: type_(type), iface_(iface)
			{
			}

			const meta_type* type() const
			{
				return type_;
			}

			const void* get_ptr() const {
				return iface_;
			}

			void reset()
			{
				iface_ = nullptr;
				type_ = nullptr;
			}

		private:
			const meta_type* type_;
			const void* iface_;
		};

	}

	namespace detail {

		struct ocb : public fragment_ptr<const void>, public refcounted_virtual {

			ocb(const meta_type* type, const void* ptr)
				: fragment_ptr(type, ptr) /*, refcount_(0)*/
			{}

			virtual ~ocb() {
			}
/*
#ifdef WIN32
			volatile long refcount_;
#else
			unsigned int refcount_;
#endif

			friend void intrusive_ptr_add_ref(ocb* o) {
#ifdef WIN32
				_InterlockedIncrement(&o->refcount_);
#else	
				++o->refcount_;
#endif
			}

			friend void intrusive_ptr_release(ocb* o) {
#ifdef WIN32
				if (_InterlockedDecrement(&o->refcount_) == 0) {
					delete o;
				}
#else
				--o->refcount_;

				if (o->refcount_ == 0) {
					delete o;
				}
#endif				
			}
*/
		};

		struct bare {};

	}

	template< typename Tag >
	class ref;

	namespace detail {
		using fragment_ref = fragment_ptr<void>;
		using const_fragment_ref = fragment_ptr<const void>;
	}

	/*
	 * object_reference - Object reference
	*/

	namespace detail {

		class object_reference {
		public:
			object_reference()
			{}

			template<typename T>
			object_reference(T* ptr)
				: ocb_(new ocb(lookup_meta_type<std::decay_t<T>>(), ptr))
			{}

			object_reference(const meta_type* type, const void* ptr)
				: ocb_(new ocb(type, ptr))
			{}

			object_reference(boost::intrusive_ptr< ocb > ocb)
				: ocb_(std::move(ocb))
			{}

			object_reference(object_reference&& other)
				: ocb_(std::move(other.ocb_))
			{
			}

			object_reference(const object_reference& other)
				: ocb_(other.ocb_)
			{
			}

			~object_reference()
			{
			}

			object_reference& operator=(const object_reference& other)
			{
				ocb_ = other.ocb_;

				return *this;
			}

			object_reference& operator=(object_reference&& other)
			{
				ocb_ = std::move(other.ocb_);

				return *this;
			}

			explicit operator bool() const
			{
				return ocb_ ? true : false;
			}

			const void* get_object_ptr() const
			{
				return ocb_->get_ptr();
			}

			const meta_type* get_object_type() const
			{
				return ocb_->type();
			}

			boost::intrusive_ptr< ocb > get_ocb() const
			{
				return ocb_;
			}

			void reset()
			{
				ocb_.reset();
			}

		protected:
			boost::intrusive_ptr< ocb > ocb_;
		};

	}


	template< typename T >
	class ref : public detail::object_reference, public detail::fragment_ptr<T>
	{
	public:
		ref()
		{}

		ref(T* ptr)
			: object_reference( ptr ), fragment_ptr(ptr)
		{}

		ref(boost::intrusive_ptr< detail::ocb > ocb, fragment_ptr<T> ptr)
			: object_reference( std::move(ocb) ), fragment_ptr( ptr )
		{}

		ref( const ref& objref, const detail::fragment_ptr<T>& fragptr )
			: object_reference( objref ), fragment_ptr( fragptr )
		{}


		template< typename OtherT >
		ref(ref<OtherT>&& other)
			: object_reference( std::move(other) ), fragment_ptr( other )
		{
			other.reset();
		}
		
		template< typename OtherT >
		ref(const ref<OtherT>& other)
			: object_reference(other), fragment_ptr(other)
		{
		}

		ref(const ref& other)
			: object_reference(other), fragment_ptr(other)
		{
		}

		~ref()
		{
		}

		template< typename OtherTag >
		ref& operator=(const ref<OtherTag>& other)
		{
			object_reference::operator=(other);
			fragment_ptr::operator=(other);
		}

		template< typename OtherTag >
		ref& operator=(ref<OtherTag>&& other)
		{
			object_reference::operator=(std::move(other));
			fragment_ptr::operator=(std::move(other));
			other.reset();

			return *this;
		}

		explicit operator bool() const
		{
			return object_reference::operator bool() && fragment_ptr::operator bool();
		}

		template<typename Interface>
		bool is() const {
			return object_reference::get_object_type() == lookup_meta_type<Interface>();
		}

		template<typename Interface, typename Callable>
		void if_(Callable&& cl) const {
			if (is<Interface>()) {
				cl(cast<Interface>(*this));
			}
		}

		template<typename Interface, typename Callable>
		void for_each(Callable&& cl) const
		{
			for (auto member : members()) {
				if (member.is<Interface>()) {
					cl(member.as<Interface>());
				}
			}
		}


		template<typename Interface>
		ref<Interface> as() const {
			return cast<Interface>(*this);
		}

		void reset()
		{
			object_reference::reset();
			fragment_ptr::reset();
		}

		up::sequence< type_member_iterator > members() const;

	};
		
	namespace detail {

		template< typename T >
		struct make_managed_ocb : public ocb {
			template< typename... Args >
			make_managed_ocb(const meta_type* type, Args&&... args)
				: ocb(type, &object_), object_(std::forward<Args>(args)...)
			{}

			T* get_object() {
				return &object_;
			}

			~make_managed_ocb() {}

			T object_;
		};

	}

	ref<void> ref_const_cast(const ref<const void>&);
	ref<void> ref_const_cast(ref<const void>&&);
	
	template< typename T, typename... Args >
	ref<T> make_managed(Args&&... args)
	{
		auto ptr = boost::intrusive_ptr< detail::make_managed_ocb<T> >(new detail::make_managed_ocb<T>(lookup_meta_type<T>(), std::forward<Args>(args)...));
		auto* objptr = ptr->get_object();

		return ref<T>(std::move(ptr), objptr);
	}

	class type_member_iterator : public boost::iterator_facade<type_member_iterator, ref<void>, std::random_access_iterator_tag, ref<void>> {
	public:
		type_member_iterator( ref<void> container, meta_type::member_iterator base )
			: container_( std::move(container) ), base_(std::move(base))
		{}

		const std::string& get_id() const {
			return base_->id();
		}

		const meta_type* get_class() const {
			return &base_->type();
		}

		ref<void> dereference() const
		{
			return base_->bind(container_);
		}

		void advance(std::ptrdiff_t offset)
		{
			base_ += offset;
		}

		bool equal(const type_member_iterator& other) const
		{
			return base_ == other.base_;
		}

		void increment()
		{
			++base_;
		}

		void decrement()
		{
			--base_;
		}

	private:
		ref<void> container_;
		meta_type::member_iterator base_;
	};

	template< typename T >
	up::sequence< type_member_iterator > ref<T>::members() const
	{
		auto seq = type()->members();
		return up::sequence< type_member_iterator >(type_member_iterator(*this, seq.begin()), type_member_iterator(*this, seq.end()));
	}

}

