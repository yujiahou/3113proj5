#include "LevelB.hpp"
#include "Utility.hpp"

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 12

constexpr char SPRITESHEET_FILEPATH[] = "assets/blenders.png",//https://openclipart.org/detail/215080/retro-character-sprite-sheet
           ENEMY_FILEPATH[]       = "assets/821.png",//https://charatoon.com/?id=821
FINGER_FILEPATH[] = "assets/finger.png";//https://uxwing.com/right-hand-pointer-with-shirt-cuff-icon/

unsigned int LEVELB_DATA[] =
{
    4,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6,
    22, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 21, 0, 0, 0, 0, 24,
    22, 0, 0, 0, 0, 21, 0, 0, 3, 0, 0, 21, 0, 0, 21, 0, 0, 0, 0, 24,
    22, 0, 3, 0, 0, 21, 0, 0, 21, 0, 0, 21, 0, 0, 21, 0, 0, 3, 0, 24,
    22, 0, 21, 0, 0, 39, 0, 0, 39, 0, 0, 21, 0, 0, 39, 0, 0, 21, 0, 24,
    22, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 21, 0, 24,
    22, 0, 21, 0, 0, 0, 0, 0, 3, 0, 0, 21, 0, 0, 3, 0, 0, 21, 0, 24,
    22, 0, 21, 0, 0, 0, 0, 0, 21, 0, 0, 39, 0, 0, 3, 0, 0, 39, 0, 24,
    22, 0, 39, 0, 0, 3, 0, 0, 21, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 24,
    22, 0, 0, 0, 0, 21, 0, 0, 21, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 24,
    22, 0, 0, 0, 0, 21, 0, 0, 21, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 24,
    40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 0,0,0, 42
};

LevelB::~LevelB()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;

    
}

void LevelB::initialise()
{
    glClearColor(0.085f, 0.07f, 0.12f, 1.0f);
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/tilemap.png");//https://snowhex.itch.io/dungeon-gathering
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 18, 7);
    

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
        
    m_game_state.player->set_position(glm::vec3(4.0f, -1.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(3.0f);
    
    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint finger_texture_id = Utility::load_texture(FINGER_FILEPATH);
    m_game_state.enemies = new Entity[ENEMY_COUNT];

    float init_xpos = 3.0f;
    float init_ypos = -3.0f;
    bool odd = true;
    float acceleration_y = 0.5f;
    for (int i = 0; i < ENEMY_COUNT-1; i++)
    {
        m_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, FLEE, IDLE);
        m_game_state.enemies[i].set_position(glm::vec3(init_xpos, init_ypos, 0.0f));
        m_game_state.enemies[i].set_movement(glm::vec3(0.0f));
        m_game_state.enemies[i].set_acceleration(glm::vec3(0.0f, acceleration_y, 0.0f));
        acceleration_y+=1.0f;
        if (odd){
            init_xpos+=2.0f;
            init_ypos-=0.5f;
            odd=!odd;
        }
        else{
            init_ypos-=2.5f;
            odd=!odd;
        }

    }
    
    m_game_state.enemies[ENEMY_COUNT-1] =  Entity(finger_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, WALKER, IDLE);
    m_game_state.enemies[ENEMY_COUNT-1].set_jumping_power(3.0f);
    
    m_game_state.enemies[ENEMY_COUNT-1].set_position(glm::vec3(15.0f, -1.0f, 0.0f));
    m_game_state.enemies[ENEMY_COUNT-1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[ENEMY_COUNT-1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));


    
    
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/tap.wav");
    //MUSIC by Brackeys, Sofia Thirslund
}

void LevelB::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++) m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
}

void LevelB::render(ShaderProgram *program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++){
        if (m_game_state.enemies[i].get_active()){
            m_game_state.enemies[i].render(program);
        }
    }
}
