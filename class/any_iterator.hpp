//  (C) Copyright Thomas Becker 2005. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// Revision History
// ================
//
// 27 Dec 2006 (Thomas Becker) Created

#ifndef ANY_ITERATOR_01102007TMB_HPP
#define ANY_ITERATOR_01102007TMB_HPP

// Includes
// ========

#include "detail/any_iterator_abstract_base.hpp"
#include "detail/any_iterator_wrapper.hpp"
#include "detail/any_iterator_metafunctions.hpp"
#include <boost/iterator/iterator_facade.hpp>

namespace util
{

	///////////////////////////////////////////////////////////////////////
	// 
	// The any_iterator is modeled after the "type erasure pattern" as set
	// forth in boost::any. The class names correspond as follows:
	//
	// boost::any              ==> any_iterator
	// boost::any::placeholder ==> any_iterator_abstract_base
	// boost::any::holder      ==> any_iterator_wrapper
	//
	template<class Value, class CategoryOrTraversal, class Reference = Value&, class Difference = std::ptrdiff_t>
	class any_iterator :
	public boost::iterator_facade<any_iterator<Value,CategoryOrTraversal,Reference,Difference>, Value, CategoryOrTraversal, Reference, Difference>
	{

		// We make every other any_iterator our friend. This is needed only for
		// the conversion from non-const to const, but there should be no harm
		// in being overly friendly here.
		template<class OtherValue, class OtherCategoryOrTraversal, class OtherReference, class OtherDifference>
		friend class any_iterator;
    
	public:
		using Traversal = typename boost::iterator_category_to_traversal<CategoryOrTraversal>::type;

	private:

		struct enabler{};  // a private type avoids misuse
		struct disabler{};  // a private type avoids misuse

		// The type-erasing abstract base class, that is, the type that the
		// impl-pointer points to.
		using abstract_base_type = detail::any_iterator_abstract_base<Value, Traversal, Reference, Difference>;
		using super_type = boost::iterator_facade<any_iterator<Value, CategoryOrTraversal, Reference, Difference>, Value, CategoryOrTraversal, Reference, Difference>;

		// The two types that this any_iterator type recognizes as const versions of
		// itself.


		using const_type_with_const_value_type = any_iterator<std::add_const_t<Value>, CategoryOrTraversal, typename detail::make_iterator_reference_const<Reference>::type, Difference>;
		//
		using const_type_with_non_const_value_type = any_iterator<std::remove_const_t<Value>, CategoryOrTraversal, typename detail::make_iterator_reference_const<Reference>::type, Difference> ;

	public:

		any_iterator() : m_pointer_to_impl(NULL)
		{}

		any_iterator(any_iterator const & rhs) : super_type(rhs)
		{
			if( rhs.m_pointer_to_impl )
			{
				m_pointer_to_impl = rhs.m_pointer_to_impl->clone();
			}
			else
			{
				m_pointer_to_impl = NULL;
			}
		}

		any_iterator& operator=(any_iterator const & rhs)
		{
			if(this != &rhs)
			{
				any_iterator tmp(rhs);
				swap(tmp);
			}

			return *this;
		}

		// Constructor from wrapped iterator. The static assert defines the
		// granularity of the type erasure.
		//
		// NOTE 1: If you want to make the constructor non-explicit, then you
		// really must replace the static assert with an enable_if, or else
		// metafunctions such as is_convertible will say the wrong thing.
		// However, if you use enable_if here, you'll descend into the ugly
		// mess described in the last section of my article at
		// http://www.artima.com/cppsource/type_erasure.html. 
		//
		// NOTE 2: If you remove the restriction that the wrapped iterator
		// cannot be an any_iterator, you must ensure that the conversions from
		// non-const to const any_iterators still work correctly. If you don't
		// do anything, you will end up with wrapping instead of conversion.
		//
		template<class WrappedIterator>
		explicit any_iterator(
			WrappedIterator const & wrapped_iterator,
			typename std::enable_if<
			!detail::is_any_iterator<WrappedIterator>::value,
			disabler
			>::type = disabler()
		)
		{
			//////////////////////////////////////////////////////////////////
			//
			// *** Message to Client ***
			//
			// If this static assert fires, you are trying to construct the any_iterator
			// from a concrete iterator that is not suitable for the type erasure that
			// the any_iterator provides.
			//
			static_assert(detail::is_iterator_type_erasure_compatible<WrappedIterator, any_iterator>::type::value, "foo");
			using wrapper_type = detail::any_iterator_wrapper<WrappedIterator, Value, Traversal, Reference, Difference>;

			m_pointer_to_impl = wrapper_type::create(wrapped_iterator);
		}

		// Assignment from wrapped iterator. The enable_if condition defines
		// the granularity of the type erasure.
		//
		// NOTE: If you remove the restriction that the wrapped iterator
		// cannot be an any_iterator, you must ensure that the conversions from
		// non-const to const any_iterators still work correctly. If you don't
		// do anything, you will end up with wrapping instead of conversion.
		template<class WrappedIterator>
		typename std::enable_if<
			detail::is_iterator_type_erasure_compatible<WrappedIterator, any_iterator>::value && !detail::is_any_iterator<WrappedIterator>::value,
			any_iterator
		>::type &
		operator=(WrappedIterator const & wrapped_iterator)
		{
			any_iterator tmp(wrapped_iterator);
			swap(tmp);
			return *this;
		}

		// Conversion from non-const to const any_iterator. There are two versions.
		// This first one applies if the target has const value type.
		operator const_type_with_const_value_type () const
		{
			const_type_with_const_value_type conversion_result;
			if( m_pointer_to_impl )
			{
			conversion_result.m_pointer_to_impl = m_pointer_to_impl->make_const_clone_with_const_value_type();
			}
			return conversion_result;
		}

		// Conversion from non-const to const any_iterator. There are two versions.
		// This second one applies if the target has non-const value type.
		operator const_type_with_non_const_value_type () const
		{
			const_type_with_non_const_value_type conversion_result;
			if( m_pointer_to_impl )
			{
			conversion_result.m_pointer_to_impl = m_pointer_to_impl->make_const_clone_with_non_const_value_type();
			}
			return conversion_result;
		}

		// Conversion to weaker traversal type
		template<typename TargetTraversal>
		operator any_iterator<Value, TargetTraversal, Reference, Difference>() const
		{
			//////////////////////////////////////////////////////////////////
			//
			// *** Message to Client ***
			//
			// If this static assert fires, you are trying to convert between two
			// any_iterator types that are not suitable for conversion.
			//
			static_assert(
				std::is_base_of<
					typename boost::iterator_category_to_traversal<TargetTraversal>::type,
					typename boost::iterator_category_to_traversal<CategoryOrTraversal>::type
				>::value, "foo");

			any_iterator<Value, TargetTraversal, Reference, Difference> conversion_result;

			if( m_pointer_to_impl )
			{
			typename boost::iterator_category_to_traversal<TargetTraversal>::type* funcSelector = NULL;
			conversion_result.m_pointer_to_impl = make_traversal_converted_version(funcSelector);
			}
			return conversion_result;
			}
    
		~any_iterator()
		{
			delete m_pointer_to_impl;
		}

	private:

		friend class boost::iterator_core_access;
    
		Reference dereference() const
		{
			return m_pointer_to_impl->dereference();
		}

		bool equal(any_iterator const & rhs) const
		{
			if( m_pointer_to_impl == rhs.m_pointer_to_impl )
			{
			return true;
			}

			if( m_pointer_to_impl == NULL || rhs.m_pointer_to_impl == NULL )
			{
			return false;
			}

			//////////////////////////////////////////////////////////////////
			//
			// *** Message to Client ***
			//
			// If the next line does not compile, you are trying to compare
			// two iterators for equality whose categories do not allow that
			// comparison.
			return m_pointer_to_impl->equal(*(rhs.m_pointer_to_impl));
		}

		void increment()
		{
			m_pointer_to_impl->increment();
		}

		void decrement()
		{
			//////////////////////////////////////////////////////////////////
			//
			// *** Message to Client ***
			//
			// If the next line does not compile, you are trying to decrement
			// an iterator whose category does not allow decrementing.
			return m_pointer_to_impl->decrement();
		}

		void advance(Difference n)
		{
			//////////////////////////////////////////////////////////////////
			//
			// *** Message to Client ***
			//
			// If the next line does not compile, you are trying to use an
			// operation that is defined only for random access iterators
			// on an iterator that is not random access.
			m_pointer_to_impl->advance(n);
		}

		Difference distance_to(any_iterator const & other) const
		{
			//////////////////////////////////////////////////////////////////
			//
			// *** Message to Client ***
			//
			// If the next line does not compile, you are trying to use an
			// operation that is defined only for random access iterators
			// on an iterator that is not random access.
			return m_pointer_to_impl->distance_to(*(other.m_pointer_to_impl));
		}

		any_iterator& swap(any_iterator& other)
		{
			// NOTE: iterator_facade doesn't have a swap method, so no call
			// to base class here.
			std::swap(m_pointer_to_impl, other.m_pointer_to_impl);
			return *this;
		}

		detail::any_iterator_abstract_base<Value, boost::incrementable_traversal_tag, Reference, Difference>* make_traversal_converted_version(boost::incrementable_traversal_tag* funcSelector) const
		{
			return m_pointer_to_impl->make_incrementable_version();
		}

		detail::any_iterator_abstract_base<Value, boost::single_pass_traversal_tag, Reference, Difference>* make_traversal_converted_version(boost::single_pass_traversal_tag* funcSelector) const
		{
			return m_pointer_to_impl->make_single_pass_version();
		}

		detail::any_iterator_abstract_base<Value, boost::forward_traversal_tag, Reference, Difference>* make_traversal_converted_version(boost::forward_traversal_tag* funcSelector) const
		{
			return m_pointer_to_impl->make_forward_version();
		}

		detail::any_iterator_abstract_base<Value, boost::bidirectional_traversal_tag, Reference, Difference>* make_traversal_converted_version(boost::bidirectional_traversal_tag* funcSelector) const
		{
			return m_pointer_to_impl->make_bidirectional_version();
		}

		abstract_base_type* m_pointer_to_impl;

	};

	// Metafunction that takes an iterator and returns an any_iterator with the same
	// traits.
	template<class iterator>
	struct make_any_iterator_type
	{
		using type =  any_iterator<detail::iterator_value_t<iterator>, detail::iterator_category_t<iterator>, detail::iterator_reference_t<iterator>, detail::iterator_difference_t<iterator>>;
	};

} // end namespace IteratorTypeErasure

#endif // ANY_ITERATOR_01102007TMB_HPP
