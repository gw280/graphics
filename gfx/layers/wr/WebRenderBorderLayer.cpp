/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "WebRenderBorderLayer.h"

#include "LayersLogging.h"
#include "mozilla/gfx/Rect.h"
#include "mozilla/webrender/webrender_ffi.h"
#include "mozilla/layers/WebRenderBridgeChild.h"
#include "mozilla/webrender/WebRenderTypes.h"

namespace mozilla {
namespace layers {

using namespace mozilla::gfx;

void
WebRenderBorderLayer::RenderLayer()
{
  WrScrollFrameStackingContextGenerator scrollFrames(this);

  Rect rect = RelativeToVisible(mRect.ToUnknownRect());
  Rect clip;
  if (GetClipRect().isSome()) {
    clip = RelativeToTransformedVisible(IntRectToRect(GetClipRect().ref().ToUnknownRect()));
  } else {
    clip = rect;
  }

  if (gfxPrefs::LayersDump()) printf_stderr("BorderLayer %p using rect:%s clip:%s\n", this, Stringify(rect).c_str(), Stringify(clip).c_str());

  Rect relBounds = TransformedVisibleBoundsRelativeToParent();
  Rect overflow(0, 0, relBounds.width, relBounds.height);
  Matrix4x4 transform;// = GetTransform();
  WrBridge()->AddWebRenderCommand(
      OpDPPushStackingContext(wr::ToWrRect(relBounds),
                              wr::ToWrRect(overflow),
                              Nothing(),
                              GetAnimations(),
                              transform,
                              WrMixBlendMode::Normal,
                              FrameMetrics::NULL_SCROLL_ID));

  WrBridge()->AddWebRenderCommand(
    OpDPPushBorder(wr::ToWrRect(rect), wr::ToWrRect(clip),
                   wr::ToWrBorderSide(mWidths[0], mColors[0]),
                   wr::ToWrBorderSide(mWidths[1], mColors[1]),
                   wr::ToWrBorderSide(mWidths[2], mColors[2]),
                   wr::ToWrBorderSide(mWidths[3], mColors[3]),
                   wr::ToWrLayoutSize(mCorners[0]),
                   wr::ToWrLayoutSize(mCorners[1]),
                   wr::ToWrLayoutSize(mCorners[3]),
                   wr::ToWrLayoutSize(mCorners[2])));
  if (gfxPrefs::LayersDump()) printf_stderr("BorderLayer %p using %s as bounds/overflow, %s for transform\n", this, Stringify(relBounds).c_str(), Stringify(transform).c_str());

  WrBridge()->AddWebRenderCommand(OpDPPopStackingContext());
}

} // namespace layers
} // namespace mozilla
