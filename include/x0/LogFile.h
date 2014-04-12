/* <x0/LogFile.h>
 *
 * This file is part of the x0 web server project and is released under AGPL-3.
 * http://www.xzero.io/
 *
 * (c) 2009-2014 Christian Parpart <trapni@gmail.com>
 */

#pragma once

#include <x0/Buffer.h>
#include <x0/Api.h>
#include <memory>
#include <atomic>
#include <string>
#include <unistd.h>
#include <pthread.h>

namespace x0 {

/**
 * Non-blocking file logging API.
 *
 * An instance of \c LogFile always spawns dedicated writer thread
 * used to write synchronously into the log file.
 *
 */
class X0_API LogFile {
public:
    explicit LogFile(const std::string& path);
    ~LogFile();

    bool write(std::unique_ptr<Buffer>&& message);

    size_t pending() const { return pending_.load(); }
    size_t dropped() const { return dropped_.load(); }
    size_t writeErrors() const { return writeErrors_.load(); }

    void cycle();
    bool cycleNow();

private:
    void init();
    int open();
    static void* start(void* self);
    void main();

private:
    std::string path_;                  ///< path to log file
    int fileFd_;                        ///< fd to log file
    int senderFd_;                      ///< fd to send message buffer to writer thread
    int receiverFd_;                    ///< fd to receive messages from senders
    std::atomic<size_t> pending_;       ///< number of pending messages
    std::atomic<size_t> dropped_;       ///< number of dropped messages (due to overload)
    std::atomic<size_t> writeErrors_;   ///< log-file write error counter
    pthread_t tid_;                     ///< writer thread ID
};

} // namespace x0
