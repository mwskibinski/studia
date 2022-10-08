function [delta_t_max, delta_t_min, pref] = delta_t_func(u, y, sin_freq)
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
end