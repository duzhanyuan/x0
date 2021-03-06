// This file is part of the "x0" project, http://github.com/christianparpart/x0>
//   (c) 2009-2017 Christian Parpart <christian@parpart.family>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <xzero/Api.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace xzero {
namespace util {

class XZERO_BASE_API BinaryMessageReader {
public:
  BinaryMessageReader(void const* buf, size_t buf_len);
  virtual ~BinaryMessageReader() {};

  uint16_t const* readUInt16();
  uint32_t const* readUInt32();
  uint64_t const* readUInt64();
  char const* readString(size_t size);
  void const* read(size_t size);

  void rewind();
  void seekTo(size_t pos);

  size_t remaining() const;

protected:
  void const* ptr_;
  size_t size_;
  size_t pos_;
};

} // namespace util
} // namespace xzero
