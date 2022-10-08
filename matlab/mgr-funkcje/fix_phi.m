function data = fix_phi(data)
    for row = 2 : size(data, 1)
        for col = 1 : size(data, 2)
            tmp = [data(row, col), data(row, col) + 360, data(row, col) - 360];
            diff = abs(data(row-1, col) - tmp);
            [~, idx] = min(diff);
            data(row, col) = tmp(idx);
        end
    end