#pragma once
#include "warningTone.h"

enum class Probe
{
    None,
    NoWarning,
    NoChange,
};

enum class ProbeSubtype
{
  Warning,
  Motor
};

struct Probe_Messages
{
    int activity_tag;
    String probe_string;
    struct WarningTone::ToneParameters toneParameters;
};

void probeTrials(bool useProbeTrials, int count, double probability);
struct Probe_Messages getProbeMessages(enum Probe probeName, ProbeSubtype probe_subtype);
