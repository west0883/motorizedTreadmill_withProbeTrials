#pragma once

enum class Probe
{
    None,
    NoWarning,
    NoChange,
};

struct Probe_Messages
{
    int activity_tag;
    String probe_string;
};

void probeTrials(bool useProbeTrials, int count, double probability);
struct Probe_Messages getProbeMessages(enum Probe probeName);
