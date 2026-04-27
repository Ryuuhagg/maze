//constant.h
#pragma once
const int WIDTH = 800;
const int HEIGHT = 600;

template <typename T>
T Clamp(T v, T min, T max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}