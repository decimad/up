#include "signal.h"
#include <algorithm>

namespace up {

	namespace detail {

		//
		// slot_base
		//

		slot_base::slot_base(signal_base* sig)
			: signal_(sig), blocked_(false), refcount_(0)
		{
		}

		bool slot_base::is_blocked() const
		{
			return blocked_;
		}

		void slot_base::block(bool value)
		{
			blocked_ = value;
		}

		slot_base::~slot_base()
		{
		}

		void intrusive_ptr_add_ref(slot_base* s)
		{
			++s->refcount_;
		}

		void intrusive_ptr_release(slot_base* s)
		{
			--s->refcount_;
			if(s->refcount_==0) {
				if(s->signal_) {
					s->signal_->remove(s);
				}
				delete s;
			}
		}

		//
		// signal_base
		//

		signal_base::signal_base()
			: iteration_count_(0)
		{
		}

		signal_base::~signal_base()
		{
			for(auto slot:slots_) {
				slot->signal_ = nullptr;
			}
		}
		
		void signal_base::start_iteration()
		{
			++iteration_count_;
		}

		void signal_base::stop_iteration()
		{
			--iteration_count_;
			collect();
		}

		void signal_base::remove(slot_base* s)
		{
			auto it = std::find(slots_.begin(), slots_.end(), s);
			
			if(it!=slots_.end()) {
				if(iteration_count_) {
					*it = nullptr;
				} else {
					slots_.erase(it);
				}
			}
		}

		void signal_base::collect()
		{
			if(iteration_count_==0) {
				auto it = std::remove_if(slots_.begin(), slots_.end(), [](slot_base* ptr) { return ptr==nullptr; });
				slots_.erase(it, slots_.end());
			}
		}

		std::vector< slot_base* >& signal_base::get_slots()
		{
			return slots_;
		}

	}
	
}