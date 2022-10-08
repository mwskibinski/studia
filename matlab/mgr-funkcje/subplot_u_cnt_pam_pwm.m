function fig = subplot_u_cnt_pam_pwm(out)
    fieldNames = ["u_cnt", "dc_a", "dc_aa", "dc_ad", "u_pam", "dc_d", "dc_dd", "dc_da"];
    leg_label = ["u_{cnt}", "dc_{A}", "dc_{AA}", "dc_{AD}", "u_{PAM}", "dc_{D}", "dc_{DD}", "dc_{DA}"];
    plot_fnc = {@plot, @stairs, @stairs, @stairs, @stairs, @stairs, @stairs, @stairs};

    fig = figure;
    for i = 1 : length(fieldNames)
        subplot(2, 4, i)
        plot_fnc{i}(out.(fieldNames(i)).Time, out.(fieldNames(i)).Data)
        grid, xlabel t, xlim tight, ylim padded
        ylabel(leg_label(i))
    end
end