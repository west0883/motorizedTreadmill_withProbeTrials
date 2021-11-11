#pragma once
#include "warningTone.h"

enum class Probe
{
    None,
    NoWarning,
    NoChange,
};

enum class ProbeSubtype1
{
  Warning,
  Motor
};

// Another subtype, to be used in printing correct messages
enum class ProbeSubtype2
{
    Blank,
    Starting,
    Stopping,
    Accelerating,
    Decelerating, 
    Maintaining
};

struct Probe_Messages
{
    int activity_tag;
    String probe_string;
    struct WarningTone::ToneParameters toneParameters;
};

void probeTrials(bool useProbeTrials, int count, double probability);
struct Probe_Messages getProbeMessages(enum Probe probeName, ProbeSubtype1 probe_subtype1, ProbeSubtype2 probe_subtype2 = ProbeSubtype2::Blank);
void checkProbeMotor (int &activityTag, String &message, Probe probe, ProbeSubtype2 probe_subtype2);
