% Funkcja do wizualizacji danych z próbek pomiarowych
% Jej jedynem argumentem jest macierz o wymiarach N x M, gdzie
% N - ile elementów, M - ile próbek
% Otrzymamy okno z czterema wykresami
function wizualizacja(dane)
    % Odczytujemy wymiary N i M
    N = size(dane, 1); M = size(dane, 2);

    % Pierwszy wykresy to po prostu dane z macierzy
    subplot(4, 1, 1);
    plot(dane', 'bx');
    % Dodatkowo dodajemy siatkę oraz 
    % dodajemy pewien odstęp po lewej i prawej
    grid; xlim([0.5, M + 0.5]);
    
    % Wykorzystanie funkcji boxplot
    subplot(4, 1, 2);
    boxplot(dane); grid;
    
    % Za trzeci wykres odpowiada funkcja wykreślająca kartę kontrolną
    % wartości średnich (Zadanie nr 3)
    subplot(4, 1, 3);
    karta_kontrolna_wartosci_srednich(dane);

    % Za czwarty wykres odpowiada funkcja wykreślająca kartę kontrolną
    % odchylenia standardowego (Zadanie nr 4)
    subplot(4, 1, 4);
    karta_kontrolna_odchylenia_std(dane);
    
    % Oś odciętych jest wspólna dla wszystkich wykresów
    xlabel('nr próbki');
end

% Funkcja, która tworzy wykres, a na nim:
% - Wartości średnie dla każdej próbki
% - Wartość średnia ze wszystkich średnich
% - Górne i dolne granice kontrolne
function karta_kontrolna_wartosci_srednich(dane)
    N = size(dane, 1); M = size(dane, 2);
    
    % Obliczamy wartość średnią (ze wszystkich) oraz
    % granice górne i dolne
    % Wartość mnożnika A_3 jest otrzymywana z osobnej funkcji
    DLK = mean(mean(dane)) - get_A_3(N) * mean(std(dane));
    GLK = mean(mean(dane)) + get_A_3(N) * mean(std(dane));
    srednia = mean(mean(dane));
    
    % Tworzenie wykresu
    hold on;
    plot(mean(dane), 'r-x');
    plot([1, M], [srednia, srednia], 'k');
    plot([1, M], [DLK, DLK], 'b');
    plot([1, M], [GLK, GLK], 'b');
    hold off;
    
    % Pozostałe "upiększacze"
    grid; xlim([0.5, M + 0.5]);
    ylabel('$\bar{x}$', 'interpreter', 'latex', 'fontsize', 14);
end

% Wartość mnożnika A_3
function wynik = get_A_3(N)
    % Tworzymy wektor wartości N, dla których mamy podane wartości A_3
    N_vec = [2:10, 15, 20, 25];
    % Macierz o dwóch kolumnach:
    % pierwsza to wartości N, a druga to mnożniki
    A_3 = zeros(length(N_vec), 2);
    A_3(:, 1) = N_vec;
    A_3(:, 2) = [ ...
        2.659, 1.954, 1.628, 1.427, 1.287, 1.182, ...
        1.099, 1.032, 0.975, 0.789, 0.680, 0.606 ...
    ];
    % Szukamy czy w macierzy znajduje się N o wartości, której potrzebujemy
    % Jeśli to, to zabieramy wartość mnożnika z drugiej kolumny
    wynik = A_3(A_3(:, 1) == N, 2);
    % Jeśli "wynik" jest pusty to znaczy, że N nie było
    % Wtedy dokonujemy interpolacji
    if isempty(wynik)
        wynik = interp1(A_3(:, 1), A_3(:, 2), N, 'cubic');
    end
end

function karta_kontrolna_odchylenia_std(dane)
    N = size(dane, 1); M = size(dane, 2);
        
    DLK = get_B_3(N) * mean(std(dane));
    GLK = get_B_4(N) * mean(std(dane));
    srednia = mean(std(dane));
    
    hold on;
    plot(std(dane), 'r-x');
    plot([1, M], [srednia, srednia], 'k');
    plot([1, M], [DLK, DLK], 'b');
    plot([1, M], [GLK, GLK], 'b');
    hold off;
    
    grid; xlim([0.5, M + 0.5]);
    ylabel('$s$', 'interpreter', 'latex', 'fontsize', 14);
end

function wynik = get_B_3(N)
    N_vec = [2:10, 15, 20, 25];
    B_3 = zeros(length(N_vec), 2);
    B_3(:, 1) = N_vec;
    B_3(:, 2) = [ ...
        0, 0, 0, 0, 0.030, 0.118, ...
        0.185, 0.239, 0.284, 0.428, 0.510, 0.565 ...
    ];
    wynik = B_3(B_3(:, 1) == N, 2);
    if isempty(wynik)
        wynik = interp1(B_3(:, 1), B_3(:, 2), N, 'cubic');
    end
end

function wynik = get_B_4(N)
    N_vec = [2:10, 15, 20, 25];
    B_4 = zeros(length(N_vec), 2);
    B_4(:, 1) = N_vec;
    B_4(:, 2) = [ ...
        3.267, 2.568, 2.266, 2.089, 1.970, 1.882, ...
        1.815, 1.761, 1.716, 1.572, 1.490, 1.435 ...
    ];
    wynik = B_4(B_4(:, 1) == N, 2);
    if isempty(wynik)
        wynik = interp1(B_4(:, 1), B_4(:, 2), N, 'cubic');
    end
end