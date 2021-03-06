#pragma once

#include <boost/statechart/transition.hpp>

namespace smacc
{
class ISmaccState;
class ISmaccStateMachine;
class ISmaccClient;
class ISmaccUpdatable;
class ISmaccComponent;
class SmaccClientBehavior;
class SignalDetector;

class LogicUnit;

namespace client_bases
{
class ISmaccActionClient;
class ISmaccSubscriber;
} // namespace client_bases

namespace introspection
{
class SmaccStateMachineInfo;
class SmaccStateInfo;
} // namespace introspection

// ----TAGS FOR TRANSITIONS -----

namespace default_transition_tags
{

// you can also use these other labels in order to have
// a better code readability and also to improve the visual representation
// in the viewer
struct DEFAULT
{
};

struct ABORT
{
};
struct SUCCESS
{
};
struct PREEMPT
{
};
struct REJECT
{
};

struct CONTINUELOOP
{
};
struct ENDLOOP
{
};

struct default_transition_name : SUCCESS
{
};
} // namespace default_transition_tags

template <class Event,
          class Destination,
          typename Tag = default_transition_tags::default_transition_name,
          class TransitionContext = boost::statechart::detail::no_context<Event>,
          void (TransitionContext::*pTransitionAction)(const Event &) =
              &boost::statechart::detail::no_context<Event>::no_function>
class Transition;

} // namespace smacc
