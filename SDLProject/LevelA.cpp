#include "LevelA.hpp"
#include "Utility.hpp"

#define LEVEL_WIDTH 25
#define LEVEL_HEIGHT 15

constexpr char SPRITESHEET_FILEPATH[] = "assets/blenders.png",//https://creazilla.com/media/clipart/7767023/electric-blender-device
           ENEMY_FILEPATH[]       = "assets/milk.png";//https://www.vecteezy.com/free-png/milk-carton



unsigned int LEVELA_DATA[] =
{
    4, 5, 5, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 97, 98, 97,
    22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 115, 116, 117,
    22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 97, 98, 97,
    22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 115, 116, 117,
    40, 0, 0, 0, 0, 58, 59, 60, 0, 0, 0, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 97, 98, 97,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 115, 116, 117,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 115, 116, 117,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 97, 98, 97,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 115, 116, 117,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 0, 0, 0, 0, 0, 97, 98, 97,
    0, 0, 0, 0, 0, 0, 85, 86, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 97, 98, 97,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 0, 0, 0, 0, 0, 97, 98, 97,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 0, 0, 0, 0, 0, 97, 98, 97,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 0, 0, 0, 0, 0, 97, 98, 97,
    0, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 40, 0, 0, 0, 0, 0, 0, 0, 115, 116, 117
};


LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    
    
}

void LevelA::initialise()
{
    glClearColor(0.085f, 0.07f, 0.12f, 1.0f);
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/tilemap.png");//https://snowhex.itch.io/dungeon-gathering
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 18, 7);
    
//    GLuint ball_texture_id =Utility::load_texture("assets/milkdrop.png");
    int player_walking_animation[4][4] =
    {
        { 8, 9, 10, 11 },
        { 4, 5, 6, 7 },
        { 12, 13, 14, 15 },
        { 0, 1, 2, 3 }
    };

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    
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
        1.02f,                      // width
        1.72f,                       // height
        PLAYER
    );
        
    m_game_state.player->set_position(glm::vec3(5.0f, -3.75f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(3.0f);
    
    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, WALKER, IDLE);
        m_game_state.enemies[i].set_jumping_power(3.0f);
    }

    m_game_state.enemies[0].set_position(glm::vec3(1.0f, -1.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_scale(glm::vec3(1.0f, 1.0f, 0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    
    
    m_game_state.win_sfx = Mix_LoadWAV("assets/coin.wav");    //MUSIC by Brackeys, Sofia Thirslund
}

void LevelA::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    }
    

    
    
}

void LevelA::render(ShaderProgram *program)
{

    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)    m_game_state.enemies[i].render(program);

    
}
