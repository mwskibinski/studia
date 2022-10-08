function data = step_data(y, step, h, final_error)
    % y0
    y0_idx = (y.t <= step.time) & (y.t >= step.time - 10*h);
    y0_t = linspace(max(step.time - 10*h, min(y.t(y0_idx))), step.time, length(y.t(y0_idx)));
    y0_t(y0_t > max(y.t(y0_idx)) | y0_t < min(y.t(y0_idx))) = [];
    if length(y0_t) <= 1
        y0_x = y.x(y0_idx);
    else
        y0_x = interp1(y.t(y0_idx), y.x(y0_idx), y0_t);
    end
    data.y0 = mean(y0_x);

    % y_fin
    y_fin_idx = y.t >= (y.t(end) - 10*h);
    y_fin_t = linspace(y.t(end) - 10*h, y.t(end), length(y.t(y_fin_idx)));
    y_fin_t(y_fin_t > max(y.t(y_fin_idx)) | y_fin_t < min(y.t(y_fin_idx))) = [];
    y_fin_x = interp1(y.t(y_fin_idx), y.x(y_fin_idx), y_fin_t);
    data.y_fin = mean(y_fin_x);

    % t_set
    % y_up = max([data.y_fin * (1 - final_error), data.y_fin * (1 + final_error)]);
    % y_down = min([data.y_fin * (1 - final_error), data.y_fin * (1 + final_error)]);
    y_up = max([ ...
        abs(data.y_fin - data.y0) * final_error + data.y_fin, ...
        abs(data.y_fin - data.y0) * -final_error + data.y_fin ...
    ]);
    y_down = min([ ...
        abs(data.y_fin - data.y0) * final_error + data.y_fin, ...
        abs(data.y_fin - data.y0) * -final_error + data.y_fin ...
    ]);
    y_in_band = y.x > y_down & y.x < y_up;

    data.t_set_idx = find(y_in_band == false, 1, 'last');
    data.t_set = y.t(data.t_set_idx) - step.time;

    % overshoot
    delta_y_fin = data.y_fin - data.y0;
    if delta_y_fin > 0
        y_max = max(y.x(y.t >= step.time));
    else
        y_max = min(y.x(y.t >= step.time));
    end
    delta_y_max = y_max - data.y0;
    data.over = (delta_y_max/delta_y_fin - 1) * 100;
    data.y_max = y_max;

    % t_dly
    t_y_max = y.t(find(y.x == data.y_max, 1));
    t_step_to_y_max_idx = y.t >= step.time & y.t <= t_y_max;
    t_step_to_y_max = y.t(t_step_to_y_max_idx);

    y_dly_t = linspace(step.time, t_y_max, length(t_step_to_y_max) * 2);
    y_dly_t(y_dly_t > max(t_step_to_y_max) | y_dly_t < min(t_step_to_y_max)) = [];
    y_dly_x = interp1(t_step_to_y_max, y.x(t_step_to_y_max_idx), y_dly_t);

    % tmp_sign = sign(y_dly_x - 0.5 * data.y_fin);
    % tmp_sign = sign(y_dly_x - 0.5 * (data.y_fin - data.y0));
    tmp_sign = sign(y_dly_x - (0.5 * (data.y_fin - data.y0) + data.y0));
    t_dly_idx = find(tmp_sign == tmp_sign(1) * -1, 1);
    data.t_dly = y_dly_t(t_dly_idx) - step.time;

    % t_ris
    % tmp_sign_t10 = sign(y_dly_x - 0.1 * data.y_fin);
    % tmp_sign_t90 = sign(y_dly_x - 0.9 * data.y_fin);
    tmp_sign_t10 = sign(y_dly_x - (0.1 * (data.y_fin - data.y0) + data.y0));
    tmp_sign_t90 = sign(y_dly_x - (0.9 * (data.y_fin - data.y0) + data.y0));
    t10_idx = find(tmp_sign_t10 == tmp_sign_t10(1) * -1, 1);
    t90_idx = find(tmp_sign_t90 == tmp_sign_t90(1) * -1, 1);
    t10 = y_dly_t(t10_idx);
    t90 = y_dly_t(t90_idx);
    data.t_ris = t90 - t10;
end