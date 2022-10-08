%%% %%% ZADANIE NR 1

clear all; close all; clc;

p = 0.05;
N = 50;

y = zeros(N, 1);
for K = 1:N
    y(K) = co_najmniej_k(p, N, K);
end

plot(1:N, y, 'bo-'); grid;
xlabel('K'); ylabel('P(K)');

% Funkcja licząca prawdopodobieństwo, 
% że co najmniej K elementów, będzie wadliwych.
% Robione poprzez sumowanie kolejnych prawdopodobieństw w pętli
% Poczynając na K elementów, a kończąc na N
function P = co_najmniej_k(p, N, K)
    P = 0;
    for i = K:N
        P = P + ...
            nchoosek(N, i) * p^i * (1 - p)^(N - i);
    end
end



%%% %%% ZADANIE NR 2

clear all; close all; clc;

% Sprawdzenie, czy argumenty, można podwać sobie naprzemian.
k_1 = 2;
k_2 = zadanie_2_2(zadanie_2_1(k_1));

% I rzeczywiście, k_1 == k_2


% W obu funkcjach, nie musimy uwzględniać mu, ani sigma,
% bo chodzi, po prostu o to, że rozkład jest znormalizowany
% więc dla takiego rozkładu obliczamy te prawdopodobieństwa.
function P = zadanie_2_1(k)
    P = (1 - normcdf(k)) * 2;
end

function k = zadanie_2_2(P)
    k = norminv(-(P / 2 - 1));
end



%%% %%% ZADANIE NR 3

clear all; close all; clc;

N = 1000;
X = normrnd(0, 1, N, 1);

% Inne wykorzystywane rozkłady.
% x = 0:1e-2:1;
% X = chi2pdf(x, 3);
% X = betapdf(x, 0.75, 0.75);

wykres_sprawdzajacy(X);
xlabel('X_{r}'); ylabel('K_{(r - 0.5)/N}');
exportgraphics(gcf, 'zd3_1000.pdf', 'ContentType', 'Vector');

% Sprawdzenie, czy nasza funkcja daje podobny wynik.
figure; normplot(X);

% Iteracyjnie nanoszę punkty na wykres.
function wykres_sprawdzajacy(X)
    X = sort(X);
    N = length(X);
    hold on;
    for r = 1:N
        K_arg = (r - 0.5) / N;
        K = norminv(K_arg);
        plot(X(r), K, 'bx');
    end
    hold off; grid;
end



%%% %%% ZADANIE NR 4

clear all; close all; clc; warning off;

K = 20;
N = 1000;

X = normrnd(0, 1, K, N);

% Obliczamy estymatory.
% Proces powtarzamy pewną dużą ilość razy.
for i = 1:1000
    X = normrnd(0, 1, K, N);
    wynik(i, :) = estymatory(X);
end

% Wykreślamy histogramy dla obliczonych wartości estymatorów.
subplot(3, 1, 1);
histogram(wynik(:, 1), 50);
xlim([0.7, 1.3]); title('\sigma_1');

subplot(3, 1, 2);
histogram(wynik(:, 2), 50);
xlim([0.7, 1.3]); title('\sigma_2');

subplot(3, 1, 3);
histogram(wynik(:, 3), 50);
xlim([0.7, 1.3]); title('\sigma_3');

% Funkcja zwraca trzy elementową macierz estymatorów.
% Wzory według instrukcji.
function wynik = estymatory(X)
    wynik = zeros(3, 1);
    K = size(X, 1); N = size(X, 2);

    wynik(1) = get_a(K) * mean(std(X));
    wynik(2) = get_b(K) * mean(max(X) - min(X));
    wynik(3) = get_c(K) * median(std(X));
end

% Parametry obliczamy w sposób podobny do tych z poprzednich zajęć. Tam
% były inne mnożniki, ale i tak robimy to w podobny sposób.
% Jeśli K jest zapisane w tabeli , to wykorzystujemy podany mnożnik,
% a jak nie to go interpolujemy.
function wynik = get_a(K)
    K_vec = [2:10, 15, 20];
    a = zeros(length(K_vec), 2);
    a(:, 1) = K_vec;
    a(:, 2) = [ ...
        1.253, 1.1284, 1.0854, 1.0638, ...
        1.051, 1.0423, 1.0363, 1.0317, ...
        1.0281, 1.018, 1.0133 ...
        ];
    wynik = a(a(:, 1) == K, 2);
    if isempty(wynik)
        wynik = interp1(a(:, 1), a(:, 2), K, 'cubic');
    end
end

% Analogicznie do funkcji get_a
function wynik = get_b(K)
    K_vec = [2:10, 15, 20];
    b = zeros(length(K_vec), 2);
    b(:, 1) = K_vec;
    b(:, 2) = [ ...
        0.8865, 0.5907, 0.4857, 0.4300, ...
        0.3946, 0.3698, 0.3512, 0.3367, ...
        0.3249, 0.2880, 0.2677 ...
        ];
    wynik = b(b(:, 1) == K, 2);
    if isempty(wynik)
        wynik = interp1(b(:, 1), b(:, 2), K, 'cubic');
    end
end

% Analogicznie do funkcji get_a
function wynik = get_c(K)
    K_vec = [2:10, 15, 20];
    c = zeros(length(K_vec), 2);
    c(:, 1) = K_vec;
    c(:, 2) = [ ...
        1.1829, 1.0646, 1.0374, 1.0260, ...
        1.0201, 1.0161, 1.0136, 1.0116, ...
        1.0103, 1.0063, 1.0046...
        ];
    wynik = c(c(:, 1) == K, 2);
    if isempty(wynik)
        wynik = interp1(c(:, 1), c(:, 2), K, 'cubic');
    end
end



%%% %%% ZADANIE NR 5

clear all; close all; clc; warning off;

N = 500;
x = 1:1e-1:50;
mu = 10;
sigma = 2;
X = normrnd(mu, sigma, N, 1);
% X = chi2pdf(x, 6);
% X = X';

% Wykreślam histogram i porównuję go, z funkcją g. prawdopodobieństwa.
[n, centers] = moj_histogram(X, 20);
[n, centers] = histogram_norm(n, centers);
figure; bar(centers, n, 'hist'); hold on;
x_tmp = linspace(min(X), max(X), N);
plot(x_tmp, normpdf(x_tmp, mu, sigma), 'r-', 'LineWidth', 2.0);
hold off; legend('Nasz histogram', 'normpdf');
exportgraphics(gcf, 'zd5_h.pdf', 'ContentType', 'Vector'); close all;

% Porównuję dystrybuantę doświadczalną z teoretyczną.
[points, dystr] = dystrybuanta(X);
figure; plot(points, dystr, 'LineWidth', 2.0); hold on;
plot(x_tmp, normcdf(x_tmp, mu, sigma), 'r-.', 'LineWidth', 2.0);
legend('D. doswiadczalna', 'D. teoretyczna');
exportgraphics(gcf, 'zd5_d.pdf', 'ContentType', 'Vector'); close all;

% Funkcja tworząca histogram. Zwraca nam środki słupków, oraz liczbę
% obiektów pasujących do danej klasy. Jako argument podajemy całą próbkę,
% oraz liczbę pożądanych klas.
function [n, centers] = moj_histogram(X, N_bars)
    zakres = linspace(min(X), max(X), N_bars + 1);

    dane = zeros(N_bars, 2);
    for i = 1:N_bars
        dane(i, :) = [mean([zakres(i), zakres(i + 1)]), 0];
        for j = 1:length(X)
            if X(j) >= zakres(i) && X(j) < zakres(i + 1)
                dane(i, 2) = dane(i, 2) + 1;
            end
        end
    end
    
    centers = dane(:, 1);
    n = dane(:, 2);
end

% Normalizacja histogram. Polega to na tym, że liczymy polę histogramu,
% a następnie dzielimy wszystkie słupki, przez wartość tego pola.
function [n_norm, centers_norm] = histogram_norm(n, centers)
    dist = abs(centers(1) - centers(2));
    pole = 0;

    for i = 1:length(n)
        pole = pole + dist * n(i);
    end

    n = n / pole;
    n_norm = n;
    centers_norm = centers;
end

% Funkcja licząca dystybuantę.
function [points, dystr] = dystrybuanta(X)
    X = sort(X);
    step = 1 / length(X);
    dane = [0, 0];
    for el = X'
        dane(end + 1, :) = [el, dane(end, 2) + step];
    end
    dane = dane(2:end, :);
    points = dane(:, 1);
    dystr = dane(:, 2);
end