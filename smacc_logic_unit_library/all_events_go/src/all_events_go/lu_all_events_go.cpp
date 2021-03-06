
#include <all_events_go/lu_all_events_go.h>
#include <smacc/common.h>

namespace smacc
{
namespace logic_units
{

using namespace smacc::introspection;
void LuAllEventsGo::onInitialized()
{
    for (auto type : eventTypes)
    {
        triggeredEvents[type] = false;
    }
}

void LuAllEventsGo::onEventNotified(const std::type_info *eventType)
{
    ROS_INFO_STREAM("LU ALL RECEIVED EVENT OF TYPE:" << demangleSymbol(eventType->name()));
    triggeredEvents[eventType] = true;

    for (auto &entry : triggeredEvents)
    {
        ROS_INFO_STREAM(demangleSymbol(entry.first->name()) << " = " << entry.second);
    }
}

bool LuAllEventsGo::triggers()
{
    ROS_INFO("LU All TRIGGERS?");
    for (auto &entry : triggeredEvents)
    {
        if (!entry.second)
            return false;
    }
    ROS_INFO("LU ALL TRIGGERED");
    return true;
}
} // namespace logic_units
} // namespace smacc