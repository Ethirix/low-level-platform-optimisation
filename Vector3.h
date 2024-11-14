#pragma once

#include <cmath>

class Vector3 {
public:
    float X, Y, Z;

    Vector3() : X(0.0f), Y(0.0f), Z(0.0f) {}
    Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

    // overload the minus operator
    Vector3 operator-(const Vector3& other) const {
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

