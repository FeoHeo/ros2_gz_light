# ros2_gz_light
**Overview**: This file contain a code that will publish a message from a ROS2 topic to control a light in Gazebo world.


## Setting up
First, make sure that you have Gazebo Fortress and ROS2 Humble installed.

Install ROS2: https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debians.html

Install Gazebo: https://gazebosim.org/docs/fortress/install_ubuntu

## Getting Started

Gazebo bridge is a really useful tool if you want to exchange messages between ROS2 and Gazebo

Install ROS-Gazebo bridge (source install recommended): https://github.com/gazebosim/ros_gz/tree/humble


Next, inside the colcon workspace (where you ran *colcon build*)

    cd src
    git clone https://github.com/FeoHeo/ros2_gz_light.git
    cd ..
    colcon build

*Optional*: I recommened using **terminator** to better keep track of your terminals here

    sudo add-apt-repository ppa:gnome-terminator

    sudo apt-get update

    sudo apt-get install terminator

## Running the code
Here I will assume that all your terminal is set to your colcon workspace

Inside terminal 1, run:

    source install/setup.bash
    ros2 run light_control_test light_control

Inside terminal 2, run:

    cd src/light_control/world
    ign gazebo light_test.sdf

Inside terminal 3, run:

    source install/setup.bash
    ros2 run ros_gz_bridge parameter_bridge /world/light_test/light_config@ros_gz_interfaces/msg/Light@ignition.msgs.Light

## Control the light

Then, you try publishing a message to */box_state*

Inside terminal 4, run:

    ros2 topic pub -1 /box_state std_msgs/msg/String "data: 'error'"

The light inside the simulation should turn red (delete -1 if you want to publish continously). Read the code inside */light_control/src/light_controller.cpp* for more colors.

