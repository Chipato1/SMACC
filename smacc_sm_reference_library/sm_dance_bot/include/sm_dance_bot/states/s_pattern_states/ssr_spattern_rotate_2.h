namespace sm_dance_bot
{
namespace s_pattern_states
{
struct SsrSPatternRotate2 : smacc::SmaccState<SsrSPatternRotate2, SS>
{
    using SmaccState::SmaccState;

    typedef mpl::list<smacc::Transition<EvActionSucceeded<ClMoveBaseZ, OrNavigation>, SsrSPatternForward2>,
                      smacc::Transition<EvActionAborted<ClMoveBaseZ, OrNavigation>, SsrSPatternForward1>>
        reactions;

    static void onDefinition()
    {
        float angle = 0;
        if (SS::direction() == TDirection::LEFT)
            angle = -90;
        else
            angle = 90;

        static_configure<OrNavigation, CbRotate>(angle);
        static_configure<OrLED, CbLEDOff>();
    }

    void onInitialize()
    {
        auto &superstate = this->context<SS>();
        ROS_INFO("[SsrSPatternRotate] SpatternRotate rotate: SS current iteration: %d/%d", superstate.iteration_count, SS::total_iterations());
    }
};
} // namespace s_pattern_states
} // namespace sm_dance_bot