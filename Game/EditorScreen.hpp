//
//  EditorScreen.hpp
//  Mahjong
//
//  Created by Ricardo Martins on 03/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#ifndef EditorScreen_hpp
#define EditorScreen_hpp


#include <Bengine/Window.hpp>
#include <Bengine/GUI.hpp>
#include <Bengine/Camera2D.hpp>
#include <Bengine/IGameScreen.hpp>

#include "ScreenIndices.h"
#include "EditorBoard.hpp"

class EditorScreen : public Bengine::IGameScreen
{
public:
    EditorScreen(Bengine::Window* window);
    ~EditorScreen();
    
    // Returns the index of the previous/next screen
    virtual int getNextScreenIndex() const override;
    virtual int getPreviousScreenIndex() const override;
    
    // Called at beginning and end of the application
    virtual void build() override;
    virtual void destroy() override;
    
    // Called when screen enters/exits focus
    virtual void onEntry() override;
    virtual void onExit() override;
    
    // Called in the main game loop
    virtual void update() override;
    virtual void draw() override;
    
private:
    void initUI();
    void checkInput();
    
    // CEGUI input handlers
    bool onExitClicked(const CEGUI::EventArgs& e);
    bool onClearClick(const CEGUI::EventArgs& e);

    Bengine::Window* window_;
    Bengine::Camera2D camera_;
    Bengine::GUI gui_;
    Bengine::GLSLProgram textureProgram_;
    Bengine::SpriteBatch tileSpriteBatch_;
    Bengine::DebugRenderer debugRenderer_;
    
    EditorBoard board_;

    // CEGUI elements
    CEGUI::PushButton* clearButton_;

    
};

#endif /* EditorScreen_hpp */
