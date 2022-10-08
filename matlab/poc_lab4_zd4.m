% Funkcja zwraca: numer klasy do której przydziela obiekt
% oraz punkty do rysowań lini
% Jako argumenty podajemy macierze 2 2współczynników dla każdej klasy
% współczynniki badanego obiektu
% oraz liczbę k.
function [klasa, linie] = kNN(klasa_1_wsp, klasa_2_wsp, klasa_3_wsp, badany_wsp, k)
    % Buduję macierz z tych współczynników
    % Ostatnia kolumna będzie iteracyjnie zapełniania dystansami punktów
    % od tego, który badamy.
    dist = [ ... 
        klasa_1_wsp(:, 1), klasa_1_wsp(:, 2), zeros(5, 1); ...
        klasa_2_wsp(:, 1), klasa_2_wsp(:, 2), zeros(5, 1); ...
        klasa_3_wsp(:, 1), klasa_3_wsp(:, 2), zeros(5, 1) ...
    ];

    % Mierzę dystans.
    % Niestety nie wykonuję żadnej normalizacji, gdyż, gdy próbowałem
    % otrzymywałem wyniki zupełnie niezgodne z rzeczywistością. Być może
    % źle jej dokonywałem. Zostałem przy zwykłym pomiarze dystansu pomiędzy dwoma pkt
    for i = 1:15
        dist(i, 3) = sqrt( ...
            (badany_wsp(1) - dist(i, 1))^2 + (badany_wsp(2) - dist(i, 2))^2 ...
        );
    end

    vec = zeros(1, 4);
    % Szukam najmniejszej liczby i zbieram jej indeksy
    % Ze względu na to w których wierszach była ta najmniejsza liczba
    % Przydzielam numer klasy
    % Pętlę wykonuję k razy.
    for i = 1:k
        ind = find(dist == min(dist(:, 3)));
        [row, col] = ind2sub(size(dist), ind);
        vec(i, 1:3) = dist(row, :);

        if row <= 5
            vec(i, 4) = 1;
        elseif row <= 10
            vec(i, 4) = 2;
        else
            vec(i, 4) = 3;
        end
        
        dist(row, 3) = NaN;
    end

    % Liczę, który numer klasy pojawił się jak często.
    klasa_1 = numel(find(vec(:, 4) == 1));
    klasa_2 = numel(find(vec(:, 4) == 2));
    klasa_3 = numel(find(vec(:, 4) == 3));
    
    % Szukam, który numer klasy pojawił się najczęściej.
    if klasa_1 > klasa_3 && klasa_1 > klasa_2
        klasa = 1;
    elseif klasa_2 > klasa_1 && klasa_2 > klasa_3
        klasa = 2;
    else
        klasa = 3;
    end
    
    % Zwracam punkty, które będą łączone liniami.
    linie = vec(:, 1:2);
end