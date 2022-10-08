function data = fix_phi_init(data)
    for col = 1 : size(data, 2)
        tmp = {data(:, col), data(:, col) - 360, data(:, col) + 360};
        tmp_sum = [sum(abs(tmp{1}(:))), sum(abs(tmp{2}(:))), sum(abs(tmp{3}(:)))];
        [~, idx] = min(tmp_sum);
        data(:, col) = tmp{idx};
    end
