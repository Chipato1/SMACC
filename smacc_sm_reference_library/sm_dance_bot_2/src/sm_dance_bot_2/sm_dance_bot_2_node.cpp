#include <sm_dance_bot_2/sm_dance_bot_2.h>

int main(int argc, char **argv)
{
    ros::init(argc, argv, "dance_bot_2");
    ros::NodeHandle nh;

    ros::Duration(5).sleep();
    smacc::run<sm_dance_bot_2::SmDanceBot2>();
}
