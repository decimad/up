#pragma once
#include <type_traits>
#include <boost/intrusive_ptr.hpp>
#include "refcounted.h"

namespace up {

	class type;

	template< typename Tag, typename T >
	class typed_ref;

	template< typename Tag >
	class object_ref;

	using const_object_ref = object_ref<const void>;
	using mutable_object_ref = object_ref<void>;

	template< typename T >
	class object_ref
	{
	public:
		object_ref()
			: cl_(nullptr), ptr_(nullptr), lt_(nullptr)
		{}


		object_ref(const type* cl, T* ptr, boost::intrusive_ptr<refcounted_virtual> lt = nullptr )
			: cl_(cl), ptr_(ptr), lt_(lt)
		{}

		template< typename OtherT >
		object_ref(object_ref<OtherT>&& other)
			: cl_(other.cl_), ptr_(other.ptr_), lt_(std::move(other.lt_))
		{
			other.ptr_ = nullptr;
			other.cl_ = nullptr;
		}
		
		template< typename OtherT >
		object_ref(const object_ref<OtherT>& other)
			: cl_(other.cl_), ptr_(other.let_), lt_(other.lt_)
		{
		}

		object_ref(const object_ref& other)
			: cl_(other.cl_), ptr_(other.ptr_), lt_(other.lt_)
		{
		}

		~object_ref()
		{
		}

		template< typename OtherTag >
		object_ref& operator=(const object_ref<OtherTag>& other)
		{
			cl_ = other.cl_;
			lt_ = other.lt_;
			ptr_ = other.ptr_;
		}

		template< typename OtherTag >
		object_ref& operator=(object_ref<OtherTag>&& other)
		{
			cl_ = other.cl_;
			lt_ = std::move(other.lt_);
			ptr_ = other.ptr_;

			return *this;
		}

		explicit operator bool() const
		{
			return ptr_!=nullptr;
		}

		T* get_ptr() const
		{
			return ptr_;
		}

		T* operator->() const
		{
			return ptr_;
		}

		const type* get_class() const
		{
			return cl_;
		}

		size_t num_members() const
		{
			return cl_->num_members();
		}

		object_ref<typename std::conditional<std::is_const<T>::value, const void, void>::type>
		get_member(size_t idx) const
		{
			return cl_->get_member(const_cast<void*>(ptr_), idx);
		}
		
		const char* member_id(size_t idx) const
		{
			return cl_->member_id(idx);
		}

		void adjust(const type* newclass, const void* newptr)
		{
			cl_ = newclass;
			ptr_ = newptr;
		}

		boost::intrusive_ptr< refcounted_virtual > get_lifetime() const
		{
			return lt_;
		}

		void reset()
		{
			lt_.reset();
			cl_ = nullptr;
			ptr_ = nullptr;
		}

	protected:
		template< typename OtherTag >
		friend class object_ref;

		const type* cl_;
		T* ptr_;
		boost::intrusive_ptr< refcounted_virtual > lt_;
	};

	using const_object = object_ref<const void>;
	using object = object_ref<void>;

}

