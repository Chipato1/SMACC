
#include <memory>
#include <functional>
#include <smacc/smacc_types.h>

namespace smacc
{
namespace introspection
{

struct StateBehaviorInfoEntry
{
    std::function<void(smacc::ISmaccState *)> factoryFunction;
    const std::type_info *behaviorType;
    const std::type_info *orthogonalType;
};

//---------------------------------------------
struct SmaccEventInfo
{
    SmaccEventInfo(std::shared_ptr<TypeInfo> eventType);

    std::string getEventTypeName();

    std::string getEventSourceName();

    std::string getObjectTagName();

    std::string label;

    std::shared_ptr<TypeInfo> eventType;

private:
};

struct SmaccTransitionInfo
{
    SmaccTransitionInfo()
    {
    }

    bool historyNode;
    int index;
    std::shared_ptr<SmaccStateInfo> sourceState;
    std::shared_ptr<SmaccStateInfo> destinyState;

    std::string transitionTag;
    std::string transitionType;
    std::shared_ptr<SmaccEventInfo> eventInfo;

    TypeInfo::Ptr transitionTypeInfo;
};
//---------------------------------------------
struct SmaccLogicUnitInfo
{
    std::shared_ptr<SmaccStateInfo> ownerState;
    std::function<void(smacc::ISmaccState *)> factoryFunction;

    const std::type_info *logicUnitType;
    std::shared_ptr<TypeInfo> objectTagType;
    std::vector<std::shared_ptr<SmaccEventInfo>> sourceEventTypes;
};

enum class SmaccStateType
{
    SUPERSTATE = 2,
    STATE = 1,
    SUPERSTATE_ROUTINE = 1
};

class SmaccStateInfo : public std::enable_shared_from_this<SmaccStateInfo>
{

public:
    typedef std::shared_ptr<SmaccStateInfo> Ptr;

    static std::map<const std::type_info *, std::vector<StateBehaviorInfoEntry>> staticBehaviorInfo;
    static std::map<const std::type_info *, std::vector<SmaccLogicUnitInfo>> logicUnitsInfo;

    int stateIndex_;
    std::string fullStateName;
    std::string demangledStateName;

    std::shared_ptr<SmaccStateMachineInfo> stateMachine_;
    std::shared_ptr<SmaccStateInfo> parentState_;
    std::vector<SmaccTransitionInfo> transitions_;

    std::vector<std::shared_ptr<SmaccStateInfo>> children_;
    int depth_;
    const std::type_info *tid_;

    SmaccStateInfo(const std::type_info *tid, std::shared_ptr<SmaccStateInfo> parentState, std::shared_ptr<SmaccStateMachineInfo> stateMachineInfo);

    SmaccStateType getStateLevel();

    inline int depth() const { return depth_; }

    void getAncestors(std::list<std::shared_ptr<SmaccStateInfo>> &ancestorsList);

    std::string getFullPath();

    template <typename StateType>
    std::shared_ptr<SmaccStateInfo> createChildState();

    template <typename EvType>
    void declareTransition(std::shared_ptr<SmaccStateInfo> &dstState, std::string transitionTag, std::string transitionType, bool history, TypeInfo::Ptr transitionTypeInfo);

    // template <typename EvSource, template <typename> typename EvType>
    // void declareTransition(std::shared_ptr<SmaccStateInfo> &dstState, std::string transitionTag, std::string transitionType, bool history);

    const std::string &toShortName() const;

    std::string getDemangledFullName() const;
};
}
}