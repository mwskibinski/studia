function [Y_phase, f] = my_fft_phi_sub(data, dt)
    new_t = data.t(1) : dt : data.t(end);
    new_x = interp1(data.t, data.x, new_t);
    L = length(new_x);
    fs = 1/dt;
    Y = fft(new_x) / L;

    Y_amp = abs(Y);
    Y_amp = Y_amp(1 : floor(L/2 + 1));
    Y_amp(2 : end-1) = 2 * Y_amp(2 : end-1);

    f = fs * (0 : (L/2)) / L;
    
%     figure
%     plot(f, Y_amp)
%     grid
%     xlabel f
%     ylabel amp

    Y_phase = angle(Y);
    Y_phase = Y_phase(1 : floor(L/2 + 1));
    Y_phase(2 : end-1) = 1 * Y_phase(2 : end-1);

%     figure
    plot(f, Y_phase)
    grid
    xlabel f
    ylabel("Phi_{" + data.label + "}")
end