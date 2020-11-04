# fixedwing_control_apm
Ardupilot安装教程
张浩哲

1.克隆源码
git clone https://github.com/ArduPilot/ardupilot.git
cd ardupilot
git submodule update --init --recursive 

2.脚本安装相关功能包
Tools/environment_install/install-prereqs-ubuntu.sh -y
. ~/.profile
之后重启

以上就已经安装了ardupilot，接下来进行SITL测试安装是否成功。

1.	配置SITL
cd ardupilot/ArduCopter
sim_vehicle.py -w //set default parameters
sim_vehicle.py --console --map //start the simulator

2.	启动SITL
cd ~/ardupilot/ArduCopter
../Tools/autotest/sim_vehicle.py --map --console 
此时会打开如下几个窗口:
 
终端依次输入：
mode guided
arm throttle
takeoff 40
此时可以在console、map窗口中观察飞机状态。
使用gazebo还需安装插件，步骤如下：

1.	安装 
git clone https://github.com/khancyr/ardupilot_gazebo
cd ardupilot_gazebo
mkdir build
cd build
cmake ..
make -j4
sudo make install

2.	测试 
Terminal1：
gazebo --verbose worlds/iris_arducopter_runway.world
Terminal2：
cd ~/ardupilot/ArduCopter

3.	../Tools/autotest/sim_vehicle.py -f gazebo-iris --console --map
结果如下：
 
参考资料：
官网：https://ardupilot.org/dev/docs/building-setup-linux.html#building-setup-linux

Tips：

1.源码下载慢试试码云，教程：
https://www.bilibili.com/video/BV1z4411m7AZ?from=search&seid=14886921739850507971

2.安装有错的一般都是原先没安装插件啥的，缺啥装啥就行。另外主要参考官网的教程，不会有错，再有问题翻翻博客都能解决。






