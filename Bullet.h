#include "utils.h"
#include "Mesh.h"

/*
* Class for handling individual Bullets.
*/
class Bullet
{
public:

    // Bullet Gameplay Variables.
    static constexpr float BULLET_RADIUS = 0.05f;
    static constexpr float BIGBULLET_FACTOR = 3.5f;
    vec3 position;
    int life = 1250;
    int state = ALIVE;
    vec3 velocity;
    static std::shared_ptr<Mesh> mesh;
    float texFrame = 0;
    // End Bullet Gameplay Variables.

    // Base Constructor.
    Bullet(vec3 pos, vec3 vel)
    {
        this->position=pos;
        if(mesh == nullptr)
            mesh = std::make_shared<Mesh>("bullet.obj");
        this->velocity = vel;
    }
    
    // Tick-like update function. Called every frame.
    void update(int elapsed)
    {
        // Update the Texture Frame for texture animations.
        this->texFrame += 0.01f*elapsed;
        while(this->texFrame > mesh->materials[0].tex->slices)
            this->texFrame -= mesh->materials[0].tex->slices;
        
        // Update position, lifetime, and the state.
        this->life -= elapsed;
        if(this->state == ALIVE)
            this->position += this->velocity*elapsed;
        if(this->life < 0)
            this->state = DEAD;
    }
    
    // Base Draw Function.
    void draw()
    {
        // Update the World Matrix and Texture Slice Uniforms to send to the GPU.
        Program::setUniform("worldMatrix",translation(this->position));
        Program::setUniform("texSlice",(this->texFrame));
        mesh->draw();
    }
    
    // Templated collision function for enemy collision detection.
    template<typename T>
    bool collidesWith(std::shared_ptr<T> enemy){
        if(this->state != ALIVE or enemy->state != ALIVE)
            return false;
        return ::collidesWith(
            this->position, BULLET_RADIUS,
            enemy->position, enemy->radius);
    }
};

        
