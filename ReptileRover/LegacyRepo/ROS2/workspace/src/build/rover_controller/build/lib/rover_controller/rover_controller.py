import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from geometry_msgs.msg import Twist

class RoverController(Node):
    def __init__(self):
        super().__init__('rover_controller')
        self.subscription = self.create_subscription(Joy, 'joy', self.joy_callback, 10)
        self.publisher = self.create_publisher(Twist, 'cmd_vel', 10)
        self.autonomous_mode = False

    def joy_callback(self, msg):
        twist = Twist()
        if self.autonomous_mode:
            # Ignore user input in autonomous mode
            return

        # Log the input from the game controller
        self.get_logger().info(f'Controller Input: Axes: {msg.axes}, Buttons: {msg.buttons}')

        twist.linear.x = msg.axes[1]  # Forward/Backward
        twist.angular.z = msg.axes[0] # Left/Right
        self.publisher.publish(twist)

    def toggle_autonomous_mode(self):
        self.autonomous_mode = not self.autonomous_mode

def main(args=None):
    rclpy.init(args=args)
    controller = RoverController()

    try:
        rclpy.spin(controller)
    except KeyboardInterrupt:
        pass

    controller.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
