namespace sm_three_some
{
namespace SS1
{

namespace sm_three_some
{
namespace ss1_states
{
//HERE WE MAKE FORWARD DECLARATIONS OF ALL SUBSTATE ROUTINES
class Ssr1;
class Ssr2;
class Ssr3;
} // namespace ss1_states
} // namespace sm_three_some

using namespace sm_three_some::ss1_states;

struct Ss1 : smacc::SmaccState<Ss1, MsRun, Ssr1, sc::has_full_history>
{
public:
    using SmaccState::SmaccState;

    typedef mpl::list<
        // smacc::Transition<EvSuperstateFinish<Ssr3>, StState1>

        // Keyboard events
        smacc::Transition<EvLoopEnd<Ssr1>, StState1>>
        reactions;

    static constexpr int total_iterations() { return 3; }
    int iteration_count = 0;

    static void onDefinition()
    {
    }

    void onInitialize()
    {
    }
}; // namespace SS4

//forward declaration for the superstate
using SS = SS1::Ss1;

#include <sm_three_some/states/ss_superstate_1/ssr_1.h>
#include <sm_three_some/states/ss_superstate_1/ssr_2.h>
#include <sm_three_some/states/ss_superstate_1/ssr_3.h>

} // namespace SS1
} // namespace sm_three_some
