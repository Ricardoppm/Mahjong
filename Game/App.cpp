//
//  App.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "App.hpp"

#include <memory>

#include <Bengine/ScreenList.hpp>

App::App(){}

App::~App(){}

void App::onInit()
{
    
}

void App::addScreens()
{
    mainMenuScreen_ = std::make_unique<MainMenuScreen>(window_.get());
    gameplayScreen_ = std::make_unique<GamePlayScreen>(window_.get());
    editorScreen_ = std::make_unique<EditorScreen>(window_.get());

    screenList_->addScreen( mainMenuScreen_.get());
    screenList_->addScreen( gameplayScreen_.get());
    screenList_->addScreen( editorScreen_.get());

    screenList_->setScreen( mainMenuScreen_->getIndex());
}

void App::onExit()
{
    
}
