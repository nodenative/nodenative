#ifndef __NATIVE_LOOP_HH__
#define __NATIVE_LOOP_HH__

#include "base.h"
#include "error.h"
#include "helper/trace.h"

namespace native
{
    class AsyncBase;
    /**
     *  Class that represents the Loop instance.
     */
    class Loop
    {
    public:
        /**
         *  Default constructor
         *  @param use_default indicates whether to use default Loop or create a new Loop.
         */
        Loop(bool use_default);
        Loop() : Loop(false) { NNATIVE_MCALL(); };

        Loop(std::shared_ptr<uv_loop_t> iLoop) : _uv_loop(iLoop) {}

        /**
         *  Destructor
         */
        ~Loop();

        /**
         *  Returns internal handle for libuv functions.
         */
        uv_loop_t* get() { return _uv_loop.get(); }

        std::shared_ptr<uv_loop_t> getShared() { return _uv_loop; }

        /**
         *  Runs the event Loop until the reference count drops to zero. Always returns zero.
         *  Internally, this function just calls uv_run() function.
         */
        bool run();

        /**
         *  Poll for new events once. Note that this function blocks if there are no pending events. Returns true when done (no active handles
         *  or requests left), or non-zero if more events are expected (meaning you
         *  should run the event Loop again sometime in the future).
         *  Internally, this function just calls uv_run_once() function.
         */
        bool run_once();

        /**
         *  Poll for new events once but don't block if there are no pending events.
         *  Internally, this function just calls uv_run_once() function.
         */
        bool run_nowait();

        /**
         *  ...
         *  Internally, this function just calls uv_update_time() function.
         */
        void update_time();

        /**
         *  ...
         *  Internally, this function just calls uv_now() function.
         */
        int64_t now();
    protected:

    private:
        Loop(const Loop&);
        void operator =(const Loop&);

    private:
        std::shared_ptr<uv_loop_t> _uv_loop;
    };

    /**
     *  Starts the default Loop.
     */
    bool run();

    /**
     *  Polls for new events once for the default Loop.
     *  Note that this function blocks if there are no pending events. Returns true when done (no active handles
     *  or requests left), or non-zero if more events are expected (meaning you
     *  should run the event Loop again sometime in the future).
     */
    bool run_once();

    /**
     *  Polls for new events once but don't block if there are no pending events for the default Loop.
     */
    bool run_nowait();

    /**
     * Returns true if the current thread is the same as event Loop thread;
     */
    bool isOnEventloopThread(std::shared_ptr<uv_loop_t> iloop);

}


#endif
