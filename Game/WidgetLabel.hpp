//
//  WidgetLabel.hpp
//  Mahjong
//
//  Created by Ricardo Martins on 14/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#ifndef WidgetLabel_hpp
#define WidgetLabel_hpp

#include <Bengine/GUI.hpp>
#include <Bengine/SpriteBatch.hpp>
#include <Bengine/SpriteFont.hpp>
#include <Bengine/Window.hpp>

class WidgetLabel {
public:
    WidgetLabel() {};
    WidgetLabel(CEGUI::Window* w, const std::string& text, float scale) :
    widget(w), text(text), scale(scale) {
        // Empty
    };
    void draw(Bengine::SpriteBatch& sb, Bengine::SpriteFont& sf, Bengine::Window* w);
    
    CEGUI::Window* widget = nullptr;
    std::string text = "";
    Bengine::ColorRGBA8 color = Bengine::ColorRGBA8(255, 255, 255, 255);
    float scale = 0.7f;
};


#endif /* WidgetLabel_hpp */
