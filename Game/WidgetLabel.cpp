//
//  WidgetLabel.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 14/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "WidgetLabel.hpp"

void WidgetLabel::draw(Bengine::SpriteBatch& sb, Bengine::SpriteFont& sf, Bengine::Window* w) {
    if (!widget->isVisible()) return;
    glm::vec2 pos;
    pos.x = widget->getXPosition().d_scale * w->getScreenWidth() - w->getScreenWidth() / 2.0f +
    widget->getWidth().d_offset / 2.0f;
    pos.y = w->getScreenHeight() / 2.0f - widget->getYPosition().d_scale * w->getScreenHeight();
    sf.draw(sb, text.c_str(), pos, glm::vec2(scale), 0.0f, color, Bengine::Justification::MIDDLE);
}
