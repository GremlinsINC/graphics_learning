#pragma once
#include "math/Vector2.hpp"
#include <array>

class Triangle {
public:
    Triangle(const Vector2& p1, const Vector2& p2, const Vector2& p3);
    
    // Установка мировых координат
    void setWorldPosition(const Vector2& position);
    void setWorldScale(const Vector2& scale);
    void setRotation(const float angleDegrees);
    
    // Получение вершин в экранных координатах
    std::array<Vector2, 3> getScreenVertices(const Vector2& screenSize) const;


    void move(const Vector2& delta);
    void scale(const Vector2& scaleDelta);
    void rotate(const float deltaDegrees);

    // Цвет треугольника
    void setColor(float r, float g, float b, float a = 1.0f);
    const std::array<float, 4>& getColor() const { return color_; }
    
private:
    std::array<Vector2, 3> localVertices_;
    std::array<Vector2, 3> worldVertices_;
    Vector2 worldPosition_{0.0f, 0.0f};
    Vector2 worldScale_{1.0f, 1.0f};
    float rotationAngle_ = 0.0f;
    std::array<float, 4> color_{1.0f, 0.0f, 0.0f, 1.0f}; // Красный
    
    void updateWorldVertices();
};
