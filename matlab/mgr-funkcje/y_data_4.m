function [y_fin, t_set, t_set_idx, t_dly, t_ris, y_max, overshoot_pct] = y_data_4(y_t, y_x, final_error, h)
    mean_fin_idx = y_t >= (y_t(end) - 10*h);
    tmp_t = linspace(y_t(end) - 10*h, y_t(end), length(y_t(mean_fin_idx)));
    tmp_t(tmp_t > max(y_t(mean_fin_idx)) | tmp_t < min(y_t(mean_fin_idx))) = [];
    tmp_x = interp1(y_t(mean_fin_idx), y_x(mean_fin_idx), tmp_t');
    y_fin = mean(tmp_x);
    
    y_up = max([y_fin * (1 - final_error), y_fin * (1 + final_error)]);
    y_down = min([y_fin * (1 - final_error), y_fin * (1 + final_error)]);
    y_in_band = y_x > y_down & y_x < y_up;

    t_set_idx = find(y_in_band == false, 1, 'last');
    t_set = y_t(t_set_idx);

    y_max = max(y_x);
    overshoot_pct = (y_max/y_fin - 1) * 100;

    y_max_idx = find(y_max == y_x, 1);
    t_y_max = y_t(y_max_idx);

    tmp_t2 = linspace(y_t(1), t_y_max, length(y_t(1 : y_max_idx) * 2));
    tmp_x2 = interp1(y_t(1 : y_max_idx), y_x(1 : y_max_idx), tmp_t2);

    tmp_sign = sign(tmp_x2 - 0.5 * y_fin);
    t_dly_idx = find(tmp_sign == tmp_sign(1) * -1, 1);
    t_dly = tmp_t2(t_dly_idx);

    tmp_sign_t10 = sign(tmp_x2 - 0.1 * y_fin);
    tmp_sign_t90 = sign(tmp_x2 - 0.9 * y_fin);
    t10_idx = find(tmp_sign_t10 == tmp_sign_t10(1) * -1, 1);
    t90_idx = find(tmp_sign_t90 == tmp_sign_t90(1) * -1, 1);
    t10 = tmp_t2(t10_idx);
    t90 = tmp_t2(t90_idx);
    t_ris = t90 - t10;
end