#include "driver_stim300.h"
#include "serial_unix.h"
#include "math.h"
#include "iostream"

// ros related
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "std_srvs/srv/trigger.hpp"
#include "std_srvs/srv/empty.hpp"
#include "std_msgs/msg/header.hpp"

bool calibration_mode{false};
constexpr int NUMBER_OF_CALIBRATION_SAMPLES{100};
constexpr double ACC_TOLERANCE{0.1};
constexpr double MAX_DROPPED_ACC_X_MSG{5};
constexpr double MAX_DROPPED_ACC_Y_MSG{5};
constexpr double MAX_DROPPED_ACC_Z_MSG{5};
constexpr double MAX_DROPPED_GYRO_X_MSG{5};
constexpr double MAX_DROPPED_GYRO_Y_MSG{5};
constexpr double MAX_DROPPED_GYRO_Z_MSG{5};
constexpr double GYRO_X_PEAK_TO_PEAK_NOISE{0.0002};
constexpr double GYRO_Y_PEAK_TO_PEAK_NOISE{0.0002};
constexpr double GYRO_Z_PEAK_TO_PEAK_NOISE{0.0002};

struct Quaternion
{
    double w, x, y, z;
};

struct EulerAngles {
    double roll, pitch, yaw;
};

Quaternion FromRPYToQuaternion(EulerAngles angles) // yaw (Z), pitch (Y), roll (X)
{
    // Abbreviations for the various angular functions
    double cy = cos(angles.yaw * 0.5);
    double sy = sin(angles.yaw * 0.5);
    double cp = cos(angles.pitch * 0.5);
    double sp = sin(angles.pitch * 0.5);
    double cr = cos(angles.roll * 0.5);
    double sr = sin(angles.roll * 0.5);

    Quaternion q;
    q.w = cy * cp * cr + sy * sp * sr;
    q.x = cy * cp * sr - sy * sp * cr;
    q.y = sy * cp * sr + cy * sp * cr;
    q.z = sy * cp * cr - cy * sp * sr;

    return q;
}
void responseCalibrateIMU(
    const std::shared_ptr<rmw_request_id_t> request_header,  // Metadata (we don't use it in this case)
    const std::shared_ptr<std_srvs::srv::Trigger::Request> request,  // The request object (empty for Trigger service)
    std::shared_ptr<std_srvs::srv::Trigger::Response> response) // The response object
{
    (void)request;  // We don't need the request in this case, as Trigger service has no request data

    if (calibration_mode == false)  // Check if the IMU is not in calibration mode
    {
        calibration_mode = true;
        response->message = "IMU in calibration mode";  // Set the response message
        response->success = true;  // Indicate success in the response
    }
    else
    {
        response->message = "IMU is already in calibration mode";  // Handle the case where it's already in calibration mode
        response->success = false;  // Indicate failure in the response
    }

    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Service triggered for calibration.");
}

// create publisher and server-client stuff
void setup(std::shared_ptr<rclcpp::Node> node, rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr& pub, rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr& service){
	pub = node->create_publisher<sensor_msgs::msg::Imu>("imu/data", 10);
	service = node->create_service<std_srvs::srv::Trigger>(
	    "imu_trigger_service", 
	    responseCalibrateIMU
	);
}

int main(int argc, char** argv)
{
  // ros node start
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("stim300_node");

  // TODO replace with get calls
  std::string device_name = "/dev/ttyUSB0"; // CHANGE to absolute name or whatever TODO
  double variance_gyro{0};
  double variance_acc{0};
  int sample_rate{0};
  double gravity{0};

  // node params?
  /*
  node.param<std::string>("device_name", device_name, "/dev/ttyUSB0");
  node.param("variance_gyro", variance_gyro,0.0001*2*4.6*pow(10,-4));
  node.param("variance_acc", variance_acc, 0.000055); 
  node.param("sample_rate", sample_rate, 125);
  node.param("gravity", gravity, 9.80665);
  */

  node->declare_parameter<std::string>("device_name", "/dev/ttyUSB0");
  node->declare_parameter<double>("variance_gyro", 0.0001 * 2 * 4.6 * pow(10, -4));
  node->declare_parameter<double>("variance_acc", 0.000055);
  node->declare_parameter<int>("sample_rate", 1000);
  node->declare_parameter<double>("gravity", 9.80665);

  // These values have been estimated by having beluga in a pool for a couple of minutes, and then calculate the variance for each values
  // JOE DID NOT ESTIMATE THESE
  sensor_msgs::msg::Imu stim300msg;
  stim300msg.angular_velocity_covariance[0] = 0.0000027474;
  stim300msg.angular_velocity_covariance[4] = 0.0000027474;
  stim300msg.angular_velocity_covariance[8] = 0.000007312;
  stim300msg.linear_acceleration_covariance[0] = 0.00041915;
  stim300msg.linear_acceleration_covariance[4] = 0.00041915;
  stim300msg.linear_acceleration_covariance[8] = 0.000018995;
  stim300msg.orientation.x = 0.00000024358;
  stim300msg.orientation.y = 0.00000024358;
  stim300msg.orientation.z = 0.00000024358;
  stim300msg.header.frame_id = "imu_0";

  rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr pub;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr service;

  setup(node, pub, service);

  // old topic for raw data
  //ros::Publisher imuSensorPublisher = node.advertise<sensor_msgs::Imu>("imu/data_raw", 1000);
  //ros::Publisher orientationPublisher = node.advertise<sensor_msgs::Imu>("imu/orientation", 1000);
  // server for calibration
  // old ros::ServiceServer service = node.advertiseService("IMU_calibration",responseCalibrateIMU);


  // New messages are sent from the sensor with sample_rate
  // As loop_rate determines how often we check for new data
  // on the serial buffer, theoretically loop_rate = sample_rate
  // should be okey, but to be sure we double it
  node->get_parameter("sample_rate", sample_rate);
  rclcpp::Rate loop_rate = rclcpp::Rate(sample_rate * 2);

  try {
    node->get_parameter("device_name", device_name);
    SerialUnix serial_driver(device_name, stim_const::BaudRate::BAUD_921600);

    DriverStim300 driver_stim300(serial_driver);

    RCLCPP_INFO(node->get_logger(), "STIM300 IMU driver initialized successfully");

    int difference_in_dataGram{0};
    int count_messages{0};
    int number_of_samples{0};
    double inclination_x{0};
    double inclination_y{0};
    double inclination_z{0};

    double average_calibration_roll{0};
    double average_calibration_pitch{0};
    double inclination_x_calibration_sum{0};
    double inclination_y_calibration_sum{0};
    double inclination_z_calibration_sum{0};
    double inclination_x_average{0};
    double inclination_y_average{0};
    double inclination_z_average{0};

    // Acc wild point filter
    std::vector<double> acceleration_buffer_x{};
    std::vector<double> acceleration_buffer_y{};
    std::vector<double> acceleration_buffer_z{};
    double dropped_acceleration_x_msg{0.0};
    double dropped_acceleration_y_msg{0.0};
    double dropped_acceleration_z_msg{0.0};

    // Gyro wild point filter
    std::vector<double> gyro_buffer_x{};
    std::vector<double> gyro_buffer_y{};
    std::vector<double> gyro_buffer_z{};
    double dropped_gyro_x_msg{0.0};
    double dropped_gyro_y_msg{0.0};
    double dropped_gyro_z_msg{0.0};

    // horrid loop of everything
    while (rclcpp::ok())
    {
      auto temp = driver_stim300.update();

      /*
      switch (temp){
        case Stim300Status::NORMAL:
		std::cout << "NORMAL" << std::endl;
		//std::cout << driver_stim300.printSensorConfig().c_str() << std::endl;
		break;
        case Stim300Status::OUTSIDE_OPERATING_CONDITIONS:
		std::cout << "OUTSIDE_OPERATING_CONDITIONS" << std::endl;
		break;
        case Stim300Status::NEW_MEASURMENT:
		std::cout << "NEW_MEASURMENT" << std::endl;
		break;
        case Stim300Status::CONFIG_CHANGED:
		std::cout << "CONFIG_CHANGED" << std::endl;
		break;
        case Stim300Status::STARTING_SENSOR:
		std::cout << "STARTING_SENSOR" << std::endl;
		break;
        case Stim300Status::SYSTEM_INTEGRITY_ERROR:
		std::cout << "SYSTEM_INTEGRITY_ERROR" << std::endl;
		break;
        case Stim300Status::OVERLOAD:
		std::cout << "OVERLOAD" << std::endl;
		break;
        case Stim300Status::ERROR_IN_MEASUREMENT_CHANNEL:
		std::cout << "ERROR_IN_MEASUREMENT_CHANNEL" << std::endl;
		break;
        case Stim300Status::ERROR:
		std::cout << "ERROR" << std::endl;
		break;
      }
      */

      switch (temp)
      {
        case Stim300Status::NORMAL:
          break;
        case Stim300Status::OUTSIDE_OPERATING_CONDITIONS:
          break;
        case Stim300Status::NEW_MEASURMENT:
	      //RCLCPP_INFO(node->get_logger(), "new meas");
              inclination_x = driver_stim300.getIncX();
              inclination_y = driver_stim300.getIncY();
              inclination_z = driver_stim300.getIncZ();
              Quaternion q;
              EulerAngles RPY;
           if (calibration_mode == true)
            {
              std::cout<<"in calibration_mode"<<std::endl;
                if(number_of_samples < NUMBER_OF_CALIBRATION_SAMPLES)
                {
                    //std::cout<<"in calibration_mode"<<std::endl;
                    number_of_samples++;
                    //std::cout<<number_of_samples<<std::endl;
                    inclination_x_calibration_sum += inclination_x;
                    inclination_y_calibration_sum += inclination_y;
                    inclination_z_calibration_sum += inclination_z;

                }
                else
                {
                    std::cout<<"in else for calib mode"<<std::endl;
                    inclination_x_average = inclination_x_calibration_sum/NUMBER_OF_CALIBRATION_SAMPLES;
                    inclination_y_average = inclination_y_calibration_sum/NUMBER_OF_CALIBRATION_SAMPLES;
                    inclination_z_average = inclination_z_calibration_sum/NUMBER_OF_CALIBRATION_SAMPLES;

                    average_calibration_roll = atan2(inclination_y_average,inclination_z_average);
                    average_calibration_pitch = atan2(-inclination_x_average,sqrt(pow(inclination_y_average,2)+pow(inclination_z_average,2)));
                    std::cout<<average_calibration_roll<<std::endl;
                    std::cout<<average_calibration_pitch<<std::endl;
                    RCLCPP_INFO(node->get_logger(), "roll: %f", average_calibration_roll);
                    RCLCPP_INFO(node->get_logger(), "pitch: %f", average_calibration_pitch);
                    RCLCPP_INFO(node->get_logger(), "IMU Calibrated");
                    calibration_mode = false;
                }
              break;  
            }
            else
            {
                    //std::cout<<"taking meas!!!"<<std::endl;
                    RPY.roll = atan2(inclination_y,inclination_z);
                    RPY.pitch = atan2(-inclination_x,sqrt(pow(inclination_y,2)+pow(inclination_z,2)));
                    q = FromRPYToQuaternion(RPY);

                    // Acceleration wild point filter

                    // Previous message
		    node->get_parameter("gravity", gravity);
                    acceleration_buffer_x.push_back(driver_stim300.getAccX() * gravity);
                    acceleration_buffer_y.push_back(driver_stim300.getAccY() * gravity);
                    acceleration_buffer_z.push_back(driver_stim300.getAccZ() * gravity);
                    stim300msg.header.stamp = node->get_clock()->now();

                    if (acceleration_buffer_x.size() == 2 && acceleration_buffer_y.size() == 2 && acceleration_buffer_z.size() == 2)
                    {
                      if(std::abs(acceleration_buffer_x.back() - acceleration_buffer_x.front()) < ACC_TOLERANCE || dropped_acceleration_x_msg > MAX_DROPPED_ACC_X_MSG )
                      {
                        stim300msg.linear_acceleration.x = acceleration_buffer_x.back();
                        dropped_acceleration_x_msg = 0;
                      }
                      else
                      {
                        RCLCPP_DEBUG(node->get_logger(), "ACC_X_MSG wild point rejected");
                        dropped_acceleration_x_msg +=1;
                      }

                      if(std::abs(acceleration_buffer_y.back() - acceleration_buffer_y.front()) < ACC_TOLERANCE || dropped_acceleration_y_msg > MAX_DROPPED_ACC_Y_MSG )
                      {
                        stim300msg.linear_acceleration.y = acceleration_buffer_y.back();
                        dropped_acceleration_y_msg = 0;
                      }
                      else
                      {
                        RCLCPP_DEBUG(node->get_logger(), "ACC_Y_MSG wild point rejected");
                        dropped_acceleration_y_msg +=1;
                      }

                      if(std::abs(acceleration_buffer_z.back() - acceleration_buffer_z.front()) < ACC_TOLERANCE || dropped_acceleration_z_msg > MAX_DROPPED_ACC_Z_MSG )
                      {
                        stim300msg.linear_acceleration.z = acceleration_buffer_z.back();
                        dropped_acceleration_z_msg = 0;
                      }
                      else
                      {
                        RCLCPP_DEBUG(node->get_logger(), "ACC_Z_MSG wild point rejected");
                        dropped_acceleration_z_msg +=1;
                      }
                      // Empty acceleration buffers
                      acceleration_buffer_x = std::vector<double>{acceleration_buffer_x.back()};
                      acceleration_buffer_y = std::vector<double>{acceleration_buffer_y.back()};
                      acceleration_buffer_z = std::vector<double>{acceleration_buffer_z.back()};
                    }
                    else
                    {
		      node->get_parameter("gravity", gravity);
                      stim300msg.linear_acceleration.x = driver_stim300.getAccX() * gravity;
                      stim300msg.linear_acceleration.y = driver_stim300.getAccY() * gravity;
                      stim300msg.linear_acceleration.z = driver_stim300.getAccZ() * gravity;
                    }

                    // Gyro wild point filter
                    gyro_buffer_x.push_back(driver_stim300.getGyroX());
                    gyro_buffer_y.push_back(driver_stim300.getGyroY());
                    gyro_buffer_z.push_back(driver_stim300.getGyroZ());

                    if(gyro_buffer_x.size() == 2 && gyro_buffer_y.size() == 2 && gyro_buffer_z.size() == 2)
                    {

                      if(std::abs(gyro_buffer_x.back() - gyro_buffer_x.front()) < std::max(2*std::abs(gyro_buffer_x.front()),GYRO_X_PEAK_TO_PEAK_NOISE) || dropped_gyro_x_msg > MAX_DROPPED_GYRO_X_MSG)
                      {
                        stim300msg.angular_velocity.x = gyro_buffer_x.back();
                        dropped_gyro_x_msg = 0;
                      }
                      else
                      {
                        RCLCPP_DEBUG(node->get_logger(), "GYRO_X_MSG wild point rejected");
                        dropped_gyro_x_msg += 1;
                      }

                      if(std::abs(gyro_buffer_y.back() - gyro_buffer_y.front()) < std::max(2*std::abs(gyro_buffer_y.front()),GYRO_Y_PEAK_TO_PEAK_NOISE) || dropped_gyro_y_msg > MAX_DROPPED_GYRO_Y_MSG)
                      {
                        stim300msg.angular_velocity.x = gyro_buffer_y.back();
                        dropped_gyro_y_msg = 0;
                      }
                      else
                      {
                        RCLCPP_DEBUG(node->get_logger(), "GYRO_Y_MSG wild point rejected");
                        dropped_gyro_y_msg += 1;
                      }

                      if(std::abs(gyro_buffer_z.back() - gyro_buffer_z.front()) < std::max(2*std::abs(gyro_buffer_z.front()),GYRO_Z_PEAK_TO_PEAK_NOISE) || dropped_gyro_z_msg > MAX_DROPPED_GYRO_Z_MSG)
                      {
                        stim300msg.angular_velocity.z = gyro_buffer_z.back();
                        dropped_gyro_z_msg = 0;
                      }
                      else
                      {
                        RCLCPP_DEBUG(node->get_logger(), "GYRO_Z_MSG wild point rejected");
                        dropped_gyro_z_msg += 1;
                      }

                      // Empty buffers
                      gyro_buffer_x = std::vector<double>{gyro_buffer_x.back()};
                      gyro_buffer_y = std::vector<double>{gyro_buffer_y.back()};
                      gyro_buffer_z = std::vector<double>{gyro_buffer_z.back()};
                    }
                    else
                    {
                      stim300msg.angular_velocity.x = driver_stim300.getGyroX();
                      stim300msg.angular_velocity.y = driver_stim300.getGyroY();
                      stim300msg.angular_velocity.z = driver_stim300.getGyroZ();
                    }
                    stim300msg.orientation.w = q.w;
                    stim300msg.orientation.x = q.x;
                    stim300msg.orientation.y = q.y;
                    stim300msg.orientation.z = q.z;
		    /*
		    std::cout << "q.w = "<< q.w << std::endl;
		    std::cout << "q.x = "<< q.x << std::endl;
		    std::cout << "q.y = "<< q.y << std::endl;
		    std::cout << "q.z = "<< q.z << std::endl;
		    */
                    pub->publish(stim300msg);
                    break;
            }
        case Stim300Status::CONFIG_CHANGED:
          RCLCPP_INFO(node->get_logger(), "Updated Stim 300 imu config: ");
          RCLCPP_INFO(node->get_logger(), "%s", driver_stim300.printSensorConfig().c_str());
	  RCLCPP_ERROR(node->get_logger(), "Joseph Goodman has no idea how to update a rate in place, maybe if we stick it behind a ptr we can just del the ptr and re-assign??");
          break;
        case Stim300Status::STARTING_SENSOR:
          RCLCPP_INFO(node->get_logger(), "Stim 300 IMU is warming up.");
          break;
        case Stim300Status::SYSTEM_INTEGRITY_ERROR:
          RCLCPP_INFO(node->get_logger(), "Stim 300 IMU system integrity error.");
          break;
        case Stim300Status::OVERLOAD:
          RCLCPP_INFO(node->get_logger(), "Stim 300 IMU overload.");
          break;
        case Stim300Status::ERROR_IN_MEASUREMENT_CHANNEL:
          RCLCPP_INFO(node->get_logger(), "Stim 300 IMU error in measurement channel.");
          break;
        case Stim300Status::ERROR:
          RCLCPP_INFO(node->get_logger(), "Stim 300 IMU: internal error.");
	  break;

      }

      loop_rate.sleep();
      rclcpp::spin_some(node);
    }
    RCLCPP_ERROR(node->get_logger(), "ros not ok");
    return 0;
  } catch (std::runtime_error &error) {
    // TODO: Reset IMU
    RCLCPP_ERROR(node->get_logger(), "%s\n", error.what());
    return 0;
  }
}
