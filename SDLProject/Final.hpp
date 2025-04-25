#include "Scene.hpp"

class Final : public Scene {
public:
    int ENEMY_COUNT = 1;
    
    ~Final();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    
    GLuint font_texture_id;
};



