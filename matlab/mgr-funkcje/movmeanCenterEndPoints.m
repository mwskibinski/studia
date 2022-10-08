function result = movmeanCenterEndPoints(data, n)
    if mod(n, 2) == 0
        n = n+1;
        disp('making n even by adding 1')
        disp("n = " + n)
    end
    result = movmean(data, n, 'Endpoints', 'fill');
    data_n = length(data);

    % for i = 1 : n-1
    for i = 1 : (n-1)/2
        window_size = i-1;
        right_bound = 2*window_size + 1;
        % [i, window_size, right_bound]
        result(i) = mean(data(1 : right_bound));
    end

    % for i = data_n : -1 : data_n - 2
    for i = data_n : -1 : data_n - (n-1)/2 + 1
        window_size = data_n - i;
        left_bound = data_n - (2*window_size);
        % [i, window_size, right_bound]
        result(i) = mean(data(left_bound : end));
    end
end