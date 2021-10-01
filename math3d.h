
#pragma once
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <initializer_list>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <array>
#include <cstring>

//"""Classes for 2D and 3D matrix/vector math."""

// ~ Some of these functions (individually noted) are based on code from TDL.
// ~ The TDL copyright is as follows:
 
// ~ Copyright 2009, Google Inc.
// ~ All rights reserved.

// ~ Redistribution and use in source and binary forms, with or without
// ~ modification, are permitted provided that the following conditions are
// ~ met:

// ~ *  Redistributions of source code must retain the above copyright
// ~ notice, this list of conditions and the following disclaimer.
// ~ *  Redistributions in binary form must reproduce the above
// ~ copyright notice, this list of conditions and the following disclaimer
// ~ in the documentation and/or other materials provided with the
// ~ distribution.
// ~ *  Neither the name of Google Inc. nor the names of its
// ~ contributors may be used to endorse or promote products derived from
// ~ this software without specific prior written permission.

// ~ THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ~ "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// ~ LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// ~ A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// ~ OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// ~ SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// ~ LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// ~ DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// ~ THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// ~ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// ~ OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.




template<typename DataType,unsigned N,typename VecType>
class Matrix{
    
    typedef Matrix<DataType,N,VecType> MyType;
    
    public:
    DataType _M[N][4];
    
    Matrix(){
        std::memset(_M,0,sizeof(_M));
    }
    
    private:
    template<typename T>
    Matrix(const std::initializer_list<T>& L){
        if( L.size() != N*N )
            throw std::runtime_error("Bad number of arguments");
        auto x = L.begin();
        for(unsigned i=0;i<N;++i){
            for(unsigned j=0;j<N;++j){
                (*this)[i][j] = *x++;
            }
        }
    }
    public:

    Matrix(DataType m0, DataType m1, DataType m2, DataType m3) : Matrix({m0,m1,m2,m3}){}
    Matrix( DataType m0, DataType m1, DataType m2,
                DataType m3, DataType m4, DataType m5,
                DataType m6, DataType m7, DataType m8) : Matrix({m0,m1,m2,m3,m4,m5,m6,m7,m8}){}
    Matrix( DataType m0, DataType m1, DataType m2, DataType m3,
                DataType m4, DataType m5, DataType m6, DataType m7,
                DataType m8, DataType m9, DataType ma, DataType mb,
                DataType mc, DataType md, DataType me, DataType mf) : Matrix({m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,ma,mb,mc,md,me,mf}){}
    
    MyType operator+(const MyType& m2) const {
        MyType r;
        for(unsigned i=0;i<N;++i){
            for(unsigned j=0;j<N;++j){
                r[i][j] = (*this)[i][j] + m2[i][j];
            }
        }
        return r;
    }
        
    MyType operator-(const MyType& m2) const {
        MyType r;
        for(unsigned i=0;i<N;++i){
            for(unsigned j=0;j<N;++j){
                r[i][j] = (*this)[i][j] - m2[i][j];
            }
        }
        return r;
    }
       
    MyType operator*(const MyType& o) const { 
        //mat * mat
        MyType R;
        for(unsigned i=0;i<N;++i){
            for(unsigned j=0;j<N;++j){
                DataType total=0;
                for(unsigned k=0;k<N;++k){
                    total += (*this)[i][k] * o[k][j];
                }
                R[i][j] = total;
            }
        }
        return R;
    }
    
    VecType operator*(const VecType& o) const { 
        //mat * vector
        VecType R;
        for(unsigned i=0;i<N;++i){
            DataType total = 0.0;
            for(unsigned j=0;j<N;++j){
                total += (*this)[i][j] * o[j];
            }
            R[i] = total;
        }
        return R;
    }

    MyType operator*(float o) const { 
        //mat * scalar
        MyType R;
        for(unsigned i=0;i<N;++i){
            for(unsigned j=0;j<N;++j){
                R[i][j] = (*this)[i][j] * o;
            }
        }
        return R;
    }
    MyType operator*(int o) const { 
        return *this * (float)o;
    }
    
    //Vector<N> operator*(const Vector<N>& v) const ;
    
    MyType operator+() const {
        return *this;
    }
    MyType operator-() const {
        return *this * -1.0f;
    }
    
    const void* tobytes() const {
        return _M;
    }
    
    struct MatRow{
        MyType& m;
        unsigned i;
        MatRow(MyType& mm,int ii) : m(mm), i(ii){ }
        DataType& operator[](unsigned j){
            //each row is padded out to be a vec4
            if( j >= N )
                throw std::runtime_error("Bad index");
            return this->m._M[i][j];
        }
    };
    struct CMatRow{
        const MyType& m;
        unsigned i;
        CMatRow(const MyType& mm,int ii) : m(mm), i(ii){ }
        DataType operator[](unsigned j) const {
            //each row is padded out to be a vec4
            if( j >= N )
                throw std::runtime_error("Bad index");
            //~ std::cout << "[" << i << "][" << j << "] = index " << i*4+j << "\n";
            return this->m._M[i][j];
        }
    };
    
    MatRow operator[](unsigned i) {
        return MatRow(*this,i);
    }
    CMatRow operator[](unsigned i) const  {
        return CMatRow(*this,i);
    }
        
    bool operator==(const MyType& o) const {
        for(unsigned i=0;i<N;++i){
            for(unsigned j=0;j<N;++j){
                if( (*this)[i][j] != o[i][j] )
                    return false;
            }
        }
        return true;
    }
    
    bool operator!=(const MyType& o) const {
        return ! (*this==o);
    }
    
    operator std::string() const {
        std::ostringstream oss;
        for(unsigned i=0;i<N;++i){
            oss << "[";
            for(unsigned j=0;j<N;++j){
                oss <<  (*this)[i][j] << "   ";
            }
            oss << "]\n";
        }
        return oss.str();
    }
        
    MyType transpose() const {
        MyType R;
        for(unsigned i=0;i<N;++i){
            for(unsigned j=0;j<N;++j){
                R[i][j] = (*this)[j][i];
            }
        }
        return R;
    }
    
    static MyType identity(){
        MyType R;
        for(unsigned i=0;i<N;++i){
            R[i][i]=1;
        }
        return R;
    }
};
 

template<typename A, unsigned int B, typename C>
Matrix<A,B,C> operator*( float f, const Matrix<A,B,C>& m){
    return m*f;
}
template<typename A, unsigned int B, typename C>
Matrix<A,B,C> operator*( double f, const Matrix<A,B,C>& m){
    return m*f;
}
template<typename A, unsigned int B, typename C>
Matrix<A,B,C> operator*( int f, const Matrix<A,B,C>& m){
    return m*f;
}
template<typename A, unsigned int B, typename C>
Matrix<A,B,C> operator*( unsigned f, const Matrix<A,B,C>& m){
    return m*f;
}

template<typename T>
struct vec2Base {
    typedef vec2Base<T> MyType;
    T x,y;
    vec2Base(){
        x=y=0;
    }
    vec2Base(T x, T y){
        this->x=x;
        this->y=y;
    }
    vec2Base(const std::initializer_list<T>& L){
        assert(L.size()==2);
        auto F = L.begin();
        this->x = *F++;
        this->y = *F;;
    }
    
    T& operator[](unsigned key){
        switch(key){
            case 0: return x;
            case 1: return y;
            default:  throw std::runtime_error("Bad index");
        }
    }
    const T operator[](unsigned key) const{
        switch(key){
            case 0: return x;
            case 1: return y;
            default:  throw std::runtime_error("Bad index");
        }
    }
    
    unsigned len() const {
        return 2;
    }
    
    void operator+=(const MyType& o){
        x += o.x;
        y += o.y;
    }
    void operator-=(const MyType& o){
        x -= o.x;
        y -= o.y;
    }
    void operator*=(const MyType& o){
        x *= o.x;
        y *= o.y;
    }
    void operator/=(const MyType& o){
        x /= o.x;
        y /= o.y;
    }
    
    const vec2Base<T> xy(){ return vec2Base<T>(x,y); }
    #include "math3dVectorImpl.h"
};


 

template<typename T>
struct vec3Base{
    typedef vec3Base<T> MyType;
    T x,y,z;
    vec3Base(){
        x=y=z=0;
    }
    vec3Base(T x, T y, T z){
        this->x=x;
        this->y=y;
        this->z=z;
    }
    vec3Base(const std::initializer_list<T>& L){
        assert(L.size()==3);
        auto F = L.begin();
        this->x = *F++;
        this->y = *F++;
        this->z = *F;
    }
    T& operator[](unsigned key){
        switch(key){
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default:  throw std::runtime_error("Bad index");
        }
    }
    const T operator[](unsigned key) const{
        switch(key){
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default:  throw std::runtime_error("Bad index");
        }
    }
    
    unsigned len() const {
        return 3;
    }

    void operator+=(const MyType& o){
        x += o.x;
        y += o.y;
        z += o.z;
    }
    void operator-=(const MyType& o){
        x -= o.x;
        y -= o.y;
        z -= o.z;
    }
    void operator*=(const MyType& o){
        x *= o.x;
        y *= o.y;
        z *= o.z;
    }
    void operator/=(const MyType& o){
        x /= o.x;
        y /= o.y;
        z /= o.z;
    }

    
    const vec2Base<T> xy() const { return vec2Base<T>(x,y); }
    const vec3Base<T> xyz() const { return vec3Base<T>(x,y,z); }
    
    #include "math3dVectorImpl.h"
};


template<typename T>
struct vec4Base{
    typedef vec4Base<T> MyType;
    T x,y,z,w;
    vec4Base(){
        x=y=z=w=0;
    }
    vec4Base(T x, T y, T z, T w){
        this->x=x;
        this->y=y;
        this->z=z;
        this->w=w;
    }
    vec4Base(const vec3Base<T>& v, T w){
        this->x=v.x;
        this->y=v.y;
        this->z=v.z;
        this->w=w;
    }
    vec4Base(const std::initializer_list<T>& L){
        assert(L.size()==4);
        auto F = L.begin();
        this->x = *F++;
        this->y = *F++;
        this->z = *F++;
        this->w = *F++;
    }
    T& operator[](unsigned key){
        switch(key){
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default:  throw std::runtime_error("Bad index");
        }
    }
    const T operator[](unsigned key) const{
        switch(key){
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default:  throw std::runtime_error("Bad index");
        }
    }

    void operator+=(const MyType& o){
        x += o.x;
        y += o.y;
        z += o.z;
        w += o.w;
    }
    void operator-=(const MyType& o){
        x -= o.x;
        y -= o.y;
        z -= o.z;
        w -= o.w;
    }
    void operator*=(const MyType& o){
        x *= o.x;
        y *= o.y;
        z *= o.z;
        w *= o.w;
    }
    void operator/=(const MyType& o){
        x /= o.x;
        y /= o.y;
        z /= o.z;
        w /= o.w;
    }
    
    unsigned len() const {
        return 4;
    }
    
    const vec2Base<T> xy() const { return vec2Base<T>(x,y); }
    const vec3Base<T> xyz() const { return vec3Base<T>(x,y,z); }
    const vec4Base<T> xyzw() const{ return vec4Base<T>(x,y,z,w); }

    #include "math3dVectorImpl.h"
        
};

template<typename ST, typename VT>
inline vec2Base<VT> operator*(ST f, const vec2Base<VT>& v){
    return v*f;
}
template<typename ST, typename VT>
inline vec3Base<VT> operator*(ST f, const vec3Base<VT>& v){
    return v*f;
}
template<typename ST, typename VT>
inline vec4Base<VT> operator*(ST f, const vec4Base<VT>& v){
    return v*f;
}



typedef vec4Base<float> vec4;
typedef vec4Base<int32_t> ivec4;
typedef vec4Base<uint32_t> uvec4;
typedef vec3Base<float> vec3;
typedef vec3Base<int32_t> ivec3;
typedef vec3Base<uint32_t> uvec3;
typedef vec2Base<float> vec2;
typedef vec2Base<int32_t> ivec2;
typedef vec2Base<uint32_t> uvec2;
typedef Matrix<float,4,vec4> mat4;
typedef Matrix<float,3,vec3> mat3;
typedef Matrix<float,2,vec2> mat2;
typedef Matrix<int32_t,4,ivec4> imat4;
typedef Matrix<int32_t,3,ivec3> imat3;
typedef Matrix<int32_t,2,ivec2> imat2;



//vector-matrix multiplication
template<typename VT, typename MT, unsigned int n>
VT operator*(const VT& v, const Matrix<MT,n,VT>& o)  {
    VT R;
    for(unsigned i=0;i<v.len();++i){
        float total=0;
        for(unsigned j=0;j<v.len();++j){
            total += v[j]*o[j][i];
        }
        R[i]=total;
    }
    return R;
}



template<typename T>
float dot(const T& v, const T& w){
    float total = 0;
    for(unsigned i=0;i<v.len();++i){
        total += v[i]*w[i];
    }
    return total;
}

inline
vec3 cross(const vec3& v, const vec3& w){
    return vec3(
        v[1]*w[2] - w[1]*v[2],
        w[0]*v[2] - v[0]*w[2],
        v[0]*w[1] - w[0]*v[1]
    );
}

inline
vec3 cross(const vec4& v, const vec4& w){
    return cross(v.xyz(),w.xyz());
}
inline
vec3 cross(const vec4& v, const vec3& w){
    return cross(v.xyz(),w);
}
inline
vec3 cross(const vec3& v, const vec4& w){
    return cross(v,w.xyz());
}


template<typename T>
float length(const T& v){
    return v.length();
}

template<typename T>
T normalize(const T& v){
    auto le=1.0f/length(v);
    return le*v;
}

template<typename T>
T transpose(const T& m){
    return m.transpose();
}

// ~ def det(M) const {
    // ~ """Return matrix determinant. From TDL.
    // ~ @param M: matrix
    // ~ @rtype: number
    // ~ """
    // ~ if type(M) == mat2:
        // ~ return m[0][0]*m[1][1] - m[0][1]*m[1][0]
    // ~ elif type(M) == mat3:
        // ~ return m[2][2] * (m[0][0] * m[1][1] - m[0][1] * m[1][0]) -              m[2][1] * (m[0][0] * m[1][2] - m[0][2] * m[1][0]) +                m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1])
    // ~ elif type(M) == mat4:
        // ~ t01 = m[0][0] * m[1][1] - m[0][1] * m[1][0]
        // ~ t02 = m[0][0] * m[1][2] - m[0][2] * m[1][0]
        // ~ t03 = m[0][0] * m[1][3] - m[0][3] * m[1][0]
        // ~ t12 = m[0][1] * m[1][2] - m[0][2] * m[1][1]
        // ~ t13 = m[0][1] * m[1][3] - m[0][3] * m[1][1]
        // ~ t23 = m[0][2] * m[1][3] - m[0][3] * m[1][2]
        // ~ return (m[3][3] * (m[2][2] * t01 - m[2][1] * t02 + m[2][0] * t12) -
             // ~ m[3][2] * (m[2][3] * t01 - m[2][1] * t03 + m[2][0] * t13) +
             // ~ m[3][1] * (m[2][3] * t02 - m[2][2] * t03 + m[2][0] * t23) -
             // ~ m[3][0] * (m[2][3] * t12 - m[2][2] * t13 + m[2][1] * t23) )
    // ~ else:
        // ~ assert 0

inline
mat2 inverse(const mat2& m){
    //Return matrix inverse. From TDL.
    auto d = 1.0f / (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
    return mat2(d * m[1][1], -d * m[0][1], -d * m[1][0], d * m[0][0]);
}
    
inline
mat3 inverse(const mat3& m){
    //Return matrix inverse. From TDL.
    auto t00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
    auto t10 = m[0][1] * m[2][2] - m[0][2] * m[2][1];
    auto t20 = m[0][1] * m[1][2] - m[0][2] * m[1][1];
    auto d = 1.0f / (m[0][0] * t00 - m[1][0] * t10 + m[2][0] * t20);
    return mat3( d * t00, -d * t10, d * t20,
          -d * (m[1][0] * m[2][2] - m[1][2] * m[2][0]),
           d * (m[0][0] * m[2][2] - m[0][2] * m[2][0]),
          -d * (m[0][0] * m[1][2] - m[0][2] * m[1][0]),
           d * (m[1][0] * m[2][1] - m[1][1] * m[2][0]),
          -d * (m[0][0] * m[2][1] - m[0][1] * m[2][0]),
           d * (m[0][0] * m[1][1] - m[0][1] * m[1][0]) );
}
inline
mat4 inverse(const mat4& m){
    //Return matrix inverse. From TDL.

    auto tmp_0 = m[2][2] * m[3][3];
    auto tmp_1 = m[3][2] * m[2][3];
    auto tmp_2 = m[1][2] * m[3][3];
    auto tmp_3 = m[3][2] * m[1][3];
    auto tmp_4 = m[1][2] * m[2][3];
    auto tmp_5 = m[2][2] * m[1][3];
    auto tmp_6 = m[0][2] * m[3][3];
    auto tmp_7 = m[3][2] * m[0][3];
    auto tmp_8 = m[0][2] * m[2][3];
    auto tmp_9 = m[2][2] * m[0][3];
    auto tmp_10 = m[0][2] * m[1][3];
    auto tmp_11 = m[1][2] * m[0][3];
    auto tmp_12 = m[2][0] * m[3][1];
    auto tmp_13 = m[3][0] * m[2][1];
    auto tmp_14 = m[1][0] * m[3][1];
    auto tmp_15 = m[3][0] * m[1][1];
    auto tmp_16 = m[1][0] * m[2][1];
    auto tmp_17 = m[2][0] * m[1][1];
    auto tmp_18 = m[0][0] * m[3][1];
    auto tmp_19 = m[3][0] * m[0][1];
    auto tmp_20 = m[0][0] * m[2][1];
    auto tmp_21 = m[2][0] * m[0][1];
    auto tmp_22 = m[0][0] * m[1][1];
    auto tmp_23 = m[1][0] * m[0][1];

    auto t0 = (tmp_0 * m[1][1] + tmp_3 * m[2][1] + tmp_4 * m[3][1]) -        (tmp_1 * m[1][1] + tmp_2 * m[2][1] + tmp_5 * m[3][1]);
    auto t1 = (tmp_1 * m[0][1] + tmp_6 * m[2][1] + tmp_9 * m[3][1]) -        (tmp_0 * m[0][1] + tmp_7 * m[2][1] + tmp_8 * m[3][1]);
    auto t2 = (tmp_2 * m[0][1] + tmp_7 * m[1][1] + tmp_10 * m[3][1]) -        (tmp_3 * m[0][1] + tmp_6 * m[1][1] + tmp_11 * m[3][1]);
    auto t3 = (tmp_5 * m[0][1] + tmp_8 * m[1][1] + tmp_11 * m[2][1]) -        (tmp_4 * m[0][1] + tmp_9 * m[1][1] + tmp_10 * m[2][1]);
    auto d = 1.0f / (m[0][0] * t0 + m[1][0] * t1 + m[2][0] * t2 + m[3][0] * t3);

    return mat4(d * t0, d * t1, d * t2, d * t3,
       d * ((tmp_1 * m[1][0] + tmp_2 * m[2][0] + tmp_5 * m[3][0]) -
          (tmp_0 * m[1][0] + tmp_3 * m[2][0] + tmp_4 * m[3][0])),
       d * ((tmp_0 * m[0][0] + tmp_7 * m[2][0] + tmp_8 * m[3][0]) -
          (tmp_1 * m[0][0] + tmp_6 * m[2][0] + tmp_9 * m[3][0])),
       d * ((tmp_3 * m[0][0] + tmp_6 * m[1][0] + tmp_11 * m[3][0]) -
          (tmp_2 * m[0][0] + tmp_7 * m[1][0] + tmp_10 * m[3][0])),
       d * ((tmp_4 * m[0][0] + tmp_9 * m[1][0] + tmp_10 * m[2][0]) -
          (tmp_5 * m[0][0] + tmp_8 * m[1][0] + tmp_11 * m[2][0])),
       d * ((tmp_12 * m[1][3] + tmp_15 * m[2][3] + tmp_16 * m[3][3]) -
          (tmp_13 * m[1][3] + tmp_14 * m[2][3] + tmp_17 * m[3][3])),
       d * ((tmp_13 * m[0][3] + tmp_18 * m[2][3] + tmp_21 * m[3][3]) -
          (tmp_12 * m[0][3] + tmp_19 * m[2][3] + tmp_20 * m[3][3])),
       d * ((tmp_14 * m[0][3] + tmp_19 * m[1][3] + tmp_22 * m[3][3]) -
          (tmp_15 * m[0][3] + tmp_18 * m[1][3] + tmp_23 * m[3][3])),
       d * ((tmp_17 * m[0][3] + tmp_20 * m[1][3] + tmp_23 * m[2][3]) -
          (tmp_16 * m[0][3] + tmp_21 * m[1][3] + tmp_22 * m[2][3])),
       d * ((tmp_14 * m[2][2] + tmp_17 * m[3][2] + tmp_13 * m[1][2]) -
          (tmp_16 * m[3][2] + tmp_12 * m[1][2] + tmp_15 * m[2][2])),
       d * ((tmp_20 * m[3][2] + tmp_12 * m[0][2] + tmp_19 * m[2][2]) -
          (tmp_18 * m[2][2] + tmp_21 * m[3][2] + tmp_13 * m[0][2])),
       d * ((tmp_18 * m[1][2] + tmp_23 * m[3][2] + tmp_15 * m[0][2]) -
          (tmp_22 * m[3][2] + tmp_14 * m[0][2] + tmp_19 * m[1][2])),
       d * ((tmp_22 * m[2][2] + tmp_16 * m[0][2] + tmp_21 * m[1][2]) -
          (tmp_20 * m[1][2] + tmp_23 * m[2][2] + tmp_17 * m[0][2])));
}

    

    
inline
mat4 axisRotation(const vec3& axis, float angle){
    //from TDL
    //axis=normalize(axis)
    float x = axis[0],
    y = axis[1],
    z = axis[2],
    xx = x * x,
    yy = y * y,
    zz = z * z,
    c = cos(angle),
    s = sin(angle),
    oneMinusCosine = 1 - c,
    zs = z*s,
    xs = x*s,
    ys = y*s,
    xy = x*y,
    xz = x*z,
    yz = y*z;
    return mat4(
        xx + (1 - xx) * c,
        xy * oneMinusCosine + zs,
        xz * oneMinusCosine - ys,
        0,
        xy * oneMinusCosine - zs,
        yy + (1 - yy) * c,
        yz * oneMinusCosine + xs,
        0,
        xz * oneMinusCosine + ys,
        yz * oneMinusCosine - xs,
        zz + (1 - zz) * c,
        0,
        0, 0, 0, 1
    );
}

inline
mat4 axisRotation(const vec4& axis, float angle){
    return axisRotation(axis.xyz(),angle);
}
  
inline
mat4 scaling(const vec3& v)  {
    //Return scaling matrix. From TDL.
    return mat4( 
        v[0], 0,0,0,
        0,v[1],0,0,
        0,0,v[2],0,
        0,0,0,1);
}

inline
mat4 scaling(float x, float y, float z)  {
    return scaling(vec3(x,y,z));
}
   
inline
mat4 scaling(float x)  {
    return scaling(vec3(x,x,x));
}

inline
mat4 translation(const vec3& v) {
    //Return translation matrix. From TDL.
    return mat4(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        v[0],v[1],v[2],1);
}

inline
mat4 translation(const vec4& v) {
    return translation(v.xyz());
}
    
inline
mat4 translation(float x, float y, float z) {
    return translation(vec3(x,y,z));
}

inline
mat3 translation2(const vec2& v){
    return mat3(
        1,0,0,
        0,1,0,
        v[0],v[1],1);
}

inline
mat3 scaling2(const vec2& v){
    return mat3(v[0],0,0,
                0,v[1],0,
                0,0,1);
}

inline
mat3 rotation2(float angle){
    /**Return 2D rotation matrix.
        @type angle: float
        @param angle: Angle in radians
        @rtype:mat3
    */
    auto c=cos(angle),
    s=sin(angle);
    return mat3( c,s,0,     -s,c,0,     0,0,1 );
}

inline    
std::ostream& operator<<(std::ostream& o, const vec4& v){
    o << (std::string)v;
    return o;
}
inline
std::ostream& operator<<(std::ostream& o, const vec3& v){
    o << (std::string)v;
    return o;
}
inline
std::ostream& operator<<(std::ostream& o, const vec2& v){
    o << (std::string)v;
    return o;
}
inline
std::ostream& operator<<(std::ostream& o, const mat4& v){
    o << (std::string)v;
    return o;
}
inline
std::ostream& operator<<(std::ostream& o, const mat3& v){
    o << (std::string)v;
    return o;
}
inline
std::ostream& operator<<(std::ostream& o, const mat2& v){
    o << (std::string)v;
    return o;
}

inline
float radians(float deg){
    return deg/180.0f * 3.14159265358979323f;
}

static_assert(sizeof(vec2) == 8, "");
static_assert(sizeof(vec3) == 12, "");
static_assert(sizeof(vec4) == 16, "");
static_assert(sizeof(ivec2) == 8, "");
static_assert(sizeof(ivec3) == 12, "");
static_assert(sizeof(ivec4) == 16, "");
static_assert(sizeof(mat2) == 32, "");
static_assert(sizeof(mat3) == 48, "");
static_assert(sizeof(mat4) == 64, "");
