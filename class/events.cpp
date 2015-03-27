#include "events.h"

namespace up {

	namespace detail {

		events_holder* last_constructed_ = nullptr;

		events_holder* get_last_constructed_holder()
		{
			return last_constructed_;
		}

		void set_last_constructed_holder(events_holder* holder)
		{
			last_constructed_ = holder;
		}

		//
		// auto_detect_holder
		//

		auto_detect_holder::auto_detect_holder()
			: holder_(get_last_constructed_holder())
		{
		}
	
		boost::intrusive_ptr<refcounted_virtual> auto_detect_holder::get_holder() {
			return holder_;
		}

	}

	//
	// events_holder
	//
	
	events_holder::events_holder()
	{
		detail::set_last_constructed_holder(this);
	}

	//
	// holder_signal_link
	//

	holder_signal_link::holder_signal_link()
	{
	}

	holder_signal_link::holder_signal_link(up::link sig_link, boost::intrusive_ptr<refcounted_virtual> holder_ptr)
		: slot_link_(std::move(sig_link)), holder_ptr_(std::move(holder_ptr))
	{
	}

	void holder_signal_link::reset()
	{
		slot_link_.reset();
		holder_ptr_.reset();
	}

	void holder_signal_link::block(bool val) {
		slot_link_->block(val);
	}

	holder_signal_link::operator bool() const {
		return slot_link_ ? true : false;
	}

}