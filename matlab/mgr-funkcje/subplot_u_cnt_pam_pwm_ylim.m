function fig = subplot_u_cnt_pam_pwm_ylim(out, pwm_down, pwm_up)
    fieldNames = ["u_cnt", "dc_a", "dc_aa", "dc_ad", "u_pam", "dc_d", "dc_dd", "dc_da"];
    leg_label = ["u_{cnt}", "dc_{A}", "dc_{AA}", "dc_{AD}", "u_{PAM}", "dc_{D}", "dc_{DD}", "dc_{DA}"];
    plot_fnc = {@plot, @stairs, @stairs, @stairs, @stairs, @stairs, @stairs, @stairs};

    pad = 0.05;
    ylim_cnt_pam = [pwm_down - (pwm_up-pwm_down) * pad, pwm_up + (pwm_up-pwm_down) * pad];
    ylim_pwm = [0 - pad, 1 + pad];

    fig = figure;
    for i = 1 : length(fieldNames)
        subplot(2, 4, i)
        plot_fnc{i}(out.(fieldNames(i)).Time, out.(fieldNames(i)).Data)
        grid, xlabel t, xlim tight, ylim padded
        ylabel(leg_label(i))

        if i == 1 || i == 5
            ylim(ylim_cnt_pam);
        else
            ylim(ylim_pwm);
        end
    end
end