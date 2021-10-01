#include "Bullet.h"
#include "Camera.h"
#include "Enemy.h"
#include "Hero.h"
#include "ImageTexture2DArray.h"
#include "Jellyfish.h"
#include "Mesh.h"
#include "Program.h"
#include "Sampler.h"
#include "Ship.h"
#include "StraightLineEnemy.h"
#include "ImageTextureCube.h"
#include "Text.h"
#include "utils.h"
#include <iostream>
#include <cassert>
#include "Particle.h"
#include "Globals.h"

// Global variables object.
std::unique_ptr<Globals> globs;

// Function used for OpenGL debug messages.
void APIENTRY debugCallback(GLenum source, GLenum msgType, GLuint msgId, GLenum severity, GLsizei length, const GLchar* message, const void* param )
{
    std::cerr << msgId << ": " << message << "\n";
    if(severity == GL_DEBUG_SEVERITY_HIGH){
        assert(0);
    }
    if( msgId == 131204 ){
        std::cout <<"?";
    }
}

// Collision check for two spheres; c = center position, r = radius
bool collidesWith( vec3 c1, float r1, vec3 c2, float r2)
{
    auto r = r1+r2;
    r = r*r;
    auto v=c1-c2;
    auto distsq = dot(v,v);
    auto coll = (distsq <= r2);
    return coll;
}

// Setup function for OpenGL and gameplay initialization.
void setup()
{
    // Make Globals unique.
    globs = std::make_unique<Globals>();

    // Initialize OpenGL parameters.
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.2f,0.4f,0.6f,1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // Bind the OpenGL sampler to the 0 and 1 texture unit.
    globs->sampler.bind(0);
    globs->sampler.bind(1);
    
    // Set the skybox texture.
    globs->skybox.materials[0].tex = std::make_shared<ImageTextureCube>("nebula-%04d.jpg");
    
    // Set the Text for the Charge value in the bottom left corner.
    globs->chargeText.setText(vec2(0,0),"Charge: 0%");
    
    // Initialize the Light Uniforms to send to the GPU.
    Program::setUniform("lightColor", vec3(1,1,1));
    Program::setUniform("lightPosition", normalize(vec4(1,1,1,0)));
    
    // Spawn the asteroids at random positions. (0,0,0) is the center of the game.
    float amt=10;
    for(int i=0;i<40;++i){
        auto p = vec3(
            uniform(-amt,amt),
            uniform(-amt,amt),
            uniform(-amt,amt)
        );
        globs->asteroids.push_back( p );
    }
    
    // Spawn the powerups at random positions.
    amt = 20;
    for(int i=0;i<40;++i){
         auto p = vec3(
            uniform(-amt,amt),
            uniform(-amt,amt),
            uniform(-amt,amt)
        );
        globs->powerupManager.add( p );
    }
    // Create dummy objects to preload textures.
    Bullet(vec3(0,0,0), vec3(0,0,0));
    StraightLineEnemy(vec3(0,0,0));
    Jellyfish(vec3(0,0,0));
}
    
// Templated reap function used to destroy gameplay objects based on their gameplay state.         
template<typename T>
void reap(std::vector<T>& L)
{
    size_t i=0;
    while(i < L.size() )
    {
        if(L[i]->state == DEAD)
        {
            auto x = L.back();
            L.pop_back();
            if(i < L.size() )
            {
                L[i] = x;
            }
        } 
        else 
        {
            i+=1;
        }
    }
}

// Function used to fire a projectile from the player/hero. Accounts for charged up blasts.
void fire(bool big)
{
    if(!big)
        Mix_PlayChannel(-1,globs->pewSound, 0 );
    else
        Mix_PlayChannel(-1,globs->bigPewSound, 0 );
    
    auto p = globs->hero.position + 0.25f * globs->hero.facing;
    auto v = 0.002f*globs->hero.facing;
    globs->bulletManager.add(p,v);
    if(big)
    {
        globs->bulletManager.add(p,v+0.0001f*globs->hero.right);
        globs->bulletManager.add(p,v+-0.0001f*globs->hero.right);
    }
}

// Function used to fire the lasers from the hero/player.
void fireLasers()
{
    auto v = globs->hero.facing;
    vec3 tmp[2] = {globs->hero.laser1Pos, globs->hero.laser2Pos};
    for(int i=0;i<2;++i)
    {
        vec3 p = tmp[i];
        p = (vec4(p,1.0f) * globs->hero.getMatrix()).xyz();
        auto lb = LaserBlast::make(p,v);
        if(lb != nullptr)
            globs->laserBlasts.push_back(lb);
    }
}

// Main Gameplay Function.
void update(int elapsed)
{
    // Event Handling Loop.
    SDL_Event ev;
    while(true)
    {
        if(!SDL_PollEvent(&ev))
            break;
        if(ev.type == SDL_QUIT)
        {
            SDL_Quit();
            exit(0);
        }
        else if( ev.type == SDL_KEYDOWN )
        {
            auto k = ev.key.keysym.sym;
            globs->keyset.insert(k);
            if(k == SDLK_p)
                globs->paused= !globs->paused;
            if(k==SDLK_h)
            {
                if( globs->hudDir == 0 ){
                    if( globs->hudY < 1 )
                        globs->hudDir = 1;
                    else
                        globs->hudDir = -1;
                } else{
                    globs->hudDir = -globs->hudDir;
                }
            }
            if(k == SDLK_q){
                SDL_Quit();
                exit(0);
            }
        }
        else if(ev.type == SDL_KEYUP && !globs->paused)
        {
            auto k = ev.key.keysym.sym;
            globs->keyset.erase(k);
            // If the spacebar has just been released, spawn a bullet.
            if(k == SDLK_SPACE)
                fire( globs->bulletCharge == 1 );
            // If E was just release fire a laser.
            if (k == SDLK_e)
                fireLasers();
        }
    }
    // Game Paused Logic.
    if(globs->paused)
        return;

    // Update enemy spawn timer.
    globs->timeToNextSpawn -= elapsed;
    if(globs->timeToNextSpawn <= 0)
    {
        // Spawn a random enemy.
        vec3 p = globs->camera.eye + 3.25 * globs->camera.look;
        p += randrange(-1,1) * globs->camera.up;
        if(randrange(0,10) < 5)
        {
            //Spawn normal ship
            globs->enemies.push_back( std::make_shared<StraightLineEnemy>(p));
        } else 
        {
            //Spawn jellyfish
            globs->enemies.push_back( std::make_shared<Jellyfish>(p));
        }
        globs->timeToNextSpawn = int(randrange(500,4000));
    }
    
    // Update the hero.
    globs->hero.update(elapsed,globs->keyset);

    // Update the camera.
    globs->camera.lookAt( 
            globs->hero.position + -1.5 * globs->hero.facing + 0.25f * globs->hero.up,
            globs->hero.position,
            globs->hero.up
    );

    // Update the rear-view mirror.
    globs->hudcamera.lookAt( 
            globs->hero.position + 0.25f * globs->hero.facing + 0.05f * globs->hero.up,
            globs->hero.position + -1*globs->hero.facing,
            globs->hero.up
    );
    
    // Update the enemies.
    for( auto e : globs->enemies )
        e->update(elapsed);
    
    // Update the lasers.
    for( auto lb : globs->laserBlasts )
        lb->update(elapsed);

    // Update the particle effects.
	for (auto pt : globs->particles)
		pt->update(elapsed);
    
    // Update the bullets.
    globs->bulletManager.update(elapsed);

    // Update the powerups.
    globs->powerupManager.update(elapsed);

    // Spawn particle system if bullet hits asteroid.
	for (size_t i = 0; i < globs->bulletManager.bullets.size(); ++i) 
    {
		for (size_t j = 0; j < globs->asteroids.size(); ++j)
		{
			if (collidesWith(globs->bulletManager.bullets[i]->position, globs->bulletManager.bullets[i]->BULLET_RADIUS, globs->asteroids[j], 0.1))
			{
				std::shared_ptr<ParticleSystem> particle = std::make_shared<ParticleSystem>(200, globs->bulletManager.bullets[i]->position);
				globs->particles.push_back(particle);
				globs->bulletManager.bullets[i]->state = DEAD;
			}
		}
	}
    // Powerup-get logic.
    if( globs->powerupManager.checkCollisions(globs->hero.position) )
    {
        Mix_PlayChannel(-1,globs->powerupSound,0);
    }
    // Check if any bullets hit an enemy.
    size_t j = 0;
    while(j < globs->enemies.size() )
    {
        auto en = globs->enemies[j];
        if(globs->bulletManager.checkCollisions(en) )
        {
            Mix_PlayChannel(-1,globs->hitSound,0);
            en->hit();
            break;
        } else {
            j+=1;
        }
    }
    // Reap gameplay objects.
    reap(globs->enemies);
    reap(globs->laserBlasts);
	reap(globs->particles);
    
    // Set the old charge value for bullet charge processing.
    auto oldCharge = globs->bulletCharge;
    
    // If spacebar is down, charge the blasters.
    if(setContains(globs->keyset,SDLK_SPACE) )
    {
        globs->bulletCharge += 0.004f*elapsed;
        if(globs->bulletCharge > 1)
            globs->bulletCharge = 1;
    } 
    else 
    {
        globs->bulletCharge -= 0.01f*elapsed;
        if(globs->bulletCharge < 0)
            globs->bulletCharge = 0;
    }
    if(globs->bulletCharge != oldCharge)
    {
        globs->chargeText.setText( vec2(0,0),
            "Charge: "+std::to_string(int(globs->bulletCharge*100))+"%");
    }
    // Update the HUD position.
    globs->hudY += globs->hudDir * 0.001f * elapsed;
    if( globs->hudY < 1.0f - globs->hudSize )
    {
        globs->hudY = 1.0f - globs->hudSize;
        globs->hudDir = 0;
    }
    if( globs->hudY > 1.0f + globs->hudSize )
    {
        globs->hudY = 1.0f + globs->hudSize;
        globs->hudDir = 0;
    }
}

// Secondary Game Rendering Function. Used to control draw order/distance.         
void drawIt()
{
    // Draw the starfield. Set OpenGL to use the Star shaders.
    globs->starProg.use();
    globs->stars.draw();
    
    // Set OpenGL to use the default shaders.
    globs->mainProg.use();

    // Draw the enemies.
    for(auto e : globs->enemies )
        e->draw();
    // Draw the lasers.
    for( auto bl : globs->laserBlasts )
        bl->draw();
    
    // Draw the hero.
    globs->hero.draw();

    // Draw the asteroids.
    for(auto m : globs->asteroids )
    {
        // Set the world matrix uniform to the asteroids translation.
        Program::setUniform("worldMatrix", translation(m) );
        globs->asteroidMesh.draw();
    }
    
    // Set the skybox shaders to be used, and draw the skybox.
    globs->skyboxProg.use();
    globs->skybox.draw();

    // Draw misc. gameplay objects.
    globs->powerupManager.draw();
    globs->bulletManager.draw();

    // Draw the particles.
	for (auto pt : globs->particles)
		pt->draw();
}

// Main Game Rendering Function.
void draw()
{
    // Draw from HUD perspective for the reflection.
    globs->fbo.setAsRenderTarget(true);
    globs->hudcamera.setUniforms();
    // Draw the scene to the reflection fbo.
    drawIt();
    globs->fbo.unsetAsRenderTarget();
    
    //draw from normal camera's perspective
    globs->camera.setUniforms();

    //draw the bright things to be glow'ed
    Program::setUniform("doingGlow", 1.0f);
    Program::updateUniforms();
    // Set to draw to the blur fbo.
    globs->blurfbo.setAsRenderTarget(true);
    drawIt();
    globs->blurfbo.unsetAsRenderTarget();
    Program::setUniform("doingGlow", 0.0f);
    Program::updateUniforms();
    globs->blurfbo.blur(19.0f,2.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    globs->camera.setUniforms();
    drawIt();

    //add the glow overlay
    Program::setUniform("worldMatrix",mat4::identity());
    Program::updateUniforms();
    glBlendFunc(GL_ONE,GL_ONE);
    globs->usqProg.use();
    globs->blurfbo.texture->bind(0);
    globs->usq.draw();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //draw the hud
    Program::setUniform("worldMatrix",
        scaling(2.0f*globs->hudSize,globs->hudSize,1.0f) * translation(0,globs->hudY,0) );
    Program::updateUniforms();

    globs->fbo.texture->bind(0);
    globs->usq.draw();
      
    globs->chargeText.draw();
}

// Main.
int main(int argc, char* argv[])
{
    // SDL initializers.
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    Mix_Init(MIX_INIT_OGG|MIX_INIT_MP3);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,1,1024);
    Mix_AllocateChannels(32);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
    
    // Window resolution.
    int WIN_WIDTH=1000;
    int WIN_HEIGHT=1000;
    auto win = SDL_CreateWindow( "Space Shaders",20,20, WIN_WIDTH,WIN_HEIGHT, SDL_WINDOW_OPENGL);
    if(!win)
    {
        std::cerr << "Could not create window\n";
        return 1;
    }
    auto rc = SDL_GL_CreateContext(win);
    if( !rc )
    {
        std::cerr << "Cannot create GL context\n";
        return 1;
    }
    
    glDebugMessageCallback( debugCallback, nullptr );
    
    // Source, type, severity, count, ids, enabled
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true );
        
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    //turn off the buffer messages that nVidia drivers like to give
    GLuint tmp[1] = {131185} ;
    glDebugMessageControl( 
        GL_DEBUG_SOURCE_API, 
        GL_DEBUG_TYPE_OTHER, 
        GL_DONT_CARE,
        1, tmp, 0 );

    setup();
    
    int cw,ch;
    SDL_GL_GetDrawableSize(win, &cw, &ch);
    Program::setUniform("screenSize", vec4(cw,ch,1.0f/(cw-1),1.0f/(ch-1)));
        
    int DESIRED_FRAMES_PER_SEC = 60;
    float DESIRED_SEC_PER_FRAME = 1.0f/DESIRED_FRAMES_PER_SEC;
    int DESIRED_MSEC_PER_FRAME = int(DESIRED_SEC_PER_FRAME * 1000);
    auto TICKS_PER_SECOND = SDL_GetPerformanceFrequency();
    int UPDATE_QUANTUM_MSEC = 5;
    
    auto lastTicks = SDL_GetPerformanceCounter();
    int accumElapsedMsec = 0;
    // Main Gameplay Loop.
    while(true)
    {
        auto nowTicks = SDL_GetPerformanceCounter();
        auto elapsedTicks = nowTicks - lastTicks;
        lastTicks = nowTicks;
        auto elapsedMsec = int(1000 * elapsedTicks / TICKS_PER_SECOND);
        accumElapsedMsec += elapsedMsec;
        while(accumElapsedMsec >= UPDATE_QUANTUM_MSEC)
        {
            update(UPDATE_QUANTUM_MSEC);
            accumElapsedMsec -= UPDATE_QUANTUM_MSEC;
        }
        draw();
        SDL_GL_SwapWindow(win);
        auto endTicks = SDL_GetPerformanceCounter();
        auto frameTicks = endTicks - nowTicks;
        auto frameMsec = int(1000*frameTicks / TICKS_PER_SECOND);
        auto leftoverMsec = DESIRED_MSEC_PER_FRAME - frameMsec;
        if(leftoverMsec > 0)
            SDL_Delay(leftoverMsec);
    }
    return 0;
}
    
