#pragma once
#include <iostream>
/// <summary>
/// 3次元ベクトル
/// </summary>

struct Vector3 final {
	float x;
	float y;
	float z;

	Vector3& operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }

	Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }

	Vector3 operator*(const float& other) const { return { x * other, y * other, z * other }; }

	Vector3& operator-=(const Vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vector3& operator=(float scalar) {
		x = scalar;
		y = scalar;
		z = scalar;
		return *this;
	}

	Vector3& operator/=(float scalar) {
		if (scalar != 0) {
			x /= scalar;
			y /= scalar;
			z /= scalar;
		} else {
			std::cerr << "Error: Division by zero" << std::endl;
		}
		return *this;
	}
};