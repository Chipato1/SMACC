/*****************************************************************************************************************
 * ReelRobotix Inc. - Software License Agreement      Copyright (c) 2018
 * 	 Authors: Pablo Inigo Blasco, Brett Aldrich
 *
 ******************************************************************************************************************/
#pragma once

#include <smacc/common.h>
namespace smacc
{

class ISmaccClient;

class ISmaccComponent
{
public:
    ISmaccComponent();

    virtual ~ISmaccComponent();

    virtual void initialize(ISmaccClient *owner);

    // Assigns the owner of this resource to the given state machine parameter object
    void setStateMachine(ISmaccStateMachine *stateMachine);

    // Returns a custom identifier defined by the specific plugin implementation
    virtual std::string getName() const;

    template <typename EventType>
    void postEvent(const EventType &ev);

    template <typename EventType>
    void postEvent();

    template <typename TObjectTag, typename TDerived>
    void configureEventSourceTypes() {}

protected:
    // A reference to the state machine object that owns this resource
    ISmaccStateMachine *stateMachine_;

    ISmaccClient *owner_;

    friend class IOrthogonal;
};
} // namespace smacc