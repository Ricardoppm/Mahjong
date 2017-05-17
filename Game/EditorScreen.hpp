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
#include <Bengine/SpriteFont.hpp>

#include "ScreenIndices.h"
#include "EditorBoard.hpp"
#include "WidgetLabel.hpp"


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
    void drawHUD();
    void loadBackground(const std::string& filePath);

    
    // CEGUI input handlers
    bool onExitClicked(const CEGUI::EventArgs& e);
    bool onClearClick(const CEGUI::EventArgs& e);
    bool onDebugToggleClick(const CEGUI::EventArgs& e);
    bool onSaveMouseClick(const CEGUI::EventArgs& e);
    bool onSaveCancelClick(const CEGUI::EventArgs& e);
    bool onSave(const CEGUI::EventArgs& e);
    bool onLoadMouseClick(const CEGUI::EventArgs& e);
    bool onLoadCancelClick(const CEGUI::EventArgs& e);
    bool onLoad(const CEGUI::EventArgs& e);
    bool onLoadDeleteClick(const CEGUI::EventArgs& e);
    
    Bengine::Window* window_;
    Bengine::Camera2D camera_;
    Bengine::GUI gui_;
    Bengine::GLSLProgram textureProgram_;
    Bengine::SpriteBatch tileSpriteBatch_;
    Bengine::SpriteBatch hudSpriteBatch_;
    Bengine::SpriteBatch bgSpriteBatch_;
    Bengine::DebugRenderer debugRenderer_;
    Bengine::SpriteFont* spriteFont_;
    
    EditorBoard board_;

    bool menuActive = false;
    bool debugRender_ = true;
    
    // Holds Widget Labels
    std::vector<WidgetLabel> widgetLabels_;

    
    // CEGUI elements
    CEGUI::PushButton* clearButton_ = nullptr;
    CEGUI::PushButton* saveButton_ = nullptr;
    CEGUI::PushButton* exitButton_ = nullptr;
    CEGUI::PushButton* loadButton_ = nullptr;
    CEGUI::ToggleButton* debugToggle_ = nullptr;
    // Save functionality
    CEGUI::FrameWindow* saveWindow_ = nullptr;
    CEGUI::PushButton* saveWindowSaveButton_ = nullptr;
    CEGUI::Combobox* saveWindowCombobox_ = nullptr;
    std::vector<CEGUI::ListboxTextItem*> saveListBoxItems_;
    // Load functionality
    CEGUI::FrameWindow* loadWindow_ = nullptr;
    CEGUI::PushButton* loadWindowLoadButton_ = nullptr;
    CEGUI::PushButton* loadWindowDeleteButton_ = nullptr;
    CEGUI::Combobox* loadWindowCombobox_ = nullptr;
    std::vector<CEGUI::ListboxTextItem*> loadListBoxItems_;


    
};

#endif /* EditorScreen_hpp */
