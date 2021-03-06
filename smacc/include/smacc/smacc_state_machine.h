/*****************************************************************************************************************
 * ReelRobotix Inc. - Software License Agreement      Copyright (c) 2018
 * 	 Authors: Pablo Inigo Blasco, Brett Aldrich
 *
 ******************************************************************************************************************/
#pragma once

#include <boost/any.hpp>
#include <map>
#include <mutex>

#include <smacc/common.h>
#include <smacc/introspection/introspection.h>
#include <smacc/introspection/smacc_state_machine_info.h>
#include <smacc/smacc_updatable.h>
#include <smacc/smacc_signal.h>

#include <smacc_msgs/SmaccStateMachine.h>
#include <smacc_msgs/SmaccTransitionLogEntry.h>
#include <smacc_msgs/SmaccStatus.h>
#include <smacc_msgs/SmaccGetTransitionHistory.h>

#include <smacc/smacc_state.h>
#include <smacc/logic_unit.h>

namespace smacc
{

using namespace smacc::introspection;

// This class describes the concept of Smacc State Machine in an abastract way.
// The SmaccStateMachineBase inherits from this state machine and from
// statechart::StateMachine<> (via multiple inheritance)
class ISmaccStateMachine
{
public:
    ISmaccStateMachine(SignalDetector *signalDetector);

    virtual ~ISmaccStateMachine();

    virtual void Reset();

    virtual void Stop();

    virtual void EStop();

    template <typename TOrthogonal>
    TOrthogonal *getOrthogonal();

    const std::map<std::string, std::shared_ptr<smacc::IOrthogonal>> &getOrthogonals() const;

    template <typename SmaccComponentType>
    void requiresComponent(SmaccComponentType *&storage);

    template <typename EventType>
    void postEvent(EventType *ev);

    template <typename EventType>
    void postEvent();

    void getTransitionLogHistory();

    template <typename T>
    bool getGlobalSMData(std::string name, T &ret);

    template <typename T>
    void setGlobalSMData(std::string name, T value);

    template <typename StateField, typename BehaviorType>
    void mapBehavior();

    void updateStatusMessage();

    std::string getStateMachineName();

    void state_machine_visualization(const ros::TimerEvent &);

    inline std::shared_ptr<SmaccStateInfo> getCurrentStateInfo() { return currentStateInfo_; }

    void publishTransition(SmaccTransitionInfo &transitionInfo);

    /// this function should be implemented by the user to create the orthogonals
    virtual void onInitialize();

    bool getTransitionLogHistory(smacc_msgs::SmaccGetTransitionHistory::Request &req, smacc_msgs::SmaccGetTransitionHistory::Response &res);

    template <typename TSmaccSignal, typename TMemberFunctionPrototype, typename TSmaccObjectType>
    boost::signals2::connection createSignalConnection(TSmaccSignal &signal, TMemberFunctionPrototype callback, TSmaccObjectType *object);

    template <typename TSmaccSignal, typename TMemberFunctionPrototype>
    boost::signals2::connection createSignalConnection(TSmaccSignal &signal, TMemberFunctionPrototype callback);

    std::list<boost::signals2::connection> stateCallbackConnections;

    void lockStateMachine(std::string msg);

    void unlockStateMachine(std::string msg);

    template <typename StateType>
    void notifyOnStateEntryStart(StateType *state);

    template <typename StateType>
    void notifyOnStateEntryEnd(StateType *state);

    template <typename StateType>
    void notifyOnStateExit(StateType *state);

    inline unsigned long getCurrentStateCounter() const { return this->stateSeqCounter_; }

    inline ISmaccState *getCurrentState() const { return this->currentState_; }

protected:
    void onInitializing(std::string smshortname);

    void onInitialized();

    template <typename TOrthogonal>
    void createOrthogonal();

    // Delegates to ROS param access with the current NodeHandle
    template <typename T>
    bool getParam(std::string param_name, T &param_storage);

    // Delegates to ROS param access with the current NodeHandle
    template <typename T>
    void setParam(std::string param_name, T param_val);

    // Delegates to ROS param access with the current NodeHandle
    template <typename T>
    bool param(std::string param_name, T &param_val, const T &default_val) const;

    // The node handle for this state
    ros::NodeHandle nh_;
    ros::NodeHandle private_nh_;

    ros::Timer timer_;
    ros::Publisher stateMachinePub_;
    ros::Publisher stateMachineStatusPub_;
    ros::Publisher transitionLogPub_;
    ros::ServiceServer transitionHistoryService_;

    // if it is null, you may be located in a transition. There is a small gap of time where internally
    // this currentState_ is null. This may change in the future.
    ISmaccState *currentState_;

    std::shared_ptr<SmaccStateInfo> currentStateInfo_;

    smacc_msgs::SmaccStatus status_msg_;

    // orthogonals
    std::map<std::string, std::shared_ptr<smacc::IOrthogonal>> orthogonals_;

private:
    std::recursive_mutex m_mutex_;

    // shared variables
    std::map<std::string, std::pair<std::function<std::string()>, boost::any>> globalData_;

    std::vector<smacc_msgs::SmaccTransitionLogEntry> transitionLogHistory_;

    smacc::SMRunMode runMode_;

    // Event to notify to the signaldetection thread that a request has been created...
    SignalDetector *signalDetector_;

    unsigned long stateSeqCounter_;

    friend class ISmaccState;

    template <typename EventType>
    void propagateEventToLogicUnits(ISmaccState *st, EventType *ev);

public:
    std::shared_ptr<SmaccStateMachineInfo> info_;
};
} // namespace smacc

#include <smacc/impl/smacc_state_impl.h>
#include <smacc/impl/smacc_client_impl.h>
#include <smacc/impl/smacc_component_impl.h>
#include <smacc/impl/smacc_orthogonal_impl.h>