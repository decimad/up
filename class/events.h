#pragma once
#include "refcounted.h"
#include "signal.h"
#include <unordered_map>

namespace up {

	class holder_signal_link {
	public:
		holder_signal_link();
		holder_signal_link(up::link sig_link, boost::intrusive_ptr<refcounted_virtual> holder_ptr);

		void reset();

		void block(bool val);
		explicit operator bool() const;

	private:
		up::link slot_link_;
		boost::intrusive_ptr< refcounted_virtual > holder_ptr_;
	};

	namespace detail {
		void set_last_constructed_holder(refcounted_virtual* holder);
	}

	struct events_holder : public refcounted_virtual
	{
		events_holder();
	};

	namespace detail {

		struct auto_detect_holder {
			auto_detect_holder();
			boost::intrusive_ptr<refcounted_virtual> get_holder();
			refcounted_virtual* holder_;
		};

	}

	template< typename Signature, typename HolderDetect = detail::auto_detect_holder >
	class holder_signal {
	public:
		template< typename... Args >
		holder_signal(Args&&... args)
			: detection_(std::forward<Args>(args)...)
		{
		}

		holder_signal_link subscribe(std::function< Signature > func) {
			return holder_signal_link(sig_.subscribe(std::move(func)), detection_.get_holder());
		}

		template< typename... Args >
		void raise(Args&&... args)
		{
			auto hold = detection_.get_holder();
			sig_.raise(std::forward<Args>(args)...);
		}

	private:
		signal< Signature > sig_;
		HolderDetect detection_;
	};


	template< typename Events >
	struct events_holder_t
	{
		template< typename MemberType, typename... Args	>
		void raise(MemberType(Events::*signal), Args&&... args) {
			(static_cast<Events*>(this)->*signal).raise(std::forward<Args>(args)...);
		}
	};

}