function data = tf_2nd_order_osc_data(tf)
    poles = pole(tf);
    p1 = poles(1);
    if numel(poles) ~= 2 || imag(p1) == 0
        disp("*** error: system must be 2nd order oscillatory")
        data = -1;
        return
    end
    
    data.alpha = abs(real(p1));
    data.omega_d = abs(imag(p1));
    data.omega_n = sqrt(data.alpha^2 + data.omega_d^2);
    data.xi = data.alpha / data.omega_n;
    if data.xi < 1/sqrt(2)
        data.omega_r = data.omega_n * sqrt(1 - 2*data.xi^2);
    else
        data.omega_r = NaN;
    end

    data.k = tf.Num{1}(end) ./ tf.Den{1}(end);

    data.T_alpha = 2*pi / data.alpha;
    data.T_omega_d = 2*pi / data.omega_d;
    data.T_omega_n = 2*pi / data.omega_n;
    
    omega_sqrt_mean = sqrt(data.xi) * data.omega_n + ...
        (1 - sqrt(data.xi)) * data.omega_d;
    data.thd_peak_freq = omega_sqrt_mean / (4*pi);