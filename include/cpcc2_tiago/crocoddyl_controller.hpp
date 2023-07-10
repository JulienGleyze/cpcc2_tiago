#ifndef CROCODDYL_CONTROLLER_HPP
#define CROCODDYL_CONTROLLER_HPP

// Libraries
#include <algorithm>
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "controller_interface/chainable_controller_interface.hpp"
#include "controller_interface/controller_interface.hpp"
#include "controller_interface/helpers.hpp"
#include "cpcc2_tiago/tiago_OCP_maker.hpp"
#include "cpcc2_tiago/visibility_control.h"
#include "hardware_interface/loaned_command_interface.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "pluginlib/class_list_macros.hpp"
#include "rclcpp/logging.hpp"
#include "rclcpp/qos.hpp"
#include "rclcpp/subscription.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "realtime_tools/realtime_buffer.h"
#include "std_msgs/msg/float64_multi_array.hpp"
// auto-generated by generate_parameter_library
#include "cpcc2_tiago_parameters.hpp"

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
  struct state {
    Eigen::VectorXd position;
    Eigen::VectorXd velocity;
    Eigen::VectorXd effort;
  };

    Model model_;
  tiago_OCP::OCP OCP_tiago_;
  rclcpp::Time prev_solve_time_ = rclcpp::Time(0, 0, RCL_ROS_TIME);
  int approx_solving_t_us_ = 5000;

  Eigen::Vector3d hand_target_ = Eigen::Vector3d(0.8, 0,
                                                 0.8);  // random target

  /// @brief Number of joints
  int n_joints_;

  /// @brief all types of state interface, in our case effort, velocity,
  /// position
  std::vector<std::string> state_interface_types_;

  std::shared_ptr<ParamListener> param_listener_;
  Params params_;

  /// @brief Current state at time t, overwritten next timestep
  state current_state_;

  /// @brief build the pinocchio reduced model
  void build_model();

  /// @brief Read the actuators state, eff, vel, pos from the hardware
  /// interface
  void read_state_from_hardware();

  /// @brief set the effort command
  /// @param interface_command command_interface to send the command to
  /// @param command_eff vector of the desired torque
  void set_u_command(Eigen::VectorXd command_u);
  void set_x_command(Eigen::VectorXd command_x);
  void set_K_command(Eigen::MatrixXd comman_K);
};
}  // namespace cpcc2_tiago
#endif
