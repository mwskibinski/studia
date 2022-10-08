function fig = subplot_y_cnt_pam_pwm(out, settling_data)
    fieldNames = ["y_cnt", "y_a", "y_aa", "y_ad", "y_pam", "y_d", "y_dd", "y_da"];
    leg_label = ["y_{cnt}", "y_{A}", "y_{AA}", "y_{AD}", "y_{PAM}", "y_{D}", "y_{DD}", "y_{DA}"];

    fig = figure;
    switch nargin
        case 1
            for i = 1 : length(fieldNames)
                subplot(2, 4, i);
                plot(out.sp.Time, out.sp.Data, out.(fieldNames(i)).Time, out.(fieldNames(i)).Data);
                grid, xlabel t, ylabel y, xlim tight, ylim padded
                legend("sp", leg_label(i), "location", "best")
            end
        case 2
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
            end
        otherwise
            disp("*** subplot_y_cnt_pam_pwm ***")
            disp("error: invalid nargin  = '" + nargin + "'")
            disp("*** subplot_y_cnt_pam_pwm ***")
            fig = -1;
            return
    end
end