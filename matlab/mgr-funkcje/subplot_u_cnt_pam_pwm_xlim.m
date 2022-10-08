function fig = subplot_u_cnt_pam_pwm_xlim(out, settling_data, step_time, xlim2_label, xlim_factor)
    if nargin ~= 5
        disp("*** subplot_y_cnt_pam_pwm_tight ***")
        disp("error: invalid nargin  = '" + nargin + "'")
        disp("*** subplot_y_cnt_pam_pwm_tight ***")
        fig = -1;
        return
    end

    fieldNames = ["u_cnt", "dc_a", "dc_aa", "dc_ad", "u_pam", "dc_d", "dc_dd", "dc_da"];
    leg_label = ["u_{cnt}", "dc_{A}", "dc_{AA}", "dc_{AD}", "u_{PAM}", "dc_{D}", "dc_{DD}", "dc_{DA}"];
    plot_fnc = {@plot, @stairs, @stairs, @stairs, @stairs, @stairs, @stairs, @stairs};

    xlims(1) = step_time;
    xlims(2) = step_time + xlim_factor * max([settling_data.y_cnt.(xlim2_label), settling_data.y_pam.(xlim2_label), ...
        settling_data.y_a.(xlim2_label), settling_data.y_d.(xlim2_label), settling_data.y_ad.(xlim2_label), ...
        settling_data.y_da.(xlim2_label), settling_data.y_aa.(xlim2_label), settling_data.y_dd.(xlim2_label)...
    ]);
    xlims(1) = step_time - (xlims(2) - xlims(1)) * 0.05;

    if xlims(2) > settling_data.y_cnt.y_t(end)
%         disp("### subplot_y_cnt_pam_pwm_xlim ###")
%         disp("warning: right time bound too big. changing to max time")
%         disp("### subplot_y_cnt_pam_pwm_xlim ###")
%         xlims(2) = settling_data.y_cnt.y_t(end);
        disp("### subplot_u_cnt_pam_pwm_xlim ###")
        disp("warning: right time bound too big. not showing plots.")
        disp("### subplot_u_cnt_pam_pwm_xlim ###")
        fig = -1;
        return
    end

    fig = figure;
    for i = 1 : length(fieldNames)
        subplot(2, 4, i)
        plot_fnc{i}(out.(fieldNames(i)).Time, out.(fieldNames(i)).Data)
        grid, xlabel t, xlim tight, ylim padded
        ylabel(leg_label(i))
        xlim(xlims);
    end
end