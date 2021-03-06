#pragma once
#include <smacc/smacc_state.h>
#include <smacc/logic_unit.h>

namespace smacc
{
using namespace smacc::introspection;
using namespace smacc::default_events;

template <class MostDerived,
          class Context,
          class InnerInitial = mpl::list<>,
          sc::history_mode historyMode = sc::has_deep_history>
class SmaccState : public sc::simple_state<
                       MostDerived, Context, InnerInitial, historyMode>,
                   public ISmaccState
{
  typedef sc::simple_state<MostDerived, Context, InnerInitial, historyMode>
      base_type;

public:
  typedef Context TContext;

  bool finishStateThrown;

  //////////////////////////////////////////////////////////////////////////
  struct my_context
  {
    my_context(typename base_type::context_ptr_type pContext) : pContext_(pContext)
    {
    }

    typename base_type::context_ptr_type pContext_;
  };

  SmaccState() = delete;

  virtual ISmaccState *getParentState()
  {
    //auto* ctx = dynamic_cast<ISmaccState*>(this->template context<Context *>());

    return parentState_;
  }

  // Constructor that initializes the state ros node handle
  SmaccState(my_context ctx)
  {
    ROS_WARN_STREAM("creatingState state: " << demangleSymbol(typeid(MostDerived).name()).c_str());
    this->set_context(ctx.pContext_);

    // storing a reference to the parent state
    auto &ps = this->template context<Context>();
    parentState_ = dynamic_cast<ISmaccState *>(&ps);
    finishStateThrown = false;

    this->getStateMachine().notifyOnStateEntryStart(static_cast<MostDerived *>(this));

    ros::NodeHandle contextNh = optionalNodeHandle(ctx.pContext_);

    ROS_DEBUG("context node handle namespace: %s", contextNh.getNamespace().c_str());
    if (contextNh.getNamespace() == "/")
    {
      contextNh = ros::NodeHandle(smacc::utils::cleanShortTypeName(typeid(Context)));
    }

    std::string classname = smacc::utils::cleanShortTypeName(typeid(MostDerived));

    this->nh = ros::NodeHandle(contextNh.getNamespace() + std::string("/") + classname);

    ROS_DEBUG("nodehandle namespace: %s", nh.getNamespace().c_str());

    this->setParam("created", true);

    // before dynamic onInitialize, we execute the onDefinition behavior configurations
    {
      ROS_DEBUG("-- STATIC STATE DESCRIPTION --");

      for (const auto &stateBehaviorsVector : SmaccStateInfo::staticBehaviorInfo)
      {
        ROS_DEBUG_STREAM(" - state info: " << demangleSymbol(stateBehaviorsVector.first->name()));
        for (auto &bhinfo : stateBehaviorsVector.second)
        {
          ROS_DEBUG_STREAM(" - client behavior: " << demangleSymbol(bhinfo.behaviorType->name()));
        }
      }

      const std::type_info *tindex = &(typeid(MostDerived));
      auto &staticDefinedBehaviors = SmaccStateInfo::staticBehaviorInfo[tindex];
      auto &staticDefinedLogicUnits = SmaccStateInfo::logicUnitsInfo[tindex];

      for (auto &bhinfo : staticDefinedBehaviors)
      {
        ROS_INFO_STREAM("- Creating static client behavior: " << demangleSymbol(bhinfo.behaviorType->name()));
        bhinfo.factoryFunction(this);
      }

      for (auto &lu : staticDefinedLogicUnits)
      {
        ROS_INFO_STREAM("- Creating static logic unit: " << demangleSymbol(lu.logicUnitType->name()));
        lu.factoryFunction(this);
      }

      ROS_INFO("---- END STATIC DESCRIPTION");
    }

    static_cast<MostDerived *>(this)->onInitialize();

    //ROS_INFO("Not behavioral State");
    static_cast<MostDerived *>(this)->onEntry();

    // here orthogonals and client behaviors are entered OnEntry
    this->getStateMachine().notifyOnStateEntryEnd(static_cast<MostDerived *>(this));
  }

  typedef typename Context::inner_context_type context_type;
  typedef typename context_type::state_iterator state_iterator;

  InnerInitial *smacc_inner_type;

  std::string getFullPathName()
  {
    auto smInfo = this->getStateMachine().info_;

    auto key = typeid(MostDerived).name();
    if (smInfo->states.count(key))
    {
      return smInfo->states[key]->getFullPath();
    }
    else
    {
      return "UnknownFullPath";
    }
  }

  std::string getFullName()
  {
    return demangleSymbol(typeid(MostDerived).name());
  }

  std::string getShortName()
  {
    return smacc::utils::cleanShortTypeName(typeid(MostDerived));
  }

  virtual ~SmaccState()
  {
    try
    {
      this->getStateMachine().lockStateMachine("state exit");
      auto fullname = demangleSymbol(typeid(MostDerived).name());
      ROS_WARN_STREAM("exiting state: " << fullname);
      this->setParam("destroyed", true);

      this->getStateMachine().notifyOnStateExit(static_cast<MostDerived *>(this));
      ROS_WARN_STREAM("state exit: " << fullname);
    }
    catch (...)
    {
    }
    this->getStateMachine().unlockStateMachine("state exit");
  }

  void throwFinishEvent()
  {
    if (!finishStateThrown)
    {
      auto *finishEvent = new EvStateFinish<MostDerived>();
      finishEvent->state = static_cast<MostDerived *>(this);
      this->postEvent(finishEvent);
      finishStateThrown = true;
    }
  }

public:
  // This method is static-polymorphic because of the curiously recurring template pattern. It
  // calls to the most derived class onEntry method if declared on smacc state construction
  void onInitialize()
  {
  }

  // This method is static-polymorphic because of the curiously recurring template pattern. It
  // calls to the most derived class onEntry method if declared on smacc state construction
  void onEntry()
  {
  }

  // this method is static-polimorphic because of the curiously recurreing pattern. It
  // calls to the most derived class onExit method if declared on smacc state destruction
  void onExit()
  {
  }

  template <typename TOrthogonal, typename TBehavior, typename... Args>
  static void static_configure(Args &&... args)
  {
    auto strorthogonal = demangleSymbol(typeid(TOrthogonal).name());
    auto strbehavior = demangleSymbol(typeid(TBehavior).name());

    ROS_INFO_STREAM("Orthogonal " << strorthogonal << " -> " << strbehavior);

    StateBehaviorInfoEntry bhinfo;
    bhinfo.factoryFunction = [=](ISmaccState *state) {
      //auto bh = std::make_shared<TBehavior>(args...);
      state->configure<TOrthogonal, TBehavior>(args...);
    };

    bhinfo.behaviorType = &(typeid(TBehavior));
    bhinfo.orthogonalType = &(typeid(TOrthogonal));

    const std::type_info *tindex = &(typeid(MostDerived));
    if (!SmaccStateInfo::staticBehaviorInfo.count(tindex))
      SmaccStateInfo::staticBehaviorInfo[tindex] = std::vector<StateBehaviorInfoEntry>();

    SmaccStateInfo::staticBehaviorInfo[tindex].push_back(bhinfo);
  }

  template <typename T>
  bool getGlobalSMData(std::string name, T &ret)
  {
    return base_type::outermost_context().getGlobalSMData(name, ret);
  }

  // Store globally in this state machine. (By value parameter )
  template <typename T>
  void setGlobalSMData(std::string name, T value)
  {
    base_type::outermost_context().setGlobalSMData(name, value);
  }

  template <typename SmaccComponentType>
  void requiresComponent(SmaccComponentType *&storage)
  {
    base_type::outermost_context().requiresComponent(storage);
  }

  virtual ISmaccStateMachine &getStateMachine()
  {
    return base_type::outermost_context();
  }

  template <typename TTransition>
  struct AddLogicUnitEventType
  {
    AddLogicUnitEventType(SmaccLogicUnitInfo &luinfo)
        : luInfo_(luinfo)
    {
    }

    SmaccLogicUnitInfo &luInfo_;
    template <typename T>
    void operator()(T)
    {
      auto sourceType = TypeInfo::getTypeInfoFromTypeid(typeid(T));
      auto evinfo = std::make_shared<SmaccEventInfo>(sourceType);
      EventLabel<T>(evinfo->label);
      luInfo_.sourceEventTypes.push_back(evinfo);
      ROS_INFO_STREAM("event: " << sourceType->getFullName());
      ROS_INFO_STREAM("event parameters: " << sourceType->templateParameters.size());
    }
  };

  template <typename TEventList>
  static void iterateLogicUnitEventTypes(SmaccLogicUnitInfo &luinfo)
  {
    using boost::mpl::_1;
    using wrappedList = typename boost::mpl::transform<TEventList, _1>::type;
    AddLogicUnitEventType<wrappedList> op(luinfo);
    boost::mpl::for_each<wrappedList>(op);
  }

  template <typename TLUnit, typename TTriggerEvent, typename TEventList, typename... TUArgs>
  static void static_createLogicUnit(TUArgs... args)
  {
    SmaccLogicUnitInfo luinfo;

    luinfo.logicUnitType = &typeid(TLUnit);

    std::string eventtypename = typeid(TTriggerEvent).name();
    auto eventType = TypeInfo::getTypeInfoFromString(eventtypename);

    if (eventType->templateParameters.size() > 1)
    {
      luinfo.objectTagType = eventType->templateParameters[1];
    }
    else
    {
      luinfo.objectTagType = nullptr;
    }

    iterateLogicUnitEventTypes<TEventList>(luinfo);

    luinfo.factoryFunction = [&, args...](ISmaccState *state) {
      auto lu = state->createLogicUnit<TLUnit, TTriggerEvent, TEventList>(args...);
      return lu;
    };

    const std::type_info *tindex = &(typeid(MostDerived));
    if (!SmaccStateInfo::logicUnitsInfo.count(tindex))
      SmaccStateInfo::logicUnitsInfo[tindex] = std::vector<SmaccLogicUnitInfo>();

    SmaccStateInfo::logicUnitsInfo[tindex].push_back(luinfo);
  }

  /*template <typename TLUnit, typename TTriggerEvent, typename... TEvArgs>
  static void static_createLogicUnit()
  {
    SmaccLogicUnitInfo luinfo;

    luinfo.logicUnitType = &typeid(TLUnit);

    std::string eventtypename = typeid(TTriggerEvent).name();
    auto eventType = TypeInfo::getTypeInfoFromString(eventtypename);

    if (eventType->templateParameters.size() > 1)
    {
      luinfo.objectTagType = eventType->templateParameters[1];
    }
    else
    {
      luinfo.objectTagType = nullptr;
    }

    walkLogicUnitSources(luinfo, typelist<TEvArgs...>());

    luinfo.factoryFunction = [&](ISmaccState *state) {
      state->createLogicUnit<TLUnit, TTriggerEvent, TEvArgs...>();
    };

    const std::type_info *tindex = &(typeid(MostDerived));
    if (!SmaccStateInfo::logicUnitsInfo.count(tindex))
      SmaccStateInfo::logicUnitsInfo[tindex] = std::vector<SmaccLogicUnitInfo>();

    SmaccStateInfo::logicUnitsInfo[tindex].push_back(luinfo);
  }
*/
  void throwLoopEventFromCondition(bool (MostDerived::*conditionFn)())
  {
    auto *thisobject = static_cast<MostDerived *>(this);
    auto condition = boost::bind(conditionFn, thisobject);
    bool conditionResult = condition();
    //ROS_INFO("LOOP EVENT CONDITION: %d", conditionResult);
    if (conditionResult)
    {
      auto evloopcontinue = new EvLoopContinue<MostDerived>();
      this->postEvent(evloopcontinue);
    }
    else
    {
      auto evloopend = new EvLoopEnd<MostDerived>();
      this->postEvent(evloopend);
    }
    ROS_INFO("POST THROW CONDITION");
  }

  //////////////////////////////////////////////////////////////////////////
  // The following declarations should be private.
  // They are only public because many compilers lack template friends.
  //////////////////////////////////////////////////////////////////////////
  // See base class for documentation
  typedef typename base_type::outermost_context_base_type
      outermost_context_base_type;
  typedef typename base_type::inner_context_ptr_type inner_context_ptr_type;
  typedef typename base_type::context_ptr_type context_ptr_type;
  typedef typename base_type::inner_initial_list inner_initial_list;

  static void initial_deep_construct(
      outermost_context_base_type &outermostContextBase)
  {
    deep_construct(&outermostContextBase, outermostContextBase);
  }

  // See base class for documentation
  static void deep_construct(
      const context_ptr_type &pContext,
      outermost_context_base_type &outermostContextBase)
  {
    const inner_context_ptr_type pInnerContext(
        shallow_construct(pContext, outermostContextBase));
    base_type::template deep_construct_inner<inner_initial_list>(
        pInnerContext, outermostContextBase);
  }

  static inner_context_ptr_type shallow_construct(
      const context_ptr_type &pContext,
      outermost_context_base_type &outermostContextBase)
  {
    const inner_context_ptr_type pInnerContext(

        new MostDerived(
            SmaccState<MostDerived, Context, InnerInitial, historyMode>::

                my_context(pContext)));
    outermostContextBase.add(pInnerContext);
    return pInnerContext;
  }
};
} // namespace smacc