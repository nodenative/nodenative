#include "native/Process.hpp"
#include "native/async.hpp"
#include "native/async.ipp"
#include "native/async/RequestPromise.hpp"
#include "native/net/net_utils.hpp"

#include <cstring>

namespace native {

std::shared_ptr<Process> Process::Create() {
  std::shared_ptr<Process> instance(new Process(Loop::GetInstanceSafe()));
  instance->init();
  return instance;
}

std::shared_ptr<Process> Process::Create(std::shared_ptr<native::Loop> iLoop) {
  std::shared_ptr<Process> instance(new Process(iLoop));
  instance->init();
  return instance;
}

Process::Process(std::shared_ptr<native::Loop> iLoop) : base::Handle(iLoop), _uvProcessInstance(new uv_process_t) {
  NNATIVE_FCALL();
}

Process::~Process() {}

void Process::init() {}

uv_process_options_t *Process::getProcessOptionsPtr() {
  if (!_uvProcessOptions) {
    _uvProcessOptions.reset(new uv_process_options_t);
    memset(_uvProcessOptions.get(), 0, sizeof(uv_process_options_t));
  }

  return _uvProcessOptions.get();
}

std::shared_ptr<Process> Process::setUid(int uid) {
  getProcessOptionsPtr()->uid = uid;
  getProcessOptionsPtr()->flags |= UV_PROCESS_SETUID;

  return getInstanceProcess();
}

std::shared_ptr<Process> Process::setGid(int gid) {
  getProcessOptionsPtr()->uid = gid;
  getProcessOptionsPtr()->flags |= UV_PROCESS_SETGID;

  return getInstanceProcess();
}

Error Process::spawn() {
  NNATIVE_ASSERT_MSG(_uvProcessInstance->data == nullptr, "Process already spawned with pid "
                                                              << _uvProcessInstance->pid);
  getProcessOptionsPtr()->exit_cb = OnExit;
  _uvProcessInstance->data = this;

  Error oError = uv_spawn(getLoop()->get(), _uvProcessInstance.get(), getProcessOptionsPtr());
  NNATIVE_DEBUG("uv_pipe_bind result: " << oError.code());
  _uvProcessOptions.reset();
  return oError;
}

Error Process::kill(int signal) { return uv_process_kill(_uvProcessInstance.get(), signal); }

Error Process::Kill(int pid, int signal) { return uv_kill(pid, signal); }

void Process::OnExit(uv_process_t *handle, int64_t exit_status, int term_signal) {
  Process *instance = static_cast<Process *>(handle->data);
  if (instance->_exitCb) {
    instance->_exitCb(exit_status, term_signal);
  }
}

} /* namespace native */
