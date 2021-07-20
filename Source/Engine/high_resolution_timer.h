#pragma once
#include <Winbase.h>

//==========================================================================
   //
   //      HighResolutionTimer class
   //
   //==========================================================================
class HighResolutionTimer
{
public:

    HighResolutionTimer() : delta_time(-1.0), paused_time(0), stopped(false)
    {
        LONGLONG counts_per_sec;
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
        seconds_per_count = 1.0 / static_cast<double>(counts_per_sec);
        frames_per_sec = (LONGLONG)(counts_per_sec / 60.0);

        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));
        base_time = this_time;
        last_time = this_time;
    }

    // Returns the total time elapsed since Reset() was called, NOT counting any
    // time when the clock is stopped.
    float timeStamp() const  // in seconds
    {
        // If we are stopped, do not count the time that has passed since we stopped.
        // Moreover, if we previously already had a pause, the distance
        // stop_time - base_time includes paused time, which we do not want to count.
        // To correct this, we can subtract the paused time from mStopTime:
        //
        //                     |<--paused_time-->|
        // ----*---------------*-----------------*------------*------------*------> time
        //  base_time       stop_time        start_time     stop_time    this_time

        if (stopped)
        {
            return static_cast<float>(((stop_time - paused_time) - base_time) * seconds_per_count);
        }

        // The distance this_time - mBaseTime includes paused time,
        // which we do not want to count.  To correct this, we can subtract
        // the paused time from this_time:
        //
        //  (this_time - paused_time) - base_time
        //
        //                     |<--paused_time-->|
        // ----*---------------*-----------------*------------*------> time
        //  base_time       stop_time        start_time     this_time
        else
        {
            return static_cast<float>(((this_time - paused_time) - base_time) * seconds_per_count);
        }
    }

    float timeInterval() const  // in seconds
    {
        return static_cast<float>(delta_time);
    }

    void reset() // Call before message loop.
    {
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));
        base_time = this_time;
        last_time = this_time;

        stop_time = 0;
        stopped = false;
    }

    void start() // Call when unpaused.
    {
        LONGLONG start_time;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&start_time));

        // Accumulate the time elapsed between stop and start pairs.
        //
        //                     |<-------d------->|
        // ----*---------------*-----------------*------------> time
        //  base_time       stop_time        start_time
        if (stopped)
        {
            paused_time += (start_time - stop_time);
            last_time = start_time;
            stop_time = 0;
            stopped = false;
        }
    }

    void stop() // Call when paused.
    {
        if (!stopped)
        {
            QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stop_time));
            stopped = true;
        }
    }

    bool tick() // Call every frame.
    {
        if (stopped)
        {
            delta_time = 0.0;
            return true;
        }

        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time));

        //�t���[������
        if (this_time < last_time + frames_per_sec) return false;

        // Time difference between this frame and the previous.
        delta_time = (this_time - last_time) * seconds_per_count;

        //�o�ߎ���
        LONGLONG dTime = this_time - last_time;

        // ��r���Ԓǉ�
        comparison_time += delta_time;

        // Prepare for next frame.
        last_time += frames_per_sec;

        if (dTime > 40000) last_time = this_time;

        // Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the
        // processor goes into a power save mode or we get shuffled to another
        // processor, then mDeltaTime can be negative.
        if (delta_time < 0.0)
        {
            delta_time = 0.0;
        }

        return true;
    }

    [[nodiscard]] LONGLONG getLastTime()      const   { return last_time; }
    [[nodiscard]] double GetDeltaTime()       const   { return delta_time; }
    [[nodiscard]] double GetComparisonTime()  const   { return comparison_time; }
    [[nodiscard]] float GetFrameRate()        const   { return frame_rate; }
    void ResetComparisonTime()   { comparison_time = 0; }

    void setFrameRate(float frame_rate)
    {
        LONGLONG counts_per_sec;
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
        seconds_per_count = 1.0 / static_cast<double>(counts_per_sec);
        frames_per_sec = (LONGLONG)(counts_per_sec / frame_rate);
        this->frame_rate = frame_rate;
    }

private:

    double seconds_per_count;
    double delta_time;
    double comparison_time = 0;
    float frame_rate;

    LONGLONG this_time = 0;
    LONGLONG base_time = 0;
    LONGLONG paused_time = 0;
    LONGLONG stop_time = 0;
    LONGLONG last_time = 0;
    //�Œ�t���[���p
    LONGLONG frames_per_sec = 0;

    bool stopped;
};