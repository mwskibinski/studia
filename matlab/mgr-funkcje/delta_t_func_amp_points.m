function [points] = delta_t_func_amp_points(u, y, sin_freq)
    % delta_t_max = 0; delta_t_min = 0; pref = 0;

    eq_eps = 1e-10;

    y.x2_max = max(y.x2);
    y.x2_min = min(y.x2);
    y.t2_max = max(y.t2(abs(y.x2 - y.x2_max) < eq_eps));
    y.t2_min = max(y.t2(abs(y.x2 - y.x2_min) < eq_eps));

    if y.x2_max > abs(y.x2_min)
        pref = "max";
    else
        pref = "min";
    end
    
    u.y.t_max_lim = [y.t2_max - 1/sin_freq, y.t2_max];
    u.y.t_min_lim = [y.t2_min - 1/sin_freq, y.t2_min];
    u.y.x_max = u.x2(u.t2 >= u.y.t_max_lim(1) & u.t2 <= u.y.t_max_lim(2));
    u.y.t_max = u.t2(u.t2 >= u.y.t_max_lim(1) & u.t2 <= u.y.t_max_lim(2));
    u.y.x_min = u.x2(u.t2 >= u.y.t_min_lim(1) & u.t2 <= u.y.t_min_lim(2));
    u.y.t_min = u.t2(u.t2 >= u.y.t_min_lim(1) & u.t2 <= u.y.t_min_lim(2));
    u.y.x_max_max = max(u.y.x_max);
    u.y.x_min_min = min(u.y.x_min);
    u.y.t_max_max = max(u.y.t_max(abs(u.y.x_max - u.y.x_max_max) < eq_eps));
    u.y.t_min_min = max(u.y.t_min(abs(u.y.x_min - u.y.x_min_min) < eq_eps));

    delta_t_max = y.t2_max - u.y.t_max_max;
    delta_t_min = y.t2_min - u.y.t_min_min;

    amp_max = y.x2_max / u.y.x_max_max;
    amp_min = y.x2_min / u.y.x_min_min;

    points.u_max_t = u.y.t_max_max;
    points.u_max_x = u.y.x_max_max;
    points.u_min_t = u.y.t_min_min;
    points.u_min_x = u.y.x_min_min;
    
    points.y_max_t = y.t2_max;
    points.y_max_x = y.x2_max;
    points.y_min_t = y.t2_min;
    points.y_min_x = y.x2_min;
end