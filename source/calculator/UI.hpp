#pragma once

#include <nwge/common/slice.hpp>
#include <nwge/data/bundle.hpp>
#include <nwge/render/Font.hpp>
#include <nwge/state.hpp>
#include <functional>

using CallbackFn = std::function<void()>;

struct Display {
  glm::vec2 pos;
  glm::vec2 size;
  using DrawFn = std::function<void(nwge::render::Cursor&)>;
  DrawFn drawFn;
};

struct Button {
  glm::vec2 pos;
  glm::vec2 size;
  nwge::String<> text;
  CallbackFn click;
};

struct Theme {
  static constexpr f32
    cDefaultMargin = 0.05f,
    cDefaultPadding = 0.05f,
    cDefaultTextH = 0.05f;
  static constexpr glm::vec4
    cDefaultTextColor = {1.0f, 1.0f, 1.0f, 1.0f},
    cDefaultBackgroundColor = {0.0f, 0.0f, 0.0f, 1.0f},
    cDefaultButtonBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f},
    cDefaultHoverTextColor = {1.0f, 1.0f, 1.0f, 1.0f},
    cDefaultHoverBackgroundColor = {0.2f, 0.2f, 0.2f, 1.0f};

  glm::vec2 anchor = {0, 0};
  f32 margin = cDefaultMargin;
  f32 padding = cDefaultPadding;
  f32 textH = cDefaultTextH;
  glm::vec4 textColor = cDefaultTextColor;
  glm::vec4 backgroundColor = cDefaultBackgroundColor;
  glm::vec4 buttonBackgroundColor = cDefaultButtonBackgroundColor;
  glm::vec4 hoverTextColor = cDefaultHoverTextColor;
  glm::vec4 hoverBackgroundColor = cDefaultHoverBackgroundColor;
};

class UI: public nwge::State {
public:
  bool preload() final;
  bool on(nwge::Event &evt) final;
  void render() const final;

protected:
  UI(Theme theme);
  UI &display(glm::vec2 size, Display::DrawFn drawFn);
  UI &button(const nwge::StringView &text, glm::vec2 size, CallbackFn click);
  UI &nextLine();

private:
  static constexpr f32
    cButtonBgZ = 0.6f,
    cButtonTextZ = 0.5f;

  Theme mTheme;
  nwge::data::Bundle mBundle{"calculator"_sv};
  nwge::render::Font mFont;

  nwge::Slice<Display> mDisplays{4};
  nwge::Slice<Button> mButtons{4};

  glm::vec2 mAnchor{0, 0};
  glm::vec2 mCursor{0, 0};
  f32 mTallestElementOnLine = 0.0f;
  void advanceCursor(glm::vec2 size);
  void advanceToNextLine();

  ssize mHover = -1;
  ssize mMouseDownHover = -1;
  void updateHover(glm::vec2 mousePos);
};
