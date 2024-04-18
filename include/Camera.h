#ifndef CAMERA_H
#define CAMERA_H

#include <ngl/Mat4.h>
#include <ngl/Vec3.h>

class Camera
{
public:
    Camera();
    void setView(const ngl::Vec3 &eye, const ngl::Vec3 &look, const ngl::Vec3 &up);
    void setPerspective(float fov, float aspect,float near, float far);
    const ngl::Mat4& getViewMatrix() const;
    const ngl::Mat4& getProjectionMatrix() const;

private:
    ngl::Vec3 m_eye, m_look, m_up;
    ngl::Mat4 m_viewMatrix, m_projectionMatrix;
    float m_fov, m_aspect, m_near, m_far;
};

#endif