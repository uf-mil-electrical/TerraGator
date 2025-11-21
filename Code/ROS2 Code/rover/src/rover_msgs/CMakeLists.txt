cmake_minimum_required(VERSION 3.8)
project(rover_msgs)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# Generate messages and services
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/MotorCommand.msg"
  "srv/I2CSend.srv"
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()