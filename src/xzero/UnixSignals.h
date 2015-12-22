// This file is part of the "x0" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//
// x0 is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License v3.0.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
#pragma once

#include <xzero/defines.h>
#include <xzero/executor/Executor.h>
#include <memory>

namespace xzero {

struct UnixSignalInfo;

/**
 * UNIX Signal Notification API.
 */
class UnixSignals {
 public:
  typedef std::function<void(const UnixSignalInfo&)> SignalHandler;
  typedef Executor::HandleRef HandleRef;

  virtual ~UnixSignals() {}

  /**
   * Runs given @p task when given @p signal was received.
   *
   * @param signo UNIX signal number (such as @c SIGTERM) you want
   *              to be called for.
   * @param task  The SignalHandler to execute upon given event.
   *
   * @note If you always want to get notified on a given signal, you must
   *       reregister yourself each time you have been fired.
   *
   * @see Executor::executeOnSignal(int signal, SignalHandler task)
   */
  virtual HandleRef executeOnSignal(int signal, SignalHandler task) = 0;

  /**
   * Creates a platform-dependant instance of UnixSignals.
   */
  static std::unique_ptr<UnixSignals> create(Executor* executor);

  /**
   * Blocks given signal.
   *
   * You can still handle this signal via the UnixSignals API,
   * but not with native system API.
   */
  static void blockSignal(int signo);

  /**
   * Unblocks given signal.
   */
  static void unblockSignal(int signo);

  /**
   * Retrieves the (technical) string representation of the given signal number.
   *
   * @param signo the signal number to convert into a string.
   *
   * @return the 1:1 string representation of the signal number, such as
   *         the string @c "SIGTERM" for the signal @c SIGTERM.
   */
  static std::string toString(int signo);
};

} // namespace xzero