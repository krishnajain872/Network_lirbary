#include "network/protocols/websocket/frame.h"
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>


namespace networklib {
namespace protocols {
namespace websocket {

// Helper function to convert 64-bit big-endian to host byte order
static uint64_t be64toh_safe(const uint8_t *bytes) {
  uint64_t result = 0;
  for (int i = 0; i < 8; ++i) {
    result = (result << 8) | bytes[i];
  }
  return result;
}

FrameParser::FrameParser()
    : state_(State::kHeader), payload_length_(0), bytes_needed_(2) {}

bool FrameParser::Parse(const char *data, size_t len, size_t &consumed,
                        Frame &out_frame) {
  size_t offset = 0;

  // 1. Fixed Header (2 bytes)
  if (state_ == State::kHeader) {
    if (len < 2)
      return false;

    uint8_t byte1 = static_cast<uint8_t>(data[offset]);
    uint8_t byte2 = static_cast<uint8_t>(data[offset + 1]);
    offset += 2;

    current_frame_.fin = (byte1 & 0x80);
    current_frame_.opcode = static_cast<OpCode>(byte1 & 0x0F);
    current_frame_.masked = (byte2 & 0x80);
    uint8_t len_code = (byte2 & 0x7F);

    if (len_code == 126) {
      state_ = State::kPayloadLength;
      bytes_needed_ = 2;
    } else if (len_code == 127) {
      state_ = State::kPayloadLength;
      bytes_needed_ = 8;
    } else {
      payload_length_ = len_code;
      state_ = current_frame_.masked ? State::kMaskKey : State::kPayload;
      bytes_needed_ = current_frame_.masked ? 4 : payload_length_;
    }
  }

  // 2. Extended Payload Length
  if (state_ == State::kPayloadLength) {
    if (len - offset < bytes_needed_)
      return false;

    if (bytes_needed_ == 2) {
      uint16_t l;
      std::memcpy(&l, data + offset, 2);
      payload_length_ = ntohs(l);
    } else {
      // 64-bit length: network byte order (big-endian)
      uint64_t l =
          be64toh_safe(reinterpret_cast<const uint8_t *>(data + offset));

      // RFC 6455: The most significant bit MUST be 0 (i.e., payload length <
      // 2^63)
      if (l & 0x8000000000000000ULL) {
        return false; // Invalid: MSB must be 0
      }

      // Sanity check: reject unreasonably large payloads (> 1GB)
      if (l > 1024 * 1024 * 1024) {
        return false; // Payload too large
      }

      payload_length_ = static_cast<size_t>(l);
    }
    offset += bytes_needed_;
    state_ = current_frame_.masked ? State::kMaskKey : State::kPayload;
    bytes_needed_ = current_frame_.masked ? 4 : payload_length_;
  }

  // 3. Mask Key
  if (state_ == State::kMaskKey) {
    if (len - offset < 4)
      return false;
    std::memcpy(mask_key_, data + offset, 4);
    offset += 4;
    state_ = State::kPayload;
    bytes_needed_ = payload_length_;
  }

  // 4. Payload
  if (state_ == State::kPayload) {
    if (len - offset < payload_length_)
      return false;

    current_frame_.payload.resize(payload_length_);
    std::memcpy(current_frame_.payload.data(), data + offset, payload_length_);

    // Unmask
    if (current_frame_.masked) {
      for (size_t i = 0; i < payload_length_; ++i) {
        current_frame_.payload[i] ^= mask_key_[i % 4];
      }
    }

    offset += payload_length_;
    out_frame = current_frame_;
    consumed = offset;

    // Reset
    state_ = State::kHeader;
    return true;
  }

  return false;
}

} // namespace websocket
} // namespace protocols
} // namespace networklib
