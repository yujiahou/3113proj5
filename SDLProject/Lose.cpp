#include "Lose.hpp"
#include "Utility.hpp"


constexpr char FONT_FILEPATH[] = "assets/font1.png";



Lose::~Lose()
{

}

void Lose::initialise()
{
    m_game_state.next_scene_id = -1;
    
    font_texture_id = Utility::load_texture(FONT_FILEPATH);

    glClearColor(0.50f, 0.56f, 0.67f, 1.0f);
}

void Lose::update(float delta_time)
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

void Lose::render(ShaderProgram *program)
{
    glClear(GL_COLOR_BUFFER_BIT);

    program->set_view_matrix(glm::mat4(1.0f));
    Utility::draw_text(program, font_texture_id, "YOU WERE CLOSE!", 0.5f, 0.05f, glm::vec3(-3.5f, 1.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Press Enter", 0.25f, 0.05f, glm::vec3(-1.35f, 0.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "to Restart", 0.25f, 0.05f, glm::vec3(-1.2f, -0.5f, 0.0f));

}


