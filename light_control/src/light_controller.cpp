


#include <functional>
#include <memory>
#include <iostream>
#include <string>

#include "rclcpp/rclcpp.hpp"

//include the std_msgs
#include "std_msgs/msg/int32.hpp"
#include "std_msgs/msg/color_rgba.hpp" //Why is it so weird? Can't it be ColorRGBA like in ROS1???
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/bool.hpp"

//include ros_bridge msgs to build the light
#include "ros_gz_interfaces/msg/light.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "geometry_msgs/msg/vector3.hpp"
#include "geometry_msgs/msg/point.hpp"
#include "geometry_msgs/msg/quaternion.hpp"


class colorTest :  public rclcpp::Node {
public:
    colorTest() : Node("light_msg_transmit") {
        //Initiate ROS subscriber and publishers
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "box_state" , 10 , std::bind(&colorTest::colorCb , this , std::placeholders::_1)
        );

        publisher_ = this->create_publisher<ros_gz_interfaces::msg::Light>("world/light_test/light_config",10);
        ackPub_ = this->create_publisher<std_msgs::msg::Bool>("server_bridge_msgs/cmd_ack",10);

    }

private:
    void color_control(std::string boxState) {
        geometry_msgs::msg::Pose lightPose;
        geometry_msgs::msg::Point posePoint;
        geometry_msgs::msg::Quaternion poseQuater;
        std_msgs::msg::ColorRGBA colorReturn;
        std_msgs::msg::ColorRGBA specularReturn;
        geometry_msgs::msg::Vector3 directionVector;

        if(boxState == "error") {  
            colorReturn.r = 1.0;
            colorReturn.g = 0.0; //Red
            colorReturn.b = 0.0;

        } else if(boxState == "closed") {
            colorReturn.r = 0.0;
            colorReturn.g = 0.0; //Blue
            colorReturn.b = 1.0; 

        } else if(boxState == "opened") {
            colorReturn.r = 0.0;
            colorReturn.g = 1.0; //Green
            colorReturn.b = 0.0;

        } else if(boxState == "partly_closed") {
            colorReturn.r = 1.0;
            colorReturn.g = 1.0; //White
            colorReturn.b = 1.0;

        } else if(boxState == "partly_opened") {
            colorReturn.r = 1.0;
            colorReturn.g = 1.0; //Yellow
            colorReturn.b = 0.0;

        }
        colorReturn.a = 1.0;


        //Building the light message, this is a ROS node so we don't have access to the light model in ignition
        //However, further suggestions showing how to take model info from iginition and only modifying diffuse is appreaciated
        std::string lightName = "user_spot_light_1";

        posePoint.x = 0 ; posePoint.y = 0 ; posePoint.z = 0;
        poseQuater.x = 0 ; poseQuater.y= 0 ; poseQuater.z=0;
        directionVector.x = 0 ; directionVector.y = 0 ; directionVector.z = -1;
        lightPose.position = posePoint;
        lightPose.orientation = poseQuater;

        specularReturn.r = 0.2; specularReturn.g = 0.2; specularReturn.b = 0.2; specularReturn.a = 1;


        lightReturn.range = 20;
        lightReturn.attenuation_constant = 0.1;
        lightReturn.attenuation_linear = 0.01;
        lightReturn.attenuation_quadratic = 0.001;

        lightReturn.cast_shadows = true;

        lightReturn.spot_inner_angle = 0.6;
        lightReturn.spot_outer_angle = 1;
        lightReturn.spot_falloff = 1;

        //Assign values
        lightReturn.name = lightName;
        lightReturn.pose = lightPose;
        lightReturn.diffuse = colorReturn;
        lightReturn.specular = specularReturn;
        lightReturn.direction = directionVector;
        lightReturn.type = ros_gz_interfaces::msg::Light::SPOT;

        lightReturn.intensity = 20; //This is a MUST have, since default intensity = 0 --> light will be off (I think)


        publisher_->publish(lightReturn);

    }

    void colorCb(const std_msgs::msg::String &msg) {
        RCLCPP_INFO_STREAM(this->get_logger() , "Received: " << msg.data);
        ackPub_.publish();
        color_control(msg.data);        
        
    };

private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    rclcpp::Publisher<ros_gz_interfaces::msg::Light>::SharedPtr publisher_;
    ros_gz_interfaces::msg::Light lightReturn;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ackPub_;
};

int main(int argc , char* argv[]) {
    rclcpp::init(argc , argv);
    rclcpp::spin(std::make_shared<colorTest>());
    rclcpp::shutdown();
    return 0;
}