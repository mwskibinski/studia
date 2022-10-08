function [Y, f] = my_fft(x, t, dt)
    new_t = t(1) : dt : t(end);
    new_x = interp1(t, x, new_t);
    L = length(new_x);
    fs = 1/dt;
    Y = fft(new_x);
    Y = abs(Y / L);
    Y = Y(1 : floor(L/2 + 1));
    Y(2 : end-1) = 2 * Y(2 : end-1);

    f = fs * (0 : (L/2)) / L;
    figure
    plot(f, Y)
    grid
    xlabel f
    ylabel amp
end