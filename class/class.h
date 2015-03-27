#pragma once
#include <cstddef>
#include <typeinfo>
#include <tuple>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "reference.h"
#include <boost/iterator/iterator_facade.hpp>


namespace up {

	class class_ {
	public:
		virtual const char* get_name() const = 0;

		virtual std::size_t num_bases() const = 0;
		virtual const class_* get_base(std::size_t idx) const = 0;

		virtual std::size_t num_members() const = 0;
		virtual const char* member_id(std::size_t idx) const = 0;
		virtual const class_* member_class(std::size_t idx) const = 0;

		virtual object_ref<void> get_member(void* ptr, std::size_t idx) const = 0;
		virtual object_ref<void> get_member(void* ptr, const std::string& id) const = 0;

		object_ref<const void> get_member(const void* ptr, const std::string& id) const {
			return get_member(const_cast<void*>(ptr), id);
		}

		object_ref<const void> get_member(const void* ptr, std::size_t& idx) const {
			return get_member(const_cast<void*>(ptr), idx);
		}

		template< typename T >
		bool is() const {
			return typeid(T) == class_id();
		}
		
		virtual const type_info& class_id() const = 0;
		virtual const type_info& instance_id(const void* instance) const = 0;

	};
		
	void register_class(std::string id, const class_* ptr);
	void register_class(const type_info&, const class_* ptr);

	const class_* lookup_class(const char* name);
	const class_* lookup_class(const type_info& info);


	class member_iterator : public boost::iterator_facade<member_iterator, const mutable_object_ref, std::random_access_iterator_tag, const mutable_object_ref> {
	public:
		const char* get_id() const {
			return cl_->member_id(index_);
		}

		const class_* get_class() const {
			return cl_->member_class(index_);
		}

		mutable_object_ref dereference() const
		{
			return cl_->get_member(object_, index_);
		}

		void advance(std::ptrdiff_t offset)
		{
			index_ += offset;
		}

		void increment()
		{
			++index_;
		}

		void decrement()
		{
			--index_;
		}

	private:
		const class_* cl_;
		void* object_;
		std::size_t index_;
	};

}
