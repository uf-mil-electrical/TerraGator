from launch import LaunchDescription 
from launch_ros.actions import Node

# calling a launch files from launch file with get_package_share_directory
# example at: keeeeths hw in sub bringup/launch

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='joy',
            executable='joy_node',
        ),

        Node(
            package='first_auto_drive',
            executable='main_node',
            # output='screen'
        ),
        Node(
            package='first_auto_drive',
            executable='sonar_node',
            #output='screen'
        ),

        Node(
            package='stim300',
            executable='stim300_driver_node',
            # output='screen'
        ),
    ])
