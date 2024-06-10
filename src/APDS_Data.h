#pragma once

#include "APDS_Channel.h"

class APDS_Data
{
public:
    union channel_pair_crossing_state_t
    {
        struct
        {
            bool RISE_OVER_UPPER_BOUND : 1;
            bool FALL_BELOW_UPPER_BOUND : 1;
            bool RISE_OVER_LOWER_BOUND : 1;
            bool FALL_BELOW_LOWER_BOUND : 1;
        } __attribute__((packed));
        uint8_t state : 4;
    };

    /* check if the data crosses the upper and lower bounds
    state bit field:
     | reserved | lr_state | r_state | l_state | d_state | u_state |
     |  32 - 20 |  19 - 16 | 15 - 12 |  11 - 8 |  7 - 4  |  3 - 0  |
    */
    union data_crossing_state_t
    {
        struct
        {
            APDS_Channel::channel_crossing_state_t u;
            APDS_Channel::channel_crossing_state_t d;
            APDS_Channel::channel_crossing_state_t l;
            APDS_Channel::channel_crossing_state_t r;
            channel_pair_crossing_state_t ud;
            channel_pair_crossing_state_t lr;
        } __attribute__((packed));
        uint32_t state : 24;
    };

public:
    APDS_Channel u;
    APDS_Channel d;
    APDS_Channel l;
    APDS_Channel r;

    int sample_count;

private:
    int lr_diff[MAX_SAMPLES]; // difference between left and right channel
    int lr_diff_prev;         // previous value of lr_diff
    int up_b_lr, low_b_lr;    // upper and lower bounds for lr_diff

    int ud_diff[MAX_SAMPLES]; // difference between up and down channel
    int ud_diff_prev;         // previous value of ud_diff
    int up_b_ud, low_b_ud;    // upper and lower bounds for ud_diff

    data_crossing_state_t crossing_state;
    channel_pair_crossing_state_t check_ud_crossing_state();
    channel_pair_crossing_state_t check_lr_crossing_state();
    channel_pair_crossing_state_t compute_ud_diff();
    channel_pair_crossing_state_t compute_lr_diff();

public:
    // constructor
    APDS_Data();
    void set_bounds_lr(const int up_b_lr, const int low_b_lr);
    void set_bounds_ud(const int up_b_ud, const int low_b_ud);
    data_crossing_state_t process_all_channel();
    data_crossing_state_t get_crossing_state();

    void printRaw();
    void printCalib();
    void printRaw_i32();
    void printLP();
    void printDot();
    void printPair();
    void printCrossingState(uint32_t val);
    void plotCrossingState(uint32_t val);
};
