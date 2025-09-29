Try running make in this directory.<br>

If it doesn't work/it cannot find the directory then do the following:<br>
**NOTE if any of the below steps fail, go to IF STILL DOESN'T WORK section, do not continue running the next commands NOTE**<br>
cd WiringPi<br>
rm wiringpi_3.2_armhf.deb<br>
./build debian<br>
mv wiringpi_3.2_armhf.deb .<br>
sudo apt install ./wiringpi_3.2_armhf.deb<br>
<br>
<br>
<br>
IF IT STILL DOESN'T WORK<br>
return to firware directory<br>
rm -r WiringPi<br>
git clone https://github.com/WiringPi/WiringPi.git <br>
cd WiringPi<br>
./build debian<br>
mv debian-template/wiringpi_3.2_armhf.deb .<br>
sudo apt install ./wiringpi_3.2_armhf.deb <br>
