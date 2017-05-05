//
//  App.hpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#ifndef App_hpp
#define App_hpp

#include <Bengine/IMainGame.hpp>

#include "MainMenuScreen.hpp"
#include "GamePlayScreen.hpp"
#include "EditorScreen.hpp"

class App : public Bengine::IMainGame
{
public:
    App();
    ~App();
    
    // Called on initialization
    virtual void onInit() override;
    //For adding all screen
    virtual void addScreens() override;
    // Called when exiting
    virtual void onExit() override;
    
private:
    std::unique_ptr<MainMenuScreen> mainMenuScreen_ = nullptr;
    std::unique_ptr<GamePlayScreen> gameplayScreen_ = nullptr;
    std::unique_ptr<EditorScreen> editorScreen_ = nullptr;

};

#endif /* App_hpp */
