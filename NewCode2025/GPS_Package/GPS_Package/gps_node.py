import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64MultiArray
from GPS_Package.gps_driver import check_gps
class GPSNode(Node):
    def __init__(self):
        super().__init__('GPS_node')
        self.publisher = self.create_publisher(Float64MultiArray, 'GPS/data', 10)
        self.timer = self.create_timer(1, self.timer_cb)
    def timer_cb(self):
        msg = Float64MultiArray()
        msg.data = check_gps()
        self.publisher.publish(msg)


def main():
    rclpy.init()
    gpsNode = GPSNode()
    rclpy.spin(gpsNode)
    gpsNode.destory_node()
    rclpy.shutdown()
