#include <smacc/smacc.h>
namespace sm_dance_bot
{
struct StAcquireSensors : smacc::SmaccState<StAcquireSensors, MsDanceBotRunMode>
{
   // transition names
   // ---- TAGS ----
   struct ON_KEYBOARD : PREEMPT
   {
   };

   struct ON_KEYBOARD2 : ABORT
   {
   };

   struct ON_SENSORS_AVAILABLE : SUCCESS
   {
   };

   struct Unit1;
   //----------------
   using SmaccState::SmaccState;

   typedef mpl::list<
       // Expected event
       //Transition<EvAllGo<LuAllEventsGo, Unit1>, StNavigateToWaypointsX, ON_SENSORS_AVAILABLE>,
       Transition<EvAllGo<LuAllEventsGo, Unit1>, StEventCountDown, ON_SENSORS_AVAILABLE>,

       //smacc::Transition<EvAllGo2<LuAl2>, StateDestiny2>,
       smacc::Transition<EvGlobalError, sc::deep_history<StAcquireSensors>>>
       reactions;

   static void onDefinition()
   {
      static_configure<OrObstaclePerception, CbLidarSensor>();
      static_configure<OrStringPublisher, CbStringPublisher>("Hello World!");
      static_configure<OrTemperatureSensor, CbConditionTemperatureSensor>();
      static_configure<OrService3, CbService3>(Service3Command::SERVICE3_ON);
      static_configure<OrUpdatablePublisher, ros_publisher_client::CbDefaultPublishLoop>();

      static_createLogicUnit<LuAllEventsGo,
                             EvAllGo<LuAllEventsGo, Unit1>,
                             mpl::list<EvTopicMessage<CbLidarSensor, OrObstaclePerception>, EvTopicMessage<CbConditionTemperatureSensor, OrTemperatureSensor>>>();

      // auto luall = static_createLogicUnit<LuAllEventsGo>();
      // luall->enablesOn<EvTopicMessage<CbLidarSensor, OrObstaclePerception>, EvTopicMessage<CbConditionTemperatureSensor, OrTemperatureSensor>>();
      // luall->throwsEvent<EvAllGo<LuAllEventsGo, Unit1>>();
   }
};
} // namespace sm_dance_bot