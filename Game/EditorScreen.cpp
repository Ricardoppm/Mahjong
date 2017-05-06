//
//  EditorScreen.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 03/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "EditorScreen.hpp"

#include <Bengine/IMainGame.hpp>
#include <Bengine/IOManager.hpp>


EditorScreen::EditorScreen(Bengine::Window* window):
window_(window)
{
    screenIndex_ = SCREEN_INDEX_EDITOR;
}

EditorScreen::~EditorScreen(){}

int EditorScreen::getNextScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}
int EditorScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_MAINMENU;
}

void EditorScreen::build()
{
    
}

void EditorScreen::destroy()
{
}

void EditorScreen::onEntry()
{
    // Initialize UI
    initUI();
    
    // Initialize Debug Renderer
    debugRenderer_.init();
    
    //Initialize SpriteBatch
    tileSpriteBatch_.init();
    
    board_.init(glm::vec2(10.f,13.1f));

    // Initialize camera
    camera_.init(window_->getScreenWidth(), window_->getScreenHeight());
    camera_.setScale(3.f);
    
    glm::vec2 boardCenter;
    boardCenter.x = board_.getNumTilesWidth() * TILE_DIM_WIDTH / 2.f;
    boardCenter.y = -board_.getNumTilesHeight() * TILE_DIM_HEIGHT / 2.f;
    camera_.setPosition(boardCenter);

    //Init Shaders
    textureProgram_.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    textureProgram_.addAttribute("vertexPosition");
    textureProgram_.addAttribute("vertexColor");
    textureProgram_.addAttribute("vertexUV");
    textureProgram_.linkShaders();
    
    
}

void EditorScreen::onExit()
{
    textureProgram_.dispose();
    
    tileSpriteBatch_.dispose();
    
    gui_.destroy();
}

void EditorScreen::update()
{
    checkInput();
    camera_.update();
    // Ignore board updates while sabe menu is open
    if(saveWindow_->isDisabled())
        board_.update(game_->getInputManager(), camera_);
}

void EditorScreen::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.3f, 1.f);
    
    textureProgram_.use();
    
    // Upload texture uniform
    GLuint textureUniform = textureProgram_.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);
    
    // Camera matrix
    glm::mat4 projectionMatrix = camera_.getCameraMatrix();
    GLuint cameraUniform = textureProgram_.getUniformLocation("transformationMatrix");
    glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    
    // Draw tiles
    tileSpriteBatch_.begin(Bengine::GlyphSortType::BACK_TO_FRONT);
    
    board_.draw(tileSpriteBatch_);
    
    tileSpriteBatch_.end();
    tileSpriteBatch_.renderBatch();
    
    // Draw grid lines
    board_.drawDebug(debugRenderer_);
    debugRenderer_.end();
    debugRenderer_.render(camera_.getCameraMatrix(), 2.f);

    textureProgram_.unuse();
    
    gui_.draw();
    
    glEnable(GL_BLEND);
}

void EditorScreen::initUI()
{
    gui_.init("GUI");
    gui_.loadScheme("TaharezLook.scheme");
    gui_.setFont("DejaVuSans-10");
    
    {// Clear Buttons
        const float X_POS = 0.87f;
        const float Y_POS = 0.05f;
        const float X_DIM = 0.1f;
        const float Y_DIM = 0.05f;
        const float PADDING = 0.07f;
        
        clearButton_ = static_cast<CEGUI::PushButton*>(gui_.createWidget("TaharezLook/Button", glm::vec4(X_POS, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "ClearButton"));
        clearButton_->setText("Clear");
        clearButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScreen::onClearClick, this));
    }

    { // Add save and back buttons
        const float X_POS = 0.87f;
        const float Y_POS = 0.12f;
        const float X_DIM = 0.1f;
        const float Y_DIM = 0.05f;
        const float PADDING = 0.07f;
        
        saveButton_ = static_cast<CEGUI::PushButton*>(gui_.createWidget("TaharezLook/Button", glm::vec4(X_POS, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "SaveButton"));
        saveButton_->setText("Save");
        saveButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScreen::onSaveMouseClick, this));
        
    }
    
    { // Add save window widgets
        saveWindow_ = static_cast<CEGUI::FrameWindow*>(gui_.createWidget("TaharezLook/FrameWindow", glm::vec4(0.3f, 0.3f, 0.4f, 0.4f), glm::vec4(0.0f), "SaveWindow"));
        saveWindow_->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&EditorScreen::onSaveCancelClick, this));
        saveWindow_->setText("Save Level");
        // Don't let user drag window around
        saveWindow_->setDragMovingEnabled(false);
        
        // Children of saveWindow
        saveWindowCombobox_ = static_cast<CEGUI::Combobox*>(gui_.createWidget(saveWindow_, "TaharezLook/Combobox", glm::vec4(0.1f, 0.1f, 0.8f, 0.4f), glm::vec4(0.0f), "SaveCombobox"));
        saveWindowSaveButton_ = static_cast<CEGUI::PushButton*>(gui_.createWidget(saveWindow_, "TaharezLook/Button", glm::vec4(0.35f, 0.8f, 0.3f, 0.1f), glm::vec4(0.0f), "SaveCancelButton"));
        saveWindowSaveButton_->setText("Save");
        saveWindowSaveButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScreen::onSave, this));
        
        // Start disabled
        saveWindow_->setAlpha(0.0f);
        saveWindow_->disable();
    }
    
    /*
     gui_.setMouseCursor("TaharezLook/MouseArrow");
     gui_.showMouseCursor();
     SDL_ShowCursor(0);*/
    
}

void EditorScreen::checkInput()
{
    SDL_Event e;
    while ( SDL_PollEvent(&e)  ) {
        game_->onSDLEvent(e);
        gui_.onSDLEvent(e);
        switch (e.type) {
            case SDL_QUIT:
                onExitClicked(CEGUI::EventArgs());
                break;
        }
    }
}

bool EditorScreen::onExitClicked(const CEGUI::EventArgs& e)
{
    currentState_ = Bengine::ScreenState::EXIT_APPLICATION;
    return true;
}

bool EditorScreen::onClearClick(const CEGUI::EventArgs& e){
    board_.restart();
    return true;
}

bool EditorScreen::onSaveMouseClick(const CEGUI::EventArgs& e) {
    // Make sure levels dir exists
    Bengine::IOManager::makeDirectory("Levels/Custom");
    
    saveWindowCombobox_->clearAllSelections();
    
    // Remove all items
    for (auto& item : saveListBoxItems_) {
        // We don't have to call delete since removeItem does it for us
        saveWindowCombobox_->removeItem(item);
    }
    saveListBoxItems_.clear();
    
    // Get all directory entries
    std::vector<Bengine::DirEntry> entries;
    Bengine::IOManager::getDirectoryEntries("Levels/Custom", entries);
    
    
    // Add all files to list box
    for (auto& e : entries) {
        // Don't add directories
        if (!e.isDirectory) {
            // Remove "Levels/" substring
            e.path.erase(0, std::string("Levels/Custom/").size());
            saveListBoxItems_.push_back(new CEGUI::ListboxTextItem(e.path));
            saveWindowCombobox_->addItem(saveListBoxItems_.back());
        }
    }
    
    saveWindow_->enable();
    saveWindow_->setAlpha(1.0f);
    //loadWindow_->disable();
    //loadWindow_->setAlpha(0.0f);
    return true;
}

bool EditorScreen::onSaveCancelClick(const CEGUI::EventArgs& e) {
    saveWindow_->disable();
    saveWindow_->setAlpha(0.0f);
    return true;
}

bool EditorScreen::onSave(const CEGUI::EventArgs& e) {
    if( board_.getTilesRemaining() == 0){
        puts("Must create player before saving.");
        return true;
    }
    
    puts("Saving game...");
    // Make sure levels dir exists again, for good measure.
    Bengine::IOManager::makeDirectory("Levels/Custom");
    
    // Save in text mode
    std::string text = "Levels/Custom/" + std::string(saveWindowCombobox_->getText().c_str());
    if ( board_.saveBoard(text)) {
        saveWindow_->disable();
        saveWindow_->setAlpha(0.0f);
        puts("File successfully saved.");
    } else {
        puts("Failed to save file.");
    }
    
    return true;
}


