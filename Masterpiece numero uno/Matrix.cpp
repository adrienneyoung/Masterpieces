
#include "Matrix.h"
#include <math.h>

Matrix::Matrix() {
    identity();
}

void Matrix::identity() {
    m[0][0] = 1.0;
    m[1][0] = 0.0;
    m[2][0] = 0.0;
    m[3][0] = 0.0;
    
    m[0][1] = 0.0;
    m[1][1] = 1.0;
    m[2][1] = 0.0;
    m[3][1] = 0.0;

    m[0][2] = 0.0;
    m[1][2] = 0.0;
    m[2][2] = 1.0;
    m[3][2] = 0.0;

    m[0][3] = 0.0;
    m[1][3] = 0.0;
    m[2][3] = 0.0;
    m[3][3] = 1.0;
}

Matrix Matrix::inverse() const {
    float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
    float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
    float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
    float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];
    
    float v0 = m20 * m31 - m21 * m30;
    float v1 = m20 * m32 - m22 * m30;
    float v2 = m20 * m33 - m23 * m30;
    float v3 = m21 * m32 - m22 * m31;
    float v4 = m21 * m33 - m23 * m31;
    float v5 = m22 * m33 - m23 * m32;
    
    float t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    float t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    float t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    float t30 = - (v3 * m10 - v1 * m11 + v0 * m12);
    
    float invDet = 1.0f / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
    
    float d00 = t00 * invDet;
    float d10 = t10 * invDet;
    float d20 = t20 * invDet;
    float d30 = t30 * invDet;
    
    float d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
    
    v0 = m10 * m31 - m11 * m30;
    v1 = m10 * m32 - m12 * m30;
    v2 = m10 * m33 - m13 * m30;
    v3 = m11 * m32 - m12 * m31;
    v4 = m11 * m33 - m13 * m31;
    v5 = m12 * m33 - m13 * m32;
    
    float d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
    
    v0 = m21 * m10 - m20 * m11;
    v1 = m22 * m10 - m20 * m12;
    v2 = m23 * m10 - m20 * m13;
    v3 = m22 * m11 - m21 * m12;
    v4 = m23 * m11 - m21 * m13;
    v5 = m23 * m12 - m22 * m13;
    
    float d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
    
    Matrix m2;
    m2.m[0][0] = d00;
    m2.m[0][1] = d01;
    m2.m[0][2] = d02;
    m2.m[0][3] = d03;
    m2.m[1][0] = d10;
    m2.m[1][1] = d11;
    m2.m[1][2] = d12;
    m2.m[1][3] = d13;
    m2.m[2][0] = d20;
    m2.m[2][1] = d21;
    m2.m[2][2] = d22;
    m2.m[2][3] = d23;
    m2.m[3][0] = d30;
    m2.m[3][1] = d31;
    m2.m[3][2] = d32;
    m2.m[3][3] = d33;
    return m2;
}

Matrix Matrix::operator * (const Matrix &m2) const {
    Matrix r;
    
    r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
    r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
    r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
    r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];
    
    r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
    r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
    r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
    r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];
    
    r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
    r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
    r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
    r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];
    
    r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
    r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
    r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
    r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];
    
    return r;
}

void Matrix::setPosition(float x, float y, float z) {
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
}

void Matrix::Translate(float x, float y, float z) {
    Matrix transMatrix;
    transMatrix.setPosition(x, y, z);
    (*this) = transMatrix * (*this);
}

void Matrix::setRotation(float rotation) {
    setRoll(rotation);
}

void Matrix::setRoll(float roll) {
    m[0][0] = cos(roll);
    m[1][0] = -sin(roll);
    m[0][1] = sin(roll);
    m[1][1] = cos(roll);
}

void Matrix::Rotate(float rotation) {
    Roll(rotation);
}

void Matrix::Roll(float roll) {
    Matrix rollMatrix;
    rollMatrix.setRoll(roll);
    (*this) = rollMatrix * (*this);
}

void Matrix::setPitch(float pitch) {
    m[1][1] = cos(pitch);
    m[2][1] = -sin(pitch);
    m[1][2] = sin(pitch);
    m[2][2] = cos(pitch);
}

void Matrix::setYaw(float yaw) {
    m[0][0] = cos(yaw);
    m[2][0] = sin(yaw);
    m[0][2] = -sin(yaw);
    m[2][2] = cos(yaw);
}

void Matrix::Pitch(float pitch) {
    Matrix pitchMatrix;
    pitchMatrix.setPitch(pitch);
    (*this) = pitchMatrix * (*this);
}

void Matrix::Yaw(float yaw) {
    Matrix yawMatrix;
    yawMatrix.setYaw(yaw);
    (*this) = yawMatrix * (*this);
}

void Matrix::setScale(float x, float y, float z) {
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;
}

void Matrix::Scale(float x, float y, float z) {
    Matrix scaleMatrix;
    scaleMatrix.setScale(x, y, z);
    (*this) = scaleMatrix * (*this);
}

void Matrix::setOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar) {
    m[0][0] = 2.0f/(right-left);
    m[1][1] = 2.0f/(top-bottom);
    m[2][2] = -2.0f/(zFar-zNear);

    m[3][0] = -((right+left)/(right-left));
    m[3][1] = -((top+bottom)/(top-bottom));
    m[3][2] = -((zFar+zNear)/(zFar-zNear));
}

void Matrix::setPerspectiveProjection(float fov, float aspect, float zNear, float zFar) {    
    m[0][0] = 1.0f/tanf(fov/2.0)/aspect;
    m[1][1] = 1.0f/tanf(fov/2.0);
    m[2][2] = (zFar+zNear)/(zNear-zFar);
    m[3][2] = (2.0f*zFar*zNear)/(zNear-zFar);
    m[2][3] = -1.0f;
    m[3][3] = 0.0f;
}