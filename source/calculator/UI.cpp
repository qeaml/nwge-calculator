#include "UI.hpp"
#include <nwge/render/draw.hpp>
#include <nwge/render/window.hpp>
#include <utility>

UI::UI(Theme theme)
  : mTheme(theme), mAnchor(mTheme.anchor), mCursor(mAnchor)
{}

UI &UI::display(glm::vec2 size, Display::DrawFn drawFn) {
  mDisplays.push({
    .pos = mCursor + mTheme.margin,
    .size = size,
    .drawFn = std::move(drawFn),
  });
  advanceCursor(size);
  return *this;
}

UI &UI::button(const nwge::StringView &text, glm::vec2 size, CallbackFn click) {
  mButtons.push({
    .pos = mCursor + mTheme.margin,
    .size = size,
    .text = text,
    .click = std::move(click),
  });
  advanceCursor(size);
  return *this;
}

UI &UI::nextLine() {
  advanceToNextLine();
  return *this;
}

bool UI::preload() {
  mBundle.nqFont("VictorMono.cfn"_sv, mFont);
  return true;
}

bool UI::on(nwge::Event &evt) {
  switch(evt.type) {
  case nwge::Event::MouseMotion:
    updateHover(evt.motion.to);
    break;

  case nwge::Event::MouseDown:
    updateHover(evt.click.pos);
    mMouseDownHover = mHover;
    break;

  case nwge::Event::MouseUp:
    updateHover(evt.click.pos);
    if(mHover > -1 && mMouseDownHover == mHover) {
      mButtons[mHover].click();
    }
    mMouseDownHover = -1;
    break;

  default:
    break;

  }
  return true;
}

void UI::render() const {
  nwge::render::clear(mTheme.backgroundColor);

  for(const auto &display: mDisplays) {
    nwge::render::color(mTheme.buttonBackgroundColor);
    nwge::render::rect({display.pos, cButtonBgZ}, {display.size});
    nwge::render::color(mTheme.textColor);
    auto cursor = mFont.cursor({display.pos + mTheme.padding, cButtonTextZ}, mTheme.textH);
    display.drawFn(cursor);
    cursor.draw();
  }

  for(ssize i = 0; i < mButtons.size(); ++i) {
    const auto &button = mButtons[i];
    if(i == mHover) {
      nwge::render::color(mTheme.hoverBackgroundColor);
    } else {
      nwge::render::color(mTheme.buttonBackgroundColor);
    }
    nwge::render::rect({button.pos, cButtonBgZ}, {button.size});

    if(i == mHover) {
      nwge::render::color(mTheme.hoverTextColor);
    } else {
      nwge::render::color(mTheme.textColor);
    }
    glm::vec2 measure = mFont.measure(button.text, mTheme.textH);
    glm::vec2 textPos{
      (button.size.x - measure.x) / 2.0f,
      (button.size.y - measure.y) / 2.0f
    };
    mFont.draw(button.text, {button.pos + textPos, cButtonTextZ}, mTheme.textH);
  }
}

void UI::advanceCursor(glm::vec2 size) {
  mCursor.x += size.x + mTheme.margin;
  mTallestElementOnLine = std::max(mTallestElementOnLine, size.y);
}

void UI::advanceToNextLine() {
  mCursor.x = mAnchor.x;
  mCursor.y += mTallestElementOnLine + mTheme.margin;
  mTallestElementOnLine = 0;
}

void UI::updateHover(glm::vec2 mousePos) {
  for(ssize i = 0; i < mButtons.size(); ++i) {
    const auto &button = mButtons[i];
    if(mousePos.x >= button.pos.x && mousePos.x < button.pos.x + button.size.x
    && mousePos.y >= button.pos.y && mousePos.y <= button.pos.y + button.size.y) {
      mHover = i;
      return;
    }
  }
  mHover = -1;
}
