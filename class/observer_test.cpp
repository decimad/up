#include <iostream>
#include <boost/signals2.hpp>
#include "observing.h"
#include "observer_test.h"

struct short_tag {};

struct iface {
	virtual void foo() {
		std::cout<<"foo called!\n";

	}
};

struct my_observer : public iface {

	void foo() override {
		link.reset();
	}

	void dosome() {

	}

	util::link link;

};

struct integer_observer {
	virtual void on_changing(int newvalue) = 0;
	virtual void on_changed(int newvalue) = 0;
	virtual void on_limits_changing(std::pair<int, int> limits) = 0;
	virtual void on_limits_changed(std::pair<int, int> limits) = 0;
};

void observer_test()
{
	int sample;

	my_observer observer;
	up::tagged_observable<iface> obs2(up::make_tag<short_tag>(&sample));

	util::observable3<iface> obs;

	iface myiface;

	auto link = obs.subscribe(&myiface);

	obs.notify(&iface::foo);

	{
		auto link2 = obs2.subscribe(&observer);
		observer.link = std::move(link2);
		obs2.notify(&iface::foo);
	}
}