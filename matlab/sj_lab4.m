%%% %%% Kod wizualizujący test istotności

function wykres_n(Z, kryt)
    X = ...
        min([Z, min(kryt)]) - 2: ...
        1e-3: ...
        max([Z, max(kryt)]) + 2;
    
    figure; hold on;
    plot(X, normpdf(X, 0, 1), 'DisplayName', 'R. prawd.')
    
    for i = 1:length(kryt)
        plot([kryt(i), kryt(i)], [0, normpdf(kryt(i), 0, 1)], ...
            'k-.*', 'DisplayName', 'W. krytyczna');
    end
        
    plot([Z, Z], [0, normpdf(Z, 0, 1)], ...
        'r-*', 'DisplayName', 'W. statystyki');
    
    hold off; grid; legend;
    xlabel('x'); ylabel('f(x)'); title('N(0, 1)');
    xlim([-inf, inf]);
end



%%% %%% ZADANIE NR 1

function [Z, kryt, H_0_odrzucic] = test_ist_mean(alfa, X, mu_0, postac_testu)
    n = length(X); m = mean(X); S = std(X);
    
    % Duża próba - korzystamy z rozkładu normalnego.
    if n > 30
        Z = (m - mu_0) / S * sqrt(n);
        
        if postac_testu == "lewostronny"
            kryt = norminv(alfa);
            H_0_odrzucic = Z <= kryt;
        elseif postac_testu == "prawostronny"
            kryt = norminv(1 - alfa);
            H_0_odrzucic = Z >= kryt;
        elseif postac_testu == "obustronny"
            kryt(1) = norminv(alfa / 2);
            kryt(2) = norminv(1 - alfa/2);
            H_0_odrzucic = Z <= kryt(1) || Z >= kryt(2);
        end
        
        wykres_n(Z, kryt);
    % Mała próba - korzystamy z rozkładu t-Studenta.
    % Modyfikujemy także Z.
    else
        Z = (m - mu_0) / S * sqrt(n - 1);
        
        if postac_testu == "lewostronny"
            kryt = tinv(alfa, n - 1);
            H_0_odrzucic = Z <= kryt;
        elseif postac_testu == "prawostronny"
            kryt = tinv(1 - alfa, n - 1);
            H_0_odrzucic = Z >= kryt;
        elseif postac_testu == "obustronny"
            kryt(1) = tinv(alfa / 2, n - 1);
            kryt(2) = tinv(1 - alfa / 2, n - 1);
            H_0_odrzucic = Z <= kryt(1) || Z >= kryt(2);
        end
        
        wykres_t(n-1, Z, kryt);
    end
end



%%% %%% ZADANIE NR 2

% Bierzemy pod uwagę tylko jeden przypadek,
% gdy odchylenia std. są nieznane.
function [Z, kryt, H_0_odrzucic] = test_ist_mean_2(alfa, X_1, X_2, postac_testu)
    n_1 = length(X_1); mu_1 = mean(X_1); s_1 = std(X_1);
    n_2 = length(X_2); mu_2 = mean(X_2); s_2 = std(X_2);
    
    % Jeśli próby są zbyt małe, to nie wykonujemy testu.
    if n_1 < 30 || n_2 < 30
        Z = NaN; kryt = NaN; H_0_odrzucic = false;
        return;
    end
    
    Z = (mu_1 - mu_2) / ...
        sqrt(s_1^2 / n_1 + s_2^2 / n_2);
    
    nu_1 = n_1 - 1; nu_2 = n_2 - 1;
    
    % Liczba stopni swobody rozkładu t.
    nu = (s_1^2 / n_1 + s_2^2 / n_2)^2 / ...
        (s_1^4 / (n_1^2 * nu_1) + s_2^4 / (n_2^2 * nu_2));  
    
    if postac_testu == "lewostronny"
        kryt = tinv(alfa, nu - 1);
        H_0_odrzucic = Z <= kryt;
    elseif postac_testu == "prawostronny"
        kryt = tinv(1 - alfa, nu - 1);
        H_0_odrzucic = Z >= kryt;
    elseif postac_testu == "obustronny"
        kryt(1) = tinv(alfa / 2, nu - 1);
        kryt(2) = tinv(1 - alfa / 2, nu - 1);
        H_0_odrzucic = Z <= kryt(1) || Z >= kryt(2);
    end
    
    wykres_t(nu, Z, kryt);
end



%%% %%% ZADANIE NR 3

% W tym teście nie ma warunków nałożonych na próbę, które
% spowodowałyby nie wykonanie testu.
function [Z, kryt, H_0_odrzucic] = test_ist_var(alfa, X, var_0, postac_testu)
    n = length(X); s = std(X);
    
    Z = n * s^2 / var_0;
    
    if postac_testu == "lewostronny"
        kryt = chi2inv(alfa, n - 1);
        H_0_odrzucic = Z <= kryt;
    elseif postac_testu == "prawostronny"
        kryt = chi2inv(1 - alfa, n - 1);
        H_0_odrzucic = Z >= kryt;
    elseif postac_testu == "obustronny"
        kryt(1) = chi2inv(alfa / 2, n - 1);
        kryt(2) = chi2inv(1 - alfa / 2, n - 1);
        H_0_odrzucic = Z <= kryt(1) || Z >= kryt(2);
    end
    
    wykres_chi2(n - 1, Z, kryt);
end



%%% %%% ZADANIE NR 4

function [Z, kryt, H_0_odrzucic] = test_ist_prop(alfa, m, n, p_0, postac_testu)
    p = m / n;
    
    % Warunki, które muszą zostać spełnione
    if n <= 50 || p <= 0.2 || p >= 0.8
        Z = NaN; kryt = NaN; H_0_odrzucic = NaN;
        return;
    end
    
    Z = (p - p_0) / sqrt(p_0 * (1 - p_0) / n);
    
    if postac_testu == "lewostronny"
        kryt = norminv(alfa);
        H_0_odrzucic = Z <= kryt;
    elseif postac_testu == "prawostronny"
        kryt = norminv(1 - alfa);
        H_0_odrzucic = Z >= kryt;
    elseif postac_testu == "obustronny"
        kryt(1) = norminv(alfa / 2);
        kryt(2) = norminv(1 - alfa/2);
        H_0_odrzucic = Z <= kryt(1) || Z >= kryt(2);
    end
    
    wykres_n(Z, kryt);
end