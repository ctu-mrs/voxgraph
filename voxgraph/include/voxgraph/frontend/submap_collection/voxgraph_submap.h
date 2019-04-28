//
// Created by victor on 08.01.19.
//

#ifndef VOXGRAPH_FRONTEND_SUBMAP_COLLECTION_VOXGRAPH_SUBMAP_H_
#define VOXGRAPH_FRONTEND_SUBMAP_COLLECTION_VOXGRAPH_SUBMAP_H_

#include <cblox/core/tsdf_esdf_submap.h>
#include <ros/ros.h>
#include <map>
#include <memory>
#include <vector>
#include "voxgraph/frontend/submap_collection/bounding_box.h"

namespace voxgraph {
class VoxgraphSubmap : public cblox::TsdfEsdfSubmap {
 public:
  typedef std::shared_ptr<VoxgraphSubmap> Ptr;
  typedef std::shared_ptr<const VoxgraphSubmap> ConstPtr;

  struct Config : cblox::TsdfEsdfSubmap::Config {
    // TODO(victorr): Read these from ROS params
    double min_voxel_weight = 1e-1;
    double max_voxel_distance = 0.5;
  };

  VoxgraphSubmap(const voxblox::Transformation &T_M_S,
                 const cblox::SubmapID &submap_id, const Config &config);

  // Create a VoxgraphSubmap based on a COPY of a TsdfLayer
  VoxgraphSubmap(const voxblox::Transformation &T_M_S,
                 const cblox::SubmapID &submap_id,
                 const voxblox::Layer<voxblox::TsdfVoxel> &tsdf_layer);

  void transformSubmap(const voxblox::Transformation &T_new_old);

  void addPoseToHistory(const ros::Time &timestamp,
                        const voxblox::Transformation &T_world_robot);

  // Indicate that the submap is finished and generate all cached members
  // NOTE: These cached members are mainly used in the registration cost funcs
  void finishSubmap();

  const ros::Time getCreationTime() const;
  const voxblox::HierarchicalIndexMap &getRelevantBlockVoxelIndices() const;
  const unsigned int getNumRelevantVoxels() const;
  const voxblox::Pointcloud &getIsosurfaceVertices() const;
  const unsigned int getNumIsosurfaceVertices() const;

  // Overlap and Bounding Box related methods
  bool overlapsWith(const VoxgraphSubmap &otherSubmap) const;
  const BoundingBox getSubmapFrameSurfaceObb() const;
  const BoundingBox getSubmapFrameSubmapObb() const;
  const BoundingBox getWorldFrameSurfaceAabb() const;
  const BoundingBox getWorldFrameSubmapAabb() const;
  const BoxCornerMatrix getWorldFrameSurfaceObbCorners() const;
  const BoxCornerMatrix getWorldFrameSubmapObbCorners() const;
  const BoxCornerMatrix getWorldFrameSurfaceAabbCorners() const;
  const BoxCornerMatrix getWorldFrameSubmapAabbCorners() const;

 private:
  typedef Eigen::Matrix<voxblox::FloatingPoint, 4, 8> HomogBoxCornerMatrix;

  Config config_;

  // Track whether this submap has been declared finished
  // and all cached values have been generated
  bool finished_ = false;

  // Oriented Bounding Boxes in submap frame
  mutable BoundingBox surface_obb_;  // around the isosurface
  mutable BoundingBox map_obb_;      // around the entire submap

  // Hash map containing the block and voxel indices of the observed voxels
  // that fall within the truncation distance
  voxblox::HierarchicalIndexMap relevant_block_voxel_indices_;
  unsigned int num_relevant_voxels_ = 0;
  void findRelevantVoxelIndices();

  // Vector containing all isosurface vertex coordinates in submap frame
  voxblox::Pointcloud isosurface_vertices_;
  unsigned int num_isosurface_vertices_ = 0;
  void findIsosurfaceVertices();

  // History of how the robot moved through the submap
  std::map<ros::Time, voxblox::Transformation> pose_history_;
};
}  // namespace voxgraph

#endif  // VOXGRAPH_FRONTEND_SUBMAP_COLLECTION_VOXGRAPH_SUBMAP_H_