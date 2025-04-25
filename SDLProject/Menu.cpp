#include "Menu.hpp"
#include "Utility.hpp"


constexpr char FONT_FILEPATH[] = "assets/font1.png";



Menu::~Menu()
{

}

void Menu::initialise()
{
    m_game_state.next_scene_id = -1;
    
    font_texture_id = Utility::load_texture(FONT_FILEPATH);

    glClearColor(0.50f, 0.56f, 0.67f, 1.0f);
}

void Menu::update(float delta_time)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        m_game_state.next_scene_id = 1;
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
}

void Menu::render(ShaderProgram *program)
{
    Utility::draw_text(program, font_texture_id, "MAKE", 0.25f, 0.05f, glm::vec3(-0.65f, 2.5f, 0.0f));
    Utility::draw_text(program, font_texture_id, "BANANA", 0.25f, 0.05f, glm::vec3(-0.9f, 2.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "SMOOTHIE!", 0.25f, 0.05f, glm::vec3(-1.25f, 1.5f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Please Enter to Start", 0.4f, 0.05f, glm::vec3(-4.25f, 1.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "You need..", 0.25f, 0.05f, glm::vec3(-1.40f, 0.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Milk: 12", 0.25f, 0.05f, glm::vec3(-1.25f, -0.5f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Banana: 8", 0.25f, 0.05f, glm::vec3(-1.4f, -1.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Keep away", 0.25f, 0.05f, glm::vec3(-1.4f, -1.5f, 0.0f));
    Utility::draw_text(program, font_texture_id, "from the LEMON!", 0.25f, 0.05f, glm::vec3(-2.1f, -2.0f, 0.0f));
}

