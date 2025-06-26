#pragma once
#include <core/vector.h>
#include <core/matrix.h>
#include<core/math.h>
#include <iostream>
namespace CGL {
    class Camera {
    public:
        Camera(vec3 eye = vec3(0.0f, 0.0f, 5.0f),
            vec3 target = vec3(0.0f, 0.0f, 0.0f),
            vec3 up = vec3(0.0f, 1.0f, 0.0f),
            float near_plane = 0.01f,
            float far_plane = 100.0f)
            : m_look_from(eye), m_look_to(target), m_up(up),
            m_near(near_plane), m_far(far_plane) {}

        virtual ~Camera() = default;
        Camera(const Camera& other) = default;

        const vec3& get_look_from() const { return m_look_from; }
        void set_look_from(const vec3& val) {
            m_look_from = val;
            update_view_matrix();
        }

        const vec3& get_look_to() const { return m_look_to; }
        void set_look_to(const vec3& val) {
            m_look_to = val;
            update_view_matrix();
        }

        const vec3& get_up() const { return m_up; }
        void set_up(const vec3& val) {
            m_up = val;
            update_view_matrix();
        }

        const mat4& get_view_matrix() const {
            return m_view_matrix;
        }

        const mat4& get_projection_matrix() const {
            return m_projection_matrix;
        }

        virtual mat4 cal_projection_matrix() = 0;
        virtual void move_forward(float delta) = 0;
        float getNearPlane() const { return m_near; }
        float getFarPlane() const { return m_far; }

    protected:
        void update_view_matrix() const {
            m_view_matrix = cgl_look_at(m_look_from, m_look_to, m_up);
        }

    protected:
        vec3 m_look_from;
        vec3 m_look_to;
        vec3 m_up;
        float m_near;
        float m_far;

        mutable mat4 m_view_matrix;
        mutable mat4 m_projection_matrix;
    };

    class PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera(vec3 look_from = vec3(0.0f, 0.0f, 5.0f),
            vec3 look_to = vec3(0.0f, 0.0f, 0.0f),
            vec3 look_up = vec3(0.0f, 1.0f, 0.0f),
            float near_plane = 0.01f,
            float far_plane = 100.0f,
            float field_of_view = 45.0f,
            float aspect_ratio = 1.0f)
            : Camera(look_from, look_to, look_up, near_plane, far_plane),
            m_fovy(field_of_view),
            m_aspect_ratio(aspect_ratio) {
            m_projection_matrix = cal_projection_matrix();
            update_view_matrix();
        }

        PerspectiveCamera(const PerspectiveCamera& other) = default;
        PerspectiveCamera& operator=(const PerspectiveCamera& other) = default;

        virtual ~PerspectiveCamera() = default;

        virtual void move_forward(float delta) override {
            m_look_from.z -= delta * 0.1f;
            update_view_matrix();
        }

        virtual mat4 cal_projection_matrix() override {
            m_projection_matrix = cgl_perspective(m_fovy, m_aspect_ratio, m_near, m_far);
            return m_projection_matrix;
        }

        void set_aspect_ratio(float ratio) {
            m_aspect_ratio = ratio;
            cal_projection_matrix();
        }

        float get_aspect_ratio() const {
            return m_aspect_ratio;
        }

    private:
        float m_fovy;
        float m_aspect_ratio;
    };

} // namespace CGL
