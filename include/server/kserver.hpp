#ifndef __KSERVER_HPP__
#define __KSERVER_HPP__

#include <codec/uuid.h>
#include <log/logger.h>
#include <math.h>
#include <utility>

#include <algorithm>
#include <codec/util.hpp>
#include <cstring>
#include <functional>
#include <interface/socket_listener.hpp>
#include <iomanip>
#include <request/request_handler.hpp>
#include <string>
#include <string_view>
#include <types/types.hpp>

class Decoder {
 public:
  Decoder(int fd, std::string name, std::function<void(int)> system_callback, std::function<void(uint8_t*, int, std::string)> file_callback)
      : index(0),
        file_buffer(nullptr),
        total_packets(0),
        packet_offset(0),
        buffer_offset(0),
        file_size(0),
        filename(name),
        m_fd(fd),
        m_cb(system_callback),
        m_file_cb(file_callback) {}

  ~Decoder() {
    if (file_buffer != nullptr) {
      delete[] file_buffer;
      file_buffer = nullptr;
    }
  }

  void processPacket(uint8_t* data) {
    bool is_first_packet = (index == 0);

    if (is_first_packet) {
      file_size = int(data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3]) - HEADER_SIZE;
      total_packets = static_cast<uint32_t>(
          ceil(static_cast<double>(file_size + HEADER_SIZE) / MAX_PACKET_SIZE));
      file_buffer = new uint8_t[total_packets * MAX_PACKET_SIZE];
      packet_offset = HEADER_SIZE;
      buffer_offset = 0;
      uint32_t first_packet_size =
          total_packets == 1 ? (file_size - HEADER_SIZE) : MAX_PACKET_SIZE;
      std::memcpy(file_buffer + buffer_offset, data + packet_offset,
                  first_packet_size);
      if (index == (total_packets - 1)) {
        // handle file, cleanup, return
        return;
      }
      index++;
      return;
    } else {
      buffer_offset = (index * MAX_PACKET_SIZE) - HEADER_SIZE;
      bool is_last_packet = (index == (total_packets - 1));
      if (!is_last_packet) {
        std::memcpy(file_buffer + buffer_offset, data, MAX_PACKET_SIZE);
      } else {
        uint32_t last_packet_size = file_size - buffer_offset - HEADER_SIZE;
        // handle file cleanup
        std::memcpy(file_buffer + buffer_offset, data, last_packet_size);
        // save or return ptr to data
        m_file_cb(file_buffer, file_size, filename);
        m_cb(m_fd);
      }
      index++;
    }
  }

 private:
  uint8_t* file_buffer;
  uint32_t index;
  uint32_t total_packets;
  uint32_t packet_offset;
  uint32_t buffer_offset;
  uint32_t file_size;
  std::string filename;
  int m_fd;
  std::function<void(int)> m_cb;
  std::function<void(uint8_t* data, int size, std::string)> m_file_cb;
};

class FileHandler {
 public:
  FileHandler(int client_fd, std::string name, uint8_t* first_packet,
              std::function<void(int)> callback)
      : socket_fd(client_fd) {
    m_decoder = new Decoder(client_fd, name, callback, [this](uint8_t* data, int size, std::string filename) {
      if (size > 0) {
            FileUtils::saveFile(data, size, filename);
          }
    });
    m_decoder->processPacket(first_packet);
  }

   FileHandler(FileHandler&& f) : m_decoder(f.m_decoder), socket_fd(f.socket_fd) {
     std::cout << "Move CONSTRUCTOR\n";
     f.m_decoder = nullptr;
    }

    FileHandler(FileHandler& f) : m_decoder(f.m_decoder) {
     std::cout << "COPY CONSTRUCTOR\n";
     f.m_decoder = nullptr;

    }

  ~FileHandler() {
    std::cout << "FileHandler DESTRUCTOR" << std::endl;
    delete m_decoder;
  }
  void processPacket(uint8_t* data) { m_decoder->processPacket(data); }
  bool isHandlingSocket(int fd) { return fd == socket_fd; }

 private:
  Decoder* m_decoder;
  int socket_fd;
};

namespace KYO {

KLogger* k_logger_ptr = KLogger::GetInstance();

auto KLOG = k_logger_ptr -> get_logger();

class KServer : public SocketListener {
 public:
 /**
  * Constructor
  */
  KServer(int argc, char** argv)
      : SocketListener(argc, argv), file_pending(false), file_pending_fd(-1) {
    KLOG->info("KServer initialized");
  }
  ~KServer() { m_file_handlers.clear(); }

  /**
   * Request Handler
   */
  void set_handler(const Request::RequestHandler&& handler) {
    m_request_handler = handler;
  }

  /**
   * File Transfer Completion
   */
  void onFileHandled(int socket_fd) {
    if (file_pending_fd == socket_fd) {
      KLOG->info("Finished handling file for client {}", socket_fd);
      file_pending_fd = -1;
      file_pending = false;
    }
  }

/**
 * Ongoing File Transfer
 */
  void handlePendingFile(std::shared_ptr<uint8_t[]> s_buffer_ptr,
                         int client_socket_fd) {
    auto handler =
        std::find_if(m_file_handlers.begin(), m_file_handlers.end(),
                     [client_socket_fd](FileHandler& handler) {
                       return handler.isHandlingSocket(client_socket_fd);
                     });
    if (handler != m_file_handlers.end()) {
      handler->processPacket(s_buffer_ptr.get());
    } else {
      std::string filename{"kyo.png"};
      FileHandler file_handler{ client_socket_fd, filename, s_buffer_ptr.get(),
std::bind(&KYO::KServer::onFileHandled, this, client_socket_fd)};
      m_file_handlers.push_back(std::forward<FileHandler>(file_handler));
    }
    return;
  }

/**
 * Start Operation
 */
  void handleStart(std::string decoded_message, int client_socket_fd) {
    // Session
    uuids::uuid const new_uuid = uuids::uuid_system_generator{}();
    m_sessions.push_back(KSession{
      .fd = client_socket_fd, .status = 1, .id = new_uuid
    });
    // Database fetch
    ServerData server_data = m_request_handler("Start");
    // Send welcome
    std::string start_message = createMessage("New Session", server_data);
    sendMessage(client_socket_fd, start_message.c_str(), start_message.size());
    // Send session info
    SessionInfo session_info{
        {"status", std::to_string(1)},
        {"uuid", uuids::to_string(new_uuid)}};
    std::string session_message = createMessage("Session Info", session_info);
    KLOG->info("Sending message: {}", session_message);
    sendMessage(client_socket_fd, session_message.c_str(),
                session_message.size());
  }

/**
 * Execute Operation
 */
  void handleExecute(std::string decoded_message, int client_socket_fd) {
    std::vector<std::string> args = getArgs(decoded_message.c_str());
    if (!args.empty()) {
      KLOG->info("Execute masks received");
      for (const auto& arg : args) {
        KLOG->info("Argument: {}", arg);
        std::string execute_status = m_request_handler(std::stoi(arg));
      }
    }
    std::string execute_response = createMessage("We'll get right on that", "");
    sendMessage(client_socket_fd, execute_response.c_str(),
                execute_response.size());
  }

/**
 * File Upload Operation
 */
  void handleFileUploadRequest(int client_socket_fd) {
    file_pending = true;
    file_pending_fd = client_socket_fd;
    std::string file_ready_message = createMessage("File Ready", "");
    sendMessage(client_socket_fd, file_ready_message.c_str(),
                file_ready_message.size());
  }

/**
 * Operations are the processing of requests
 */
  void handleOperation(std::string decoded_message, int client_socket_fd) {
    KOperation op = getOperation(decoded_message.c_str());
    if (isStartOperation(op.c_str())) {  // Start
        KLOG->info("Start operation");
        handleStart(decoded_message, client_socket_fd);
        return;
      } else if (isStopOperation(op.c_str())) {  // Stop
        KLOG->info(
            "Stop operation. Shutting down client and closing connection");
        shutdown(client_socket_fd, SHUT_RDWR);
        close(client_socket_fd);
        return;
      } else if (isExecuteOperation(op.c_str())) {  // Process execution request
        KLOG->info("Execute operation");
        handleExecute(decoded_message, client_socket_fd);
        return;
      } else if (isFileUploadOperation(op.c_str())) {  // File upload request
        KLOG->info("File upload operation");
        handleFileUploadRequest(client_socket_fd);
        return;
      }
  }

/**
 * Override
 */
  virtual void onMessageReceived(
      int client_socket_fd, std::weak_ptr<uint8_t[]> w_buffer_ptr) override {
    // Get ptr to data
    std::shared_ptr<uint8_t[]> s_buffer_ptr = w_buffer_ptr.lock();

    if (file_pending) {  // Handle packets for incoming file
      KLOG->info("File to be processed");
      handlePendingFile(s_buffer_ptr, client_socket_fd);
      return;
    }
    // For other cases, handle operations or read messages
    std::string decoded_message = getDecodedMessage(s_buffer_ptr);  //
    std::string json_message = getJsonString(decoded_message);
    KLOG->info("Client message: {}", json_message);
    // Handle operations
    if (isOperation(decoded_message.c_str())) {
      KLOG->info("Received operation");
      handleOperation(decoded_message, client_socket_fd);
    }  // isOperation
    // TODO: handle regular messages
  }

 private:
  Request::RequestHandler m_request_handler;
  bool file_pending;
  int file_pending_fd;
  std::vector<FileHandler> m_file_handlers;
  std::vector<KSession> m_sessions;
};
};      // namespace KYO
#endif  // __KSERVER_HPP__
