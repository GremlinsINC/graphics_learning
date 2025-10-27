#pragma once
#include "Vector2.hpp"
#include <array>
#include <cmath>
#include <iostream>

class Matrix3 {
public:
    std::array<std::array<float, 3>, 3> m{};

    Matrix3() {
        setIdentity();
    }

    static Matrix3 identity() {
        Matrix3 result;
        result.setIdentity();
        return result;
    }

    static Matrix3 translation(float tx, float ty) {
        Matrix3 result = identity();
        result.m[0][2] = tx;
        result.m[1][2] = ty;
        return result;
    }

    static Matrix3 scale(float sx, float sy) {
        Matrix3 result = identity();
        result.m[0][0] = sx;
        result.m[1][1] = sy;
        return result;
    }

    static Matrix3 rotation(float angleDegrees) {
        Matrix3 result = identity();
        float radians = angleDegrees * 3.1415926535f / 180.0f;
        float c = std::cos(radians);
        float s = std::sin(radians);

        result.m[0][0] = c;
        result.m[0][1] = -s;
        result.m[1][0] = s;
        result.m[1][1] = c;
        return result;
    }

    Matrix3 operator*(const Matrix3& other) const {
        Matrix3 result;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                result.m[row][col] = 
                    m[row][0] * other.m[0][col] +
                    m[row][1] * other.m[1][col] +
                    m[row][2] * other.m[2][col];
            }
        }
        return result;
    }

    Vector2 transformPoint(const Vector2& v) const {
        float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2];
        float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2];
        return {x, y};
    }

    void print() const {
        std::cout << "Matrix3:\n";
        for (int i = 0; i < 3; ++i)
            std::cout << "[" << m[i][0] << " " << m[i][1] << " " << m[i][2] << "]\n";
    }

private:
    void setIdentity() {
        m = {{{1, 0, 0},
              {0, 1, 0},
              {0, 0, 1}}};
    }
};
