
#include "probe.h"

// Assign probe trials. Input total number of trials and the probability of any
// kind of probe trial happening
void probeTrials(bool useProbeTrials, int count, double probability)
{
    // Don't use the 0 index or the last entry of stageParameters, because those
    // will be the first and last rests
    for (int i = 1; i < count; i++)
    {
        // Default the probe in stageParameters to be "None"
        stageParameters[i].probe = Probe::None;

        // If the user doesn't want to use probe trials, skip changing them to
        // probe
        if (!useProbeTrials)
        {
            continue;
        }

        // Get a random number from 1 to 100
        randomSeed(analogRead(A5));

        double prob1 = random(1, 101) / 100;

        // If that number is equal to or above the entered probability, this
        // will not be a probe trial
        if (prob1 >= probability)
        {
            continue;
        }

        // Now, get a random number either 0 or 1
        randomSeed(analogRead(A5));
        int prob2 = random(0, 2);

        switch (prob2)
        {
            case 0:
                stageParameters[i].probe = Probe::NoWarning;
                break;

            case 1:
                stageParameters[i].probe = Probe::NoChange;
                break;
        }
    }
}

// Both of these will be reported at the motor's time.
struct Probe_Messages getProbeMessages(Probe probeName)
{
    struct Probe_Messages probe_messages;

    switch (probeName)
    {
        case Probe::NoWarning:
        {
            probe_messages.activity_tag = 13;
            probe_messages.probe_string = "Probe: no warning tone.";
            break;
        }

        case Probe::NoChange:
        {
            probe_messages.activity_tag = 14;
            probe_messages.probe_string = "Probe: no change in motor.";
            break;
        }
    }

    return probe_messages;
}
