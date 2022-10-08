function data = stepinfo_v2(y, step, h, final_error)
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

    data.sinfo = stepinfo(y.x, y.t, data.y_fin, data.y0, "SettlingTimeThreshold", final_error);
end