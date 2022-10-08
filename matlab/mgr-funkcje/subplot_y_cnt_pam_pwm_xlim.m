function fig = subplot_y_cnt_pam_pwm_xlim(out, settling_data, step_time, xlim2_label, xlim_factor)
    if nargin ~= 5
        disp("*** subplot_y_cnt_pam_pwm_tight ***")
        disp("error: invalid nargin  = '" + nargin + "'")
        disp("*** subplot_y_cnt_pam_pwm_tight ***")
        fig = -1;
        return
    end

    fieldNames = ["y_cnt", "y_a", "y_aa", "y_ad", "y_pam", "y_d", "y_dd", "y_da"];
    leg_label = ["y_{cnt}", "y_{A}", "y_{AA}", "y_{AD}", "y_{PAM}", "y_{D}", "y_{DD}", "y_{DA}"];

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
        disp("### subplot_y_cnt_pam_pwm_xlim ###")
        disp("warning: right time bound too big. not showing plots.")
        disp("### subplot_y_cnt_pam_pwm_xlim ###")
        fig = -1;
        return
    end

    ylims_y(1) = min([settling_data.y_cnt.y_max, settling_data.y_pam.y_max, ...
        settling_data.y_a.y_max, settling_data.y_d.y_max, settling_data.y_ad.y_max, ...
        settling_data.y_da.y_max, settling_data.y_aa.y_max, settling_data.y_dd.y_max, ...
        settling_data.y_cnt.y0, settling_data.y_pam.y0, ...
        settling_data.y_a.y0, settling_data.y_d.y0, settling_data.y_ad.y0, ...
        settling_data.y_da.y0, settling_data.y_aa.y0, settling_data.y_dd.y0, ...
    ]);
    ylims_y(2) = max([settling_data.y_cnt.y_max, settling_data.y_pam.y_max, ...
        settling_data.y_a.y_max, settling_data.y_d.y_max, settling_data.y_ad.y_max, ...
        settling_data.y_da.y_max, settling_data.y_aa.y_max, settling_data.y_dd.y_max, ...
        settling_data.y_cnt.y0, settling_data.y_pam.y0, ...
        settling_data.y_a.y0, settling_data.y_d.y0, settling_data.y_ad.y0, ...
        settling_data.y_da.y0, settling_data.y_aa.y0, settling_data.y_dd.y0, ...
    ]);
    ylims_y = sort(ylims_y);
    ylims_y(1) = ylims_y(1) - (ylims_y(2) - ylims_y(1)) * 0.05;
    ylims_y(2) = ylims_y(2) + (ylims_y(2) - ylims_y(1)) * 0.05;

    fig = figure;
    for i = 1 : length(fieldNames)
        sbp = subplot(2, 4, i);
        plot(out.sp.Time, out.sp.Data, out.(fieldNames(i)).Time, out.(fieldNames(i)).Data);
        grid, xlabel t, ylabel y, xlim tight, ylim padded
        legend("sp", leg_label(i), "location", "best")
        update_plot_settling({fig, sbp}, ...
            settling_data.(fieldNames(i)).y_t, settling_data.(fieldNames(i)).y_x, ...
            settling_data.(fieldNames(i)).y0, settling_data.(fieldNames(i)).y_fin, ...
            settling_data.(fieldNames(i)).t_set_idx, settling_data.final_error ...
        )
        xlim(xlims);
        ylim(ylims_y);
    end
end