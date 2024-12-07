#ifndef LAB1_THREEBODYPROBLEM
#define LAB1_THREEBODYPROBLEM

#include "Core.hpp"

#include <array>

using Defines::resourcesPath;

namespace Lab1 {

struct Body {
	static constexpr auto kGravityConstant = 6.67430e4;
	static constexpr auto kSoftening	   = 10;
	static constexpr auto kDistanceThresh  = 50;
	static constexpr auto kX			   = 1280;
	static constexpr auto kY			   = 720;

	float mass{1}; // kg
	cv::Point2f position{0}; // pixel position
	cv::Vec2f acceleration{0}; // m/s^2
	cv::Scalar color{0, 0, 0};
	Body(float aMass, cv::Point2f aPosition, float aAcceleration, cv::Scalar aColor):
		mass(aMass), position(aPosition), acceleration(aAcceleration), color(aColor)
	{
	}
	Body()						  = default;
	Body(const Body &)			  = default;
	Body &operator=(const Body &) = default;

	void recalculateAccel(Body &aFirstAffector, Body &aSecondAffector)
	{
		auto distanceVector1
			= cv::Vec2f(aFirstAffector.position.x - position.x, aFirstAffector.position.y - position.y);
		auto distanceVector2
			= cv::Vec2f(aSecondAffector.position.x - position.x, aSecondAffector.position.y - position.y);

		auto distance1 = cv::norm(aFirstAffector.position - position) + kSoftening;
		auto distance2 = cv::norm(aSecondAffector.position - position) + kSoftening;

		distanceVector1 /= std::pow(distance1, 3);
		distanceVector2 /= std::pow(distance2, 3);

		acceleration
			= kGravityConstant * (aFirstAffector.mass * distanceVector1 + aSecondAffector.mass * distanceVector2);
	}

	void recalculatePosition(int aTimeDelta)
	{
		position.x += acceleration[0] * aTimeDelta * aTimeDelta; // ax * t^2
		position.y += acceleration[1] * aTimeDelta * aTimeDelta; // ay * t^2

		// if (position.x < 0)
		// 	position.x = 100;
		// else if (position.x >= kX)
		// 	position.x = kX - 100;

		// if (position.y < 0)
		// 	position.y = 100;
		// else if (position.y >= kY)
		// 	position.y = kY - 100;
	}
};

class ThreeBodyProblem {

	static constexpr auto kTimeStep		  = 1; // milliseconds
	static constexpr auto kFrames		  = 10000; // frames
	static constexpr auto kTrajectorySize = 1000; // points

public:
	ThreeBodyProblem(const std::string aCanvasFilename): canvas(cv::imread(resourcesPath / aCanvasFilename))
	{
		cv::resize(canvas, canvas, {Body::kX, Body::kY});
		initBodies();
	}

	static inline auto animate()
	{
		auto animationThread = std::make_unique<std::thread>([] {
			ThreeBodyProblem animation("white_background.jpg");
			animation.emit();
		});
		return animationThread;
	}

private:
	void initBodies()
	{
		bodies.at(0) = Body(1, {200, 200}, 0, {255, 0, 0});
		bodies.at(1) = Body(2, {1200, 200}, 0, {0, 255, 0});
		bodies.at(2) = Body(3, {700, 600}, 0, {0, 0, 255});
	}

	void emit()
	{
		auto oldCanvas = canvas.clone();
		for (size_t i = 0; i < kFrames; ++i) {
			canvas = oldCanvas.clone();
			std::cout << "frame: " << i << std::endl;
			for (size_t j = 0; j < bodies.size(); ++j) {
				auto &body = bodies[j];
				body.recalculateAccel(bodies[(j + 1) % 3], bodies[(j + 2) % 3]);
				body.recalculatePosition(kTimeStep);
				if (trajectories[j].size() < kTrajectorySize)
					trajectories[j].push_back(body.position);
				else
					trajectories[j][trajectoryIndex[j]] = body.position;
				auto newIndex	   = (trajectoryIndex[j] + 1) % kTrajectorySize;
				trajectoryIndex[j] = newIndex;
				cv::circle(canvas, body.position, 5, body.color, -1);
				if (trajectories[j].size() > 2) {
				    for (size_t k = 0; k < trajectories[j].size(); ++k) {
				        auto point1 = (trajectoryIndex[j] + k) % kTrajectorySize;
				        auto point2 = (trajectoryIndex[j] + k + 1) % kTrajectorySize;
				        cv::line(canvas, trajectories[j][point1], trajectories[j][point2], bodies[j].color, 1);
				    }
				} // TODO
			}
			cv::imshow("Three Body Problem", canvas);
			if (cv::waitKey(kTimeStep) == Defines::kQuitKey)
				break;
		}
	}

private:
	cv::Mat canvas;
	std::array<Body, 3> bodies;
	std::array<std::vector<cv::Point2f>, 3> trajectories;
	std::array<size_t, 3> trajectoryIndex{0, 0, 0};
};

} // namespace Lab1

#endif /* LAB1_THREEBODYPROBLEM */
