import rclpy
from rclpy.node import Node
import cv2
from cv_bridge import CvBridge
from sensor_msgs.msg import Image

class RoverCamera(Node):
    def __init__(self):
        super().__init__('rover_camera')
        self.publisher = self.create_publisher(Image, 'camera/image', 10)
        self.bridge = CvBridge()
        self.cap = cv2.VideoCapture(0)

    def timer_callback(self):
        ret, frame = self.cap.read()
        if ret:
            img_msg = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')
            self.publisher.publish(img_msg)

def main(args=None):
    rclpy.init(args=args)
    camera_node = RoverCamera()

    timer_period = 0.1  # seconds
    camera_node.create_timer(timer_period, camera_node.timer_callback)

    try:
        rclpy.spin(camera_node)
    except KeyboardInterrupt:
        pass

    camera_node.destroy_node()
    camera_node.cap.release()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
