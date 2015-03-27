#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <boost/intrusive_ptr.hpp>

namespace up {

	namespace detail {

		class signal_base;

		class slot_base {
		public:
			slot_base(signal_base* sig);
			virtual ~slot_base();

			bool is_blocked() const;
			void block(bool value);

		private:
			signal_base* signal_;
			bool blocked_;
			std::size_t refcount_;

			friend void intrusive_ptr_add_ref(slot_base* s);
			friend void intrusive_ptr_release(slot_base* s);
			friend class signal_base;
		};

		template< typename Signature >
		struct slot;
		
		template< typename Ret, typename... Args >
		struct slot< Ret(Args...) > : public slot_base
		{
			slot(signal_base* sig, std::function<Ret(Args...)> function)
				: slot_base(sig), function_(std::move(function))
			{
			}

			template< typename... Args >
			void invoke(Args&&... args) {
				if(!is_blocked()) {
					function_(std::forward<Args>(args)...);
				}
			}

			std::function< Ret(Args...) > function_;
		};
		
	}

	using link = boost::intrusive_ptr<detail::slot_base>;
		
	namespace detail {
		
		template< typename T, typename... Args >
		boost::intrusive_ptr<T> make_intrusive(Args&&... args)
		{
			return boost::intrusive_ptr<T>(new T(std::forward<Args>(args)...));
		}

		class signal_base {
		public:
			signal_base();
			~signal_base();
		
			template< typename T, typename... Args >
			link connect( Args&&... args )
			{
				auto ptr = make_intrusive<T>(this, std::forward<Args>(args)...);
				slots_.push_back(ptr.get());
				return ptr;
			}
		
			void remove(slot_base* s);

		protected:	
			void start_iteration();
			void stop_iteration();
			void collect();

			std::vector< slot_base* >& get_slots();

		private:
			std::vector< slot_base* > slots_;
			std::size_t iteration_count_;
		};

	}

	template< typename Signature > struct signal;

	template< typename Ret, typename... Args >
	struct signal< Ret(Args...) > : private detail::signal_base {
		using slot_type = detail::slot<Ret(Args...)>;

		template< typename... Args >
		void raise(Args&&... args) {
			start_iteration();
			auto& slots = get_slots();
			for(auto& slot : slots) {
				if(slot) {
					static_cast<slot_type*>(slot)->invoke(std::forward<Args>(args)...);
				}
			}
			stop_iteration();
		}

		link subscribe(std::function<Ret(Args...)> func) {
			return connect<slot_type>(std::move(func));
		}
	};

}