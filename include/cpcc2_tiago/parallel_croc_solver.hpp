#pragma once

// boost
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/thread/thread_time.hpp>
// cpcc2_tiago
#include <cpcc2_tiago/model_builder.hpp>
#include <cpcc2_tiago/tiago_OCP.hpp>
#include <cpcc2_tiago/utils.hpp>
// auto-generated by generate_parameter_library
#include <cpcc2_tiago_parameters.hpp>

namespace cpcc2_tiago {

class ParallelCrocSolver {
 private:
  pin::Model model_;
  pin::Data data_;

  tiago_OCP::OCP OCP_tiago_;

  std::vector<std::string> joints_names_;

  Eigen::VectorXd x_meas_;
  Eigen::VectorXd us_;
  Eigen::VectorXd xs0_;
  Eigen::VectorXd xs1_;
  Eigen::MatrixXd Ks_;

  CircularVector<20> solving_time_vector_;
  CircularVector<20> solver_freq_vector_;

  boost::interprocess::managed_shared_memory crocoddyl_shm_;

  boost::interprocess::named_mutex mutex_{boost::interprocess::open_or_create,
                                          mutex_name.c_str()};

  shared_vector *x_meas_shm_ = nullptr;
  shared_vector *us_shm_ = nullptr;
  shared_vector *xs0_shm_ = nullptr;
  shared_vector *xs1_shm_ = nullptr;
  shared_vector *Ks_shm_ = nullptr;
  shared_vector *target_shm_ = nullptr;

  shared_string *urdf_xml_ = nullptr;

  bool *solver_started_shm_ = nullptr;
  bool *is_first_update_done_shm_ = nullptr;
  bool *start_sending_cmd_shm_ = nullptr;
  bool *urdf_xml_sent_ = nullptr;

  double *current_t_shm_ = nullptr;

  double last_current_time_ = 0;
  double OCP_time_step_ = 0;
  double OCP_solver_frequency_ = 0;

  pin::FrameIndex lh_id_ = -1;

  int OCP_horizon_length_ = 0;
  int OCP_solver_iterations_ = 0;
  int n_joints_ = 0;

  bool is_first_update_done_ = false;

  /// @brief Read parameters
  void read_params();

  /// @brief Resize vectors
  void resize_vectors();

  /// @brief Create the shared memory
  void init_shared_memory();

  /// @brief get the ROS time in order to be synchronized
  /// @return the ROS time
  double get_ROS_time();

  double read_current_t();

  /// @brief Read x from the shared memory
  /// @return the state as a VectorXd
  Eigen::VectorXd read_controller_x();

  /// @brief Read target from the shared memory
  /// @return the target as a Vector3d
  Eigen::Vector3d read_controller_target();

  /// @brief Send the result of the controller to the shared memory
  void send_controller_result(Eigen::VectorXd us, Eigen::VectorXd xs0,
                              Eigen::VectorXd xs1, Eigen::MatrixXd Ks);

 public:
  ParallelCrocSolver();

  void update();
  void wait();
};

}  // namespace cpcc2_tiago
