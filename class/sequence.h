#pragma once
#include <algorithm>
#include <cassert>
#include <boost/iterator/iterator_traits.hpp>

namespace up {

	namespace detail {

		template< typename IteratorType, bool IsPointer = std::is_pointer<IteratorType>::value >
		struct value_type
		{
			typedef typename IteratorType::value_type type;
		};

		template< typename IteratorType >
		struct value_type< IteratorType, true >
		{
			typedef typename std::remove_pointer<IteratorType>::type type;
		};

		template< typename IteratorType >
		struct is_random_access_iterator
		{
			static const bool value = std::is_convertible< typename boost::iterator_category<IteratorType>::type, boost::random_access_traversal_tag >::value;
		};

		template< typename IteratorType >
		struct is_bidirectional_access_iterator
		{
			static const bool value = std::is_convertible< typename boost::iterator_category<IteratorType>::type, boost::bidirectional_traversal_tag >::value;
		};

	}

	template< typename Iterator >
	class sequence
	{
	public:
		typedef Iterator iterator;
		typedef std::size_t size_type;
		typedef typename Iterator::difference_type difference_type;
		typedef typename boost::iterator_value<iterator>::type value_type;
		typedef typename boost::iterator_reference<iterator>::type reference;
		typedef const typename Iterator::value_type& const_reference;

		sequence( Iterator begin, Iterator end )
			: begin_(begin), end_(end)
		{
		}

		template< typename OtherA, typename OtherB >
		sequence(OtherA&& begin, OtherB&& end)
			: begin_(std::forward<OtherA>(begin)), end_(std::forward<OtherB>(end))
		{
		}

		template< typename Other >
		sequence(const sequence<Other>& other)
			: begin_(other.begin()), end_(other.end())
		{
		}

		iterator begin() const
		{
			return begin_;
		}

		iterator end() const
		{
			return end_;
		}

		reference front()
		{
			return *begin_;
		}

		const_reference front() const
		{
			return *begin_;
		}

		reference back()
		{
			auto it = end_;
			--it;
			return *it;
		}

		const_reference back() const
		{
			auto it = end_;
			--it;
			return *it;
		}

		//typename std::enable_if< is_random_access_iterator<iterator>::value, value_type& >::type
		reference operator[]( unsigned int index )
		{
			return *(begin_+index);
		}
		
		//typename std::enable_if< !is_random_access_iterator<iterator>::value, value_type& >::type
		reference at( unsigned int index )
		{
			Iterator buff = begin_;
			std::advance( buff, index );
			return *buff;
		}

		void increment_front()
		{
			if( begin_ != end_ ) ++begin_;
		}

		//typename std::enable_if< !is_random_access_iterator<iterator>::value >::type
		void advance_front( difference_type n )
		{
			if( end_-begin_ >= n ) begin_ += n;
			else begin_ = end_;
		}

		//typename std::enable_if< is_bidirectional_access_iterator<iterator>::value >::type
		void decrement_back()
		{
			if( begin_ != end_ ) --end_;
		}

		//typename std::enable_if< is_random_access_iterator<iterator>::value >::type
		void advance_back( difference_type n )
		{
			if( n < 0 && end_-begin() < -n ) {
				end_ = begin_;
			} else {
				end_ += n;
			}
		}

		//typename std::enable_if< is_bidirectional_access_iterator<iterator>::value >::type
		void contract()
		{
			increment_front();
			decrement_back();
		}

		//typename std::enable_if< is_bidirectional_access_iterator<iterator>::value >::type
		void contract( size_type n )
		{
			advance_front( n );
			advance_back( -n );
		}

		//typename std::enable_if< !is_random_access_iterator<iterator>::value, std::size_type >::type
		std::size_t size() const
		{
			return end_ - begin_;
		}

	private:
		Iterator begin_, end_;
	};

	template< typename SequenceType, typename DestinationIterator >
	void copy( SequenceType& sequence, DestinationIterator dest )
	{
		std::copy( sequence.begin(), sequence.end(), dest );
	}

	template< typename SequenceType, typename DestinationIterator >
	void copy_backward( SequenceType& sequence, DestinationIterator dest_end )
	{
		std::copy_backward( sequence.begin(), sequence.end(), dest_end );
	}

	template< typename SequenceType, typename DestinationIterator >
	void move( SequenceType& sequence, DestinationIterator dest )
	{
		std::move( sequence.begin(), sequence.end(), dest );
	}

	template< typename SequenceType, typename DestinationIterator >
	void move_backwad( SequenceType& sequence, DestinationIterator dest_end )
	{
		std::move_backward( sequence.begin(), sequence.end(), dest_end );
	}

	// Unecessary
	template< typename ArgType >
	sequence< typename ArgType::iterator > to_sequence( ArgType& arg )
	{
		return sequence< typename ArgType::iterator >( arg.begin(), arg.end() );
	}

	template< typename SequenceType, typename FuncType >
	void for_each( SequenceType& sequence, FuncType& dest )
	{
		std::for_each( sequence.begin(), sequence.end(), std::ref(dest) );
	}

	template< typename IteratorType >
	sequence< IteratorType > to_sequence( const IteratorType& arg1, const IteratorType& arg2 )
	{
		return sequence< IteratorType >( arg1, arg2 );
	}

	template< typename SequenceType >
	auto find(SequenceType& sequence, const typename SequenceType::value_type& val) -> decltype(sequence.begin())
	{
		return std::find( sequence.begin(), sequence.end(), val );
	}

	template< typename SequenceType >
	auto lower_bound(SequenceType& sequence, const typename SequenceType::value_type& val) -> decltype(sequence.begin())
	{
		return std::lower_bound( sequence.begin(), sequence.end(), val );
	}

	template< typename SequenceType, typename Compare >
	auto lower_bound(SequenceType& sequence, const typename SequenceType::value_type& val, Compare& compare) -> decltype(sequence.begin())
	{
		return std::lower_bound( sequence.begin(), sequence.end(), val, compare );
	}

	template< typename SequenceType >
	auto upper_bound(SequenceType& sequence, const typename SequenceType::value_type& val) -> decltype(sequence.begin())
	{
		return std::upper_bound( sequence.begin(), sequence.end(), val );
	}

	template< typename SequenceType, typename Compare >
	auto upper_bound(SequenceType& sequence, const typename SequenceType::value_type& val, Compare& compare) -> decltype(sequence.begin())
	{
		return std::upper_bound( sequence.begin(), sequence.end(), val, compare );
	}

	template< typename SequenceType, typename ValType >
	auto find_closest(SequenceType& sequence, const ValType& val) -> decltype(sequence.begin())
	{
		if( sequence.size() && val < sequence.front() ) {
			return sequence.end();
		}
		auto it = lower_bound( sequence, val );
		if( it != sequence.begin() && it != sequence.end() ) {
			if( types::abs( *(it-1) - val ) < types::abs(*it - val ) ) {
				return it-1;
			} else {
				return it;
			}
		} else {
			return it;
		}
	}

	template< typename SequenceType, typename ValType, typename Compare >
	auto find_closest(SequenceType& sequence, const ValType& val, Compare& compare) -> decltype(sequence.begin())
	{
		if( compare(val,sequence.front()) ) {
			return sequence.end();
		}
		auto it = lower_bound( sequence, val, compare );
		if( it != sequence.begin() && it != sequence.end() ) {
			return compare.Closer( *(it-1), *it, val );
		} else {
			return it;
		}
	}


	// if val is smaller than all elements, returns pair(begin, begin);
	// if val is bigger than all elements, returns pair(end, end);
	// if val is inbetween two elements, returns pair(smaller_it, bigger_it);
	template< typename SequenceType >
	auto find_surrounding( SequenceType& sequence, const typename SequenceType::value_type& val )
		-> std::pair< decltype(sequence.begin()), decltype(sequence.begin()) >
	{
		auto it = lower_bound( sequence, val );
		if( it != sequence.begin() && it != sequence.end() ) {
			return std::make_pair( it-1, it );
		} else {
			return std::make_pair( it, it );
		}
	}

	template< typename SequenceType, typename Compare >
	auto find_surrounding( SequenceType& sequence, const typename SequenceType::value_type& val, Compare& compare )
		-> std::pair< decltype(sequence.begin()), decltype(sequence.begin()) >
	{
		auto it = lower_bound( sequence, val, compare );
		if( it != sequence.begin() && it != sequence.end() ) {
			return std::make_pair( it-1, it );
		} else {
			return std::make_pair( it, it );
		}
	}

}