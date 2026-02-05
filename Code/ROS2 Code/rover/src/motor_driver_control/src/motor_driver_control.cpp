#include "rclcpp/rclcpp.hpp"
#include "rover_msgs/msg/motor_command.hpp"
#include "rover_srvs/srv/i2c_send.hpp"
#include "std_msgs/msg/bool.hpp"

using namespace std::chrono_literals;

class MotorDriverNode : public rclcpp::Node
{
public:
    MotorDriverNode() : Node("motor_driver_node")
    {
        // Parameters
        this->declare_parameter("i2c_address", 0x20);
        motor_i2c_addr_ = this->get_parameter("i2c_address").as_int();

        // Create subscriber to motor command topic
        motor_cmd_sub_ = this->create_subscription<rover_msgs::msg::MotorCommand>(
            "/motor/command", 10,
            std::bind(&MotorDriverNode::motorCmdCallback, this, std::placeholders::_1));

        // Create subscriber to safety stop
        safety_stop_sub_ = this->create_subscription<std_msgs::msg::Bool>(
            "/safety/stop", 10,
            std::bind(&MotorDriverNode::safetyStopCallback, this, std::placeholders::_1));

        // Create client for I2C service
        i2c_client_ = this->create_client<rover_srvs::srv::I2CSend>("/i2c_send");

        RCLCPP_INFO(this->get_logger(), "Motor Driver Node initialized.");
    }

private:
    void motorCmdCallback(const rover_msgs::msg::MotorCommand::SharedPtr msg)
    {
        if (safety_engaged_)
        {
            RCLCPP_WARN(this->get_logger(), "Ignoring motor command — safety stop active.");
            return;
        }

        // Build data packet: [left_speed, right_speed, direction]
        std::vector<uint8_t> data = {
            static_cast<uint8_t>(msg->left_speed),
            static_cast<uint8_t>(msg->right_speed),
            static_cast<uint8_t>(msg->direction)
        };

        sendI2CCommand(data);
    }

    void safetyStopCallback(const std_msgs::msg::Bool::SharedPtr msg)
    {
        if (msg->data)
        {
            safety_engaged_ = true;
            RCLCPP_WARN(this->get_logger(), "Safety stop engaged! Sending stop command...");

            std::vector<uint8_t> stop_data = {0, 0, 2}; // 2 = brake
            sendI2CCommand(stop_data);
        }
        else
        {
            safety_engaged_ = false;
            RCLCPP_INFO(this->get_logger(), "Safety stop released.");
        }
    }

    void sendI2CCommand(const std::vector<uint8_t>& data)
    {
        // Wait for I2C service to become available
        if (!i2c_client_->wait_for_service(500ms))
        {
            RCLCPP_ERROR(this->get_logger(), "I2C service not available!");
            return;
        }

        auto request = std::make_shared<rover_srvs::srv::I2CSend::Request>();
        request->address = motor_i2c_addr_;
        request->data = data;

        auto result_future = i2c_client_->async_send_request(request);

        // Optionally, block for short duration
        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), result_future, 200ms)
            == rclcpp::FutureReturnCode::SUCCESS)
        {
            auto response = result_future.get();
            if (response->success)
                RCLCPP_INFO(this->get_logger(), "Motor command sent via I2C.");
            else
                RCLCPP_ERROR(this->get_logger(), "I2C transmission failed.");
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "Timeout waiting for I2C service response.");
        }
    }

    // Node components
    rclcpp::Subscription<rover_msgs::msg::MotorCommand>::SharedPtr motor_cmd_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr safety_stop_sub_;
    rclcpp::Client<rover_srvs::srv::I2CSend>::SharedPtr i2c_client_;

    int motor_i2c_addr_;
    bool safety_engaged_ = false;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotorDriverNode>());
    rclcpp::shutdown();
    return 0;
}