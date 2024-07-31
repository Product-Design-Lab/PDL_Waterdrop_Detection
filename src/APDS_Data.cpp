#include "APDS_Data.h"
#include "Adafruit_TinyUSB.h"

void APDS_Data::set_bounds_lr(const int up_b_lr, const int low_b_lr)
{
    this->up_b_lr = up_b_lr;
    this->low_b_lr = low_b_lr;
}

void APDS_Data::set_bounds_ud(const int up_b_ud, const int low_b_ud)
{
    this->up_b_ud = up_b_ud;
    this->low_b_ud = low_b_ud;
}

APDS_Data::channel_pair_crossing_state_t APDS_Data::compute_ud_diff()
{
    ud_diff[0] = ud_diff_prev;
    for (int i = 0; i <= sample_count; i++)
    {
        ud_diff[i + 1] = u.get_lp()[i] - d.get_lp()[i];
        // Serial.printf("ud_diff[%d]:%d\n", i, ud_diff[i]);
    }
    ud_diff_prev = ud_diff[sample_count];

    return check_ud_crossing_state();
}

APDS_Data::channel_pair_crossing_state_t APDS_Data::check_ud_crossing_state()
{
    channel_pair_crossing_state_t state = {};
    for (int i = 0; i < sample_count; i++)
    {
        state.RISE_OVER_UPPER_BOUND |= (bool)(ud_diff[i] < up_b_ud && ud_diff[i + 1] > up_b_ud);
        state.FALL_BELOW_UPPER_BOUND |= (bool)(ud_diff[i] > up_b_ud && ud_diff[i + 1] < up_b_ud);
        state.RISE_OVER_LOWER_BOUND |= (bool)(ud_diff[i] > low_b_ud && ud_diff[i + 1] < low_b_ud);
        state.FALL_BELOW_LOWER_BOUND |= (bool)(ud_diff[i] < low_b_ud && ud_diff[i + 1] > low_b_ud);
    }
    return state;
}

APDS_Data::channel_pair_crossing_state_t APDS_Data::compute_lr_diff()
{
    lr_diff[0] = lr_diff_prev;
    for (int i = 0; i <= sample_count; i++)
    {
        lr_diff[i + 1] = l.get_lp()[i] - r.get_lp()[i];
        // Serial.printf("lr_diff[%d]:%d\n", i, lr_diff[i]);
    }
    lr_diff_prev = lr_diff[sample_count];

    return check_lr_crossing_state();
}

APDS_Data::channel_pair_crossing_state_t APDS_Data::check_lr_crossing_state()
{
    channel_pair_crossing_state_t state = {};
    for (int i = 0; i < sample_count; i++)
    {
        state.RISE_OVER_UPPER_BOUND |= (bool)(lr_diff[i] < up_b_lr && lr_diff[i + 1] > up_b_lr);
        state.FALL_BELOW_UPPER_BOUND |= (bool)(lr_diff[i] > up_b_lr && lr_diff[i + 1] < up_b_lr);
        state.RISE_OVER_LOWER_BOUND |= (bool)(lr_diff[i] > low_b_lr && lr_diff[i + 1] < low_b_lr);
        state.FALL_BELOW_LOWER_BOUND |= (bool)(lr_diff[i] < low_b_lr && lr_diff[i + 1] > low_b_lr);
    }
    return state;
}

APDS_Data::data_crossing_state_t APDS_Data::process_all_channel()
{
    data_crossing_state_t temp_state = {};
    temp_state.u.state = u.process_single_channel(sample_count).state;
    temp_state.d.state = d.process_single_channel(sample_count).state;
    temp_state.l.state = l.process_single_channel(sample_count).state;
    temp_state.r.state = r.process_single_channel(sample_count).state;
    temp_state.ud.state = compute_ud_diff().state;
    temp_state.lr.state = compute_lr_diff().state;

    crossing_state.state |= temp_state.state;
    return temp_state;
}

APDS_Data::data_crossing_state_t APDS_Data::get_crossing_state()
{
    data_crossing_state_t state = crossing_state;
    crossing_state.state = 0;
    return state;
}

int APDS_Data::sum_lp_cross_count(data_crossing_state_t state)
{
    int sum = state.u.LP_CROSS_UPPER_BOUND + state.u.LP_CROSS_LOWER_BOUND + state.d.LP_CROSS_UPPER_BOUND +
              state.d.LP_CROSS_LOWER_BOUND + state.l.LP_CROSS_UPPER_BOUND + state.l.LP_CROSS_LOWER_BOUND +
              state.r.LP_CROSS_UPPER_BOUND + state.r.LP_CROSS_LOWER_BOUND;
    return sum;
}

int APDS_Data::sum_dot_cross_count(data_crossing_state_t state)
{
    int sum = state.u.DOT_CROSS_UPPER_BOUND + state.u.DOT_CROSS_LOWER_BOUND + state.d.DOT_CROSS_UPPER_BOUND +
              state.d.DOT_CROSS_LOWER_BOUND + state.l.DOT_CROSS_UPPER_BOUND + state.l.DOT_CROSS_LOWER_BOUND +
              state.r.DOT_CROSS_UPPER_BOUND + state.r.DOT_CROSS_LOWER_BOUND;
    return sum;
}

int APDS_Data::sum_pair_cross_count(data_crossing_state_t state)
{
    int sum = state.ud.RISE_OVER_UPPER_BOUND + state.ud.FALL_BELOW_UPPER_BOUND + state.ud.RISE_OVER_LOWER_BOUND +
              state.ud.FALL_BELOW_LOWER_BOUND + state.lr.RISE_OVER_UPPER_BOUND + state.lr.FALL_BELOW_UPPER_BOUND +
              state.lr.RISE_OVER_LOWER_BOUND + state.lr.FALL_BELOW_LOWER_BOUND;
    return sum;
}

void APDS_Data::printRaw()
{
    for (int i = 0; i < sample_count; i++)
    {
        Serial.printf("il:%d, ir:%d, iu:%d, id:%d\n", l.get_raw_u8()[i], r.get_raw_u8()[i], u.get_raw_u8()[i], d.get_raw_u8()[i]);
    }
}

void APDS_Data::printCalib()
{
    Serial.printf("cl:%d, cr:%d, cu:%d, cd:%d\n", l.get_calibValue(), r.get_calibValue(), u.get_calibValue(), d.get_calibValue());
}

void APDS_Data::printRaw_i32()
{
    for (int i = 0; i < sample_count; i++)
    {
        Serial.printf("il:%d, ir:%d, iu:%d, id:%d\n", l.get_raw_i32()[i], r.get_raw_i32()[i], u.get_raw_i32()[i], d.get_raw_i32()[i]);
    }
}

void APDS_Data::printLP()
{
    for (int i = 0; i < sample_count; i++)
    {
        Serial.printf("ll:%d, lr:%d, lu:%d, ld:%d\n", l.get_lp()[i], r.get_lp()[i], u.get_lp()[i], d.get_lp()[i]);
    }
}

void APDS_Data::printDot()
{
    for (int i = 0; i < sample_count; i++)
    {
        // Serial.printf("dl:%.2f, dr:%.2f, du:%.2f, dd:%.2f\n", l.dot[i], r.dot[i], u.dot[i], d.dot[i]);
        Serial.printf("dl:%d, dr:%d, du:%d, dd:%d\n", l.get_dot()[i], r.get_dot()[i], u.get_dot()[i], d.get_dot()[i]);
    }
}

void APDS_Data::printPair()
{
    for (int i = 0; i < sample_count; i++)
    {
        Serial.printf("l-r:%d, u-d:%d\n", lr_diff[i], ud_diff[i]);
    }
}

// Function to print a 32-bit unsigned integer in binary format
void APDS_Data::printCrossingState(uint32_t val)
{
    // Start from the most significant bit (leftmost)
    for (int i = 31; i >= 0; i--)
    {
        // Check if the current bit is set (1) or not (0)
        printf("%d", (val >> i) & 1);

        // Add a space for better readability
        if (i % 4 == 0)
        {
            printf(" ");
        }
    }
    printf("\n");
}

void APDS_Data::plotCrossingState(uint32_t val)
{
    // give each bit a name for better readability, plot each bit
    const char *names[] = {
        "U_U",   // up channel cross upper bound
        "U_D",   // up channel cross lower bound
        "DU_U",  // difference in up channel cross upper bound
        "DU_D",  // difference in up channel cross lower bound
        "D_U",   // down channel cross upper bound
        "D_D",   // down channel cross lower bound
        "DD_U",  // difference in down channel cross upper bound
        "DD_D",  // difference in down channel cross lower bound
        "L_U",   // left channel cross upper bound
        "L_D",   // left channel cross lower bound
        "DL_U",  // difference in left channel cross upper bound
        "DL_D",  // difference in left channel cross lower bound
        "R_U",   // right channel cross upper bound
        "R_D",   // right channel cross lower bound
        "DR_U",  // difference in right channel cross upper bound
        "DR_D",  // difference in right channel cross lower bound
        "LR_RU", // difference in left and right channel rise above upper bound
        "LR_FU", // difference in left and right channel fall below upper bound
        "LR_RL", // difference in left and right channel rise above lower bound
        "LR_FL", // difference in left and right channel fall below lower bound
    };
    for (int i = 0; i < 20; i++)
    {
        Serial.printf("%s:%d,", names[i], (val >> i) & 1);
    }
    Serial.printf("\n");
}

APDS_Data::APDS_Data()
{
    sample_count = 0;
}
