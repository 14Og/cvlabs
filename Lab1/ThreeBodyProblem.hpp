#ifndef LAB1_NEWTHREEBODYPROBLEM
#define LAB1_NEWTHREEBODYPROBLEM

#include "CoreHeaders.hpp"

#include <array>
#include <string>

namespace ThreeBP {

static constexpr auto kStep			   = 0.1;
static constexpr auto kDistanceTresh   = 50;
static constexpr auto kGravityConstant = 6.67430e4;
static constexpr auto kFrames		   = 10000;
static constexpr auto kTrajectoryTrace = 1000;
static constexpr auto kDelayMs		   = 10;

struct Body {
	static constexpr auto kSize		 = 5; // circle size on canvas
	static constexpr auto kSoftening = 80;
	float mass{0.};
	cv::Point2f pos{0., 0.};
	cv::Point2f vel{0., 0.};
	cv::Scalar color{0, 0, 0};

	Body(float aMass, cv::Point2f aPos, cv::Point2f aVel, cv::Scalar aColor):
		mass(aMass), pos(aPos), vel(aVel), color(aColor)
	{
	}
	Body()						  = delete;
	Body(const Body &)			  = default;
	Body &operator=(const Body &) = default;

	cv::Point2f calculateAccel(const Body &aAffector1, const Body &aAffector2)
	{
		auto distanceVector1 = cv::Vec2f(aAffector1.pos.x - pos.x, aAffector1.pos.y - pos.y);
		auto distanceVector2 = cv::Vec2f(aAffector2.pos.x - pos.x, aAffector2.pos.y - pos.y);

		auto distance1		 = cv::norm(aAffector1.pos - pos) + kSoftening;
		auto distance2		 = cv::norm(aAffector2.pos - pos) + kSoftening;

		distanceVector1 /= std::pow(distance1, 3);
		distanceVector2 /= std::pow(distance2, 3);
		return kGravityConstant * (aAffector1.mass * distanceVector1 + aAffector2.mass * distanceVector2);
	}
};

class RungeKutta4Runner {
	static constexpr auto kX = 1280;
	static constexpr auto kY = 720;
	using Bodies			 = std::array<Body, 3>;
	using BodiesPoints		 = std::array<cv::Point2f, 3>;
	using Trajectories		 = std::array<std::vector<cv::Point2f>, 3>;

private:
	RungeKutta4Runner()										= delete;
	RungeKutta4Runner(const RungeKutta4Runner &)			= delete;
	RungeKutta4Runner &operator=(const RungeKutta4Runner &) = delete;
	RungeKutta4Runner(std::string aCanvas):
		canvas(cv::imread(Defines::resourcesPath / aCanvas)),
		bodies{
			// clang-format off
            Body(4., {200.,200.}, {0., 1.}, {255,0,0}),
            Body(4., {1200.,200.}, {1., 0.}, {0,255,0}),
            Body(4., {700.,600.}, {-1., -1.}, {0,0,255})
			// clang-format on
		}
	{
		cv::resize(canvas, canvas, {kX, kY});
	}

public:
	static inline void animate()
	{
		RungeKutta4Runner runner("white_background.jpg");
		runner.emit();
		cv::destroyAllWindows();
	}

private:
	void step()
	{
		BodiesPoints k1Pos, k2Pos, k3Pos, k4Pos;
		BodiesPoints k1Vel, k2Vel, k3Vel, k4Vel;
		Bodies tempBodies = bodies;

		// K1
		for (size_t i = 0; i < bodies.size(); ++i) {
			k1Pos[i] = bodies[i].vel;
			k1Vel[i] = bodies[i].calculateAccel(bodies[(i + 1) % 3], bodies[(i + 2) % 3]);
		}

		// K2
		for (size_t i = 0; i < bodies.size(); ++i) {
			tempBodies[i].pos += 0.5 * k1Pos[i] * kStep;
			tempBodies[i].vel += 0.5 * k1Vel[i] * kStep; // change bodies
			k2Pos[i] = tempBodies[i].vel;
		}

		for (size_t i = 0; i < bodies.size(); ++i) {
			k2Vel[i] = tempBodies[i].calculateAccel(tempBodies[(i + 1) % 3], tempBodies[(i + 2) % 3]);
		}

		// K3
		tempBodies = bodies;
		for (size_t i = 0; i < bodies.size(); ++i) {
			tempBodies[i].pos += 0.5 * k2Pos[i] * kStep;
			tempBodies[i].vel += 0.5 * k2Vel[i] * kStep;
			k3Pos[i] = tempBodies[i].vel;
		}

		for (size_t i = 0; i < bodies.size(); ++i) {
			k3Vel[i] = tempBodies[i].calculateAccel(tempBodies[(i + 1) % 3], tempBodies[(i + 2) % 3]);
		}

		// K4
		tempBodies = bodies;
		for (size_t i = 0; i < bodies.size(); ++i) {
			tempBodies[i].pos += k3Pos[i] * kStep;
			tempBodies[i].vel += k3Vel[i] * kStep;
			k4Pos[i] = tempBodies[i].vel;
		}

		for (size_t i = 0; i < bodies.size(); ++i) {
			k4Vel[i] = tempBodies[i].calculateAccel(tempBodies[(i + 1) % 3], tempBodies[(i + 2) % 3]);
		}

		// Recalculating bodies params
		for (size_t i = 0; i < bodies.size(); ++i) {
			bodies[i].pos += (kStep / 6) * (k1Pos[i] + 2 * k2Pos[i] + 2 * k3Pos[i] + k4Pos[i]);
			bodies[i].vel += (kStep / 6) * (k1Vel[i] + 2 * k2Vel[i] + 2 * k3Vel[i] + k4Vel[i]);
		}
	}

	void putBodies()
	{
		for (const auto &body : bodies) {
			cv::circle(canvas, body.pos, body.kSize, body.color, -1);
		}
	}

	void putPositionsText(int aFrameNum)
	{
		cv::putText(canvas, "Frame " + std::to_string(aFrameNum), {20, 30}, cv::FONT_HERSHEY_SIMPLEX, 0.5, {0, 0, 0}, 1,
			cv::LINE_AA);
		for (size_t i = 0; i < bodies.size(); ++i) {
			std::string message;
			message += "Body #" + std::to_string(i) + ": " + std::to_string(static_cast<int>(bodies[i].pos.x)) + ", "
				+ std::to_string(static_cast<int>(bodies[i].pos.y));
			cv::putText(canvas, message, {20, 30 + 30 * (static_cast<int>(i) + 1)}, cv::FONT_HERSHEY_SIMPLEX, 0.5,
				{0, 0, 0}, 1, cv::LINE_AA);
		}
	}

	void putTrajectories()
	{
		for (size_t i = 0; i < bodies.size(); ++i)
			for (auto pointIt = trajectories[i].begin(); pointIt != std::prev(trajectories[i].end()); ++pointIt)
				cv::line(canvas, *pointIt, *(pointIt + 1), bodies[i].color);
	}

	void emit()
	{
		auto originalCanvas = canvas.clone();
		for (size_t frame = 0; frame < kFrames; ++frame) {
			canvas = originalCanvas.clone();
			for (size_t i = 0; i < bodies.size(); ++i) {
				trajectories[i].push_back(bodies[i].pos);
			}
			step();
			putBodies();
			putPositionsText(frame);
			putTrajectories();
			cv::imshow("Three Body Problem", canvas);
			if (cv::waitKey(kDelayMs) == Defines::kQuitKey)
				break;
		}
	}

private:
	cv::Mat canvas;
	Bodies bodies;
	Trajectories trajectories;
};
} // namespace ThreeBP

#endif /* LAB1_NEWTHREEBODYPROBLEM */
