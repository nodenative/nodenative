#include "native/loop.h"
#include "native/helper/trace.h"

#include <memory>

namespace native {

loop::loop(bool use_default) {
    NNATIVE_FCALL();
    if(use_default) {
        static std::weak_ptr<uv_loop_t> savedPtr;

        if(!savedPtr.expired()) {
            _uv_loop = savedPtr.lock();
            return;
        }

        // don't delete the default loop
        _uv_loop = std::shared_ptr<uv_loop_t>(uv_default_loop(), [](uv_loop_t* iLoop){
                NNATIVE_DEBUG("destroying default loop...");
                int res = 1;
                do {
                    res = uv_loop_close(iLoop);
                } while (res != 0);
            });

        if(0 != uv_loop_init(_uv_loop.get())) {
            NNATIVE_DEBUG("error to init loop " << (_uv_loop.get()));
            _uv_loop.reset();
        }

        savedPtr = _uv_loop;

    } else {
        std::unique_ptr<uv_loop_t> loopInstance(new uv_loop_t);
        if(0 == uv_loop_init(loopInstance.get())) {
            _uv_loop = std::shared_ptr<uv_loop_t>(loopInstance.release(), [](uv_loop_t* iLoop){
                NNATIVE_DEBUG("destroying specified loop..");
                    int res = 1;
                    do {
                        res = uv_loop_close(iLoop);
                    } while (res != 0);
                    delete iLoop;
                });
        }
    }
}

loop::~loop()
{
    NNATIVE_FCALL();
}

bool loop::run() {
    NNATIVE_FCALL();
    NNATIVE_ASSERT(_uv_loop);
    return (uv_run(_uv_loop.get(), UV_RUN_DEFAULT) == 0); 
}

bool loop::run_once() {
    NNATIVE_FCALL();
    return (uv_run(_uv_loop.get(), UV_RUN_ONCE) == 0); 
}

void loop::update_time()
{
    uv_update_time(_uv_loop.get());
}

int64_t loop::now()
{
    return uv_now(_uv_loop.get());
}

bool run()
{
    loop currLoop(true);
    return (uv_run(currLoop.get(), UV_RUN_DEFAULT) == 0);
}

bool run_once()
{
    loop currLoop(true);
    return (uv_run(currLoop.get(), UV_RUN_ONCE) == 0);
}

bool run_nowait()
{
    loop currLoop(true);
    return (uv_run(currLoop.get(), UV_RUN_NOWAIT) == 0);
}

} /* namespace native */
