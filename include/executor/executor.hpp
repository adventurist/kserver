#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <string_view>

#include "execxx.h"

typedef std::function<bool()> Action;
typedef std::function<std::string(std::string)> EventCallback;

class ProcessManager {
 public:
  virtual void request(const char* path) = 0;
  virtual void setEventCallback(EventCallback callback_function) = 0;
  virtual void notifyProcessEvent(std::string status) = 0;
};

class ProcessExecutor : public ProcessManager {
 public:
  class ProcessDaemon {
   public:
    ProcessDaemon(const char* path, Action action = NULL)
        : m_path(std::move(path)), m_action(std::move(action)) {}
    // Disable copying
    ProcessDaemon(const ProcessDaemon&) = delete;
    ProcessDaemon& operator=(const ProcessDaemon&) = delete;

    bool run_() {
      std::vector<std::string> v_args{};
      v_args.push_back(std::string(m_path));
      v_args.push_back(std::string("--test"));
      std::string returned_string = qx(v_args);
      std::cout << "Returned from process: " << returned_string << std::endl;
      if (returned_string.size() > 0) {
        return true;
      }
      return false;
    }

    bool run() {
      std::future<bool> result_future = std::async(&ProcessDaemon::run_, this);
      bool result = result_future.get();
      return result;
    }

   private:
    const char* m_path;
    Action m_action;
  };

  ProcessExecutor(void* config) : m_config(config) {}
  ~ProcessExecutor() { /* Kill processes? Log for processes? */
  }

  virtual void setEventCallback(EventCallback f) { m_callback = f; }

  virtual void notifyProcessEvent(std::string status) { m_callback(status); }

  virtual void request(const char* path) {
    if (path[0] != '\0') {
      ProcessDaemon* pd_ptr = new ProcessDaemon(path);
      auto result = pd_ptr->run();
      if (result) {
        std::string status_report{path};
        status_report += " successfully completed";
        notifyProcessEvent(status_report);
      }
    }
  }

 private:
  EventCallback m_callback;
  void* m_config;
};