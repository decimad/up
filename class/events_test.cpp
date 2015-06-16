#include "integral.h"
#include <iostream>

#include "meta.h"

template< typename... Elems >
struct some_list {};

using type = meta::union_list_t< some_list<int, float>, some_list<short, bool>, some_list<double, float> >;



template< typename T >
struct TestTransformer
{
	using type = int;
};

using transformed = meta::transform_list_t< type, TestTransformer >;


void func(int value)
{
	transformed foo;
	std::cout<<"foo! "<<value<<"\n";
}

void events_test() {
/*
	// test event outliving
	{
		up::integer_events evs;
		{
			auto conn = evs.changed.subscribe(func);
			evs.changed.raise(4);
		}
	}

	// test connection outliving
	{
		up::holder_signal_link conn;
		{
			up::integer_events evs2;
			conn = evs2.changed.subscribe(func);
		}
	}

	// test removal during iteration
	{
		up::integer_events evs3;
		up::holder_signal_link conn;
		conn = evs3.changed.subscribe([&conn](int value) { conn.reset(); });
		evs3.changed.raise(4);
	}

	up::integer_events evs;
	evs.raise(&up::integer_events::changed, 4);
*/
}