#include "LevelC.hpp"
#include "Utility.hpp"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/blenders.png",//https://openclipart.org/detail/215080/retro-character-sprite-sheet
           ENEMY_FILEPATH[]       = "assets/lemon.png",//https://charatoon.com/?id=2024
GLASS_FILEPATH[]       = "assets/glass.png",//https://pngtree.com/so/empty-glass
GLASS_BAD_FILEPATH[] = "assets/glass_bad.png";//https://www.pngegg.com/en/png-dxujt


unsigned int LEVELC_DATA[] =
{
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    22, 0, 0, 0, 0, 0, 0, 57, 0, 0, 0, 0, 0, 0,
    22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 91,
    22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12, 13, 14,
    22, 0, 0, 0, 0, 0, 0, 0, 11, 13, 48, 48, 48, 32,
    22, 0, 0, 0, 0, 0, 0, 0, 29, 48, 48, 48, 48, 50
};



LevelC::~LevelC()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    
}

void LevelC::initialise()
{
    glClearColor(0.085f, 0.07f, 0.12f, 1.0f);
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/tilemap.png");//https://snowhex.itch.io/dungeon-gathering
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 18, 7);
    
    
    int player_walking_animation[4][4] =
    {
        { 8, 9, 10, 11 },
        { 4, 5, 6, 7 },
        { 12, 13, 14, 15 },
        { 0, 1, 2, 3 }
    };
    
    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    m_game_state.player = new Entity(
         player_texture_id,         // texture id
         5.0f,                      // speed
         acceleration,              // acceleration
         5.0f,                      // jumping power
         player_walking_animation,  // animation index sets
         0.0f,                      // animation time
         4,                         // animation frame amount
         0,                         // current animation index
         4,                         // animation column amount
         4,                         // animation row amount
         1.0f,                      // width
         1.0f,                       // height
         PLAYER
         );
    
    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    
    // Jumping
    m_game_state.player->set_jumping_power(3.0f);
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint glass_texture_id = Utility::load_texture(GLASS_FILEPATH);
    GLuint glass_bad_texture_id = Utility::load_texture(GLASS_BAD_FILEPATH);
    
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    
    
    m_game_state.enemies[0] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[0].set_jumping_power(3.0f);
    
    m_game_state.enemies[0].set_position(glm::vec3(8.0f, -4.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    
    float init_xpos = 3.0f;
    float init_ypos = 7.0f;
    for (int i = 1; i < ENEMY_COUNT-1; i++){
        m_game_state.enemies[i] =  Entity(glass_bad_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
        m_game_state.enemies[i].set_jumping_power(3.0f);
        
        m_game_state.enemies[i].set_position(glm::vec3(init_xpos-4.0f, init_ypos-4.0f, 0.0f));
        m_game_state.enemies[i].set_movement(glm::vec3(0.0f));
        m_game_state.enemies[i].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
        init_xpos+=2.0f;
    }
    
    
    m_game_state.enemies[ENEMY_COUNT-1] =  Entity(glass_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, WALKER, IDLE);
    m_game_state.enemies[ENEMY_COUNT-1].set_jumping_power(3.0f);
    
    m_game_state.enemies[ENEMY_COUNT-1].set_position(glm::vec3(-4.0f, -4.0f, 0.0f));
    m_game_state.enemies[ENEMY_COUNT-1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[ENEMY_COUNT-1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_game_state.attack_sfx = Mix_LoadWAV("assets/power_up.wav");
    m_game_state.win_sfx = Mix_LoadWAV("assets/coin.wav");
    //MUSIC by Brackeys, Sofia Thirslund
    
}

void LevelC::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 1;
    for (int i = 0; i < ENEMY_COUNT; i++) m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    
}

void LevelC::render(ShaderProgram *program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)    m_game_state.enemies[i].render(program);
    
}
