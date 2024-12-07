#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <vector>

using body_state = cv::Point2d;
using body_state_vector = std::vector<body_state>;

template <size_t bodies_num>
class Body {
    static constexpr size_t kBodiesNum = bodies_num;
    static constexpr double G { 6.671e-1 };

private:
    size_t id;

public:
    cv::Point2d position { 0 }; // Позиция (x, y)
    cv::Point2d velocity { 0 }; // Скорость (vx, vy)
    double mass { 0 }; // Масса тела

    Body(size_t body_id, cv::Point2d pos, cv::Point2d vel, double m)
        : id(body_id)
        , position(pos)
        , velocity(vel)
        , mass(m)
    {
    }

    body_state compute_accel(const std::array<Body, kBodiesNum>& bodies);

    bool is_equal(const Body<bodies_num>& body) const
    {
        if (id == body.id) {
            return true;
        }
        return false;
    }
};

template <size_t bodies_num>
body_state Body<bodies_num>::compute_accel(const std::array<Body, kBodiesNum>& bodies)
{
    body_state accel { 0, 0 };

    for (const Body<bodies_num>& body : bodies) {
        if (!body.is_equal(*this)) {
            cv::Point2d direction = body.position - position;
            double distance = sqrt(direction.x * direction.x + direction.y * direction.y);
            accel += G * body.mass * direction / pow(distance, 3);
        }
    }

    return accel;
}

template <size_t bodies_num>
std::array<body_state_vector, bodies_num> rk4_run(const double h, const size_t n, std::array<Body<bodies_num>, bodies_num>& bodies)
{
    std::array<cv::Point2d, bodies_num> k1_pos, k2_pos, k3_pos, k4_pos;
    std::array<cv::Point2d, bodies_num> k1_vel, k2_vel, k3_vel, k4_vel;

    body_state_vector rA(n);
    body_state_vector rB(n);
    body_state_vector rC(n);

    rA[0] = bodies[0].position;
    rB[0] = bodies[1].position;
    rC[0] = bodies[2].position;

    body_state_vector vA(n);
    body_state_vector vB(n);
    body_state_vector vC(n);

    vA[0] = bodies[0].velocity;
    vB[0] = bodies[1].velocity;
    vC[0] = bodies[2].velocity;

    for (int step = 1; step < n; ++step) {
        // Вычисляем k1
        for (size_t i = 0; i < bodies.size(); ++i) {
            k1_pos[i] = bodies[i].velocity;
            k1_vel[i] = bodies[i].compute_accel(bodies);
        }

        // Вычисляем k2
        for (size_t i = 0; i < bodies.size(); ++i) {
            Body temp_body = bodies[i];
            temp_body.position += 0.5 * k1_pos[i] * h;
            temp_body.velocity += 0.5 * k1_vel[i] * h;
            k2_pos[i] = temp_body.velocity;
            k2_vel[i] = temp_body.compute_accel(bodies);
        }

        // Вычисляем k3
        for (size_t i = 0; i < bodies.size(); ++i) {
            Body temp_body = bodies[i];
            temp_body.position += 0.5 * k2_pos[i] * h;
            temp_body.velocity += 0.5 * k2_vel[i] * h;
            k3_pos[i] = temp_body.velocity;
            k3_vel[i] = temp_body.compute_accel(bodies);
        }

        // Вычисляем k4
        for (size_t i = 0; i < bodies.size(); ++i) {
            Body temp_body = bodies[i];
            temp_body.position += k3_pos[i] * h;
            temp_body.velocity += k3_vel[i] * h;
            k4_pos[i] = temp_body.velocity;
            k4_vel[i] = temp_body.compute_accel(bodies);
        }

        rA[step] = bodies[0].position = (rA.at(step - 1) + (h / 6) * (k1_pos[0] + 2 * k2_pos[0] + 2 * k3_pos[0] + k4_pos[0]));
        rB[step] = bodies[1].position = (rB.at(step - 1) + (h / 6) * (k1_pos[1] + 2 * k2_pos[1] + 2 * k3_pos[1] + k4_pos[1]));
        rC[step] = bodies[2].position = (rC.at(step - 1) + (h / 6) * (k1_pos[2] + 2 * k2_pos[2] + 2 * k3_pos[2] + k4_pos[2]));

        vA[step] = bodies[0].velocity = (vA.at(step - 1) + (h / 6) * (k1_vel[0] + 2 * k2_vel[0] + 2 * k3_vel[0] + k4_vel[0]));
        vB[step] = bodies[1].velocity = (vB.at(step - 1) + (h / 6) * (k1_vel[1] + 2 * k2_vel[1] + 2 * k3_vel[1] + k3_vel[1]));
        vC[step] = bodies[2].velocity = (vC.at(step - 1) + (h / 6) * (k1_vel[2] + 2 * k2_vel[2] + 2 * k3_vel[2] + k4_vel[2]));
    }

    std::array<body_state_vector, bodies_num> final_array = {
        rA,
        rB,
        rC
    };

    return final_array;
}
