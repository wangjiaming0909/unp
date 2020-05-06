#pragma once
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <memory>

namespace reactor
{

template <typename T, typename Len_T>
class Mess
{
public:
  using MessPtr = std::shared_ptr<T>;
  Mess& operator==(const Mess&) = default;
  ~Mess() {}
  static MessPtr makeMess(const T& m)
  {
    Mess mess{};
    mess.mess_ = new T{};
    mess.len_ = m.ByteSizeLong();
    return mess;
  }

  MessPtr getMess() const {return mess_;}
  Len_T getLen() const {return len_;}
protected:
  Mess() : mess_{}, len_{0} { }
private:
  MessPtr mess_;
  Len_T len_;
};

//every T should define len field in it's proto file
template <typename T, typename Len_T>
class Decoder
{
public:
  enum DecodeState
  {
    IDLE = 0,
    LEN_DECODED,
    PARTIALY_PARSED,
    COMPLETED,
    ERROR
  };
public:
  Decoder() : mess_{}, bytesParsed_{0}, bytesRemainToParse_{0}, state_{IDLE} { }
  ~Decoder() { }

  Len_T decode(const char* d, Len_T len)
  {
    Len_T bytesParsed = 0;
    while(len - bytesParsed > 0 && state_ != COMPLETED)
    {
      switch (state_)
      {
        case IDLE:
          if (len < sizeof(Len_T))
            break;
          bytesParsed += decodeLen(d, len);
          if (bytesParsed == sizeof(Len_T))
          {
            state_ = LEN_DECODED;
          }
          break;
        case LEN_DECODED:
        case PARTIALY_PARSED:
          {
            if (len - bytesParsed < bytesRemainToParse_) return bytesParsed;
            bytesParsed += decodeRemain(d + bytesParsed, len - bytesParsed);
            if (bytesRemainToParse_ == 0) state_ = COMPLETED;
            break;
          }
        case COMPLETED:
          break;
        case ERROR:
          return 0;
        default:
          break;
      }
    }
    return bytesParsed;
  }

  bool isCompleted() const { return state_ == COMPLETED; }

  T& getMess() { return mess_; }

  void reset()
  {
    mess_.Clear();
    bytesParsed_ = 0;
    bytesRemainToParse_ = 0;
    state_ = IDLE;
  }

private:
  Len_T decodeLen(const char* d, Len_T len)
  {
    if (len < sizeof(Len_T)) return 0;
    auto ret = mess_.ParsePartialFromArray(d, sizeof(Len_T));
    if (!ret)
    {
      state_ = ERROR;
      reset();
      return 0;
    }
    bytesParsed_ += sizeof(Len_T);
    bytesRemainToParse_ = mess_.len() - bytesParsed_;
    return sizeof(Len_T);
  }

  Len_T decodeRemain(const char* d, Len_T len)
  {
    auto bytesParsedOld = bytesParsed_;
    if (len <= 0) return 0;
    T tmp{};
    auto ret = tmp.ParsePartialFromArray(d, std::min(len, bytesRemainToParse_));
    if (!ret)
    {
      /*
      state_ = ERROR;
      reset();
      return 0;
      */
    }
    mess_.MergeFrom(tmp);
    if (!mess_.IsInitialized())//if not completed, means len < bytesRemainToParse_
    {
      bytesParsed_ += len;
      bytesRemainToParse_ -= len;
    } else//means one message completed
    {
      bytesParsed_ += bytesRemainToParse_;
      bytesRemainToParse_ = 0;
    }
    return bytesParsed_ - bytesParsedOld;
  }

private:
  T mess_;
  Len_T bytesParsed_;
  Len_T bytesRemainToParse_;
  DecodeState state_;
};

}
