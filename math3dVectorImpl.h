
//this file is only included from within math3d.h
//do not include it directly.
 
//componentwise multiplication
MyType operator*(const MyType& o) const {
    MyType L;
    for(unsigned i=0;i<len();++i){
        L[i] = (T)((*this)[i] * o[i]);
    }
    return L;
}

//scalar mult
MyType operator*(float o) const {
    MyType L;
    for(unsigned i=0;i<len();++i){
        L[i] = (T)((*this)[i] * o);
    }
    return L;
}

MyType operator*(double o) const {
    MyType L;
    for(unsigned i=0;i<len();++i){
        L[i] = (T)((*this)[i] * (T)o);
    }
    return L;
}
MyType operator*(int o) const {
    MyType L;
    for(unsigned i=0;i<len();++i){
        L[i] = (T)((*this)[i] * (T)o);
    }
    return L;
}
MyType operator*(unsigned o) const {
    MyType L;
    for(unsigned i=0;i<len();++i){
        L[i] = (T)((*this)[i] * (T)o);
    }
    return L;
}

//negation
MyType operator-() const {
    return *this * -1.0f;
}

//unary positive
MyType operator+() const {
    return *this;
}

MyType operator+(const MyType& o) const {
    MyType L;
    for(unsigned i=0;i<len();++i){
        L[i] = (*this)[i] + o[i];
    }
    return L;
}

MyType operator-(const MyType& o) const {
    MyType L;
    for(unsigned i=0;i<len();++i){
        L[i] = (*this)[i] - o[i];
    }
    return L;
}

MyType operator/(float f){
    return  *this * float(1.0f/f);
}

bool operator==(const MyType& o) const {
    for(unsigned i=0;i<len();++i){
        if((*this)[i] != o[i])
            return false;
    }
    return true;
}

bool operator!=(const MyType& o)const {
    return ! (*this == o);
}

float length() const {
    return magnitude();
}

float magnitude() const {
    return sqrt(magnitudeSq());
}

float magnitudeSq() const {
    float total=0;
    for(unsigned i=0;i<len();++i){
        total += (*this)[i] * (*this)[i];
    }
    return total;
}

MyType normalized() const {
    return  *this * 1.0f/magnitude() ;
}

bool isZero() const {
    return magnitudeSq() == 0.0f;
}

const std::unique_ptr<T[]> tobytes() const {
    std::unique_ptr<T[]> F(new T[len()]);
    for(unsigned i=0;i<len();++i)
        F[i] = (*this)[i];
    return F;
}

operator std::string() const {
    std::ostringstream oss;
    oss << "vec" << len() << "(";
    for(unsigned i=0;i<len();++i){
        oss << " " << (*this)[i];
    }
    oss << ")";
    return oss.str();
}
