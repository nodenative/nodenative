#include "native/loop.h"
#include "native/helper/trace.h"

using namespace native;

loop::loop(bool use_default) {
    NNATIVE_FCALL();
    if(use_default) {
        // don't delete the default loop
        _uv_loop = std::shared_ptr<uv_loop_t>(uv_default_loop(), [](uv_loop_t* iLoop){
                int res = 1;
                do {
                    res = uv_loop_close(iLoop);
                } while (res != 0);
            });
    } else {
        std::unique_ptr<uv_loop_t> loopInstance(new uv_loop_t);
        if(0 == uv_loop_init(loopInstance.get())) {
            _uv_loop = std::shared_ptr<uv_loop_t>(loopInstance.release(), [](uv_loop_t* iLoop){
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
}

bool loop::run() {
    NNATIVE_FCALL();
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

bool native::run()
{
    return (uv_run(uv_default_loop(), UV_RUN_DEFAULT) == 0);
}

bool native::run_once()
{
    return (uv_run(uv_default_loop(), UV_RUN_ONCE) == 0);
}

bool native::run_nowait()
{
    return (uv_run(uv_default_loop(), UV_RUN_NOWAIT) == 0);
}
