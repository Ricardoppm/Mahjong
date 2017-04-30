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
    gameplayScreen_ = std::make_unique<GamePlayScreen>(window_.get());
    
    screenList_->addScreen( gameplayScreen_.get());
    
    screenList_->setScreen( gameplayScreen_->getIndex());
}

void App::onExit()
{
    
}
