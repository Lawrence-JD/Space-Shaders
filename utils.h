#pragma once

#include "glfuncs.h"
#include <SDL.h>
#include <SDL_mixer.h>
#undef main
#pragma comment(lib, "windowscodecs.lib")
#include "math3d.h"
#include <random>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <array>
#include <cmath>

/*
* This file contains helpful utility values and functions.
*/

// Gameplay states for enemies
#define ALIVE 1
#define DYING 2
#define DEAD 3

// Collision check for two spheres; c=center, r=radius
bool collidesWith( vec3 c1, float r1, vec3 c2, float r2);

// Functions for generating random numbers.
float uniform(float min, float max);
float randrange(float min, float max);
inline float randrange(int a, int b){ return randrange(float(a),float(b)); }

// Templated helper function used to see if a set contains the given element.
template<typename T, typename U>
bool setContains( const std::set<T>& s, const U& item)
{
    return s.find(item) != s.end();
}

bool endswith(const std::string& s1, const std::string& s2);

//http://www.martinbroadhurst.com/how-to-trim-a-stdstring.html
inline
std::string trim(const std::string& s){
    std::string r = s;
    auto q = r.find_first_not_of(" \t\n\r");
    if( q != std::string::npos )
        r = r.substr(q);
    q = r.find_last_not_of(" \t\n\r");
    if( q != std::string::npos ){
        q++;
        r = r.substr(0,q);
    }
    return r;
}

// Helper function to see if string 1 ends with string 2.
inline
bool endswith(const std::string& s1, const std::string& s2)
{
    auto idx = s1.rfind(s2);
    if( idx != std::string::npos && idx == s1.length() - s2.length() )
        return true;
    else
        return false;
}

inline
float uniform(float min, float max)
{
    static std::minstd_rand engine(42);
    static std::uniform_real_distribution<float> distro;
    float t = distro(engine);
    return min + t * (max-min);
}

inline
float randrange(float min, float max)
{
    return uniform(min,max);
}


