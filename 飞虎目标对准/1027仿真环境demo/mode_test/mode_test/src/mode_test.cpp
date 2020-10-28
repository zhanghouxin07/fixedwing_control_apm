/**
 * @file offb_node.cpp
 * @brief Offboard control example node, written with MAVROS version 0.19.x, PX4 Pro Flight
 * Stack and tested in Gazebo SITL
 */

#include <ros/ros.h>
#include <bits/stdc++.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/AttitudeTarget.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/Thrust.h>
#include "tf/transform_datatypes.h"
#include <tf2/LinearMath/Quaternion.h>

using namespace std;
#define pi 3.141592653
#define P 1.0

mavros_msgs::State current_state;
std_msgs::Float64 e_u, e_v;
bool isdectected = false;

void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
    bool connected = current_state.connected;
    bool armed = current_state.armed;
    std::string mode = current_state.mode;

    ROS_INFO("******** Received state cb *********");
    ROS_INFO("%sconnected to ardupilot", connected ? "" : "Not ");
    ROS_INFO("%sarmed", armed ? "" : "Not ");
    ROS_INFO("%s", mode.c_str());
}
void e_u_cb(const std_msgs::Float64::ConstPtr& msg){
    e_u=*msg;
}
void e_v_cb(const std_msgs::Float64::ConstPtr& msg){
    e_v=*msg;
}
void isdectected_cb(const std_msgs::Bool::ConstPtr& msg){
    isdectected=msg->data;
    if(isdectected)
        ROS_INFO("dectected !");
     else
         ROS_INFO("not dectected !");
        
}
float rad2ang(float input){
    return input*pi/180.0;
}
// pose compute
pair<float,float> pose_compute(std_msgs::Float64 e_u,std_msgs::Float64 e_v){
    float eu=e_u.data,ev=e_v.data;
    pair<float,float> ans=make_pair(eu*P,ev*P);
    return ans;
}
mavros_msgs::AttitudeTarget att_cmd_transfer(float roll,float pitch,float yaw,float thrust){
    mavros_msgs::AttitudeTarget att_cmd;
    tf2::Quaternion quat;
    quat.setRPY(0.0, 0.0, 0.0);

    att_cmd.thrust = thrust;
    att_cmd.body_rate.z = 0.0;
    quat.setRPY(rad2ang(roll), rad2ang(pitch), rad2ang(yaw));
    att_cmd.orientation.x = quat.x();
    att_cmd.orientation.y = quat.y();
    att_cmd.orientation.z = quat.z();
    att_cmd.orientation.w = quat.w();
    return att_cmd;
}
int  cnt = 0, cnt_1000 = 0;
void testCtrl( mavros_msgs::AttitudeTarget &att_cmd) {
            cnt++;
        if(cnt == 100) {
            cnt_1000++;
            cout<<"cnt_1000   "<<cnt_1000<<endl;
            if(cnt_1000 % 2 == 0){
                att_cmd=att_cmd_transfer(0.0, -10.0, 0.0, 0.5);
                // att_cmd.thrust = 0.5;
                // att_cmd.body_rate.z = 0.0;
                // quat.setRPY(rad2ang(0.0), rad2ang(0.0), rad2ang(-20.0));
                // att_cmd.orientation.x = quat.x();
                // att_cmd.orientation.y = quat.y();
                // att_cmd.orientation.z = quat.z();
                // att_cmd.orientation.w = quat.w();
            }
            if(cnt_1000 % 2 == 1){
                att_cmd=att_cmd_transfer(0.0, -10.0, 0.0, 0.5);
                // att_cmd.thrust = 0.5;
                // att_cmd.body_rate.z = -0.0;
                // quat.setRPY(rad2ang(0.0), rad2ang(0.0), rad2ang(20.0));
                // att_cmd.orientation.x = quat.x();
                // att_cmd.orientation.y = quat.y();
                // att_cmd.orientation.z = quat.z();
                // att_cmd.orientation.w = quat.w();
            }
        }
        if(cnt == 100) cnt = 0;
}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "offb_node");
    ros::NodeHandle nh;
    // visual input subscriber
    ros::Subscriber e_u_sub = nh.subscribe<std_msgs::Float64>
            ("/e_u", 10, e_u_cb);
    ros::Subscriber e_v_sub = nh.subscribe<std_msgs::Float64>
            ("/e_v", 10, e_v_cb);
   ros::Subscriber isdectected_sub = nh.subscribe<std_msgs::Bool>
            ("/isdectected", 10, isdectected_cb);
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("/mavros/state", 10, state_cb);
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("/mavros/setpoint_position/local", 10);
    ros::Publisher local_att_pub = nh.advertise<mavros_msgs::AttitudeTarget>
        ("/mavros/setpoint_raw/attitude", 10);
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
            ("/mavros/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
            ("/mavros/set_mode");

    // the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);

    // wait for FCU connection
    ROS_INFO("Waiting for connecting to FCU.");
    while(ros::ok() && !current_state.connected){
        ros::spinOnce();
        rate.sleep();
    }
    ROS_INFO("Successfully connect to FCU.");

    mavros_msgs::AttitudeTarget att_cmd;
    att_cmd.type_mask =
                            mavros_msgs::AttitudeTarget::IGNORE_ROLL_RATE | //;
                            mavros_msgs::AttitudeTarget::IGNORE_PITCH_RATE;
    att_cmd.thrust = 0.8;
//    att_cmd.body_rate.x = 0;
//    att_cmd.body_rate.y = 0;
    att_cmd.body_rate.z = 0.3;
    tf2::Quaternion quat;
    quat.setRPY(0.0, 0.0, 0.0);

    while(ros::ok()){
        // pair<float,float> inp=pose_compute(e_u,e_v);
        // att_cmd=att_cmd_transfer(0.5,inp.first,inp.second,0.0);
        //
       if(isdectected ) {     
            // pair<float,float> inp=pose_compute(e_u,e_v);
            // att_cmd=att_cmd_transfer(0.5,inp.first,inp.second,0.0);
            testCtrl(att_cmd);
            local_att_pub.publish(att_cmd);
       }

       

        ros::spinOnce();
        rate.sleep();
    }

       
    return 0;
}
