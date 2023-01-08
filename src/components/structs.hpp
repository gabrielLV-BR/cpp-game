#pragma once

#include <math.h>

namespace structs {

    class vector3 {
    public:
        float x, y, z;

        vector3(float x, float y, float z) 
            : x(x), y(y), z(z) 
        {}

        vector3 operator+(vector3& a) {
            return { a.x + this->x, a.y + this->y, a.z + this->z };
        }

        vector3 operator-(vector3& a) {
            return { a.x - this->x, a.y - this->y, a.z - this->z };
        }

        template<typename T>
        vector3 operator*(T a) {
            return { this->x * a, this->y * a, this->z * a };
        }

        template<typename T>
        vector3 operator/(T a) {
            return { this->x / a, this->y / a, this->z / a };
        }

        float length() {
            return sqrt(x*x + y*y + z*z);
        }

        vector3 cross(vector3& o) {
            return {
                this->y * o.z + this->z * o.y,
                this->x * o.z + this->z * o.x,
                this->x * o.y + this->y * o.x
            };
        }

        float dot(vector3& o) {
            return this->x * o.x + this->y * o.y + this->z * o.z;
        }

        vector3 normalized() {
            return *this / length();
        }
    };

    struct quaternion {
        float x, y, z, w;
        //TODO: everything
    };

    struct dimension2D {
        float w, h;
    };

    struct color {
        float r, g, b, a;
    };

    struct rect {
        dimension2D dimensions;
        struct color color;
    };

}