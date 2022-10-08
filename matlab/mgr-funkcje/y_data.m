function [y_fin, t_set, t_set_idx] = y_data(y_t, y_x, final_error, h)
    % y_fin = y_x(end);
    % y_fin = mean(y_x(round(end*0.95) : end));
    
    mean_fin_idx = y_t >= (y_t(end) - 10*h);
    % y_fin = mean(y_x(mean_fin_idx));
    
    % tmp_t = (y_t(end) - 10*h) : dt : y_t(end);
    tmp_t = linspace(y_t(end) - 10*h, y_t(end), length(y_t(mean_fin_idx)));
    tmp_t(tmp_t > max(y_t(mean_fin_idx)) | tmp_t < min(y_t(mean_fin_idx))) = [];
    tmp_x = interp1(y_t(mean_fin_idx), y_x(mean_fin_idx), tmp_t');

    y_fin = mean(tmp_x);
    
    % y_up = y_fin * (1 + final_error);
    % y_down = y_fin * (1 - final_error);
    % y_lims = sort([y_fin * (1 - final_error), y_fin * (1 + final_error)]);
    y_up = max([y_fin * (1 - final_error), y_fin * (1 + final_error)]);
    y_down = min([y_fin * (1 - final_error), y_fin * (1 + final_error)]);
    y_in_band = y_x > y_down & y_x < y_up;
    % y_in_band = y_x > y_lim(1) & y_x < y_lims(2);
    t_set_idx = find(y_in_band == false, 1, 'last');
    t_set = y_t(t_set_idx);
end