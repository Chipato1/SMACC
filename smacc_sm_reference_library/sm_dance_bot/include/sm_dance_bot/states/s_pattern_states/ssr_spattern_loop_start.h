namespace sm_dance_bot
{
namespace s_pattern_states
{
struct SsrSPatternLoopStart : smacc::SmaccState<SsrSPatternLoopStart, SS>
{
  using SmaccState::SmaccState;
  typedef mpl::list<smacc::Transition<EvLoopContinue<SsrSPatternLoopStart>, SsrSPatternRotate1, CONTINUELOOP>> reactions;

  static void onDefinition()
  {
  }

  void onInitialize()
  {
  }

  bool loopCondition()
  {
    auto &superstate = this->context<SS>();
    return ++superstate.iteration_count == superstate.total_iterations();
  }

  void onEntry()
  {
    throwLoopEventFromCondition(&SsrSPatternLoopStart::loopCondition);
  }
};

} // namespace s_pattern_states
} // namespace sm_dance_bot