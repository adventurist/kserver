// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_INSTATASK_KDATA_H_
#define FLATBUFFERS_GENERATED_INSTATASK_KDATA_H_

#include "flatbuffers/flatbuffers.h"

namespace KData {

struct IGTask;

struct IGTask FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_FILENAME = 6,
    VT_TIME = 8,
    VT_DESCRIPTION = 10,
    VT_HASHTAGS = 12,
    VT_REQUESTED_BY = 14,
    VT_REQUESTED_BY_PHRASE = 16,
    VT_PROMOTE_SHARE = 18,
    VT_LINK_BIO = 20
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const flatbuffers::String *filename() const {
    return GetPointer<const flatbuffers::String *>(VT_FILENAME);
  }
  uint16_t time() const {
    return GetField<uint16_t>(VT_TIME, 0);
  }
  const flatbuffers::String *description() const {
    return GetPointer<const flatbuffers::String *>(VT_DESCRIPTION);
  }
  const flatbuffers::String *hashtags() const {
    return GetPointer<const flatbuffers::String *>(VT_HASHTAGS);
  }
  const flatbuffers::String *requested_by() const {
    return GetPointer<const flatbuffers::String *>(VT_REQUESTED_BY);
  }
  const flatbuffers::String *requested_by_phrase() const {
    return GetPointer<const flatbuffers::String *>(VT_REQUESTED_BY_PHRASE);
  }
  const flatbuffers::String *promote_share() const {
    return GetPointer<const flatbuffers::String *>(VT_PROMOTE_SHARE);
  }
  const flatbuffers::String *link_bio() const {
    return GetPointer<const flatbuffers::String *>(VT_LINK_BIO);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID) &&
           VerifyOffset(verifier, VT_FILENAME) &&
           verifier.VerifyString(filename()) &&
           VerifyField<uint16_t>(verifier, VT_TIME) &&
           VerifyOffset(verifier, VT_DESCRIPTION) &&
           verifier.VerifyString(description()) &&
           VerifyOffset(verifier, VT_HASHTAGS) &&
           verifier.VerifyString(hashtags()) &&
           VerifyOffset(verifier, VT_REQUESTED_BY) &&
           verifier.VerifyString(requested_by()) &&
           VerifyOffset(verifier, VT_REQUESTED_BY_PHRASE) &&
           verifier.VerifyString(requested_by_phrase()) &&
           VerifyOffset(verifier, VT_PROMOTE_SHARE) &&
           verifier.VerifyString(promote_share()) &&
           VerifyOffset(verifier, VT_LINK_BIO) &&
           verifier.VerifyString(link_bio()) &&
           verifier.EndTable();
  }
};

struct IGTaskBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(IGTask::VT_ID, id, 0);
  }
  void add_filename(flatbuffers::Offset<flatbuffers::String> filename) {
    fbb_.AddOffset(IGTask::VT_FILENAME, filename);
  }
  void add_time(uint16_t time) {
    fbb_.AddElement<uint16_t>(IGTask::VT_TIME, time, 0);
  }
  void add_description(flatbuffers::Offset<flatbuffers::String> description) {
    fbb_.AddOffset(IGTask::VT_DESCRIPTION, description);
  }
  void add_hashtags(flatbuffers::Offset<flatbuffers::String> hashtags) {
    fbb_.AddOffset(IGTask::VT_HASHTAGS, hashtags);
  }
  void add_requested_by(flatbuffers::Offset<flatbuffers::String> requested_by) {
    fbb_.AddOffset(IGTask::VT_REQUESTED_BY, requested_by);
  }
  void add_requested_by_phrase(flatbuffers::Offset<flatbuffers::String> requested_by_phrase) {
    fbb_.AddOffset(IGTask::VT_REQUESTED_BY_PHRASE, requested_by_phrase);
  }
  void add_promote_share(flatbuffers::Offset<flatbuffers::String> promote_share) {
    fbb_.AddOffset(IGTask::VT_PROMOTE_SHARE, promote_share);
  }
  void add_link_bio(flatbuffers::Offset<flatbuffers::String> link_bio) {
    fbb_.AddOffset(IGTask::VT_LINK_BIO, link_bio);
  }
  explicit IGTaskBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  IGTaskBuilder &operator=(const IGTaskBuilder &);
  flatbuffers::Offset<IGTask> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<IGTask>(end);
    return o;
  }
};

inline flatbuffers::Offset<IGTask> CreateIGTask(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    flatbuffers::Offset<flatbuffers::String> filename = 0,
    uint16_t time = 0,
    flatbuffers::Offset<flatbuffers::String> description = 0,
    flatbuffers::Offset<flatbuffers::String> hashtags = 0,
    flatbuffers::Offset<flatbuffers::String> requested_by = 0,
    flatbuffers::Offset<flatbuffers::String> requested_by_phrase = 0,
    flatbuffers::Offset<flatbuffers::String> promote_share = 0,
    flatbuffers::Offset<flatbuffers::String> link_bio = 0) {
  IGTaskBuilder builder_(_fbb);
  builder_.add_link_bio(link_bio);
  builder_.add_promote_share(promote_share);
  builder_.add_requested_by_phrase(requested_by_phrase);
  builder_.add_requested_by(requested_by);
  builder_.add_hashtags(hashtags);
  builder_.add_description(description);
  builder_.add_filename(filename);
  builder_.add_id(id);
  builder_.add_time(time);
  return builder_.Finish();
}

inline flatbuffers::Offset<IGTask> CreateIGTaskDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    const char *filename = nullptr,
    uint16_t time = 0,
    const char *description = nullptr,
    const char *hashtags = nullptr,
    const char *requested_by = nullptr,
    const char *requested_by_phrase = nullptr,
    const char *promote_share = nullptr,
    const char *link_bio = nullptr) {
  auto filename__ = filename ? _fbb.CreateString(filename) : 0;
  auto description__ = description ? _fbb.CreateString(description) : 0;
  auto hashtags__ = hashtags ? _fbb.CreateString(hashtags) : 0;
  auto requested_by__ = requested_by ? _fbb.CreateString(requested_by) : 0;
  auto requested_by_phrase__ = requested_by_phrase ? _fbb.CreateString(requested_by_phrase) : 0;
  auto promote_share__ = promote_share ? _fbb.CreateString(promote_share) : 0;
  auto link_bio__ = link_bio ? _fbb.CreateString(link_bio) : 0;
  return KData::CreateIGTask(
      _fbb,
      id,
      filename__,
      time,
      description__,
      hashtags__,
      requested_by__,
      requested_by_phrase__,
      promote_share__,
      link_bio__);
}

inline const KData::IGTask *GetIGTask(const void *buf) {
  return flatbuffers::GetRoot<KData::IGTask>(buf);
}

inline const KData::IGTask *GetSizePrefixedIGTask(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<KData::IGTask>(buf);
}

inline bool VerifyIGTaskBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<KData::IGTask>(nullptr);
}

inline bool VerifySizePrefixedIGTaskBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<KData::IGTask>(nullptr);
}

inline void FinishIGTaskBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<KData::IGTask> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedIGTaskBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<KData::IGTask> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace KData

#endif  // FLATBUFFERS_GENERATED_INSTATASK_KDATA_H_
