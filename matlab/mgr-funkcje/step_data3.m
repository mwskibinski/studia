function data = step_data3(y, sp, h, final_error)
    % Interpolacja sygnału
    data.N = numel(y.t) * 10;
    data.t = linspace(y.t(1), y.t(end), data.N);
    data.dt = data.t(2) - data.t(1);
    data.x = interp1(y.t, y.x, data.t, 'spline');

    % delete y for convenience
    clear y;

    % y0
    y0_idx = (data.t <= sp.time) & (data.t >= sp.time - 10*h);
    data.y0 = mean(data.x(y0_idx));

    % y_fin
    y_fin_idx = data.t >= (data.t(end) - 10*h);
    data.y_fin = mean(data.x(y_fin_idx));

    % t_set
    y_up = max([ ...
        abs(data.y_fin - data.y0) * final_error + data.y_fin, ...
        abs(data.y_fin - data.y0) * -final_error + data.y_fin ...
    ]);
    y_down = min([ ...
        abs(data.y_fin - data.y0) * final_error + data.y_fin, ...
        abs(data.y_fin - data.y0) * -final_error + data.y_fin ...
    ]);
    y_in_band = data.x > y_down & data.x < y_up;

    data.t_set_idx = find(y_in_band == false, 1, 'last');
    data.t_set = data.t(data.t_set_idx) - sp.time;

    % overshoot
    delta_y_fin = data.y_fin - data.y0;
    if delta_y_fin > 0
        y_max = max(data.x(data.t >= sp.time));
    else
        y_max = min(data.x(data.t >= sp.time));
    end
    delta_y_max = y_max - data.y0;
    data.over = (delta_y_max/delta_y_fin - 1) * 100;
    data.y_max = y_max;

    % węższy zakres analizowanych danych
    t_y_max = data.t(find(data.x == data.y_max, 1));
    t_step_to_y_max_idx = data.t >= sp.time & data.t <= t_y_max;
    t_step_to_y_max = data.t(t_step_to_y_max_idx);
    y_step_to_y_max = data.x(t_step_to_y_max_idx);

    % t_dly
    dly_sign = sign(y_step_to_y_max - (0.5 * (data.y_fin - data.y0) + data.y0));
    t_dly_idx = find(dly_sign == dly_sign(1) * -1, 1);
    data.t_dly = t_step_to_y_max(t_dly_idx) - sp.time;
    if isempty(data.t_dly)
        data.t_dly = -1;
    end

    % t_ris
    sign_t10 = sign(y_step_to_y_max - (0.1 * (data.y_fin - data.y0) + data.y0));
    sign_t90 = sign(y_step_to_y_max - (0.9 * (data.y_fin - data.y0) + data.y0));
    t10_idx = find(sign_t10 == sign_t10(1) * -1, 1);
    t90_idx = find(sign_t90 == sign_t90(1) * -1, 1);
    t10 = t_step_to_y_max(t10_idx);
    t90 = t_step_to_y_max(t90_idx);
    data.t_ris = t90 - t10;
    if isempty(data.t_ris)
        data.t_ris = -1;
    end

    % Wskaźniki całkowe
    int_idx = data.t >= sp.time & data.t <= (sp.time + data.t_set);
    % t_int = data.t(int_idx);
    t_int = data.t(int_idx) - sp.time;
    x_int = data.x(int_idx);
    e = sp.fin - x_int;

    data.i1 = sum(abs(e) .* data.dt);
    data.i2 = sum(e.^2 .* data.dt);
    data.i3 = sum(t_int .* abs(e) .* data.dt);
    data.i4 = sum(t_int.^2 .* abs(e) .* data.dt);
end