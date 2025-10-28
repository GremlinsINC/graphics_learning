#include "Triangle.hpp"
#include "math/Matrix3.hpp"
#include <iostream>

Triangle::Triangle(const Vector2& p1, const Vector2& p2, const Vector2& p3) {
    localVertices_ = {p1, p2, p3};
    updateWorldVertices();
}


void Triangle::setColor(float r, float g, float b, float a) {
    color_ = {r, g, b, a};
}


void Triangle::updateWorldVertices() {
    try {
        std::cout << "Triangle: Updating world vertices, position: (" 
                  << worldPosition_.x << ", " << worldPosition_.y 
                  << "), scale: (" << worldScale_.x << ", " << worldScale_.y << ")" << std::endl;
        
        Matrix3 transform = Matrix3::translation(worldPosition_.x, worldPosition_.y)
          * Matrix3::scale(worldScale_.x, worldScale_.y)
          * Matrix3::rotation(rotationAngle_);
        
        for (int i = 0; i < 3; ++i) {
            worldVertices_[i] = transform.transformPoint(localVertices_[i]);
            std::cout << "Triangle: Local vertex " << i << ": (" 
                      << localVertices_[i].x << ", " << localVertices_[i].y << ")" << std::endl;
            std::cout << "Triangle: World vertex " << i << ": (" 
                      << worldVertices_[i].x << ", " << worldVertices_[i].y << ")" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Triangle: Error in updateWorldVertices: " << e.what() << std::endl;
        throw;
    }
}


std::array<Vector2, 3> Triangle::getScreenVertices(const Vector2& screenSize) const {
    std::array<Vector2, 3> screenVertices;

    try {
        std::cout << "Triangle: Converting to screen coordinates, screen size: " 
                  << screenSize.x << "x" << screenSize.y << std::endl;

        for (int i = 0; i < 3; ++i) {
            int sx = (worldVertices_[i].x + 1.0f) * 0.5f * screenSize.x;
            int sy = (1.0f - worldVertices_[i].y) * 0.5f * screenSize.y;

            screenVertices[i] = {sx, sy};

            std::cout << "Triangle: World vertex " << i << ": (" 
                      << worldVertices_[i].x << ", " << worldVertices_[i].y << ")"
                      << " -> Screen vertex: (" 
                      << sx << ", " << sy << ")" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Triangle: Error in getScreenVertices: " << e.what() << std::endl;
        throw;
    }

    return screenVertices;
}


void Triangle::setWorldPosition(const Vector2& position) {
    worldPosition_ = position;
    updateWorldVertices();
}


void Triangle::setWorldScale(const Vector2& scale) {
    worldScale_ = scale;
    updateWorldVertices();
}


void Triangle::setRotation(float angleDegrees) {
    rotationAngle_ = angleDegrees;
    updateWorldVertices();
}


void Triangle::move(const Vector2& delta) {
    worldPosition_.x += delta.x;
    worldPosition_.y += delta.y;
    updateWorldVertices();
}


void Triangle::rotate(float deltaDegrees) {
    rotationAngle_ += deltaDegrees;
    updateWorldVertices();
}

void Triangle::scale(const Vector2& scaleDelta) {
    worldScale_.x *= scaleDelta.x;
    worldScale_.y *= scaleDelta.y;
    updateWorldVertices();
}
