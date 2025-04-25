/**
 * Author: Yujia Hou
 * Assignment: Banana Lemon Smoothie
 * Date due: Apr 25th, 2025, 2:00pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LIVES 3
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.hpp"
#include "Utility.hpp"
#include "Scene.hpp"
#include "LevelA.hpp"
#include "Menu.hpp"
#include "LevelB.hpp"
#include "LevelC.hpp"
#include "Lose.hpp"
#include "Win.hpp"
#include "Effects.hpp"
#include "Final.hpp"

// ––––– CONSTANTS ––––– //
constexpr int WINDOW_WIDTH  = 640*2,
          WINDOW_HEIGHT = 480*2;

constexpr int CD_QUAL_FREQ    = 44100,
          AUDIO_CHAN_AMT  = 2,
          AUDIO_BUFF_SIZE = 4096;

constexpr char BGM_FILEPATH[] = "assets/Morning.mp3";//"Morning" Kevin MacLeod (incompetech.com)
constexpr int    LOOP_FOREVER = -1;

Mix_Music *g_music;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_lit.glsl",
           F_SHADER_PATH[] = "shaders/fragment_lit.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char FONT_FILEPATH[] = "assets/font1.png";

enum AppStatus { RUNNING, TERMINATED };

// ––––– GLOBAL VARIABLES ––––– //
Scene  *g_current_scene;
Menu   *g_menu;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;
Lose   *g_lose;
Win    *g_win;
Final  *g_final;

Effects *g_effects;
Scene   *g_levels[7];

SDL_Window* g_display_window;


ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;
glm::vec3 win_pos = glm::vec3(13.0f, -4.0f, 0.0f);

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;
bool lose = false;
bool win = false;
bool pause = false;
float lives = LIVES;
bool faded = false;
bool start_fading = false;
bool played_win = false;
bool B_win = false;
float fade_in_timer = 0.0f;
int balls_fired = 0;
float level_timer = 0.0f;
int num_collected = 0;
int current_level = 0;
GLuint font_texture_id;
float timer = -1.0f;
float win_timer = 0.0f;

AppStatus g_app_status = RUNNING;

void swtich_to_scene(Scene *scene);
void initialise();
void process_input();
void update();
void render();
void shutdown();

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene *scene)
{
    std::cout << "switched to scene: " << scene << std::endl;
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Banana Smoothie!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    

        Mix_OpenAudio(
            CD_QUAL_FREQ,
            MIX_DEFAULT_FORMAT,
            AUDIO_CHAN_AMT,
            AUDIO_BUFF_SIZE
            );
    
    g_music = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_music, LOOP_FOREVER);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    font_texture_id = Utility::load_texture(FONT_FILEPATH);
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_menu = new Menu();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    g_lose   = new Lose();
    g_win    = new Win();
    g_final  = new Final();
    
    g_levels[0] = g_menu;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    g_levels[4] = g_lose;
    g_levels[5] = g_win;
    g_levels[6] = g_final;
    
    // Start at menu
    switch_to_scene(g_levels[0]);
    
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    
}

void process_input()
{
    if (g_current_scene->get_state().player != nullptr) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        g_app_status = TERMINATED;
                        break;
                        
                    case SDLK_RETURN:
                        if (g_current_scene == g_menu){
                            switch_to_scene(g_levelA);
                        }
                        else if (g_current_scene == g_win){
                            switch_to_scene(g_levels[current_level+1]);
                        }
                        else if (g_current_scene == g_lose){
                            switch_to_scene(g_levels[0]);
                            level_timer = 0.0f;
                            timer = -1.0f;
                            num_collected=0;
                        }
                        break;
                        
                    case SDLK_p:
                        pause = !pause;
                        break;
                        
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    if (!lose && !win && g_current_scene->get_state().player != nullptr){
        if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
        else if (key_state[SDL_SCANCODE_DOWN])  g_current_scene->get_state().player->move_down();
        else if (key_state[SDL_SCANCODE_UP])  g_current_scene->get_state().player->move_up();
        
        if (g_current_scene->get_state().player != nullptr) {
            if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
                g_current_scene->get_state().player->normalise_movement();
        }
        
    }

    
}

void update()
{
    if (g_current_scene->get_state().player != nullptr) {
        
        float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
        float delta_time = ticks - g_previous_ticks;
        g_previous_ticks = ticks;
        if(g_current_scene==g_levelA || g_current_scene==g_levelB || g_current_scene==g_levelC){
            float curr_timer = SDL_GetTicks() / 1000.0f;
            if (timer<0){
                timer = curr_timer;
            }
            level_timer = curr_timer - timer;
            std::cout << level_timer << std::endl;
        }
        
        delta_time += g_accumulator;
        
        if (delta_time < FIXED_TIMESTEP)
        {
            g_accumulator = delta_time;
            return;
        }
        
        while (delta_time >= FIXED_TIMESTEP) {
            g_current_scene->update(FIXED_TIMESTEP);
            if (win && start_fading &&!faded){
                g_effects->update(FIXED_TIMESTEP);
            }
            
            
            g_is_colliding_bottom = g_current_scene->get_state().player->get_collided_bottom();
            
            delta_time -= FIXED_TIMESTEP;
        }
        
        if (B_win){
            win_timer += 10.0f * delta_time;
            if(win_timer > 3.0f){
                switch_to_scene(g_win);
                current_level+=1;
                win_timer = 0.0f;
                B_win = false;
            }
        }
        if (played_win){
            win_timer += 10.0f * delta_time;
            if(win_timer > 3.0f){
                switch_to_scene(g_final);
            }
        }
        g_accumulator = delta_time;
        if (g_current_scene == g_levelA && level_timer >= 40.0f)
        {
            if(num_collected<12){
                switch_to_scene(g_lose);
                if (g_current_scene==g_lose){
                    level_timer = 0.0f;
                    timer = -1.0f;
                    num_collected=0;
                }
            }
            else{
                switch_to_scene(g_win);
                current_level+=1;
                if (g_current_scene==g_win){
                    level_timer = 0.0f;
                    timer = -1.0f;
                    num_collected=0;
                }
            }
                 
        }
        if (g_current_scene == g_levelB && level_timer >= 60.0f) {
            if(num_collected<8 || !B_win){
                switch_to_scene(g_lose);
                if (g_current_scene==g_lose){
                    level_timer = 0.0f;
                    timer = -1.0f;
                    num_collected=0;
                }
            }
            else{
                switch_to_scene(g_win);
                current_level+=1;
                if (g_current_scene==g_win){
                    level_timer = 0.0f;
                    timer = -1.0f;
                    num_collected=0;
                }
            }
        }
        if (g_current_scene == g_levelC && level_timer >= 30.0f) {
            switch_to_scene(g_lose);
        }
        
        if(g_current_scene == g_levelA && g_current_scene->get_state().player->check_collision(&g_current_scene->get_state().enemies[0])){
            Mix_PlayChannel(-1,  g_current_scene->get_state().win_sfx, 0);

            num_collected+=1;
            glm::vec3 position = g_current_scene->get_state().player->get_position();
            
            g_current_scene->get_state().enemies[0].set_position(glm::vec3(9999.99f));
            glm::vec3 acceleration = g_current_scene->get_state().enemies[0].get_acceleration();
            g_current_scene->get_state().enemies[0].set_acceleration(glm::vec3(acceleration.x+1.0f,acceleration.y+1.0f,acceleration.z+1.0f));
            
            if((g_current_scene->get_state().player->get_movement().x) > 0){
                
                
                g_current_scene->get_state().enemies[0].set_position(glm::vec3(position.x-5.0f,position.y,0.0f));
            
            }
            else if ((g_current_scene->get_state().player->get_movement().x) < 0){
                
                g_current_scene->get_state().enemies[0].set_position(glm::vec3(position.x+6.0f,position.y,0.0f));

            }
            else if((g_current_scene->get_state().player->get_movement().y) > 0){
                
                g_current_scene->get_state().enemies[0].set_position(glm::vec3(position.x,position.y-5.0f,0.0f));

            }
            else{
                
                g_current_scene->get_state().enemies[0].set_position(glm::vec3(position.x,position.y+6.0f,0.0f));

            }
            
        }

        if(g_current_scene == g_levelB){
            for (int i = 0; i < 9; i++){
                if(g_current_scene->get_state().player->check_collision(&g_current_scene->get_state().enemies[8])){
                    if(num_collected==8){
                        B_win = true;
                        g_current_scene->get_state().enemies[8].set_position(glm::vec3(9999.99f));
                        Mix_PlayChannel(-1,  g_current_scene->get_state().jump_sfx, 0);
                        g_current_scene->get_state().enemies[8].deactivate();
                        level_timer = 0.0f;
                        timer = -1.0f;
                        num_collected=0;
                    }
                }
                else if(g_current_scene->get_state().player->check_collision(&g_current_scene->get_state().enemies[i]) && g_current_scene->get_state().enemies[i].get_active()){
                    num_collected+=1;
                    g_current_scene->get_state().enemies[i].set_position(glm::vec3(9999.99f));
                    Mix_PlayChannel(-1,  g_current_scene->get_state().jump_sfx, 0);
                    g_current_scene->get_state().enemies[i].deactivate();

                }
            }
        }
        if(g_current_scene == g_levelC){
            std::cout << "final position: " << g_current_scene->get_state().player->get_position().x << "y: " << g_current_scene->get_state().player->get_position().y << std::endl;
        }
        if(g_current_scene == g_levelC && g_current_scene->get_state().player->check_collision(&g_current_scene->get_state().enemies[0])){
            if (lives > 0){
                lives -= 1;
                Mix_PlayChannel(-1,  g_current_scene->get_state().attack_sfx, 0);
            }
            else{
                lose = true;
                if (g_current_scene==g_lose){
                    level_timer = 0.0f;
                    timer = -1.0f;
                    num_collected=0;
                }
            }
        }

        
        if(g_current_scene == g_levelC && g_current_scene->get_state().player->check_collision(&g_current_scene->get_state().enemies[4])){
            g_current_scene->get_state().enemies[4].set_position(glm::vec3(9999.99f));
            Mix_PlayChannel(-1,  g_current_scene->get_state().win_sfx, 0);
            played_win = true;
        }
        
        if (lose == true){
            switch_to_scene(g_lose);
            if (g_current_scene==g_lose){
                level_timer = 0.0f;
                timer = -1.0f;
                num_collected=0;
            }
        }
        
        if (win && !faded){
            if (!start_fading){
                g_effects->start(FADEOUT);
                start_fading=true;
            }
            else{
                fade_in_timer += delta_time * 1.0f;
                if(fade_in_timer >= 0.5f){
                    switch_to_scene(g_win);
                    faded=true;
                    start_fading=false;
                }
            }
        }
        
        
        g_view_matrix = glm::mat4(1.0f);
        
        if(g_current_scene->get_state().player != nullptr){
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, -g_current_scene->get_state().player->get_position().y, 0));
        }
    }
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
       
    glClear(GL_COLOR_BUFFER_BIT);
       
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);
       
    if(win && !faded && start_fading){
        g_effects->render();
    }
    if(g_current_scene == g_levelA){
        g_shader_program.SetLightPosition(g_current_scene->get_state().player->get_position());
    }

    glm::vec3 timer_position = -glm::vec3(g_view_matrix[3]) + glm::vec3(-4.5f, 3.0f, 0.0f);
    glm::vec3 collection_position = -glm::vec3(g_view_matrix[3]) + glm::vec3(-4.5f, 2.5f, 0.0f);

    if(g_current_scene!=g_menu && g_current_scene!=g_win && g_current_scene!=g_lose && g_current_scene!=g_final){
        std::string timer_text = "Timer: " + std::to_string(level_timer);
        Utility::draw_text(&g_shader_program, font_texture_id, timer_text, 0.25f, 0.05f, timer_position);
        std::string collection_text = "Collected: " + std::to_string(num_collected);
        Utility::draw_text(&g_shader_program, font_texture_id, collection_text, 0.25f, 0.05f, collection_position);
    }
    if(g_current_scene == g_levelA){
        Utility::draw_text(&g_shader_program, font_texture_id, "Collect 12 in 40 seconds!", 0.25f, 0.05f, glm::vec3(timer_position.x, timer_position.y+0.5f,timer_position.z));
    }
    if(g_current_scene == g_levelB){
        Utility::draw_text(&g_shader_program, font_texture_id, "START BLENDING after collecting!", 0.25f, 0.05f, glm::vec3(timer_position.x, timer_position.y+0.5f,timer_position.z));
    }
    if(g_current_scene == g_levelC){
        Utility::draw_text(&g_shader_program, font_texture_id, "Find a good cup!", 0.25f, 0.05f, glm::vec3(timer_position.x, timer_position.y+0.5f,timer_position.z));
    }

    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete g_menu;
    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete g_lose;
    delete g_win;
    delete g_effects;
    
    
    Mix_FreeChunk(g_current_scene->get_state().jump_sfx);
    Mix_FreeChunk(g_current_scene->get_state().win_sfx);
    Mix_FreeChunk(g_current_scene->get_state().attack_sfx);
    Mix_FreeMusic(g_current_scene->get_state().bgm);
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        if (!pause){
            update();
        }
        
        if (g_current_scene->get_state().next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);
        
        
        render();
    }

    shutdown();
    return 0;
}
