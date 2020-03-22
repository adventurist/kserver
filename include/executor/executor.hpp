#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <string_view>

#include <executor/execxx.hpp>
#include <executor/scheduler.hpp>
#include <log/logger.h>
#include <database/kdb.hpp>

namespace Executor {

auto KLOG = KLogger::GetInstance() -> get_logger();

/** Function Types */
typedef std::function<void(std::string, int, int)> ProcessEventCallback;
typedef std::function<void(std::string, int, std::string, int)>
    TrackedEventCallback;
/** Manager Interface */
class ProcessManager {
 public:
  virtual void request(std::string_view path, int mask, int client_id,
                       std::vector<std::string> argv) = 0;
  virtual void request(std::string_view path, int mask, int client_id,
                       std::string request_id,
                       std::vector<std::string> argv, bool is_scheduled_task) = 0;

  virtual void setEventCallback(ProcessEventCallback callback_function) = 0;
  virtual void setEventCallback(TrackedEventCallback callback_function) = 0;

  virtual void notifyProcessEvent(std::string status, int mask,
                                  int client_id) = 0;
  virtual void notifyTrackedProcessEvent(std::string status, int mask,
                                         std::string request_id,
                                         int client_id) = 0;
};

const char *findWorkDir(std::string_view path) {
  return path.substr(0, path.find_last_of("/")).data();
}

/** Impl */
std::string run_(std::string_view path, std::vector<std::string> argv) {
  std::vector<std::string> v_args{};
  v_args.push_back(std::string(path));
  for (const auto &arg : argv) {
    v_args.push_back(arg);
  }

  const char *executable_path = path.data();

  std::string work_dir{findWorkDir(path)};

  /* qx wraps calls to fork() and exec() */
  return std::string(qx(v_args, work_dir));
}
/** Process Executor - implements Manager interface */
class ProcessExecutor : public ProcessManager {
 public:
  /** Daemon to run process */
  class ProcessDaemon {
   public:
    /** Constructor/Destructor */
    ProcessDaemon(std::string_view path, std::vector<std::string> argv)
        : m_path(std::move(path)), m_argv(std::move(argv)) {}
    ~ProcessDaemon(){/* Clean up */};
    /** Disable copying */
    ProcessDaemon(const ProcessDaemon &) = delete;
    ProcessDaemon(ProcessDaemon &&) = delete;
    ProcessDaemon &operator=(const ProcessDaemon &) = delete;
    ProcessDaemon &operator=(ProcessDaemon &&) = delete;

    /** Uses async and future to call implementation*/
    std::string run() {
      std::future<std::string> result_future =
          std::async(std::launch::async, &run_, m_path, m_argv);
      std::string result = result_future.get();
      return result;
    }

   private:
    std::string_view m_path;
    std::vector<std::string> m_argv;
  };
  /** Constructor / Destructor */
  ProcessExecutor() {}
  ~ProcessExecutor() {
    std::cout << "Executor destroyed"
              << std::endl; /* Kill processes? Log for processes? */
  }
  /** Disable copying */
  ProcessExecutor(const ProcessExecutor & e) : m_callback(e.m_callback), m_tracked_callback(e.m_tracked_callback) {}
  ProcessExecutor(ProcessExecutor && e) : m_callback(e.m_callback), m_tracked_callback(e.m_tracked_callback) {
    e.m_callback = nullptr;
    e.m_tracked_callback = nullptr;
  }

  ProcessExecutor &operator=(const ProcessExecutor& e) {
    this->m_callback = nullptr;
    this->m_tracked_callback = nullptr;
    this->m_callback = e.m_callback;
    this->m_tracked_callback = e.m_tracked_callback;
    return *this;
  };

  ProcessExecutor &operator=(ProcessExecutor && e) {
    if (&e != this) {
      m_callback = e.m_callback;
      m_tracked_callback = e.m_tracked_callback;
      e.m_callback = nullptr;
      e.m_tracked_callback = nullptr;
    }
    return *this;
  }
  /** Set the callback */
  virtual void setEventCallback(ProcessEventCallback f) override { m_callback = f; }
  virtual void setEventCallback(TrackedEventCallback f) override {
    m_tracked_callback = f;
  }
  /** Callback to be used upon process completion */
  virtual void notifyProcessEvent(std::string status, int mask,
                                  int client_socket_fd) override {
    m_callback(status, mask, client_socket_fd);
  }
  virtual void notifyTrackedProcessEvent(std::string status, int mask,
                                         std::string request_id,
                                         int client_socket_fd) override {
    m_tracked_callback(status, mask, request_id, client_socket_fd);
  }

  /* Request execution of an anonymous task */
  virtual void request(std::string_view path, int mask, int client_socket_fd,
                       std::vector<std::string> argv) override {
    if (path[0] != '\0') {
      ProcessDaemon *pd_ptr = new ProcessDaemon(path, argv);
      auto process_std_out = pd_ptr->run();
      if (!process_std_out.empty()) {
        notifyProcessEvent(process_std_out, mask, client_socket_fd);
      }
      delete pd_ptr;
    }
  }
  /** Request the running of a process being tracked with an ID */
  virtual void request(std::string_view path, int mask, int client_socket_fd,
                       std::string request_id,
                       std::vector<std::string> argv, bool scheduled_task) override {
    if (path[0] != '\0') {
      ProcessDaemon *pd_ptr = new ProcessDaemon(path, argv);
      auto process_std_out = pd_ptr->run();
      if (!process_std_out.empty()) {
        notifyTrackedProcessEvent(process_std_out, mask, request_id,
                                  client_socket_fd);
        if (scheduled_task) {
          Database::KDB kdb{};

          QueryFilter filter{{"id", request_id}};
          std::string result = kdb.update("schedule", {"completed"}, {"true"}, filter, "id");

          if (!result.empty()) {
            KLOG->info("Updated task {} to reflect its completion", result);
          }
        }
      }
      delete pd_ptr;
    }
  }

  void executeTask(int client_socket_fd, Scheduler::Task task) {
    KLOG->info("Executing task");
    KApplication app_info = getAppInfo(task.execution_mask);
    auto is_ready_to_execute = std::stoi(task.datetime) > 0;  // if close to now
    auto flags = task.execution_flags;
    auto envfile = task.envfile;

    if (is_ready_to_execute) {
      std::string id_value{std::to_string(task.id)};

      request(ConfigParser::getExecutorScript(), task.execution_mask,
                       client_socket_fd, id_value, {id_value}, true);
    }
  }

 private:
  static KApplication getAppInfo(int mask) {
    Database::KDB kdb{};
    KApplication k_app{};
    QueryValues values = kdb.select("apps", {"path", "data", "name"},
                                    {{"mask", std::to_string(mask)}});

    for (const auto &value_pair : values) {
      if (value_pair.first == "path") {
        k_app.path = value_pair.second;
      } else if (value_pair.first == "data") {
        k_app.data = value_pair.second;
      } else if (value_pair.first == "name") {
        k_app.name = value_pair.second;
      }
    }
    return k_app;
  }

  ProcessEventCallback m_callback;
  TrackedEventCallback m_tracked_callback;
};
} //namespace