//  (C) Copyright Thomas Becker 2005. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// File Name
// =========
//
// metafunctions.h

// Description
// ===========
//
// Metafunctions for any_iterator

#ifndef ANY_ITERATOR_METAFUNCTIONS_01102007TMB_HPP
#define ANY_ITERATOR_METAFUNCTIONS_01102007TMB_HPP

// Revision History
// ================
//
// 27 Dec 2006 (Thomas Becker) Created

// Includes
// ========
#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_traits.hpp>

namespace util
{

  template<class Value, class Traversal, class Reference, class Difference>
  class any_iterator;

  namespace detail
  {
	  template< typename T >
	  using iterator_reference_t = typename boost::iterator_reference<T>::type;

	  template< typename T >
	  using iterator_value_t = typename boost::iterator_value<T>::type;

	  template< typename T >
	  using iterator_category_t = typename boost::iterator_category<T>::type;

	  template< typename T >
	  using iterator_traversal_t = typename boost::iterator_traversal<T>::type;

	  template< typename T >
	  using iterator_difference_t = typename boost::iterator_difference<T>::type;



    ///////////////////////////////////////////////////////////////////////
    // 
    template<typename T>
    struct remove_reference_and_const
    {
		using type = std::decay_t<T>; // std::remove_const_t<std::remove_reference_t<std::remove_const_t<T>>>;
    };
    
	template<typename T>
	using remove_reference_and_const_t = typename remove_reference_and_const<T>::type;

    ///////////////////////////////////////////////////////////////////////
    // 
    template<typename IteratorReference>
    struct make_iterator_reference_const
    {
      using type = 
		  typename std::conditional_t< 
			std::is_reference<IteratorReference>::value,
			std::decay_t<IteratorReference> const &,
			std::decay_t<IteratorReference> const
		  >;
    };

    ///////////////////////////////////////////////////////////////////////
    // 
    template<class WrappedIterator, class AnyIterator>
    struct value_types_erasure_compatible
    {
      // Really, we just want WrappedIterator's value_type to convert to
      // AnyIterator's value_type. But many real world output iterators
      // define their value_type as void. Therefore, we simply ignore
      // the value type for output iterators. That's fine because for
      // output iterators, the relevant type erasure information is all
      // in the reference type.

		using type = std::conditional_t<
			std::is_same< iterator_category_t<WrappedIterator>, std::output_iterator_tag>::value ||
			std::is_convertible< iterator_value_t<WrappedIterator>, iterator_value_t<AnyIterator> >::value,
			std::true_type,
			std::false_type >;

    };

    ///////////////////////////////////////////////////////////////////////
    // 
    template<class WrappedIterator, class AnyIterator>
    struct reference_types_erasure_compatible_1
    {
      using type = typename std::is_convertible<iterator_reference_t<WrappedIterator>, iterator_reference_t<AnyIterator>>::type;
    };

	template<typename WrappedIterator, typename AnyIterator>
	using reference_types_erasure_compatible_1_t = typename reference_types_erasure_compatible_1<WrappedIterator, AnyIterator>::type;

    ///////////////////////////////////////////////////////////////////////
    // 
    template<class WrappedIterator, class AnyIterator>
    struct reference_types_erasure_compatible_2
    {
      using type = std::conditional_t<
        std::is_reference<iterator_reference_t<AnyIterator>>::value,
        std::is_reference<iterator_reference_t<WrappedIterator>>,
        std::true_type
      >;
    };

	template<typename WrappedIterator, typename AnyIterator>
	using reference_types_erasure_compatible_2_t = typename reference_types_erasure_compatible_2<WrappedIterator, AnyIterator>::type;

    ///////////////////////////////////////////////////////////////////////
    // 
    template<class WrappedIterator, class AnyIterator>
    struct reference_types_erasure_compatible_3
    {
      using type = std::conditional_t<
			std::is_reference<iterator_reference_t<AnyIterator>>::value && std::is_reference<iterator_reference_t<WrappedIterator>>::value,

			std::conditional_t<
				std::is_convertible< remove_reference_and_const_t<iterator_reference_t<WrappedIterator>>, remove_reference_and_const_t<iterator_reference_t<AnyIterator>> >::value,
				std::true_type,
				std::false_type 
			>,
			std::true_type
      >;
    };

	template<typename WrappedIterator, typename AnyIterator>
	using reference_types_erasure_compatible_3_t = typename reference_types_erasure_compatible_3<WrappedIterator, AnyIterator>::type;

    ///////////////////////////////////////////////////////////////////////
    // 
    template<class WrappedIterator, class AnyIterator>
    struct reference_types_erasure_compatible
    {
      // Output iterators are weird. Many real world output iterators
      // define their reference type as void. In the world of boost
      // iterators, that's terribly wrong, because in that world, an
      // iterator's reference type is always the result type of
      // operator* (and that makes very good sense, too). Therefore,
      // when WrappedIterator is an output iterator, we use
      // WrappedIterator& for WrappedIterator's reference type,
      // because that's the real, true reference type. Moreover,
      // we just require that WrappedIterator& convert to AnyIterator's
      // reference type. The other subtleties are not relevant.

		using type = std::conditional_t<
			(std::is_same<iterator_category_t<WrappedIterator>, std::output_iterator_tag>::value && std::is_convertible<WrappedIterator&, iterator_reference_t<AnyIterator>>::value)
			|| (reference_types_erasure_compatible_1_t<WrappedIterator, AnyIterator>::value 
				&& reference_types_erasure_compatible_2_t<WrappedIterator, AnyIterator>::value
				&& reference_types_erasure_compatible_3_t<WrappedIterator, AnyIterator>::value
				),
			std::true_type,
			std::false_type >;
    };

    ///////////////////////////////////////////////////////////////////////
    // 
    template<class WrappedIterator, class AnyIterator>
    struct difference_types_erasure_compatible
    {
      // Difference type matters only for random access iterators.
      
		using type = std::conditional_t<
			!std::is_same<
				// Do not use boost::iterator_traversal<AnyIterator>::type here,
				// as it does not equal the traversal tag.
				typename AnyIterator::Traversal,
				boost::random_access_traversal_tag
			>::value
			||	( std::is_convertible<iterator_difference_t<WrappedIterator>, iterator_difference_t<AnyIterator>>::value
				  && std::is_convertible<iterator_difference_t<AnyIterator>, iterator_difference_t<WrappedIterator>>::value )
			,
			std::true_type,
			std::false_type	
		>;
    };

    ///////////////////////////////////////////////////////////////////////
    // 
    template<class WrappedIterator, class AnyIterator>
    struct traversal_types_erasure_compatible
    {
		using type = std::conditional_t<
			std::is_same<
			  // Do not use boost::iterator_traversal<AnyIterator>::type here,
			  // as it does not equal the traversal tag.
			  typename AnyIterator::Traversal,
			  iterator_traversal_t<WrappedIterator>
			>::value ||
			std::is_base_of<
			  // Do not use boost::iterator_traversal<AnyIterator>::type here,
			  // as it does not equal the traversal tag.
			  typename AnyIterator::Traversal,
			  iterator_traversal_t<WrappedIterator>
			>::value,
			std::true_type,
			std::false_type
      >;
    };

    ///////////////////////////////////////////////////////////////////////
    // 
    template<class WrappedIterator, class AnyIterator>
    struct is_iterator_type_erasure_compatible :
		std::conditional_t<
			value_types_erasure_compatible<WrappedIterator, AnyIterator>::type::value
			&&	reference_types_erasure_compatible<WrappedIterator, AnyIterator>::type::value
			&&	difference_types_erasure_compatible<WrappedIterator, AnyIterator>::type::value
			&&	traversal_types_erasure_compatible<WrappedIterator, AnyIterator>::type::value,
			std::true_type,
			std::false_type
		>
    {
    };

    ///////////////////////////////////////////////////////////////////////
    // 
    template<class SomeIterator>
    struct is_any_iterator : public std::false_type {};
    
	//
    template<class Value, class Traversal, class Reference, class Difference>
    struct is_any_iterator< any_iterator<Value, Traversal, Reference, Difference > > :
		public std::true_type
    {
    };
    
  } // end namespace detail

} // end namespace IteratorTypeErasure

#endif // ANY_ITERATOR_METAFUNCTIONS_01102007TMB_HPP
