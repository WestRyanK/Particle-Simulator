#include "../Engine/Stopwatch.h"

using CodeMonkeys::Engine::Engine::Stopwatch;


float Stopwatch::get_seconds_from_last_measurement()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = (t2 - t1);
    t1 = t2;

    return duration.count();
}