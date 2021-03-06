// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_KMESSAGE_KDATA_H_
#define FLATBUFFERS_GENERATED_KMESSAGE_KDATA_H_

#include "flatbuffers/flatbuffers.h"

namespace KData {

struct Message;

struct Message FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_DATA = 6
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const flatbuffers::Vector<uint8_t> *data() const {
    return GetPointer<const flatbuffers::Vector<uint8_t> *>(VT_DATA);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID) &&
           VerifyOffset(verifier, VT_DATA) &&
           verifier.VerifyVector(data()) &&
           verifier.EndTable();
  }
};

struct MessageBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(Message::VT_ID, id, 0);
  }
  void add_data(flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data) {
    fbb_.AddOffset(Message::VT_DATA, data);
  }
  explicit MessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  MessageBuilder &operator=(const MessageBuilder &);
  flatbuffers::Offset<Message> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Message>(end);
    return o;
  }
};

inline flatbuffers::Offset<Message> CreateMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data = 0) {
  MessageBuilder builder_(_fbb);
  builder_.add_data(data);
  builder_.add_id(id);
  return builder_.Finish();
}

inline flatbuffers::Offset<Message> CreateMessageDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    const std::vector<uint8_t> *data = nullptr) {
  auto data__ = data ? _fbb.CreateVector<uint8_t>(*data) : 0;
  return KData::CreateMessage(
      _fbb,
      id,
      data__);
}

inline const KData::Message *GetMessage(const void *buf) {
  return flatbuffers::GetRoot<KData::Message>(buf);
}

inline const KData::Message *GetSizePrefixedMessage(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<KData::Message>(buf);
}

inline bool VerifyMessageBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<KData::Message>(nullptr);
}

inline bool VerifySizePrefixedMessageBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<KData::Message>(nullptr);
}

inline void FinishMessageBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<KData::Message> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedMessageBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<KData::Message> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace KData

#endif  // FLATBUFFERS_GENERATED_KMESSAGE_KDATA_H_
