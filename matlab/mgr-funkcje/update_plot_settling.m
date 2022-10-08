function [] = update_plot_settling(fig, y_t, y_x, y0, y_fin, t_set_idx, final_error)
    % figure(fig), fig_sbp = subplot(2, 1, 1);
    if class(fig) == string('matlab.ui.Figure')
        figure(fig), fig_sbp = subplot(2, 1, 1);
    elseif class(fig) == string('cell')
        figure(fig{1}), fig_sbp = subplot(fig{2});
    end
    hold on
    plot(y_t(t_set_idx), y_x(t_set_idx), 'k*')
    p_up = plot([y_t(1), y_t(end)], ...
        [(y_fin - y0) * final_error + y_fin, (y_fin - y0) * final_error + y_fin], ...
        'k-.');
    p_fin = plot([y_t(1), y_t(end)], [y_fin, y_fin], 'k--');
    p_down = plot([y_t(1), y_t(end)], ...
        [(y_fin - y0) * -final_error + y_fin, (y_fin - y0) * -final_error + y_fin], ...
        'k-.');
    p_up.Color(4) = 0.5; p_fin.Color(4) = 0.5; p_down.Color(4) = 0.5;
    hold off
    fig_sbp.Legend.String = fig_sbp.Legend.String([1, 2]);
end

% function [] = update_plot_settling(fig, y_t, y_x, y_fin, t_set_idx, final_error)
%     % figure(fig), fig_sbp = subplot(2, 1, 1);
%     if class(fig) == string('matlab.ui.Figure')
%         figure(fig), fig_sbp = subplot(2, 1, 1);
%     elseif class(fig) == string('cell')
%         figure(fig{1}), fig_sbp = subplot(fig{2});
%     end
%     hold on
%     plot(y_t(t_set_idx), y_x(t_set_idx), 'k*')
%     p_up = plot([y_t(1), y_t(end)], [y_fin * (1+final_error), y_fin * (1+final_error)], 'k-.');
%     p_fin = plot([y_t(1), y_t(end)], [y_fin, y_fin], 'k--');
%     p_down = plot([y_t(1), y_t(end)], [y_fin * (1-final_error), y_fin * (1-final_error)], 'k-.');
%     p_up.Color(4) = 0.5; p_fin.Color(4) = 0.5; p_down.Color(4) = 0.5;
%     hold off
%     fig_sbp.Legend.String = fig_sbp.Legend.String([1, 2]);
% end