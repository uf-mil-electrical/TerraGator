Repo for parts for the MIL Rover

<img width="394" alt="Old Rover CAD Screenshot" src="https://github.com/amcaleer271/Mars-Rover/assets/93012410/63f4e23b-9c5b-4e58-aedf-c8c5b46f9684">

contact: adammcaleer@ufl.edu

## Project Architecture
<img width="700" alt="Iamge overview" src="https://github.com/kberon/Mars-Rover/assets/115107058/b77744f1-999a-49fe-8464-b7bfb6649720">

### Finished tasks
Firmware has somewhat shifted away from a build system using wiringPI as a HAL for c++ - have moved towards a build system for assembly and C for performance. <br>
The basics of the drivers for PWM are done under the c_drivers directory, and drivers for an IMU and BME280 are available under the main branch now.<br>
We have now also added configurations for sonar in the form of a kernel interrupt - this being done once again in hopes of better performance<br>
<br>
Our high level software team has finished configuring ROS for our external computer nodes, and have dived into the details of ROS through toturials.<br>
<br>
Additionally, the team has dived into the battery monitor board design, and details of the future voltage dividing procedure.<br>
<br>
The team has also spent a great deal of time researching and buying parts. Included in the documentation directory are details on that procedure, especailly for the motor selection<br>
<br>
We encourage you to explore the repo and read the subdirectory README's and the different available documentation

## Bugs
Firmware bugs are as follows: PWM through DMA drivers needs to become a persistant object to capture memory leaks - otherwise causes issues when run multiple times<br>
Our sonar modules work independently, but need some kind of daisychaning or internal delays to prevent interference with one another.

## Set up to run:
1. Load the gpio_interface module for PWM
```
cd /Mars-Rover/firmware/kernel_modules/
sudo insmod gpio_interface.ko
```

## Installation:
1. Clone our repo
```
git clone https://github.com/kberon/Mars-Rover.git
```
2. Instantiate the kernel modules on start up
```
sudo nano /etc/systemd/system/load_custom_modules.service
```
In that file write:
```
[Unit]
Description=Load Custom Sonar Kernel Modules
After=network.target

[Service]
Type=oneshot
ExecStart=/sbin/insmod /home/raspberry/Mars-Rover/firmware/sonar_1/sonar_1.ko
ExecStart=/sbin/insmod /home/raspberry/Mars-Rover/firmware/sonar_2/sonar_2.ko
ExecStart=/sbin/insmod /home/raspberry/Mars-Rover/firmware/sonar_3/sonar_3.ko
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
```
Save the file and run the following commands:
```
sudo systemctl daemon-reload
sudo systemctl enable load_custom_modules.service
```

3. For the sensor setup, we will need to install the WiringPi library
```
cd Mars-Rover/firmware/drivers
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build debian
sudo dpkg -i debian-template/wiringpi_3.10_arm64.deb
```
4. Docker setup
Get the install script:
```
curl -fsSL https://get.docker.com -o get-docker.sh
chmod +x get-docker.sh
```
Remove an existing docker installations with:
```
sudo apt-get purge docker-ce docker-ce-cli containerd.io -y
```
Run the script:
```
./get-docker.sh
```
Fix permissions:
```
sudo usermod -aG docker pi
sudo systemctl unmask docker
sudo chmod 666 /var/run/docker.sock
```
Install docker-compose:
```
pip3 -v install docker-compose
```
Start and reboot Docker:
```
sudo systemctl start docker
sudo init 6
```   
6. To run our camera + sensors executable:
```
sudo apt-get install libgtk-3-dev
sudo apt-get install libopencv-dev
sudo apt install python3-opencv
```
