#ifndef __INSTAGRAM_HPP__
#define __INSTAGRAM_HPP__

#include <codec/util.hpp>
#include <executor/task_handlers/task.hpp>
#include <executor/scheduler.hpp>
#include <iostream>
#include <vector>

namespace Task {
  /**
   * IGTaskIndex
   *
   * These indices describe the order of arguments expected for processing of an IGTask
   */
  namespace IGTaskIndex {
    static constexpr uint8_t MASK = Task::TaskIndexes::MASK;
    static constexpr uint8_t FILEINFO = 1;
    static constexpr uint8_t DATETIME = 2;
    static constexpr uint8_t DESCRIPTION = 3;
    static constexpr uint8_t HASHTAGS = 4;
    static constexpr uint8_t REQUESTED_BY = 5;
    static constexpr uint8_t REQUESTED_BY_PHRASE = 6;
    static constexpr uint8_t PROMOTE_SHARE = 7;
    static constexpr uint8_t LINK_BIO = 8;
    static constexpr uint8_t IS_VIDEO = 9;
    static constexpr uint8_t HEADER = 10;
    static constexpr uint8_t USER = 11;
  }

class IGTaskHandler {
 public:
  static Scheduler::Task prepareTask(std::vector<std::string> argv,
                                     std::string uuid) {
    if (!FileUtils::createTaskDirectory(uuid)) {
      std::cout << "UNABLE TO CREATE TASK DIRECTORY! Returning empty task"
                << std::endl;
      return Scheduler::Task{};
    }

    auto mask = argv.at(IGTaskIndex::MASK);
    auto file_info = argv.at(IGTaskIndex::FILEINFO);
    auto datetime = argv.at(IGTaskIndex::DATETIME);
    auto description = argv.at(IGTaskIndex::DESCRIPTION);
    auto hashtags = argv.at(IGTaskIndex::HASHTAGS);
    auto requested_by = argv.at(IGTaskIndex::REQUESTED_BY);
    auto requested_by_phrase = argv.at(IGTaskIndex::REQUESTED_BY_PHRASE);
    auto promote_share = argv.at(IGTaskIndex::PROMOTE_SHARE);
    auto link_bio = argv.at(IGTaskIndex::LINK_BIO);
    auto is_video = argv.at(IGTaskIndex::IS_VIDEO) == "1";
    auto header = argv.at(IGTaskIndex::HEADER);
    auto user = argv.at(IGTaskIndex::USER);

    std::vector<FileInfo> task_files = FileUtils::parseFileInfo(file_info);

    std::string media_filename = get_executable_cwd();
    for (int i = 0; i < task_files.size(); i++) {
      std::cout << "Filename returned: " << task_files.at(i).first << std::endl;
      task_files.at(i).first =
          media_filename + "/data/" + uuid + "/" + task_files.at(i).first;
    }

    std::string env_file_string{"#!/usr/bin/env bash\n"};
    env_file_string += "HEADER='" + header + "'\n";
    env_file_string += "DESCRIPTION='" + description + "'\n";
    env_file_string += "HASHTAGS='" + hashtags + "'\n";
    env_file_string += "REQUESTED_BY='" + requested_by + "'\n";
    env_file_string += "REQUESTED_BY_PHRASE='" + requested_by_phrase + "'\n";
    env_file_string += "PROMOTE_SHARE='" + promote_share + "'\n";
    env_file_string += "LINK_BIO='" + link_bio + "'\n";
    env_file_string += "FILE_TYPE='";
    env_file_string += is_video ? "video'\n" : "image'\n";
    env_file_string += "USER='" + user + "'\n";

    std::string env_filename = FileUtils::saveEnvFile(env_file_string, uuid);

    return Scheduler::Task{
        .execution_mask = std::stoi(mask),
        .datetime = datetime,
        .file = (!task_files.empty()),
        .files = task_files,
        .envfile = env_filename,
        .execution_flags =
            "--description=$DESCRIPTION --hashtags=$HASHTAGS "
            "--requested_by=$REQUESTED_BY --media=$FILE_TYPE "
            "--requested_by_phrase=$REQUESTED_BY_PHRASE "
            "--promote_share=$PROMOTE_SHARE --link_bio=$LINK_BIO "
            "--header=$HEADER --user=$USER"};
  }
};
}  // namespace Task

#endif  // __INSTAGRAM_HPP__
