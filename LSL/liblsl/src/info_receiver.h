#ifndef INFO_RECEIVER_H
#define INFO_RECEIVER_H

#include "inlet_connection.h"
#include <boost/thread.hpp>

namespace lsl {

	/// Internal class of an inlet that is responsible for retrieving the info of the inlet.
	/// The actual communication runs in an internal background thread, while the public function (info()) waits for the thread to finish.
	/// The public function has an optional timeout after which it gives up, while the background thread continues to do its job (so the next public-function call may succeed within the timeout).
	/// The background thread terminates only if the info_receiver is destroyed or the underlying connection is lost or shut down.
	class info_receiver {
	public:
		/// Construct a new info receiver for a given connection.
		info_receiver(inlet_connection &conn);

		/// Destructor. Stops the background activities.
		~info_receiver();

		/**
		* Retrieve the complete information of the given stream, including the extended description (.desc() field).
		* @param timeout Timeout of the operation (default: no timeout).
		* @throws timeout_error (if the timeout expires), or lost_error (if the stream source has been lost).
		*/
		const stream_info_impl &info(double timeout=FOREVER);

	private:
		/// The info reader thread.
		void info_thread();
		
		/// function polled by the condition variable
		bool info_ready() { return fullinfo_ || conn_.lost(); }


		// the underlying connection
		inlet_connection &conn_;					// reference to our connection

		// background reader thread and the data generated by it
		boost::thread info_thread_;					// pulls the info in the background
		stream_info_impl_p fullinfo_;				// the full stream_info_impl object (retrieved by the info thread)
		boost::mutex fullinfo_mut_;					// mutex to protect the fullinfo
		boost::condition_variable fullinfo_upd_;	// condition variable to indicate that an update for the fullinfo is available
	};

}

#endif

