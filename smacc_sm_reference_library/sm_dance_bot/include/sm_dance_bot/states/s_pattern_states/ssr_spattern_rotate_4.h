namespace sm_dance_bot
{
namespace s_pattern_states
{
struct SsrSPatternRotate4 : smacc::SmaccState<SsrSPatternRotate4, SS>
{
    using SmaccState::SmaccState;

    typedef mpl::list<smacc::Transition<EvActionSucceeded<ClMoveBaseZ, OrNavigation>, SsrSPatternForward4>,
                      smacc::Transition<EvActionAborted<ClMoveBaseZ, OrNavigation>, SsrSPatternForward3>>
        reactions;

    static void onDefinition()
    {
    }

    void onInitialize()
    {
        auto &superstate = this->context<SS>();
        ROS_INFO("[SsrSPatternRotate] SpatternRotate rotate: SS current iteration: %d/%d", superstate.iteration_count, SS::total_iterations());

        float angle = 0;
        if (superstate.direction() == TDirection::LEFT)
            angle = 90;
        else
            angle = -90;

        this->configure<OrNavigation, CbRotate>(angle);
        this->configure<OrLED, CbLEDOff>();
    }
};
} // namespace s_pattern_states
} // namespace sm_dance_bot