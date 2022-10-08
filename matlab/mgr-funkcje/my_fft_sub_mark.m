function [Y, f] = my_fft_sub_mark(data, dt, f_harm)
    new_t = data.t(1) : dt : data.t(end);
    new_x = interp1(data.t, data.x, new_t);
    L = length(new_x);
    fs = 1/dt;
    Y = fft(new_x);
    Y = abs(Y / L);
    Y = Y(1 : floor(L/2 + 1));
    Y(2 : end-1) = 2 * Y(2 : end-1);

    f = fs * (0 : (L/2)) / L;

    plot(f, Y, 'LineWidth', 1.25)
    grid
    xlabel f
    ylabel("Amp_{" + data.label + "}")

    hold on
    for i = 1 : 10
        p = plot([f_harm, f_harm] * i, [0, max(Y)], '-.k');
        p.Color(4) = 0.5;
    end
    hold off

    set(gca, 'Children', flipud(get(gca, 'Children')) )
end