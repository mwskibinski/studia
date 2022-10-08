function data = step_data2_interp(y, step, h, final_error)
    % Interpolacja sygnału
    data.N = numel(y.t) * 10;
    data.t = linspace(y.t(1), y.t(end), data.N);
    data.dt = data.t(2) - data.t(1);
    data.x = interp1(y.t, y.x, data.t, 'spline');

    %%% Ruchoma średnia
    data.movmean_N = h / data.dt;
    data.x = movmean(data.x, data.movmean_N);
    
    % Usunięcie paru ostatnich punktów
    data.x(end - ceil(data.movmean_N / 2) : end) = [];
    data.t(end - ceil(data.movmean_N / 2) : end) = [];

    % y_a.x_interp_movmean_step = movmean(y_a.x_interp(step_idx : end), y_a.movmean_N, 'Endpoints', 'fill');
    % y_a.x_interp_movmean_step = movmeanCenterEndPoints(y_a.x_interp(step_idx : end), y_a.movmean_N);
    % y_a.x_interp_movmean_step = movmean(y_a.x_interp(step_idx : end), [y_a.movmean_N, 0]);
    % y_a.t_interp_movmean_step = y_a.t_interp(step_idx : end);
    %%%

    % delete y for convenience
    clear y;

    % y0
    y0_idx = (data.t <= step.time) & (data.t >= step.time - 10*h);
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
    data.t_set = data.t(data.t_set_idx) - step.time;

    % overshoot
    delta_y_fin = data.y_fin - data.y0;
    if delta_y_fin > 0
        y_max = max(data.x(data.t >= step.time));
    else
        y_max = min(data.x(data.t >= step.time));
    end
    delta_y_max = y_max - data.y0;
    data.over = (delta_y_max/delta_y_fin - 1) * 100;
    data.y_max = y_max;

    % węższy zakres analizowanych danych
    t_y_max = data.t(find(data.x == data.y_max, 1));
    t_step_to_y_max_idx = data.t >= step.time & data.t <= t_y_max;
    t_step_to_y_max = data.t(t_step_to_y_max_idx);
    y_step_to_y_max = data.x(t_step_to_y_max_idx);

    % t_dly
    dly_sign = sign(y_step_to_y_max - (0.5 * (data.y_fin - data.y0) + data.y0));
    t_dly_idx = find(dly_sign == dly_sign(1) * -1, 1);
    data.t_dly = t_step_to_y_max(t_dly_idx) - step.time;

    % t_ris
    sign_t10 = sign(y_step_to_y_max - (0.1 * (data.y_fin - data.y0) + data.y0));
    sign_t90 = sign(y_step_to_y_max - (0.9 * (data.y_fin - data.y0) + data.y0));
    t10_idx = find(sign_t10 == sign_t10(1) * -1, 1);
    t90_idx = find(sign_t90 == sign_t90(1) * -1, 1);
    t10 = t_step_to_y_max(t10_idx);
    t90 = t_step_to_y_max(t90_idx);
    data.t_ris = t90 - t10;
end