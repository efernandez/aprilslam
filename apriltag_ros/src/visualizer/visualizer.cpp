#include "apriltag_ros/visualizer/visualizer.h"

namespace apriltag_ros {

void ApriltagVisualizer::PublishApriltagsMarker(
    const apriltag_ros::Apriltags& apriltags) {
  static std::set<int> old_ids;
  const std::vector<Apriltag>& tags = apriltags.apriltags;

  // Get current ids
  std::set<int> new_ids;
  std::for_each(tags.begin(), tags.end(),
                [&](const Apriltag& tag) { new_ids.insert(tag.id); });

  // Get union of new and old ids
  std::set<int> union_ids;
  std::set_union(old_ids.begin(), old_ids.end(), new_ids.begin(), new_ids.end(),
                 std::inserter(union_ids, union_ids.begin()));

  // Difference of new and union should be delete
  std::set<int> del_ids;
  std::set_difference(union_ids.begin(), union_ids.end(), new_ids.begin(),
                      new_ids.end(), std::inserter(del_ids, del_ids.begin()));

  // Add and delete markers
  visualization_msgs::MarkerArray marker_array;
  for (const apriltag_ros::Apriltag& tag : apriltags.apriltags) {
    visualization_msgs::Marker marker;
    marker.header = apriltags.header;
    marker.ns = tag.family;
    marker.id = tag.id;
    marker.type = visualization_msgs::Marker::CUBE;
    marker.action = visualization_msgs::Marker::ADD;
    marker.scale.x = marker.scale.y = tag.size;
    marker.scale.z = marker.scale.x / 10;
    marker.color.r = marker.color.a = 0.5;
    marker.pose = tag.pose;
    marker_array.markers.push_back(marker);
  }

  for (int id : del_ids) {
    visualization_msgs::Marker marker;
    marker.header = apriltags.header;
    marker.ns = "36h11";  // super hacky
    marker.id = id;
    marker.action = visualization_msgs::Marker::DELETE;
    marker_array.markers.push_back(marker);
  }

  pub_markers_.publish(marker_array);
  old_ids = new_ids;
}

}  // namespace apriltag_ros