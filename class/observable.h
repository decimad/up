#pragma once
#include <cassert>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

/*
model subscriber implementation using links to represent connections.
model and subscriber can be removed during iteration.
if no observers are registered, size is shared_ptr + unique_ptr (lightweight).
*/

namespace util {


	class link;

    namespace detail {

        struct observable3_storage {
        public:
			observable3_storage();
            ~observable3_storage();

            size_t size() const;

        public:
            void start_iteration();
            void stop_iteration();

            void collect();

        public:
            void unsubscribe(void* observer);
            void subscribe(void* observer);
           
			std::vector<void*>& get_observers();

		private:
			friend class lazy_storage;

			std::vector<void*> observers_;
			std::size_t iterations_;
        };

		struct iteration_ref {
			iteration_ref(std::shared_ptr< observable3_storage > ref);
			~iteration_ref();
			explicit operator bool() const;
			observable3_storage* operator->();

			std::shared_ptr< observable3_storage > ref_;
		};

		class lazy_storage
		{
		public:
			std::shared_ptr< observable3_storage > get_storage();
			iteration_ref try_get_storage();
			
			link subscribe(void* ptr);
			void unsubscribe(void* ptr);

		private:
			std::weak_ptr< observable3_storage > ptr_;
		};

		struct external_storage
		{
			external_storage(observable3_storage* storage)
				: storage_(storage)
			{
			}

			observable3_storage* get_storage()
			{
				return storage_;
			}

			observable3_storage* storage_;
		};

    }

	class link {
	public:
		link();
		link(std::shared_ptr<detail::observable3_storage> storage, void* observer);
		link(link&& other);
		
		~link();

		link& operator=(link&& other);
		void reset();
		explicit operator bool() const;
		
	private:
		link(const link&) = delete;
		void operator=(const link&) = delete;

		std::shared_ptr<detail::observable3_storage> ref_;
		void* observer_;
	};


    template< typename ObserverInterface, typename StorageAccess = detail::lazy_storage >
    class observable3
    {
    public:
		template< typename... Args >
		observable3(Args&&... args)
			: access_(std::forward<Args>(args)...)
		{
		}

        void subscribe_unmanaged(ObserverInterface* obs)
        {
            return access_.subscribe_unmanaged(static_cast<void*>(obs));
        }

        link subscribe(ObserverInterface* obs) {
            return access_.subscribe(static_cast<void*>(obs));
        }

        // Must only be called for observers subscribed with subscribe_unmanaged
        void unsubscribe(ObserverInterface* obs)
        {
			access_.unsubscribe(static_cast<void*>(obs));
        }

        template< typename... Args1, typename... Args2 >
        void notify(void(ObserverInterface::*Ptr)(Args1...), Args2... args)
        {
            static_assert(sizeof...(Args1) == sizeof...(Args2), "argument counts don't match");

			auto storage = access_.try_get_storage();
            if (storage) {
				auto& vec = storage->get_observers();

                for (auto* void_ptr : vec) {
                    if (void_ptr) {
                        auto* typed_ptr = static_cast<ObserverInterface*>(void_ptr);
                        (typed_ptr->*Ptr)(args...);
                    }
                }    
            }
        }

        template< typename... Args1, typename... Args2 >
        void notify_ignore(void(ObserverInterface::*Ptr)(Args1...), ObserverInterface* ignore, Args2... args)
        {
            static_assert(sizeof...(Args1) == sizeof...(Args2), "argument counts don't match");

			auto storage = access_.try_get_storage();
			
			if(storage) {
				auto& vec = storage->get_observers();
				
				for (auto* void_ptr : vec) {
                    if (void_ptr && void_ptr != ignore) {
                        auto* typed_ptr = static_cast<ObserverInterface*>(void_ptr);
                        (typed_ptr->*Ptr)(args...);
                    }            
                }
            }
        }


	private:
		StorageAccess access_;
    };

    template< typename Class >
    struct subscriber_mixin {

        template< typename Model >
        void subscribe(Model& model) {
            links_.emplace_back(model.subscribe(static_cast<Class*>(this)));
        }

        void clear_links() {
            links_.clear();
        }

    private:
        std::vector< link > links_;
    };

}