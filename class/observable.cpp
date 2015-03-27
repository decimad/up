#include "stdafx.h"
#include "observable.h"

namespace util {

	//
	// link
	//

    link::link()
    {
    }

    link::link(std::shared_ptr<detail::observable3_storage> storage, void* observer)
        : ref_(std::move(storage)), observer_(observer)
    {
    }

    link::link(link&& other)
        : ref_(std::move(other.ref_)), observer_(other.observer_)
    {
    }

    link& link::operator=(link&& other)
	{
        ref_ = std::move(other.ref_);
        observer_ = other.observer_;

		return *this;
    }

    link::operator bool() const
	{
        return ref_.operator bool();
    }

    link::~link()
    {
		reset();
    }

    void link::reset()
    {
        if (ref_) {
			ref_->unsubscribe(observer_);
			ref_.reset();
        }
    }

    namespace detail {

		//
		// observable3_storage
		//

		observable3_storage::observable3_storage()
			: iterations_(0)
        {
        }

		observable3_storage::~observable3_storage()
        {
        }

        size_t observable3_storage::size() const
		{
            return observers_.size();
        }

		void observable3_storage::subscribe(void* observer)
		{
			observers_.push_back(observer);
		}

        void observable3_storage::unsubscribe(void* observer)
		{
            for (auto*& ptr : observers_) {
                if (ptr == observer) {
                    ptr = nullptr;
                }
            }
            collect();
        }

        void observable3_storage::start_iteration()
        {
            ++iterations_;
        }

        void observable3_storage::stop_iteration()
        {
            --iterations_;
            collect();
        }

        void observable3_storage::collect()
		{
			if(iterations_==0) {
				auto it = std::remove_if(observers_.begin(), observers_.end(), [](void* ptr) { return ptr==nullptr; });
				observers_.erase(it, observers_.end());
			}
        }

		std::vector<void*>& observable3_storage::get_observers()
		{
			return observers_;
		}

		//
		// lazy_storage
		//
					
		std::shared_ptr< observable3_storage > lazy_storage::get_storage() {
			auto result = ptr_.lock();

			if(!result) {
				result = std::make_shared< observable3_storage >();
				ptr_ = result;
			}

			return result;
		}

		iteration_ref lazy_storage::try_get_storage() {
			return iteration_ref(ptr_.lock());
		}

		link lazy_storage::subscribe(void* ptr) {
			auto store = get_storage();
			store->subscribe(ptr);
			return link(std::move(store), ptr);
		}

		void lazy_storage::unsubscribe(void* ptr) {

		}

		//
		// lazy_storage::iteration_ref
		//

		iteration_ref::iteration_ref(std::shared_ptr< observable3_storage > ref)
			: ref_(std::move(ref))
		{
			if(ref_) {
				ref_->start_iteration();
			}
		}

		iteration_ref::~iteration_ref() {
			if(ref_) {
				ref_->stop_iteration();
			}
		}

		iteration_ref::operator bool() const {
			return ref_ ? true : false;
		}

		observable3_storage* iteration_ref::operator->() {
			return ref_.get();
		}


    }


}