function [phase_max, phase_min, amp_max, amp_min, pref] = delta_t_func_amp_phase_2(u, y, sin_freq)
    [delta_t_max, delta_t_min, amp_max, amp_min, pref] = delta_t_func_amp(u, y, sin_freq);
    phase_max = -1 * delta_t_max * sin_freq * 360;
    phase_min = -1 * delta_t_min * sin_freq * 360;
end