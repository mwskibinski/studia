function [out_x, out_y] = psc_lab2(signals, fs, window, S)

    % Podmieniane tylko, jeśli jest tylko 1 arg. wejściowy.
    plot_x_label = "częstotliwość";

    % W zależności od ilości arg. wejściowych następuje inna inicjalizacja:
    % - domyślnie, częst. próbkowania ustawiona jako 2*pi
    % (częst. względna),
    % - okno domyślne to prostokątne,
    % - liczba segmentów to 1.
    switch nargin
        case 1
            fs = 2*pi; plot_x_label = "częstotliwość względna";
            window = "rectangular"; S = 1;
        case 2
            window = "rectangular"; S = 1;
        case 3
            S = 1;
    end

    % Wyjściowe macierzy danych.
    Y = zeros(size(signals, 1), size(signals, 2) / (2*S));
    Omega = zeros(size(signals, 1), size(signals, 2) / (2*S));

    % Dla każdego sygnału wykonujemy te same operacje.
    for signal_n = 1 : size(signals, 1)
        % Pobierz sygnał do transformacji.
        y = signals(signal_n, :);
    
        % Jego długość.
        N = length(y);

        % Jeśli nazwa okna to łańcuch znakowy to zmieniany on jest na
        % string -- można łatwiej wykonywać relacje logiczne.
        window = string(window);
    
        % Długość segmentu i bin.
        N_p = N / S;
        bin = 2 * pi / N_p;

        % Macierz zawierająca transformatę po wygaładzaniu/uśrednianiu.
        U_S = zeros(1, N_p);
    
        % Zapisane przez nas równania na okna.
        if window == "bartlett"
            w(1 : floor(N_p/2) + 1) = 2 * (0 : floor(N_p/2)) / N_p;
            w(ceil(N_p/2) + 1 : N_p) = w(floor(N_p/2) : -1 : 1);
        elseif window == "hann"
            w = 0.5 - 0.5 * cos(2 * pi * (0:N_p-1)/N_p);
        elseif window == "hamming"
            w = 0.54 - 0.46 * cos(2 * pi * (0:N_p-1)/N_p);
        elseif window == "blackman"
            w = 0.42 - 0.5 * cos(2 * pi * (0:N_p-1)/N_p) + ...
                0.08 * cos(4 * pi * (0:N_p-1)/N_p);
        elseif window == "blackman-harris"
            w = 0.35875 - 0.48829 * cos(2 * pi * (0:N_p-1)/N_p) + ...
                0.14128 * cos(4 * pi * (0:N_p-1)/N_p) - ...
                0.01168 * cos(6 * pi * (0:N_p-1)/N_p);
        else
            % W innym wypadku okno prostokątne.
            w = 1;
        end
    
        % Dla każdego odcinka wykonujemy transformację.
        for ii = 1:S
            % Odpowiednie indeksy danych wejściowych,
            idx = (N_p * (ii - 1) + 1) : (N_p * (ii));
            y_i = y(idx) .* w;
        
            % Macierz przechowująca transformatę dla tego segmentu.
            U = zeros(1, N_p);
    
            % Transformacja Fouriera.
            for k = 0 : N_p-1
                U(k + 1) = U(k + 1) + ...
                    sum(y_i .* exp(-j * k .* (0 : N_p-1) * bin));
            end
        
            % Wynik wchodzi podzielony przez liczbę segmentów.
            U_S = U_S + 1/S * abs(U).^2;
        end
        
        % Bierzemy tylko połowę transformaty pod uwagę.
        U_S = U_S(1 : ceil(end/2));

        % Częstotliwość na osi poziomej.
        omega = fs/N_p * (0 : N_p - 1);
        omega = omega(1 : ceil(end/2));
        
        % Macierze wyjściowe.
        Y(signal_n, :) = U_S;
        Omega(signal_n, :) = omega;
    end

    % W zależności od ilości pożądanych arg. wyjściowych:
    % - dane są wypluwane na wyjście,
    % - lub nanoszone są na wykres.
    switch nargout
        case 0
            hold on;
            set(gca,'yscale','log');
            for i = 1 : size(Y, 1)
                legendName = ['sygnał ', num2str(i)];
                % Te wartości, które wynoszą 0, zostają podmienione,
                % by wykres logarytmiczny mógł powstać.
                Y(Y(i, :) == 0) = 1e-30;
                semilogy(Omega(i, :), Y(i, :), ...
                    'DisplayName', legendName);
            end
            grid on;
            xlim([-inf, inf]);
            xlabel(plot_x_label); ylabel("|X|^2");
            hold off;
            legend;
        case 2
            out_x = Omega;
            out_y = Y;
    end
end