#include "rclcpp/rclcpp.hpp"
#include "rover_msgs/msg/motor_command.hpp"
#include "std_msgs/msg/bool.hpp"
#include <iostream>

class CLIDebugNode : public rclcpp::Node
{
public:
    CLIDebugNode() : Node("cli_debug_node")
    {
        motor_pub_ = this->create_publisher<rover_msgs::msg::MotorCommand>("/motor/command", 10);
        safety_pub_ = this->create_publisher<std_msgs::msg::Bool>("/safety/stop", 10);

        RCLCPP_INFO(this->get_logger(), "CLI Debug Node ready. Type 'help' for commands.");

        // Run a background thread for input
        input_thread_ = std::thread([this]() { inputLoop(); });
    }

    ~CLIDebugNode()
    {
        input_thread_.join();
    }

private:
    void inputLoop()
    {
        std::string cmd;
        while (rclcpp::ok())
        {
            std::cout << "> ";
            std::getline(std::cin, cmd);

            if (cmd == "help")
            {
                std::cout << "Commands:\n"
                          << "  drive L R D  - Send motor command (L=left%, R=right%, D=dir)\n"
                          << "  stop         - Engage safety stop\n"
                          << "  resume       - Release safety stop\n"
                          << "  quit         - Exit program\n";
            }
            else if (cmd.starts_with("drive"))
            {
                int l, r, d;
                if (sscanf(cmd.c_str(), "drive %d %d %d", &l, &r, &d) == 3)
                {
                    rover_msgs::msg::MotorCommand msg;
                    msg.left_speed = l;
                    msg.right_speed = r;
                    msg.direction = d;
                    motor_pub_->publish(msg);
                    std::cout << "Sent motor command.\n";
                }
                else
                {
                    std::cout << "Usage: drive <left> <right> <dir>\n";
                }
            }
            else if (cmd == "stop")
            {
                std_msgs::msg::Bool msg;
                msg.data = true;
                safety_pub_->publish(msg);
                std::cout << "Safety stop engaged.\n";
            }
            else if (cmd == "resume")
            {
                std_msgs::msg::Bool msg;
                msg.data = false;
                safety_pub_->publish(msg);
                std::cout << "Safety stop released.\n";
            }
            else if (cmd == "quit")
            {
                rclcpp::shutdown();
                break;
            }
            else if (!cmd.empty())
            {
                std::cout << "Unknown command: " << cmd << "\n";
            }
        }
    }

    rclcpp::Publisher<rover_msgs::msg::MotorCommand>::SharedPtr motor_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr safety_pub_;
    std::thread input_thread_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CLIDebugNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}