#pragma once 

#include "math3d.h"
#include "Program.h"

/*
* Class for handling the game's Camera.
*/
class Camera
{
public:
    float aspectRatio = 1.0f;

    // Camera Position
    vec3 eye;

    // Camera matricies for screen->world position transformations/projections.
    mat4 viewMatrix;
    mat4 projMatrix;

    // Clipping plane distances.
    float hither = 0.1f;
    float yon = 100.0f;

    // Field of View values.
    float fov_v;
    float fov_h;

    // Camera direction vectors.
    vec3 right, up, look;

    // Base Constructor. Postion, Center of Interest, Up Vector, Field of View angle, Aspect ratio.
    Camera(vec3 eye, vec3 coi, vec3 up, float fov_v=30, float aspectRatio = 1.0f )
    {
        this->lookAt(eye,coi,up);
        this->fov_v = radians(fov_v);
        this->fov_h = aspectRatio * this->fov_v;
        this->aspectRatio = aspectRatio;
        this->updateProjMatrix();
    }
    
    // Function for move the cameras focus.
    void lookAt(vec3 eye, vec3 coi, vec3 up)
    {
        this->eye = eye;
        this->look = normalize(coi-this->eye);
        this->right = normalize(cross(this->look,up));
        this->up = cross( this->right, this->look );
        this->updateViewMatrix();
    }
    
    // Function for updating the View Matrix.
    void updateViewMatrix()
    {
        this->viewMatrix = mat4(
            this->right.x, this->up.x, -this->look.x, 0,
            this->right.y, this->up.y, -this->look.y, 0,
            this->right.z, this->up.z, -this->look.z, 0,
            -dot(this->eye,this->right), -dot(this->eye,this->up), dot(this->eye,this->look), 1.0f);
    }
    
    // Function for updating the Projection Matrix.
    void updateProjMatrix()
    {
        float P = 1 + (2*this->yon)/(this->hither-this->yon);
        float Q = 2 * this->hither * this->yon / (this->hither - this->yon );
        this->projMatrix = mat4(
            1/tan(this->fov_h), 0,                  0,      0,
            0,                  1/tan(this->fov_v), 0,      0,
            0,                  0,                  P,     -1,
            0,                  0,                  Q,      0
        );
    }
    
    // Standard Uniform Updater.
    void setUniforms()
    {
        Program::setUniform("viewMatrix", this->viewMatrix);
        Program::setUniform("projMatrix", this->projMatrix);
        Program::setUniform("eyePos", this->eye);
    }

    // Function for rotating the camera.
    void turn( float amt )
    {
        auto M = axisRotation( this->up, amt );
        this->look = (vec4(this->look,0)*M).xyz();
        this->right = (vec4(this->right,0)*M).xyz();
        this->updateViewMatrix();
    }
    
    // Function for tilting the camera.
    void tilt( float amt )
    {
        auto M = axisRotation( this->right, amt );
        this->look = (vec4(this->look,0)*M).xyz();
        this->up = (vec4(this->up,0)*M).xyz();
        this->updateViewMatrix();
    }
    
    // Function for walking the camera.
    void walk( float amt )
    {
        this->strafe(0,0,amt);
    }
    
    // Function for strafing the camera.
    void strafe(float dr, float du, float dl)
    {
        auto delta = dr * this->right + du * this->up + dl * this->look;
        this->eye = this->eye + delta;
        this->updateViewMatrix();
    }
};

        
