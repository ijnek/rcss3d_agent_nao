// Copyright 2022 Kenji Brameld
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <vector>
#include "sim_to_soccer_vision_3d.hpp"
#include "polar_to_point.hpp"
#include "soccer_vision_3d_msgs/msg/ball.hpp"
#include "deg2rad.hpp"

namespace rcss3d_nao
{
namespace sim_to_soccer_vision_3d
{

soccer_vision_3d_msgs::msg::BallArray getBallArray(
  const std::optional<rcss3d_agent_msgs::msg::Ball> & ball)
{
  soccer_vision_3d_msgs::msg::BallArray ballArray;
  ballArray.header.frame_id = "CameraTop_frame";
  if (ball.has_value()) {
    auto ballV = ball.value();
    auto converted = rcss3d_nao::polar_to_point(
      ballV.center.r, deg2rad(ballV.center.phi), deg2rad(ballV.center.theta));

    soccer_vision_3d_msgs::msg::Ball ball;
    ball.center = converted;
    ballArray.balls.push_back(ball);
  }
  return ballArray;
}

soccer_vision_3d_msgs::msg::GoalpostArray getGoalpostArray(
  const std::vector<rcss3d_agent_msgs::msg::Goalpost> & goalposts)
{
  soccer_vision_3d_msgs::msg::GoalpostArray goalpostArray;
  goalpostArray.header.frame_id = "CameraTop_frame";
  for (auto & goalpost : goalposts) {
    auto converted = rcss3d_nao::polar_to_point(
      goalpost.top.r, deg2rad(goalpost.top.phi), deg2rad(goalpost.top.theta));

    soccer_vision_3d_msgs::msg::Goalpost goalpostConverted;
    goalpostConverted.bb.center.position.x = converted.x;
    goalpostConverted.bb.center.position.y = converted.y;
    // Must subtract 0.4m (half of the goalpost height) to get the center's z-coordinate
    // because the top of the goalpost is observed. Note that this calculation doesn't take into
    // account the orientation of the goalpost, but it seems like this is the best we can do for
    // now.
    // 0.7071 - 0.4 = 0.3031m
    goalpostConverted.bb.center.position.z = converted.z - 0.4;
    // Diameter of the goalpost is 0.1m (this is an estimate, based of SPL rules)
    goalpostConverted.bb.size.x = 0.1;
    goalpostConverted.bb.size.y = 0.1;
    // Height of the goalpost is 0.8m
    goalpostConverted.bb.size.z = 0.8;

    goalpostArray.posts.push_back(goalpostConverted);
  }
  return goalpostArray;
}

}  // namespace sim_to_soccer_vision_3d
}  // namespace rcss3d_nao
