import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/reptilerover/ReptileRover/NewCode2025/my_joy_listener/install/my_joy_listener'
