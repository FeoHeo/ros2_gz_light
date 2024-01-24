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


## Publish from MqttX

You can also publish messages from MqttX to ROS2. And from ROS2, it will transfer the message to Gazebo. MqttX is a topic broker, it will give you a space to better organize messaages and publish message easily, without using *ros2 topic pub* every time.

You can install MqttX from the Ubuntu store (the app store on your sidebar)

Then install mqtt bridge, this is a tool which will provide a bridge between mqttX and ROS2.

    sudo apt install python3-pip
    sudo apt install ros-humble-rosbridge-library
    sudo apt install mosquitto mosquitto-clients

Inside colcon workspace

    cd src
    git clone https://github.com/groove-x/mqtt_bridge.git
    pip3 install -r dev-requirements.txt
    git checkout ros2

Now as you have your files installed, you can configure the topics that you want to pub, sub to.

More details for the bridge is in: https://github.com/groove-x/mqtt_bridge/blob/ros2/README.md 

Navigate to *{WORKSPACE}/src/mqtt_bridge/config/demo_params.yaml*. Then copy-paste this code in.

    mqtt_bridge_node:
    ros__parameters:
        mqtt:
        client:
            protocol: 4      # MQTTv311
        connection:
            host: "localhost"
            port: 1883
            keepalive: 60
        private_path: "device/001"
        serializer: json:dumps
        deserializer: json:loads
        n_bridges: 7
        bridge:
        bridge1: ["mqtt_bridge.bridge:RosToMqttBridge","std_msgs.msg:Bool","/ping","ping"]
        bridge2: ["mqtt_bridge.bridge:MqttToRosBridge","std_msgs.msg:Bool","ping","/pong"]
        bridge3: ["mqtt_bridge.bridge:RosToMqttBridge","std_msgs.msg:String","/echo","echo"]
        bridge4: ["mqtt_bridge.bridge:MqttToRosBridge","std_msgs.msg:String","echo","/back"]
        bridge5: ["mqtt_bridge.bridge:RosToMqttBridge","std_msgs.msg:String","/private/echo","~/echo"]
        bridge6: ["mqtt_bridge.bridge:MqttToRosBridge","std_msgs.msg:String","~/echo", "/private/back"]


        bridge7: ["mqtt_bridge.bridge:MqttToRosBridge","std_msgs.msg:String","test","/box_state"]
        bridge8: ["mqtt_bridge.bridge:RosToMqttBridge","std_msgs.msg:Bool","/server_bridge_msgs/cmd_ack","ack"]

Now run MqttX, click on and **add a new conenction**. It can either a [add connection] button or the plus symbol next to [connections]

Then, change the **Host** to: mqtt:// localhost

change Port to: 1883

Next, **Connect** and add a **New Subscription**. Set the topic to 'test'. From the chat section, change topic to 'test'.

And the content of the chatbox:

    {
        "data": "error"
    }

Run the bridge:

    ros2 launch mqtt_bridge demo.launch.py

Now you should see the topic being published to */box_state*

In another terminal, run:

    ros2 topic echo /box_state

You should see the message being published to the topic. Run the full ROS2 bridge, process and world. As instructed above and when sending message through MqttX, the light in Gazebo should change according to it.

Remember to check for number of connections, connection type in the .yaml file as it is commonly mistaken

The code also publish a topic named *ack* to mqtt_bridge, this will publish **true** whenever it received input(just something I used for debugging)
