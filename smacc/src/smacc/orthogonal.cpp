#include <smacc/smacc_orthogonal.h>
#include <smacc/impl/smacc_state_machine_impl.h>
#include <smacc/smacc_client_behavior.h>

namespace smacc
{
void IOrthogonal::setStateMachine(ISmaccStateMachine *value)
{
    this->stateMachine_ = value;
    this->onInitialize();
}

void IOrthogonal::addClientBehavior(std::shared_ptr<smacc::SmaccClientBehavior> clBehavior)
{
    if (clBehavior != nullptr)
    {
        ROS_INFO("Setting Ortho %s State behavior: %s", this->getName().c_str(), clBehavior->getName().c_str());
        clBehavior->stateMachine_ = this->stateMachine_;
        clBehavior->currentOrthogonal = this;

        clientBehaviors_.push_back(clBehavior);
    }
    else
    {
        ROS_INFO("Not behavioral State by orthogonal");
    }
}

void IOrthogonal::onInitialize()
{
}

std::string IOrthogonal::getName() const
{
    return demangleSymbol(typeid(*this).name());
}

void IOrthogonal::onEntry()
{
    if (clientBehaviors_.size() > 0)
    {
        for (auto &clBehavior : clientBehaviors_)
        {
            ROS_INFO("Orthogonal %s OnEntry, current Behavior: %s",
                     this->getName().c_str(),
                     clBehavior->getName().c_str());

            clBehavior->onEntry();
        }
    }
    else
    {
        ROS_INFO("Orthogonal %s OnEntry", this->getName().c_str());
    }
}

void IOrthogonal::onExit()
{
    if (clientBehaviors_.size() > 0)
    {
        for (auto &clBehavior : clientBehaviors_)
        {
            ROS_INFO("Orthogonal %s OnExit, current Behavior: %s", this->getName().c_str(), clBehavior->getName().c_str());
            clBehavior->onExit();
        }
        clientBehaviors_.clear();
    }
    else
    {
        ROS_INFO("Orthogonal %s OnExit", this->getName().c_str());
    }
}
} // namespace smacc