from first_auto_drive.wheels.dc_motor_driver import DCMotor

class TankDriveTrain():
    def __init__(self, send):
        # which GPIO pins and PWM channels correspond to each wheel
        '''
        Legend: (channel number, gpio_one, gpio_two)
            TOP-DOWN VIEW OF ROVER
            +-------------------+
            |                   |
         L1 | (0, 5, 6)         | R1 (3, 4, 17)
            |                   |
         L2 | (1, 23, 24)       | R2 (4, 27, 22)
            |                   |
         L3 | (2, 12, 13)       | R3 (5, 25, 16)
            +-------------------+
            HINGES ON THIS SIDE
        '''

        self.send = send

        self.L1 = DCMotor(0, 5, 6, send)
        self.L2 = DCMotor(1, 23, 24, send)
        self.L3 = DCMotor(2, 12, 13, send)
        self.R1 = DCMotor(3, 4, 17, send)
        self.R2 = DCMotor(4, 27, 22, send)
        self.R3 = DCMotor(5, 25, 16, send)

    def stop(self):
        commands = []

        a, b = self.L1.force_break()
        commands.append(a)
        commands.append(b)

        a, b = self.L2.force_break()
        commands.append(a)
        commands.append(b)

        a, b = self.L3.force_break()
        commands.append(a)
        commands.append(b)

        a, b = self.R1.force_break()
        commands.append(a)
        commands.append(b)

        a, b = self.R2.force_break()
        commands.append(a)
        commands.append(b)

        a, b = self.R3.force_break()
        commands.append(a)
        commands.append(b)

        self.send("\n".join(commands))

    def left(self, speed):
        self.L1.set_speed(speed)
        self.L2.set_speed(speed)
        self.L3.set_speed(speed)
        self.R1.set_speed(speed)
        self.R2.set_speed(speed)
        self.R3.set_speed(speed)

        commands = []

        a, b = self.L1.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.L2.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.L3.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.R1.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.R2.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.R3.go_forward()
        commands.append(a)
        commands.append(b)

        self.send("\n".join(commands))

    def right(self, speed):
        self.L1.set_speed(speed)
        self.L2.set_speed(speed)
        self.L3.set_speed(speed)
        self.R1.set_speed(speed)
        self.R2.set_speed(speed)
        self.R3.set_speed(speed)

        commands = []

        a, b = self.L1.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.L2.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.L3.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.R1.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.R2.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.R3.go_backwards()
        commands.append(a)
        commands.append(b)

        self.send("\n".join(commands))

    def forward(self, speed):
        self.L1.set_speed(speed)
        self.L2.set_speed(speed)
        self.L3.set_speed(speed)
        self.R1.set_speed(speed)
        self.R2.set_speed(speed)
        self.R3.set_speed(speed)

        commands = []

        a, b = self.L1.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.L2.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.L3.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.R1.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.R2.go_forward()
        commands.append(a)
        commands.append(b)

        a, b = self.R3.go_forward()
        commands.append(a)
        commands.append(b)

        self.send("\n".join(commands))
    def backward(self, speed):
        self.L1.set_speed(speed)
        self.L2.set_speed(speed)
        self.L3.set_speed(speed)
        self.R1.set_speed(speed)
        self.R2.set_speed(speed)
        self.R3.set_speed(speed)

        commands = []

        a, b = self.L1.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.L2.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.L3.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.R1.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.R2.go_backwards()
        commands.append(a)
        commands.append(b)

        a, b = self.R3.go_backwards()
        commands.append(a)
        commands.append(b)

        self.send("\n".join(commands))


