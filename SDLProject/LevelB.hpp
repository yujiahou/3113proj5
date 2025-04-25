#include "Scene.hpp"

class LevelB : public Scene {
public:
    int ENEMY_COUNT = 9;
    
    ~LevelB();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
