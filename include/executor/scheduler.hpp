#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <database/DatabaseConnection.h>
#include <log/logger.h>

#include <codec/util.hpp>
#include <config/config_parser.hpp>
#include <database/kdb.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace Scheduler {

typedef std::function<void(std::string, int, int, int, std::vector<std::string>)> ScheduleEventCallback;

struct Task {
  int execution_mask;
  std::string datetime;
  bool file;
  std::vector<FileInfo> files;
  std::string envfile;
  std::string execution_flags;
  int id = 0;
};

class DeferInterface {
 public:
  virtual void schedule(Task task) = 0;
};

class CalendarManagerInterface {
 public:
  virtual std::vector<Task> fetchTasks() = 0;
};

auto KLOG = KLogger::GetInstance() -> get_logger();

class Scheduler : public DeferInterface, CalendarManagerInterface {
 public:
  Scheduler() {}
  Scheduler(ScheduleEventCallback fn) : m_event_callback(fn) {
    if (!ConfigParser::initConfig()) {
      KLOG->info("Unable to load config");
    }
  }

  // TODO: Implement move / copy constructor

  ~Scheduler() { KLOG->info("Scheduler destroyed"); }

  virtual void schedule(Task task) {
    // verify and put in database
    Database::KDB kdb{};

    std::string insert_id =
        kdb.insert("schedule", {"time", "mask", "flags", "envfile"},
                   {task.datetime, std::to_string(task.execution_mask),
                    task.execution_flags, task.envfile},
                   "id");
    auto result = !insert_id.empty();

    if (!insert_id.empty()) {
      KLOG->info("Request to schedule task was accepted\nID {}", insert_id);
      m_event_callback("", task.execution_mask, task.id, -1, {"Schedule Task", "Success", std::to_string(task.id), std::to_string(task.files.size())});
      for (const auto &file : task.files) {
        KLOG->info("Recording file in DB: {}", file.first);
        kdb.insert("file", {"name", "sid"}, {file.first, insert_id});
      }
    }
  }

  std::vector<Task> parseTasks(QueryValues&& result) {
    int id{};
    std::string mask, flags, envfile, time, filename;
    std::vector<Task> tasks;
    for (const auto &v : result) {
      if (v.first == "mask") {
        mask = v.second;
      }
      if (v.first == "flags") {
        flags = v.second;
      }
      if (v.first == "envfile") {
        envfile = v.second;
      }
      if (v.first == "time") {
        time = v.second;
      }
      if (v.first == "id") {
        id = std::stoi(v.second);
      }
      if (!envfile.empty() && !flags.empty() && !time.empty() &&
          !mask.empty() && id > 0) {
        // TODO: Get files and add to task before pushing into vector
        tasks.push_back(
            Task{.execution_mask = std::stoi(mask),
                 .datetime = time,
                 .file = true,  // Change this default value later after we
                                // implement booleans in the DB abstraction
                 .files = {},
                 .envfile = envfile,
                 .execution_flags = flags,
                 .id = id});
        id = 0;
        filename.clear();
        envfile.clear();
        flags.clear();
        time.clear();
        mask.clear();
      }
    }
    return tasks;
  }

  virtual std::vector<Task> fetchTasks() {
    Database::KDB kdb{};  // get DB
    std::string past_15_minute_timestamp = std::to_string(TimeUtils::unixtime() - 900);
    std::string future_5_minute_timestamp =
        std::to_string(TimeUtils::unixtime() + 300);
    return parseTasks(kdb.selectMultiFilter(
        "schedule",                                  // table
        {"id", "time", "mask", "flags", "envfile"},  // fields
        {CompFilter{"time", std::move(past_15_minute_timestamp), std::move(future_5_minute_timestamp)},
          GenericFilter{"completed", "=", "false"}}));
  }

 private:
  ScheduleEventCallback m_event_callback;
};
}  // namespace Scheduler

#endif  // __SCHEDULER_HPP__
