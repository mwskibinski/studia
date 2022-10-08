function result = approx_1_order(y, u, step_time)
    % Wartości w stanie ustalonym
    y_fin = y.x(end);
    u_fin = u.x(end);

    % Wzmocnienie
    result.k = y_fin / u_fin;
    
    % Indeks wektorów, gdzie czas jest najbliższy wartości 'step_time'
    [~, y.step_idx] = min(abs(y.t - step_time));
    [~, u.step_idx] = min(abs(u.t - step_time));

    % Wartość początkowa
    y_0 = y.x(y.step_idx);
    
    % Dalej pod uwagę jest brana tylko ta część sygnału wyjściowego
    % od początku zmiany skokowej wejścia do końca danych
    y.x = y.x(y.step_idx : end);
    y.t = y.t(y.step_idx : end);

    % Różnica między wartość w stanie ustalonym a początkową
    delta_y = y_fin - y_0;

    % Wartości wyjścia dla czasów t_2 i t_1
    y_2 = y_0 + 0.283 * delta_y;
    y_1 = y_0 + 0.632 * delta_y;

    result.y_2 = y_2;
    result.y_1 = y_1;
    
    % Znak różnic między sygnałem a wartością poszukiwaną
    sign_y2 = sign(y.x - y_2);
    sign_y1 = sign(y.x - y_1);

    % Znajdź pierwsze (albo ostatnie [nie wiem]) wystąpienie zmiany znaku
    % Znaczy to tyle, że chodzi o pierwszy (albo ostatni) raz, gdy sygnał
    % osiąga wartość 'y_2' albo 'y_1'. Dla wielu sygnałów nie ma to
    % znaczenia.
    % ( 'first' czy 'last'? )
    y_2_idx = find(abs(sign_y2(2:end) - sign_y2(1:end-1)) > 1, 1, 'first');
    y_1_idx = find(abs(sign_y1(2:end) - sign_y1(1:end-1)) > 1, 1, 'first');

    % Czasy t_2 i t_1
    t_2 = y.t(y_2_idx) - step_time;
    t_1 = y.t(y_1_idx) - step_time;

    result.t_2 = t_2;
    result.t_1 = t_1;

    % Stała czasowa inercji I rzędu i stała opóźnienia
    result.T = 1.5 * (t_1 - t_2);
    result.T_o = t_1 - result.T;
end

% function result = approx_1_order(y, u, step_time)
%     % Wartości w stanie ustalonym
%     y_fin = y.x(end);
%     u_fin = u.x(end);
% 
%     % Wzmocnienie
%     result.k = y_fin / u_fin;
%     
%     % Indeks wektorów, gdzie czas jest najbliższy wartości 'step_time'
%     [~, y.step_idx] = min(abs(y.t - step_time));
%     [~, u.step_idx] = min(abs(u.t - step_time));
% 
%     % Wartość początkowa
%     y_0 = y.x(y.step_idx);
%     
%     % Dalej pod uwagę jest brana tylko ta część sygnału wyjściowego
%     % od początku zmiany skokowej wejścia do końca danych
%     y.x = y.x(y.step_idx : end);
%     y.t = y.t(y.step_idx : end);
% 
%     % Różnica między wartość w stanie ustalonym a początkową
%     delta_y = y_fin - y_0;
% 
%     % Wartości wyjścia dla czasów t_2 i t_1
%     y_2 = y_0 + 0.283 * delta_y;
%     y_1 = y_0 + 0.632 * delta_y;
% 
%     result.y_2 = y_2;
%     result.y_1 = y_1;
%     
%     if delta_y >= 0
%         y_max = max(y.x);
%         idx_from_step_to_max = y.x >= y_0 & y.x <= y_max;
%     else
%         y_max = min(y.x);
%         idx_from_step_to_max = y.x <= y_0 & y.x >= y_max;
%     end
% 
%     y.x_from_step_to_max = y.x(idx_from_step_to_max);
%     y.t_from_step_to_max = y.t(idx_from_step_to_max);
% 
%     % Indeksy wektorów gdzie sygnał osiągnął wartość najbliższą y_2 i y_1
%     [~, y_2_idx] = min(abs(y.x_from_step_to_max - y_2));
%     [~, y_1_idx] = min(abs(y.x_from_step_to_max - y_1));
% 
%     % Czasy t_2 i t_1
%     t_2 = y.t_from_step_to_max(y_2_idx) - step_time;
%     t_1 = y.t_from_step_to_max(y_1_idx) - step_time;
% 
%     result.t_2 = t_2;
%     result.t_1 = t_1;
% 
%     % Stała czasowa inercji I rzędu i stała opóźnienia
%     result.T = 1.5 * (t_1 - t_2);
%     result.T_o = t_1 - result.T;
% end