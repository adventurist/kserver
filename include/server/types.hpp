#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <string>
#include <cstring>

#define MAX_BUFFER_SIZE (49152)
#define SMALL_BUFFER_SIZE (8192)

static constexpr int MAX_PACKET_SIZE = 4096;
static constexpr int HEADER_SIZE = 4;

template <typename MessageProcessor>
void MessageHandler(MessageProcessor processor, int client_socket_fd,
                    std::string message) {
  processor(client_socket_fd, message);
}

/**
 * SYSTEM EVENTS
 */
static constexpr int SYSTEM_EVENTS__FILE_UPDATE = 1;
static constexpr int SYSTEM_EVENTS__PROCESS_EXECUTION_REQUESTED = 2;
static constexpr int SYSTEM_EVENTS__SCHEDULED_TASKS_READY = 3;
static constexpr int SYSTEM_EVENTS__SCHEDULED_TASKS_NONE = 4;
static constexpr int SYSTEM_EVENTS__SCHEDULER_SUCCESS = 5;
static constexpr int SYSTEM_EVENTS__SCHEDULER_FAIL = 6;

/**
 * FILE HANDLING STATES
 */
static constexpr int FILE_HANDLE__SUCCESS = 1;
static constexpr int FILE_HANDLE__FAILURE = 2;

static constexpr const char* const PING = "253";
static constexpr const char* const PONG = "PONG";
static constexpr size_t PONG_SIZE = 4;

bool isPing(std::string s) {
  return s.size() == 3 && strcmp(s.c_str(), PING) == 0;
}
typedef struct {
  int timestamp;
  int client_fd;
  uint8_t *f_ptr;
  size_t size;

} ReceivedFile;

#endif  // __TYPES_HPP__
