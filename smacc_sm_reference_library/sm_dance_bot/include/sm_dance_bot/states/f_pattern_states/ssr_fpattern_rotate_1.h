namespace sm_dance_bot
{
namespace f_pattern_states
{
template <typename SS>
struct SsrFPatternRotate1 : smacc::SmaccState<SsrFPatternRotate1<SS>, SS>
{
  typedef SmaccState<SsrFPatternRotate1<SS>, SS> TSsr;
  using TSsr::SmaccState;
  using TSsr::context_type;
  
  typedef smacc::Transition<EvActionSucceeded<ClMoveBaseZ, OrNavigation>, SsrFPatternForward1<SS>> reactions;

  static void onDefinition()
  {
    float angle = 0;
    if (SS::direction() == TDirection::LEFT)
      angle = 90;
    else
      angle = -90;

     TSsr::template static_configure<OrNavigation, CbRotate>(angle);
     TSsr::template static_configure<OrLED, CbLEDOff>();
  }

  void onInitialize()
  {
    
  }
};
}
}