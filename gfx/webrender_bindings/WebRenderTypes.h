/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GFX_WEBRENDERTYPES_H
#define GFX_WEBRENDERTYPES_H

#include "mozilla/webrender/webrender_ffi.h"
#include "mozilla/Maybe.h"
#include "mozilla/gfx/Types.h"

// The infrastructure and plumbing to use the render thread is not entirely in
// place yet. In order to land code and work in parallel we have to maintain
// the two code paths. Hopefully we'll be able to remove this very soon.
// Note that if you are adding code to the non-MOZ_USE_RENDER_THREAD branches
// it is very likely that you are adding work to someone else and thus making
// him very sad.
//
// The MOZ_USE_RENDER_THREAD are disabled by default because they are not in a
// state where tests can pass. As soon as we can pass the test we'll just remove
// the non-MOZ_USE_RENDER_THREAD code.
// In the mean time, if you are working on making the render thread work, change
// define below to true instead of false.
#define MOZ_USE_RENDER_THREAD false

typedef mozilla::Maybe<WrImageMask> MaybeImageMask;

namespace mozilla {
namespace wr {

static inline WrMixBlendMode ToWrMixBlendMode(gfx::CompositionOp compositionOp)
{
  switch (compositionOp)
  {
      case gfx::CompositionOp::OP_MULTIPLY:
        return WrMixBlendMode::Multiply;
      case gfx::CompositionOp::OP_SCREEN:
        return WrMixBlendMode::Screen;
      case gfx::CompositionOp::OP_OVERLAY:
        return WrMixBlendMode::Overlay;
      case gfx::CompositionOp::OP_DARKEN:
        return WrMixBlendMode::Darken;
      case gfx::CompositionOp::OP_LIGHTEN:
        return WrMixBlendMode::Lighten;
      case gfx::CompositionOp::OP_COLOR_DODGE:
        return WrMixBlendMode::ColorDodge;
      case gfx::CompositionOp::OP_COLOR_BURN:
        return WrMixBlendMode::ColorBurn;
      case gfx::CompositionOp::OP_HARD_LIGHT:
        return WrMixBlendMode::HardLight;
      case gfx::CompositionOp::OP_SOFT_LIGHT:
        return WrMixBlendMode::SoftLight;
      case gfx::CompositionOp::OP_DIFFERENCE:
        return WrMixBlendMode::Difference;
      case gfx::CompositionOp::OP_EXCLUSION:
        return WrMixBlendMode::Exclusion;
      case gfx::CompositionOp::OP_HUE:
        return WrMixBlendMode::Hue;
      case gfx::CompositionOp::OP_SATURATION:
        return WrMixBlendMode::Saturation;
      case gfx::CompositionOp::OP_COLOR:
        return WrMixBlendMode::Color;
      case gfx::CompositionOp::OP_LUMINOSITY:
        return WrMixBlendMode::Luminosity;
      default:
        return WrMixBlendMode::Normal;
  }
}

static inline WrColor ToWrColor(const gfx::Color& color)
{
  WrColor c;
  c.r = color.r;
  c.g = color.g;
  c.b = color.b;
  c.a = color.a;
  return c;
}

static inline WrBorderSide ToWrBorderSide(const LayerCoord width, const gfx::Color& color)
{
  WrBorderSide bs;
  bs.width = width;
  bs.color = ToWrColor(color);
  bs.style = WrBorderStyle::Solid;
  return bs;
}

static inline WrLayoutSize ToWrLayoutSize(const LayerSize size)
{
  WrLayoutSize ls;
  ls.width = size.width;
  ls.height = size.height;
  return ls;
}

template<class T>
static inline WrRect ToWrRect(const gfx::RectTyped<T>& rect)
{
  WrRect r;
  r.x = rect.x;
  r.y = rect.y;
  r.width = rect.width;
  r.height = rect.height;
  return r;
}

template<class T>
static inline WrRect ToWrRect(const gfx::IntRectTyped<T>& rect)
{
  return ToWrRect(IntRectToRect(rect));
}

struct ByteBuffer
{
  ByteBuffer(size_t aLength, uint8_t* aData)
    : mLength(aLength)
    , mData(aData)
    , mOwned(false)
  {}

  ByteBuffer()
    : mLength(0)
    , mData(nullptr)
    , mOwned(false)
  {}

  bool
  Allocate(size_t aLength)
  {
    MOZ_ASSERT(mData == nullptr);
    mData = (uint8_t*)malloc(aLength);
    if (!mData) {
      return false;
    }
    mLength = aLength;
    mOwned = true;
    return true;
  }

  ~ByteBuffer()
  {
    if (mData && mOwned) {
      free(mData);
    }
  }

  const Range<uint8_t> AsSlice() const { return Range<uint8_t>(mData, mLength); }

  Range<uint8_t> AsSlice() { return Range<uint8_t>(mData, mLength); }

  bool operator==(const ByteBuffer& other) const {
    return mLength == other.mLength &&
          !(memcmp(mData, other.mData, mLength));
  }

  size_t mLength;
  uint8_t* mData;
  bool mOwned;
};

struct WindowId {
  explicit WindowId(WrWindowId aHandle) : mHandle(aHandle) {}
  WindowId() : mHandle(0) {}
  bool operator<(const WindowId& aOther) const { return mHandle < aOther.mHandle; }
  bool operator==(const WindowId& aOther) const { return mHandle == aOther.mHandle; }

  WrWindowId mHandle;
};

struct PipelineId {
  explicit PipelineId(WrPipelineId aHandle) : mHandle(aHandle) {}
  PipelineId() : mHandle(0) {}
  bool operator<(const PipelineId& aOther) const { return mHandle < aOther.mHandle; }
  bool operator==(const PipelineId& aOther) const { return mHandle == aOther.mHandle; }

  WrPipelineId mHandle;
};

// TODO: We need to merge this with the notion of transaction id.
struct Epoch {
  explicit Epoch(WrEpoch aHandle) : mHandle(aHandle) {}
  Epoch() : mHandle(0) {}
  bool operator<(const Epoch& aOther) const { return mHandle < aOther.mHandle; }
  bool operator==(const Epoch& aOther) const { return mHandle == aOther.mHandle; }

  WrEpoch mHandle;
};

struct FontKey {
  explicit FontKey(WrFontKey aHandle) : mHandle(aHandle) {}
  FontKey() : mHandle(0) {}
  bool operator<(const FontKey& aOther) const { return mHandle < aOther.mHandle; }
  bool operator==(const FontKey& aOther) const { return mHandle == aOther.mHandle; }

  WrFontKey mHandle;
};

struct ImageKey {
  explicit ImageKey(WrImageKey aHandle) : mHandle(aHandle) {}
  ImageKey() : mHandle(0) {}
  bool operator<(const ImageKey& aOther) const { return mHandle < aOther.mHandle; }
  bool operator==(const ImageKey& aOther) const { return mHandle == aOther.mHandle; }

  WrImageKey mHandle;
};


} // namespace wr
} // namespace mozilla

#endif /* GFX_WEBRENDERTYPES_H */
