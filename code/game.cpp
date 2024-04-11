#include <stdexcept>
#include <string>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>
#include <iostream>

#include <path_config.h>

#include "game.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Globals that define the OpenGL window and viewport
const char *window_title_g = "Game Demo";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);

// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;


Game::Game(void)
{
    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void)
{

    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
    }

    // Set whether window can be resized
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); 

    // Create a window and its OpenGL context
    window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's OpenGL context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
    }

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Initialize sprite geometry
    sprite_ = new Sprite();
    sprite_->CreateGeometry();

    // Initialize particle geometry
    explosion_particles_ = new Particles();
    dynamic_cast<Particles*>(explosion_particles_)->CreateGeometry(1.0f, 0.0f, 0.0f, 5);

    bullet_particles_ = new Particles();
    dynamic_cast<Particles*>(bullet_particles_)->CreateGeometry(0.06, 0.47f, 0.0f, 100);

    // Initialize background sprite geometry
    background_sprite_ = new BackgroundSprite();
    background_sprite_->CreateGeometry();

    // Initialize health bar sprite geometry
    health_bar_sprite_ = new HealthBarSprite();
    health_bar_sprite_->CreateGeometry();

    // Initialize the various shaders
    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());
    particle_shader_.Init((resources_directory_g + std::string("/particle_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/particle_fragment_shader.glsl")).c_str());
    ui_shader_.Init((resources_directory_g+std::string("/ui_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/ui_fragment_shader.glsl")).c_str());
    text_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/text_fragment_shader.glsl")).c_str());

    // Initialize game state
    state_ = START_SCREEN;

    // Initialize time
    current_time_ = 0.0;
}


Game::~Game()
{
    // Free memory for all objects
    // Only need to delete objects that are not automatically freed
    delete sprite_;
    delete explosion_particles_;
    delete bullet_particles_;
    delete background_sprite_;
    delete health_bar_sprite_;
    for (int i = 0; i < game_objects_.size(); i++){
        std::vector <GameObject*> current_vector = *game_objects_[i];
        for (int j = 0; j < current_vector.size(); j++) {
            if (current_vector[j] != nullptr || current_vector[j] != NULL)
                delete current_vector[j];
        }
    }
    delete explosion_particle;
    delete bullet_particle;

    delete end_screen_lose_;
    delete end_screen_win_;
    delete start_screen_;

    delete[] tex_;
    delete[] background_tex_;

    for (int i = 0; i < turret_templates_.size(); i++) {
        delete turret_templates_[i];
    }

    for (int i = 0; i < bullet_templates_.size(); i++) {
        delete bullet_templates_[i];
    }

    // Close window
    glfwDestroyWindow(window_);
    glfwTerminate();
}


void Game::Setup(void)
{
    Helper::setSeed();

    // Setup the game world

    // Load textures
    SetAllTextures();
    
    // Setup the main GameObject vector
    game_objects_.push_back(&weapon_display_objects_);
    game_objects_.push_back(&player_objects_);
    game_objects_.push_back(&enemy_objects_);
    game_objects_.push_back(&collectible_objects_);
    game_objects_.push_back(&background_objects_);
    game_objects_.push_back(&explosion_objects_);
    game_objects_.push_back(&ui_objects_);
    game_objects_.push_back(&bullet_objects_);
    game_objects_.push_back(&text_objects_);

    // Setup the player object (position, texture, vertex count)
    player_objects_.push_back(new PlayerGameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, player_body_tex_));
    player = dynamic_cast<PlayerGameObject*>(player_objects_[0]);       // player pointer
    float pi_over_two = glm::pi<float>() / 2.0f;
    player_objects_[0]->SetRotation(pi_over_two);

    // Default turret
    auto* default_turret = new TurretObject(player, TurretType::NORMAL, glm::vec3(player->GetPosition().x, player->GetPosition().y, 1.0), sprite_, &sprite_shader_, turret_tex_);
    default_turret->SetScale(2.0f);
    default_turret->SetFireCooldown(0.3f);
    turret_templates_.push_back(default_turret);
    default_turret->setHealth(100.0f);

    // Machine turret
    auto* machine_turret = new TurretObject(player, TurretType::MACHINE_GUN, glm::vec3(player->GetPosition().x, player->GetPosition().y, 1.0), sprite_, &sprite_shader_, tex_[12]);
    machine_turret->SetScale(2.0f);
    machine_turret->SetHitbox(0.7f);
    machine_turret->SetFireCooldown(0.1f);
    turret_templates_.push_back(machine_turret);
    machine_turret->setHealth(500.0f);

    // Cannon
    auto* cannon = new TurretObject(player, TurretType::CANNON, glm::vec3(player->GetPosition().x, player->GetPosition().y, 1.0), sprite_, &sprite_shader_, tex_[16]);
    cannon->SetScale(2.0f);
    cannon->SetHitbox(0.9f);
    cannon->SetFireCooldown(1.0f);
    cannon->SetParentRotationOffset(- glm::pi<float>() / 2.0f);
    turret_templates_.push_back(cannon);

    // homing turret
    auto* homing_turret = new TurretObject(player, TurretType::HOMING, glm::vec3(player->GetPosition().x, player->GetPosition().y, 1.0), sprite_, &sprite_shader_, tex_[26]);
    homing_turret->SetScale(2.0f);
    homing_turret->SetHitbox(0.7f);
    homing_turret->SetFireCooldown(0.5f);
    turret_templates_.push_back(homing_turret);
    homing_turret->setHealth(1000.0f);

    // Setup explosion_particle
    explosion_particle = new ParticleSystem(glm::vec3(0, 0, 0), explosion_particles_, &particle_shader_, particle_tex_, NULL, EXPLOSION_PARTICLE, 5.0f, 0.7f, 0.0f);

    // Setup bullet_particle
    bullet_particle = new ParticleSystem(glm::vec3(0, 0, 0), bullet_particles_, &particle_shader_, particle_tex_, NULL, BULLET_PARTICLE, 1.0f, 10.0f, 0.0f);

    // Set up bullets
    auto* normal_bullet = new BulletObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, bullet_tex_, 30.0f, glm::vec3(0.0f, 1.0f, 0.0f), 20.0f, 2.0f, 0.1f, BulletType::NORMAL);
    normal_bullet->setWeight(80.0f);
    normal_bullet->SetScale(10.0);
    normal_bullet->SetTrail(bullet_particle, 0.2f);
    bullet_templates_.push_back(normal_bullet);

    auto* machine_bullet = new BulletObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[13], 30.0f, glm::vec3(0.0f, 1.0f, 0.0f), 25.0f, 2.0f, 0.1f, BulletType::MACHINE_BULLET);
    machine_bullet->setWeight(30.0f);
    machine_bullet->SetScale(5.0);
    machine_bullet->SetTrail(bullet_particle, 0.1f);
    bullet_templates_.push_back(machine_bullet);

    auto* cannonball = new BulletObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[17], 100.0f, glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, 2.0f, 0.1f, BulletType::CANNONBALL);
    cannonball->setWeight(200.0f);
    cannonball->SetScale(5.0);
    bullet_templates_.push_back(cannonball);

    auto* bigbullet = new BulletObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[19], 100.0f, glm::vec3(0.0f, 1.0f, 0.0f), 15.0f, 10.0f, 0.1f, BulletType::BIGBULLET);
    bigbullet->setWeight(100.0f);
    bigbullet->SetScale(10.0);
    bigbullet->SetTrail(bullet_particle, 0.8f);
    bullet_templates_.push_back(bigbullet);

    auto* homing_bullet = new BulletObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[27], 30.0f, glm::vec3(0.0f, 1.0f, 0.0f), 15.0f, 10.0f, 0.1f, BulletType::HOMING);
    homing_bullet->setWeight(80.0f);
    homing_bullet->SetScale(5.0);
    homing_bullet->SetEnemyObjects(&enemy_objects_);
    bullet_templates_.push_back(homing_bullet);


    default_turret->SetBullet(normal_bullet);
    machine_turret->SetBullet(machine_bullet);
    cannon->SetBullet(cannonball);
    homing_turret->SetBullet(homing_bullet);

    auto* player_turret = new TurretObject(*default_turret);
    player->SetTurret(default_turret);
    player->SetDefaultTurret(default_turret);

    // Setup other objects
    if (TEST) {
        // test
        enemy_objects_.push_back(new MeleeEnemyObject(glm::vec3(2.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, melee_enemy_tex_, player, arm_tex_, saw_tex_));
        enemy_objects_[0]->SetRotation(pi_over_two);

        // Setup collectibles
        auto* four_turrets = new CollectibleGameObject(glm::vec3(5.0f, 5.0f, 0.0f), sprite_, &sprite_shader_, tex_[7], CollectibleType::FOUR_TURRETS);
        collectible_objects_.push_back(four_turrets);

        auto* machine_turret_collectible = new CollectibleGameObject(glm::vec3(-5.0f, -5.0f, 0.0f), sprite_, &sprite_shader_, tex_[14], CollectibleType::MACHINE_GUN);
        collectible_objects_.push_back(machine_turret_collectible);

        auto* shotgun_collectible = new CollectibleGameObject(glm::vec3(5.0f, -5.0f, 0.0f), sprite_, &sprite_shader_, tex_[15], CollectibleType::SHOTGUN);
        collectible_objects_.push_back(shotgun_collectible);

        auto* cannon_collectible = new CollectibleGameObject(glm::vec3(0.0f, -10.0f, 0.0f), sprite_, &sprite_shader_, tex_[18], CollectibleType::CANNON);
        collectible_objects_.push_back(cannon_collectible);

        auto* homing_collectible = new CollectibleGameObject(glm::vec3(0.0f, 10.0f, 0.0f), sprite_, &sprite_shader_, tex_[25], CollectibleType::HOMING);
        collectible_objects_.push_back(homing_collectible);

        // Setup win condition collectibles
        win_1 = new CollectibleGameObject(glm::vec3(20.0f, 20.0f, 0.0f), sprite_, &sprite_shader_, tex_[8], CollectibleType::WIN_CONDITION);
        win_2 = new CollectibleGameObject(glm::vec3(-20.0f, -20.0f, 0.0f), sprite_, &sprite_shader_, tex_[9], CollectibleType::WIN_CONDITION);
        win_3 = new CollectibleGameObject(glm::vec3(20.0f, -20.0f, 0.0f), sprite_, &sprite_shader_, tex_[10], CollectibleType::WIN_CONDITION);
        win_4 = new CollectibleGameObject(glm::vec3(-20.0f, 20.0f, 0.0f), sprite_, &sprite_shader_, tex_[11], CollectibleType::WIN_CONDITION);

        win_1->SetMovement(1.0, 3.5, glm::vec2(win_1->GetPosition().x, win_1->GetPosition().y));
        win_2->SetMovement(1.0, 3.5, glm::vec2(win_2->GetPosition().x, win_2->GetPosition().y));
        win_3->SetMovement(1.0, 3.5, glm::vec2(win_3->GetPosition().x, win_3->GetPosition().y));
        win_4->SetMovement(1.0, 3.5, glm::vec2(win_4->GetPosition().x, win_4->GetPosition().y));

        win_1->SetScale(3.0f);
        win_2->SetScale(3.0f);
        win_3->SetScale(3.0f);
        win_4->SetScale(3.0f);

        // add to collectible objects
        collectible_objects_.push_back(win_1);
        collectible_objects_.push_back(win_2);
        collectible_objects_.push_back(win_3);
        collectible_objects_.push_back(win_4);
        exit_door = new CollectibleGameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[29], CollectibleType::WIN_CONDITION);
        exit_door->SetScale(5.0f);
    }
    else {
        // normal game
        if (SPAWN_ENEMY) {
            while (enemy_objects_.size() < STARTING_ENEMY_COUNT) {
                spawnEnemyNearPlayer();
            }
        }
        // Setup collectibles
        spawnCollectible();
        spawnCollectible();
        spawnCollectible();

        // Setup win condition collectibles
        win_1 = spawnWinningCollectible(tex_[8]);
        win_2 = spawnWinningCollectible(tex_[9]);
        win_3 = spawnWinningCollectible(tex_[10]);
        win_4 = spawnWinningCollectible(tex_[11]);
        exit_door = new CollectibleGameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[29], CollectibleType::WIN_CONDITION);
        exit_door->SetScale(5.0f);
    }
    
    // Setup background
    GameObject* background = new BackgroundControllerObject(background_sprite_, &sprite_shader_, background_tex_, player);
    background_objects_.push_back(background);

    // Setup start & end screens
    end_screen_lose_ = new GameObject(glm::vec3(0, 0, 0), sprite_, &sprite_shader_, end_screen_lose_tex_);
    end_screen_lose_->SetScale(30);

    end_screen_win_ = new GameObject(glm::vec3(0, 0, 0), sprite_, &sprite_shader_, end_screen_win_tex_);
    end_screen_win_->SetScale(30);

    start_screen_ = new GameObject(glm::vec3(0, 0, 0), sprite_, &sprite_shader_, start_screen_tex_);
    start_screen_->SetScale(20);

    // Setup collision handler
    collision_handler.setUp(&player_objects_, &enemy_objects_, &bullet_objects_,
                            &collectible_objects_, turret_templates_, &win_1, &win_2, &win_3, &win_4);


   

    // Setup UI
    UIObject* new_ui = new UIObject(player->GetPosition(), sprite_, &ui_shader_, particle_tex_, player);
    new_ui->setUp(health_bar_sprite_, arrow_tex_, &sprite_shader_, (GameObject**) & win_1, (GameObject**) & win_2, (GameObject**) & win_3, (GameObject**) & win_4);
    ui_objects_.push_back(new_ui);

    // set up text objects
    auto* health_text = new TextGameObject(glm::vec3(0.0f, 0.0f, -10.0f), sprite_, &text_shader_, tex_[24]);
    health_text->SetScale(glm::vec2(12.0/2, 1.0/2));
    health_text->SetOffset(glm::vec2(-8.0f, -8.0f), player);
    text_objects_.push_back(health_text);

    auto* current_weapons_text = new TextGameObject(glm::vec3(0.0f, 0.0f, -10.0f), sprite_, &text_shader_, tex_[24]);
    current_weapons_text->SetScale(glm::vec2(12.0/2, 1.0/2));
    current_weapons_text->SetOffset(glm::vec2(3.5f, -6.5f), player);
    current_weapons_text->SetText("Your current weapons:");
    text_objects_.push_back(current_weapons_text);

    auto* get_to_exit_text = new TextGameObject(glm::vec3(0.0f, 0.0f, -10.0f), sprite_, &text_shader_, tex_[24]);
    get_to_exit_text->SetScale(glm::vec2(12.0/2, 1.0/2));
    get_to_exit_text->SetOffset(glm::vec2(0.0f, 9.0f), player);
    get_to_exit_text->SetText("Get to the exit!");
    get_to_exit_text->SetVisible(false);
    text_objects_.push_back(get_to_exit_text);

    auto* follow_arrow_text1 = new TextGameObject(glm::vec3(0.0f, 0.0f, -10.0f), sprite_, &text_shader_, tex_[24]);
    follow_arrow_text1->SetScale(glm::vec2(15.0/2, 1.0/2));
    follow_arrow_text1->SetOffset(glm::vec2(0.0f, 8.0f), player);
    follow_arrow_text1->SetText("Follow the arrow and collect the");
    text_objects_.push_back(follow_arrow_text1);

    auto* follow_arrow_text2 = new TextGameObject(glm::vec3(0.0f, 0.0f, -10.0f), sprite_, &text_shader_, tex_[24]);
    follow_arrow_text2->SetScale(glm::vec2(15.0/2, 1.0/2));
    follow_arrow_text2->SetOffset(glm::vec2(0.0f, 7.5f), player);
    follow_arrow_text2->SetText("Tokens of Appreciation!");
    text_objects_.push_back(follow_arrow_text2);

    auto* time_text = new TextGameObject(glm::vec3(0.0f, 0.0f, -10.0f), sprite_, &text_shader_, tex_[24]);
    time_text->SetScale(glm::vec2(8.0/2, 1.0/2));
    time_text->SetOffset(glm::vec2(-10.0f, 8.0f), player);
    text_objects_.push_back(time_text);

    auto* finish_text = new TextGameObject(glm::vec3(0.0f, 0.0f, -10.0f), sprite_, &text_shader_, tex_[24]);
    finish_text->SetScale(glm::vec2(18.0/2, 1.0));
    finish_text->SetOffset(glm::vec2(0.0f, -4.0f), player);
    finish_text->SetVisible(false);
    text_objects_.push_back(finish_text);

    // Setup spawn timers
    player_enemy_spawn_timer.setTime(MAX_ENEMY_SPAWN_TIME);
    collectible_1_enemy_spawn_timer.setTime(MAX_ENEMY_SPAWN_TIME);
    collectible_2_enemy_spawn_timer.setTime(MAX_ENEMY_SPAWN_TIME);
    collectible_3_enemy_spawn_timer.setTime(MAX_ENEMY_SPAWN_TIME);
    collectible_4_enemy_spawn_timer.setTime(MAX_ENEMY_SPAWN_TIME);
    collectible_spawn_timer.setTime(COLLECTIBLE_SPAWN_TIME);

}

void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}


void Game::SetTexture(GLuint w, const char *fname)
{
    // Bind texture buffer
    glBindTexture(GL_TEXTURE_2D, w);

    // Load texture from a file to the buffer
    int width, height;
    unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
    if (!image){
        std::cout << "Cannot load texture " << fname << std::endl;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Texture Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Game::SetAllTextures(void)
{
    // Load all textures that we will need
    // Declare all the textures here
    const char *texture[] =
        {"/textures/player.png", "/textures/green_enemy.png", "/textures/blue_enemy.png",
        "/textures/red_enemy.png", "/textures/orb.png", "/textures/turret.png",
        "/textures/bullet.png", "/textures/4turret_collectible.png", "/textures/win1.png",
        "/textures/win2.png", "/textures/win3.png", "/textures/win4.png", "/textures/machine_turret.png",
        "/textures/machine_bullet.png", "/textures/machine_turret_collectible.png", "/textures/shotgun_collectible.png",
        "/textures/cannon.png", "/textures/cannonball.png", "/textures/cannon_collectible.png",
        "/textures/bigbullet.png",  "/textures/losing.png", "/textures/winning.png",
        "/textures/arrow.png", "/textures/normal_turret_collectible.png", "/textures/font.png",
        "/textures/homing_turret_collectible.png", "/textures/homing_turret.png", "/textures/homing_bullet.png",
        "/textures/start_screen.png", "/textures/door.png", "/textures/arm.png",
        "/textures/saw.png" };
    // Declare background textures
    const char *background_texture[] = {"/textures/background.png"};
    // Get number of declared textures
    int num_textures = sizeof(texture) / sizeof(char *);
    int num_background_textures = sizeof(background_texture) / sizeof(char*);
    // Allocate a buffer for all texture references
    tex_ = new GLuint[num_textures];
    background_tex_ = new GLuint[num_background_textures];
    glGenTextures(num_textures, tex_);
    glGenTextures(num_background_textures, background_tex_);
    // Load each texture
    for (int i = 0; i < num_textures; i++){
        SetTexture(tex_[i], (resources_directory_g+std::string(texture[i])).c_str());
    }
    // Load background texture
    for (int i = 0; i < num_background_textures; i++){
        SetTexture(background_tex_[i], (resources_directory_g+std::string(background_texture[i])).c_str());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    }
    // Set first texture in the array as default
    glBindTexture(GL_TEXTURE_2D, tex_[0]);

    // set texture references
    player_body_tex_ = tex_[0];
    turret_tex_ = tex_[5];

    bullet_tex_ = tex_[6];

    melee_enemy_tex_ = tex_[1];
    ranged_enemy_tex_ = tex_[2];
    charge_enemy_tex_ = tex_[3];
    
    particle_tex_ = tex_[4];

    end_screen_lose_tex_ = tex_[20];
    end_screen_win_tex_ = tex_[21];
    start_screen_tex_ = tex_[28];

    arrow_tex_ = tex_[22];

    arm_tex_ = tex_[30];
    saw_tex_ = tex_[31];
}


void Game::MainLoop(void)
{
    // Loop while the user did not close the window
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window_)){

        // Calculate delta time
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Update window events like input handling
        glfwPollEvents();

        // Handle user input
        switch (state_) {
        case IN_GAME:
            HandleControlsInGame(delta_time);
            break;
        case END_SCREEN_LOSE:
        case END_SCREEN_WIN:
            HandleControlsEndScreen(delta_time);
            break;
        case START_SCREEN:
             HandleControlsStartScreen(delta_time);
             break;
        }

        // Update all the game objects
        Update(delta_time);

        // Render all the game objects
        Render();

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);
    }
}


void Game::HandleControlsInGame(double delta_time)
{
    // Get current position and angle
    glm::vec3 curpos = player->GetPosition();
    float angle = player->GetRotation();
    // Compute current bearing direction
    glm::vec3 dir = player->GetBearing();
    // Adjust motion increment and angle increment 
    // if translation or rotation is too slow
    float speed = delta_time*1500.0;
    float motion_increment = 0.01*speed;
    float angle_increment = (glm::pi<float>() / 1800.0f)*speed;

    // Check for player input and make changes accordingly
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
    {
        front_input = 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_RELEASE)
    {
        front_input = 0.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
    {
        back_input = 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_RELEASE)
    {
        back_input = 0.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
    {
        right_input = 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_RELEASE)
    {
        right_input = 0.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
    {
        left_input = 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_RELEASE)
    {
        left_input = 0.0f;
    }

    // shoot bullet
    // primary key
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        player->ShootTurrets(bullet_objects_, WeaponShootKey::PRIMARY);
    }

    // secondary key
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
        player->ShootTurrets(bullet_objects_, WeaponShootKey::SECONDARY);
    }

    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_, true);
    }

    if (glfwGetKey(window_, GLFW_KEY_Z) == GLFW_PRESS) {
        // test key
        if (TEST) {
            spawnEnemyNearPlayer();
        }
    }

    if (glfwGetKey(window_, GLFW_KEY_X) == GLFW_PRESS) {
        if (TEST) {
            player->SetPosition(glm::vec3(3, 199, 0));
        }
    }

    // Get mouse pixel position in the window
    double mouseX, mouseY;
    glfwGetCursorPos(window_, &mouseX, &mouseY);

    // Get window size
    int width, height;
    glfwGetWindowSize(window_, &width, &height);

    // normalize mouse position, centered at the middle of the window
    float x = (float)mouseX - (width / 2);
    float y = - ((float)mouseY - (height / 2));

    x = x / width;
    y = y / height;

    // call player's TurnTurrets method
    player->TurnTurrets(glm::vec2(x, y), delta_time);
}


void Game::HandleControlsEndScreen(double delta_time)
{
    // Check for player input and make changes accordingly
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
    if (glfwGetKey(window_, GLFW_KEY_COMMA) == GLFW_PRESS) {
        state_ = IN_GAME;   // for testing
    }
}


void Game::HandleControlsStartScreen(double delta_time)
{
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
    // start the game if the player presses the space key
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
        state_ = IN_GAME;
        start_time = glfwGetTime();
    }
}


void Game::Update(double delta_time)
{
    // Update time
    current_time_ += delta_time;

    // Update movement input vecotr
    movement_input.x = front_input - back_input;
    movement_input.y = right_input - left_input;
    player->SetMovementInput(movement_input);

    switch (state_) {
    case START_SCREEN: {
        break;
    }
    case IN_GAME: {

        // Update all game objects
        for (int i = 0; i < game_objects_.size(); i++) {
            std::vector<GameObject*> current_vector = *game_objects_[i];

            for (int j = 0; j < current_vector.size(); j++) {
                // Get current game object
                GameObject* current_game_object = current_vector[j];

                // Update current game object
                current_game_object->Update(delta_time);
            }
        }

        // remove dead objects
        // Player
        if (player->isDead()) {
            state_ = END_SCREEN_LOSE;
        }

        // Bullets
        for (int j = 0; j < bullet_objects_.size(); j++) {
            // dynamic cast to bullet object
            BulletObject* bullet = dynamic_cast<BulletObject*>(bullet_objects_[j]);

            if (!bullet->IsAlive())
            {
                Helper::removeByIndex(&bullet_objects_, j);
                --j;
            }
        }

        // Enemies  
        for (int i = 0; i < enemy_objects_.size(); i++) {
            GameObject* current_enemy = enemy_objects_[i];

            if (current_enemy->isDead()) {
                // spawn explosion
                spawnExplosion(current_enemy->GetPosition(), current_enemy->GetScale());
                // remove enemy
                Helper::removeByIndex(&enemy_objects_, i);
                --i;
            }

            // too far from player
            if (glm::length(current_enemy->GetPosition() - player->GetPosition()) >= ENEMY_DESPAWN_RANGE) {
                // remove enemy
                Helper::removeByIndex(&enemy_objects_, i);
                --i;
            }
        }

        // Explosion
        for (int i = 0; i < explosion_objects_.size(); i++) {
            Explosion* current_explosion = dynamic_cast<Explosion*>(explosion_objects_[i]);

            if (current_explosion->timesUp()) {
                // remove explosion
                Helper::removeByIndex(&explosion_objects_, i);
            }
        }

        // Spawn enemiesS

        // near player
        if (player_enemy_spawn_timer.timesUp()) {
            if (SPAWN_ENEMY) {
                spawnEnemyNearPlayer();
                // randomize enemy spawn time
                float new_time = Helper::generateFloat(MIN_ENEMY_SPAWN_TIME, max(MIN_ENEMY_SPAWN_TIME, (float)(MAX_ENEMY_SPAWN_TIME - current_time_ / 20)));
                player_enemy_spawn_timer.setTime(new_time);
            }
        }

        // near win collectibles
        if (SPAWN_ENEMY) {
            spawnEnemyNearCollectible();
        }

        // Spawn collectibles

        if (collectible_spawn_timer.timesUp()) {
            spawnCollectible();
            // randomize collectible spawn time
            float new_time = Helper::generateFloat(COLLECTIBLE_SPAWN_TIME / 4, COLLECTIBLE_SPAWN_TIME);
            collectible_spawn_timer.setTime(new_time);
        }

        // handle collision
        collision_handler.Update(delta_time);

        // Update timers
        player_enemy_spawn_timer.Update(delta_time);
        
        if (win_1 != NULL && glm::length(player->GetPosition() - win_1->GetPosition()) <= COLLECIBLE_ENEMY_SPAWN_RANGE) {
            collectible_1_enemy_spawn_timer.Update(delta_time);
        }
        if (win_2 != NULL && glm::length(player->GetPosition() - win_2->GetPosition()) <= COLLECIBLE_ENEMY_SPAWN_RANGE) {
            collectible_2_enemy_spawn_timer.Update(delta_time);
        }
        if (win_3 != NULL && glm::length(player->GetPosition() - win_3->GetPosition()) <= COLLECIBLE_ENEMY_SPAWN_RANGE) {
            collectible_3_enemy_spawn_timer.Update(delta_time);
        }
        if (win_4 != NULL && glm::length(player->GetPosition() - win_4->GetPosition()) <= COLLECIBLE_ENEMY_SPAWN_RANGE) {
            collectible_4_enemy_spawn_timer.Update(delta_time);
        }

        collectible_spawn_timer.Update(delta_time);

        // clear everything from weapon display
        for (auto & weapon_display_object : weapon_display_objects_) {
            delete weapon_display_object;
        }
        weapon_display_objects_.clear();

        // loop through player's turrets, then create an icon for each one
        // the icons are just collectible objects that moves with the player and doesn't get its collision checked
        vector<TurretObject *> *player_turrets = player->GetTurrets();
        for (int i = 0; i < player_turrets->size(); i++) {
            TurretObject* current_turret = player_turrets->at(i);
            CollectibleGameObject* icon;
            switch (current_turret->GetType()) {
                case TurretType::NORMAL:{
                    icon = new CollectibleGameObject(glm::vec3(player->GetPosition().x + i*1.8+1.0, player->GetPosition().y - 8.0, 0), sprite_, &sprite_shader_, tex_[23], CollectibleType::FOUR_TURRETS);
                    break;
                }
                case TurretType::MACHINE_GUN:{
                    icon = new CollectibleGameObject(glm::vec3(player->GetPosition().x + i*1.8+1.0, player->GetPosition().y - 8.0, 0), sprite_, &sprite_shader_, tex_[14], CollectibleType::MACHINE_GUN);
                    break;
                }
                case TurretType::CANNON:
                {
                    icon = new CollectibleGameObject(glm::vec3(player->GetPosition().x + i*1.8+1.0, player->GetPosition().y - 8.0, 0), sprite_, &sprite_shader_, tex_[18], CollectibleType::CANNON);
                    break;
                }
                case TurretType::HOMING:
                {
                    icon = new CollectibleGameObject(glm::vec3(player->GetPosition().x + i*1.8+1.0, player->GetPosition().y - 8.0, 0), sprite_, &sprite_shader_, tex_[25], CollectibleType::HOMING);
                    break;
                }
            }
            icon->SetRotation(-glm::pi<float>() / 2.0f);
            icon->SetScale(2.0f);
            weapon_display_objects_.push_back(icon);
        }

        // update text objects
        for (int i = 0; i < text_objects_.size(); i++) {
            text_objects_[i]->Update(delta_time);
        }

        // check health and update UI
        auto* health_number = dynamic_cast<TextGameObject*>(text_objects_[0]);
        string health_text = "Health: " + std::to_string((int)player->getHealth()) + " / " + std::to_string((int)player->getMaxHealth());
        health_number->SetText(health_text);

        // check all 4 win conditions are collected, then spawn the door at 0,0
        if (win_1 == nullptr && win_2 == nullptr && win_3 == nullptr && win_4 == nullptr && !exit_door_opened) {
            exit_door->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
            exit_door_opened = true;
            win_1 = exit_door;
            collectible_objects_.push_back(exit_door);

            // update the instructions
            auto get_to_exit_text = dynamic_cast<TextGameObject*>(text_objects_[2]);
            get_to_exit_text->SetVisible(true);
            auto follow_arrow_text1 = dynamic_cast<TextGameObject*>(text_objects_[3]);
            follow_arrow_text1->SetVisible(false);
            auto follow_arrow_text2 = dynamic_cast<TextGameObject*>(text_objects_[4]);
            follow_arrow_text2->SetVisible(false);
        }

        // update timer
        auto* time_text = dynamic_cast<TextGameObject*>(text_objects_[5]);
        double time_elapsed = glfwGetTime() - start_time;
        int minutes = time_elapsed / 60.0;
        int seconds = time_elapsed - minutes * 60.0;
        string time_string = "Time: " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
        time_text->SetText(time_string);

        break;
    }
    case END_SCREEN_LOSE:
        break;

    case END_SCREEN_WIN: {
        // if we haven't calculated the finish time yet, calculate it and set the text to that time
        if (finish_time == -1.0) {
            finish_time = glfwGetTime() - start_time;
            auto* finish_text = dynamic_cast<TextGameObject*>(text_objects_[6]);
            finish_text->SetVisible(true);
            int minutes = finish_time / 60.0;
            int seconds = finish_time - minutes * 60.0;
            string time_string = "Final time: " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
            finish_text->SetText(time_string);
        } else {
            auto* finish_text = dynamic_cast<TextGameObject*>(text_objects_[6]);
            finish_text->Update(delta_time);
        }
        break;
    }
    }

    if (exit_door_opened) {
        if (win_1 == nullptr) {
            state_ = END_SCREEN_WIN;
        } else {
            exit_door->Update(delta_time);
        }
    }
}


void Game::Render(void){

    // Clear background
    glClearColor(viewport_background_color_g.r,
                 viewport_background_color_g.g,
                 viewport_background_color_g.b, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use aspect ratio to properly scale the window
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    glm::mat4 window_scale_matrix;
    if (width > height){
        float aspect_ratio = ((float) width)/((float) height);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/aspect_ratio, 1.0f, 1.0f));
    } else {
        float aspect_ratio = ((float) height)/((float) width);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f/aspect_ratio, 1.0f));
    }

    // Set view to zoom out
    float camera_zoom = 0.10f;
    glm::mat4 camera_zoom_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(camera_zoom, camera_zoom, camera_zoom));
    glm::mat4 camera_translate_matrix;
    glm::mat4 view_matrix;

    switch (state_) {
    case START_SCREEN: {
        // set camera at 0, 0, 0
        camera_translate_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        view_matrix = window_scale_matrix * camera_zoom_matrix * camera_translate_matrix;

        start_screen_->Render(view_matrix, current_time_);
        break;
    }
    case IN_GAME:
        // set camera as follow player
        camera_translate_matrix = glm::translate(glm::mat4(1.0f), -player->GetPosition());
        view_matrix = window_scale_matrix * camera_zoom_matrix * camera_translate_matrix;

        // Render all game objects
        for (int i = 0; i < game_objects_.size(); i++) {
            std::vector<GameObject*> current_vector = *game_objects_[i];

            for (int j = 0; j < current_vector.size(); j++) {
                current_vector[j]->Render(view_matrix, current_time_);
            }
        }

        if (exit_door_opened) {
            exit_door->Render(view_matrix, current_time_);
        }

        break;
    
    case END_SCREEN_LOSE:
        // set camera at 0, 0, 0
        camera_translate_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        view_matrix = window_scale_matrix * camera_zoom_matrix * camera_translate_matrix;

        end_screen_lose_->Render(view_matrix, current_time_);
        break;

    case END_SCREEN_WIN:
        // set camera at 0, 0, 0
        camera_translate_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        view_matrix = window_scale_matrix * camera_zoom_matrix * camera_translate_matrix;

        end_screen_win_->Render(view_matrix, current_time_);
        text_objects_[6]->Render(view_matrix, current_time_);

        break;
    }
}


// Object Spawner
void Game::spawnExplosion(glm::vec3 position, float scale) {
    GameObject* new_explosion = new Explosion(position, explosion_particle->getGeometry(), explosion_particle->getShader(), explosion_particle->getTexture(), explosion_particle->getType(), explosion_particle->getCycle(), explosion_particle->getSpeed(), explosion_particle->getGravity(), scale);
    explosion_objects_.push_back(new_explosion);
}


void Game::spawnEnemy(glm::vec3 position, EnemyType enemy_type) {
    float pi_over_two = glm::pi<float>() / 2.0f;
    GameObject* new_enemy;

    switch (enemy_type) {
    case EnemyType::MELEE_ENEMY:
        new_enemy = new MeleeEnemyObject(position, sprite_, &sprite_shader_, melee_enemy_tex_, player, arm_tex_, saw_tex_);
        break;

    case EnemyType::RANGED_ENEMY:
        new_enemy = new RangedEnemyObject(position, sprite_, &sprite_shader_, ranged_enemy_tex_, player, &bullet_objects_);
        dynamic_cast<RangedEnemyObject*>(new_enemy)->setBullets(bullet_templates_[0]);
        break;

    case EnemyType::CHARGE_ENEMY:
        new_enemy = new ChargeEnemyObject(position, sprite_, &sprite_shader_, charge_enemy_tex_, player, &bullet_objects_);
        dynamic_cast<ChargeEnemyObject*>(new_enemy)->setBullets(bullet_templates_[3]);
        break;
    }

    new_enemy->SetRotation(pi_over_two);
    enemy_objects_.push_back(new_enemy);
}


void Game::spawnEnemyNearPlayer() {
    if (enemy_objects_.size() < MAX_ENEMY_COUNT) {
        int total_spawn_weight = MELEE_ENEMY_SPAWN_WEIGHT + RANGED_ENEMY_SPAWN_WEIGHT + CHARGE_ENEMY_SPAWN_WEIGHT;
        int spawn_type = Helper::generateFloat(0, total_spawn_weight);

        float new_enemy_hitbox;
        EnemyType new_enemy_type;

        // checks which type to spawn
        if (spawn_type < MELEE_ENEMY_SPAWN_WEIGHT) {
            // spawns melee
            new_enemy_hitbox = MELEE_ENEMY_SIZE / 2;
            new_enemy_type = EnemyType::MELEE_ENEMY;
        }
        else if (spawn_type < RANGED_ENEMY_SPAWN_WEIGHT + MELEE_ENEMY_SPAWN_WEIGHT){
            // spawns ranged
            new_enemy_hitbox = RANGED_ENEMY_SIZE / 2;
            new_enemy_type = EnemyType::RANGED_ENEMY;
        }
        else {
            // spawns charge
            new_enemy_hitbox = CHARGE_ENEMY_SIZE / 2;
            new_enemy_type = EnemyType::CHARGE_ENEMY;
        }


        for (int i = 0; i < SPAWN_ATTEMPT; i++) {
            bool collides = false;
            glm::vec3 new_position = Helper::generate2dPosition(player->GetPosition(), MIN_SPAWNABLE_DISTANCE_FROM_PLAYER, MAX_SPAWNABLE_DISTANCE_FROM_PLAYER);

            // check for collisions with other enemies
            for (int j = 0; j < enemy_objects_.size(); j++) {
                GameObject* current_enemy = enemy_objects_[j];
                float distance_to_enemy = glm::length(new_position - current_enemy->GetPosition());
                if (distance_to_enemy < (new_enemy_hitbox + current_enemy->getHitbox())) {
                    // collides with enemy
                    collides = true;
                    break;
                }
            }

            if (!collides) {    // no collision 
                spawnEnemy(new_position, new_enemy_type);
                break;
            }
        }
    }
}

void Game::spawnEnemyNearCollectible() {
    if (enemy_objects_.size() < MAX_ENEMY_COUNT) {
        // win 1

        if (win_1 != NULL) {
            if (collectible_1_enemy_spawn_timer.timesUp()) {
                
                // randomize enemy spawn time
                float new_time = Helper::generateFloat(MIN_ENEMY_SPAWN_TIME, MAX_ENEMY_SPAWN_TIME);
                collectible_1_enemy_spawn_timer.setTime(new_time);

                int total_spawn_weight = MELEE_ENEMY_SPAWN_WEIGHT + RANGED_ENEMY_SPAWN_WEIGHT + CHARGE_ENEMY_SPAWN_WEIGHT;
                int spawn_type = Helper::generateFloat(0, total_spawn_weight);

                float new_enemy_hitbox;
                EnemyType new_enemy_type;

                // checks which type to spawn
                if (spawn_type < MELEE_ENEMY_SPAWN_WEIGHT) {
                    // spawns melee
                    new_enemy_hitbox = MELEE_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::MELEE_ENEMY;
                }
                else if (spawn_type < RANGED_ENEMY_SPAWN_WEIGHT + MELEE_ENEMY_SPAWN_WEIGHT) {
                    // spawns ranged
                    new_enemy_hitbox = RANGED_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::RANGED_ENEMY;
                }
                else {
                    // spawns charge
                    new_enemy_hitbox = CHARGE_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::CHARGE_ENEMY;
                }


                for (int i = 0; i < SPAWN_ATTEMPT; i++) {
                    bool collides = false;
                    glm::vec3 new_position = Helper::generate2dPosition(win_1->GetPosition(), MIN_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE, MAX_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE);
                    // check for collisions with players

                    float distance_to_player = glm::length(new_position - player->GetPosition());
                    if (distance_to_player < NON_SPAWNABLE_DISTANCE_FROM_PLAYER) {
                        // too close to player
                        collides = true;
                        break;
                    }


                    // check for collisions with other enemies
                    for (int j = 0; j < enemy_objects_.size(); j++) {
                        GameObject* current_enemy = enemy_objects_[j];
                        float distance_to_enemy = glm::length(new_position - current_enemy->GetPosition());
                        if (distance_to_enemy < (new_enemy_hitbox + current_enemy->getHitbox())) {
                            // collides with enemy
                            collides = true;
                            break;
                        }
                    }

                    if (!collides) {    // no collision 
                        spawnEnemy(new_position, new_enemy_type);
                        break;
                    }
                }
            }
        }

        // win 2

        if (win_2 != NULL) {
            if (collectible_2_enemy_spawn_timer.timesUp()) {

                // randomize enemy spawn time
                float new_time = Helper::generateFloat(MIN_ENEMY_SPAWN_TIME, MAX_ENEMY_SPAWN_TIME);
                collectible_2_enemy_spawn_timer.setTime(new_time);

                int total_spawn_weight = MELEE_ENEMY_SPAWN_WEIGHT + RANGED_ENEMY_SPAWN_WEIGHT + CHARGE_ENEMY_SPAWN_WEIGHT;
                int spawn_type = Helper::generateFloat(0, total_spawn_weight);

                float new_enemy_hitbox;
                EnemyType new_enemy_type;

                // checks which type to spawn
                if (spawn_type < MELEE_ENEMY_SPAWN_WEIGHT) {
                    // spawns melee
                    new_enemy_hitbox = MELEE_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::MELEE_ENEMY;
                }
                else if (spawn_type < RANGED_ENEMY_SPAWN_WEIGHT + MELEE_ENEMY_SPAWN_WEIGHT) {
                    // spawns ranged
                    new_enemy_hitbox = RANGED_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::RANGED_ENEMY;
                }
                else {
                    // spawns charge
                    new_enemy_hitbox = CHARGE_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::CHARGE_ENEMY;
                }


                for (int i = 0; i < SPAWN_ATTEMPT; i++) {
                    bool collides = false;
                    glm::vec3 new_position = Helper::generate2dPosition(win_2->GetPosition(), MIN_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE, MAX_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE);
                    // check for collisions with players

                    float distance_to_player = glm::length(new_position - player->GetPosition());
                    if (distance_to_player < NON_SPAWNABLE_DISTANCE_FROM_PLAYER) {
                        // too close to player
                        collides = true;
                        break;
                    }


                    // check for collisions with other enemies
                    for (int j = 0; j < enemy_objects_.size(); j++) {
                        GameObject* current_enemy = enemy_objects_[j];
                        float distance_to_enemy = glm::length(new_position - current_enemy->GetPosition());
                        if (distance_to_enemy < (new_enemy_hitbox + current_enemy->getHitbox())) {
                            // collides with enemy
                            collides = true;
                            break;
                        }
                    }

                    if (!collides) {    // no collision 
                        spawnEnemy(new_position, new_enemy_type);
                        break;
                    }
                }
            }
        }

        // win 3

        if (win_3 != NULL) {
            if (collectible_3_enemy_spawn_timer.timesUp()) {

                // randomize enemy spawn time
                float new_time = Helper::generateFloat(MIN_ENEMY_SPAWN_TIME, MAX_ENEMY_SPAWN_TIME);
                collectible_3_enemy_spawn_timer.setTime(new_time);

                int total_spawn_weight = MELEE_ENEMY_SPAWN_WEIGHT + RANGED_ENEMY_SPAWN_WEIGHT + CHARGE_ENEMY_SPAWN_WEIGHT;
                int spawn_type = Helper::generateFloat(0, total_spawn_weight);

                float new_enemy_hitbox;
                EnemyType new_enemy_type;

                // checks which type to spawn
                if (spawn_type < MELEE_ENEMY_SPAWN_WEIGHT) {
                    // spawns melee
                    new_enemy_hitbox = MELEE_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::MELEE_ENEMY;
                }
                else if (spawn_type < RANGED_ENEMY_SPAWN_WEIGHT + MELEE_ENEMY_SPAWN_WEIGHT) {
                    // spawns ranged
                    new_enemy_hitbox = RANGED_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::RANGED_ENEMY;
                }
                else {
                    // spawns charge
                    new_enemy_hitbox = CHARGE_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::CHARGE_ENEMY;
                }


                for (int i = 0; i < SPAWN_ATTEMPT; i++) {
                    bool collides = false;
                    glm::vec3 new_position = Helper::generate2dPosition(win_3->GetPosition(), MIN_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE, MAX_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE);
                    // check for collisions with players

                    float distance_to_player = glm::length(new_position - player->GetPosition());
                    if (distance_to_player < NON_SPAWNABLE_DISTANCE_FROM_PLAYER) {
                        // too close to player
                        collides = true;
                        break;
                    }


                    // check for collisions with other enemies
                    for (int j = 0; j < enemy_objects_.size(); j++) {
                        GameObject* current_enemy = enemy_objects_[j];
                        float distance_to_enemy = glm::length(new_position - current_enemy->GetPosition());
                        if (distance_to_enemy < (new_enemy_hitbox + current_enemy->getHitbox())) {
                            // collides with enemy
                            collides = true;
                            break;
                        }
                    }

                    if (!collides) {    // no collision 
                        spawnEnemy(new_position, new_enemy_type);
                        break;
                    }
                }
            }
        }

        // win 4

        if (win_4 != NULL) {
            if (collectible_4_enemy_spawn_timer.timesUp()) {

                // randomize enemy spawn time
                float new_time = Helper::generateFloat(MIN_ENEMY_SPAWN_TIME, MAX_ENEMY_SPAWN_TIME);
                collectible_4_enemy_spawn_timer.setTime(new_time);

                int total_spawn_weight = MELEE_ENEMY_SPAWN_WEIGHT + RANGED_ENEMY_SPAWN_WEIGHT + CHARGE_ENEMY_SPAWN_WEIGHT;
                int spawn_type = Helper::generateFloat(0, total_spawn_weight);

                float new_enemy_hitbox;
                EnemyType new_enemy_type;

                // checks which type to spawn
                if (spawn_type < MELEE_ENEMY_SPAWN_WEIGHT) {
                    // spawns melee
                    new_enemy_hitbox = MELEE_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::MELEE_ENEMY;
                }
                else if (spawn_type < RANGED_ENEMY_SPAWN_WEIGHT + MELEE_ENEMY_SPAWN_WEIGHT) {
                    // spawns ranged
                    new_enemy_hitbox = RANGED_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::RANGED_ENEMY;
                }
                else {
                    // spawns charge
                    new_enemy_hitbox = CHARGE_ENEMY_SIZE / 2;
                    new_enemy_type = EnemyType::CHARGE_ENEMY;
                }


                for (int i = 0; i < SPAWN_ATTEMPT; i++) {
                    bool collides = false;
                    glm::vec3 new_position = Helper::generate2dPosition(win_4->GetPosition(), MIN_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE, MAX_SPAWNABLE_DISTANCE_FROM_COLLECTIBLE);
                    // check for collisions with players

                    float distance_to_player = glm::length(new_position - player->GetPosition());
                    if (distance_to_player < NON_SPAWNABLE_DISTANCE_FROM_PLAYER) {
                        // too close to player
                        collides = true;
                        break;
                    }


                    // check for collisions with other enemies
                    for (int j = 0; j < enemy_objects_.size(); j++) {
                        GameObject* current_enemy = enemy_objects_[j];
                        float distance_to_enemy = glm::length(new_position - current_enemy->GetPosition());
                        if (distance_to_enemy < (new_enemy_hitbox + current_enemy->getHitbox())) {
                            // collides with enemy
                            collides = true;
                            break;
                        }
                    }

                    if (!collides) {    // no collision 
                        spawnEnemy(new_position, new_enemy_type);
                        break;
                    }
                }
            }
        }

    }
}


void Game::spawnCollectible() {

    glm::vec3 spawn_position = Helper::generate2dPosition(player->GetPosition(), (COLLECTIBLE_SPAWN_RANGE - player->getHitbox()) / 4, COLLECTIBLE_SPAWN_RANGE);

    int total_spawn_weight = FOUR_TURRETS_SPAWN_WEIGHT + MACHINE_GUN_SPAWN_WEIGHT + SHOTGUN_SPAWN_WEIGHT + CANNON_SPAWN_WEIGHT + HOMING_SPAWN_WEIGHT;
    int spawn_type = Helper::generateFloat(0, total_spawn_weight);

    // checks which type to spawn
    if (spawn_type < FOUR_TURRETS_SPAWN_WEIGHT) {
        // spawns four turret
        auto* four_turrets = new CollectibleGameObject(spawn_position, sprite_, &sprite_shader_, tex_[7], CollectibleType::FOUR_TURRETS);
        collectible_objects_.push_back(four_turrets);
    }
    else if (spawn_type < MACHINE_GUN_SPAWN_WEIGHT + FOUR_TURRETS_SPAWN_WEIGHT) {
        // spawns machine gun
        auto* machine_turret_collectible = new CollectibleGameObject(spawn_position, sprite_, &sprite_shader_, tex_[14], CollectibleType::MACHINE_GUN);
        collectible_objects_.push_back(machine_turret_collectible);
    }
    else if (spawn_type < SHOTGUN_SPAWN_WEIGHT + MACHINE_GUN_SPAWN_WEIGHT + FOUR_TURRETS_SPAWN_WEIGHT) {
        // spawns shotgun
        auto* shotgun_collectible = new CollectibleGameObject(spawn_position, sprite_, &sprite_shader_, tex_[15], CollectibleType::SHOTGUN);
        collectible_objects_.push_back(shotgun_collectible);
    }
    else if (spawn_type < CANNON_SPAWN_WEIGHT + SHOTGUN_SPAWN_WEIGHT + MACHINE_GUN_SPAWN_WEIGHT + FOUR_TURRETS_SPAWN_WEIGHT) {
        // spawns cannon
        auto* cannon_collectible = new CollectibleGameObject(spawn_position, sprite_, &sprite_shader_, tex_[18], CollectibleType::CANNON);
        collectible_objects_.push_back(cannon_collectible);
    } else {
        // spawns homing
        auto* homing_turret_collectible = new CollectibleGameObject(spawn_position, sprite_, &sprite_shader_, tex_[25], CollectibleType::HOMING);
        collectible_objects_.push_back(homing_turret_collectible);
    }
}


CollectibleGameObject* Game::spawnWinningCollectible(GLuint texture) {
    glm::vec3 spawn_position = Helper::generate2dPosition(player->GetPosition(), player->getHitbox() * 4, MAX_WIN_COLLECTIBLE_SPAWN_RANGE);
    auto* win_collectible = new CollectibleGameObject(spawn_position, sprite_, &sprite_shader_, texture, CollectibleType::WIN_CONDITION);
    win_collectible->SetMovement(1.0, 3.5, glm::vec2(win_collectible->GetPosition().x, win_collectible->GetPosition().y));
    win_collectible->SetScale(3.0f);
    // add to collectible objects
    collectible_objects_.push_back(win_collectible);
    return win_collectible;
}
      
} // namespace game
