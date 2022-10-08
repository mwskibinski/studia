function h = plot_sp_y_u(out, which_y)
    switch which_y
        case 'cnt'
            y_t = out.y_cnt.Time; y_x = out.y_cnt.Data;
            u_t = out.u_cnt.Time; u_x = out.u_cnt.Data;
            y_leg = "y_{cnt}"; u_lab = "u_{cnt}";
            plot_u_fnc = @plot;
        case 'PAM'
            y_t = out.y_pam.Time; y_x = out.y_pam.Data;
            u_t = out.u_pam.Time; u_x = out.u_pam.Data;
            y_leg = "y_{PAM}"; u_lab = "u_{PAM}";
            plot_u_fnc = @stairs;
        case {'PWM A', 'A'}
            y_t = out.y_a.Time; y_x = out.y_a.Data;
            u_t = out.dc_a.Time; u_x = out.dc_a.Data;
            y_leg = "y_{A}"; u_lab = "dc_{A}";
            plot_u_fnc = @stairs;
        case {'PWM D', 'D'}
            y_t = out.y_d.Time; y_x = out.y_d.Data;
            u_t = out.dc_d.Time; u_x = out.dc_d.Data;
            y_leg = "y_{D}"; u_lab = "dc_{D}";
            plot_u_fnc = @stairs;
        case {'PWM AA', 'AA'}
            y_t = out.y_aa.Time; y_x = out.y_aa.Data;
            u_t = out.dc_aa.Time; u_x = out.dc_aa.Data;
            y_leg = "y_{AA}"; u_lab = "dc_{AA}";
            plot_u_fnc = @stairs;
        case {'PWM DD', 'DD'}
            y_t = out.y_dd.Time; y_x = out.y_dd.Data;
            u_t = out.dc_dd.Time; u_x = out.dc_dd.Data;
            y_leg = "y_{DD}"; u_lab = "dc_{DD}";
            plot_u_fnc = @stairs;
        case {'PWM AD', 'AD'}
            y_t = out.y_ad.Time; y_x = out.y_ad.Data;
            u_t = out.dc_ad.Time; u_x = out.dc_ad.Data;
            y_leg = "y_{AD}"; u_lab = "dc_{AD}";
            plot_u_fnc = @stairs;
        case {'PWM DA', 'DA'}
            y_t = out.y_da.Time; y_x = out.y_da.Data;
            u_t = out.dc_da.Time; u_x = out.dc_da.Data;
            y_leg = "y_{DA}"; u_lab = "dc_{DA}";
            plot_u_fnc = @stairs;
        otherwise
            disp("*** plot_sp_y_u ***")
            disp("error: invalid which_y = '" + which_y + "'")
            disp("*** plot_sp_y_u ***")
            h = -1;
            return
    end

    h = figure;
    
    subplot(2, 1, 1)
    plot(out.sp.Time, out.sp.Data)
    hold on, plot(y_t, y_x), hold off
    grid, xlabel t, ylabel y, xlim tight, ylim padded
    legend("sp", y_leg, "location", "best")

    subplot(2, 1, 2)
    plot_u_fnc(u_t, u_x)
    grid, xlabel t, ylabel(u_lab), xlim tight, ylim padded
end