import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from sonar_master.sonar_driver import single_measure

class MainNode(Node):
    def __init__(self):
        super().__init__('main_node')
        self.publisher = self.create_publisher(String, 'sonar/data', 10)
        


def main():
    rclpy.init()
    my_node = MainNode()

    while True:
        measurement = single_measure()
        print(measurement)
        ros_message = String()
        ros_message.data = str(measurement)
        print(ros_message.data)
        my_node.publisher.publish(ros_message)
    
    my_node.destroy_node()
    rclpy.shutdown()


