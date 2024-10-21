#pragma once

#include <cmath>

class Vec3 {
public:
    float X, Y, Z;

    Vec3() : X(0.0f), Y(0.0f), Z(0.0f) {}
    Vec3(float x, float y, float z) : X(x), Y(y), Z(z) {}

    // overload the minus operator
    Vec3 operator-(const Vec3& other) const {
        return {X - other.X, Y - other.Y, Z - other.Z};
    }

    // Normalize the vector
    void Normalise() {
	    if (float length = std::sqrt(X * X + Y * Y + Z * Z); length != 0.0f) {
            X /= length;
            Y /= length;
            Z /= length;
        }
    }

    // get the length of a vector
    [[nodiscard]] float Length() const {
        return std::sqrt(X * X + Y * Y + Z * Z);
    }
};

