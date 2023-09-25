#pragma once

// pinocchio
#include <pinocchio/algorithm/parallel/aba.hpp>
// interfaces
#include <controller_interface/controller_interface.hpp>
#include <hardware_interface/loaned_command_interface.hpp>
#include <hardware_interface/types/hardware_interface_type_values.hpp>
// pluginlib
#include <pluginlib/class_list_macros.hpp>
// rosbag2_cpp
#include <rosbag2_cpp/writer.hpp>
// msgs
#include <std_msgs/msg/empty.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <std_msgs/msg/string.hpp>
// cpcc2_tiago
#include <cpcc2_tiago/model_builder.hpp>
#include <cpcc2_tiago/parallel_croc_solver.hpp>
#include <cpcc2_tiago/tiago_OCP.hpp>
#include <cpcc2_tiago/utils.hpp>
#include <cpcc2_tiago/visibility_control.hpp>
// auto-generated by generate_parameter_library
#include <cpcc2_tiago_parameters.hpp>

namespace cpcc2_tiago {

/// @brief Effort Controller (Higher Level Controller) to set reference
/// interfaces received from Chainable Controller
class CrocoddylController : public controller_interface::ControllerInterface {
 public:
  /// @brief Documentation Inherited
  CPCC2_TIAGO_PUBLIC
  controller_interface::CallbackReturn on_init() override;

  /// @brief Documentation Inherited
  CPCC2_TIAGO_PUBLIC
  controller_interface::InterfaceConfiguration command_interface_configuration()
      const override;

  /// @brief Documentation Inherited
  CPCC2_TIAGO_PUBLIC
  controller_interface::InterfaceConfiguration state_interface_configuration()
      const override;

  /// @brief Documentation Inherited
  CPCC2_TIAGO_PUBLIC
  controller_interface::return_type update(
      const rclcpp::Time& time, const rclcpp::Duration& period) override;

  /**
   * Derived controller have to declare parameters in this method.
   * Error handling does not have to be done. It is done in `on_init`-method of
   * this class.
   */
  void declare_parameters();

  /**
   * Derived controller have to read parameters in this method and set
   * `command_interface_types_` variable. The variable is then used to propagate
   * the command interface configuration to controller manager. The method is
   * called from `on_configure`-method of this class.
   *
   * It is expected that error handling of exceptions is done.
   *
   * \returns controller_interface::CallbackReturn::SUCCESS if parameters are
   * successfully read and their values are allowed,
   * controller_interface::CallbackReturn::ERROR otherwise.
   */

  controller_interface::CallbackReturn read_parameters();

 private:
  ParallelCrocSolver pcs_;

  std::shared_ptr<ParamListener> param_listener_;
  Params params_;

  /// @brief all types of state interface, in our case effort, velocity,
  /// position
  std::vector<std::string> state_interface_types_;

  int n_joints_;
  std::vector<std::string> joints_names_;

  rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr
      target_subscriber_;

  // live logging publisher
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr
      end_effect_pos_error_pub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr
      end_effect_pos_pub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr
      effort_command_pub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr
      real_effort_pub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr x_meas_pub_;
  rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr start_pub_;

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr urdf_sub_;

  /// @brief rosbag writer to log data
  std::unique_ptr<rosbag2_cpp::Writer> writer_;

  /// rosbag messages to log data
  std_msgs::msg::Float64MultiArray log_msg_err_;
  std_msgs::msg::Float64MultiArray log_msg_pos_;
  std_msgs::msg::Float64MultiArray log_msg_eff_;
  std_msgs::msg::Float64MultiArray log_msg_real_eff_;
  std_msgs::msg::Float64MultiArray log_msg_x_meas_;

  pin::Model model_;
  pin::Data data_;

  pin::FrameIndex lh_id_;

  rclcpp::Time start_update_time_ = rclcpp::Time(0, 0, RCL_ROS_TIME);
  rclcpp::Time last_update_time_ = rclcpp::Time(0, 0, RCL_ROS_TIME);

  Eigen::VectorXd x_meas_;
  Eigen::VectorXd us_;
  Eigen::VectorXd xs0_;
  Eigen::VectorXd xs1_;
  Eigen::MatrixXd Ks_;

  // is the first crocoddyl controller update done : target set
  bool is_first_update_ = true;

  CircularVector<50> update_freq_vector_;

  /// @brief Current state at time t, overwritten next timestep
  State current_state_;

  Eigen::VectorXd real_effort_;

  Eigen::Vector3d end_effector_target_;
  Eigen::Vector3d end_effector_pos_;

  Eigen::Vector3d pos_error_;

  /// @brief Read the solver results from the shared memory
  void read_solver_results();

  /// @brief Read the actuators state, eff, vel, pos from the hardware
  /// interface
  /// @param current_state the current state to be updated
  State read_state_from_hardware();

  Eigen::VectorXd read_effort_from_hardware();

  /// @brief Callback to update the target from the subscriber
  /// @param msg the message containing the target
  void update_target_from_subscriber(
      const std_msgs::msg::Float64MultiArray::SharedPtr msg);

  /// @brief Set u command to the reference interfaces
  /// @param command_u the command to be set
  void set_u_command(const Eigen::VectorXd& command_u);

  /// @brief Set x0 command to the reference interfaces
  /// @param command_x the command to be set
  void set_x0_command(const Eigen::VectorXd& command_x);

  /// @brief Set x1 command to the reference interfaces
  /// @param command_x the command to be set
  void set_x1_command(const Eigen::VectorXd& command_x);

  /// @brief Set K command to the reference interfaces
  /// @param command_K the command to be set
  void set_K_command(const Eigen::MatrixXd& comman_K);
};

}  // namespace cpcc2_tiago
