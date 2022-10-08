%%% %%% ZADANIE NR 2

clear all; close all; clc;

% Funkcja
f = @(x) 0.1 * (x.^4 - 20*x.^2 + 5*x);

% x początkowy oraz zakres dla wykresu [-5, 5]
x = -5:1e-2:5;
x_0 = -0;

% Znajdujemy minimum oraz wartość w tym punkcie
[x_min, f_min] = fminsearch(f, x_0);

% Wykres
hold on;
plot(x, f(x), 'LineWidth', 2.0);
plot(x_0, f(x_0), 'gx', 'LineWidth', 2.0);
plot(x_min, f_min, 'rx', 'LineWidth', 2.0);
hold off;
legend('f(x)', 'f(x_0)', 'f(x_{opt})', 'location', 'north');
grid; xlabel('x'); ylabel('f(x)');
name = ['zd2_x', num2str(x_0), '.pdf'];
exportgraphics(gcf, name, 'ContentType', 'Vector'); close all;



%%% %%% ZADANIE NR 3

sigma = 0.0025;
f = @(x) 0.1 * (x.^4 - 20*x.^2 + 5*x) + normrnd(0, sigma, 1, length(x));



%%% %%% ZADANIE NR 4

clear all; close all; clc;

% Funkcja. f_plot wykorzystujemy tylko do wykreślenia
f = @(x) x(1) .* sin(x(1)) + x(2) .* sin(x(2));
f_plot = @(x1, x2) x1 .* sin(x1) + x2 .* sin(x2);

% Zakres x
x = 0:1e-1:12;
[X, Y] = meshgrid(x, x);

% Wykres
surf(X, Y, f_plot(X, Y));

% Punkt poczatkowy
x_0 = [3, 3];

[x_min, f_min] = fminsearch(f, x_0);

% Umieszczamy punkty na wykresie
hold on;
plot3(x_0(1), x_0(2), f(x_0), 'g.', 'Markersize', 20);
plot3(x_min(1), x_min(2), f_min, 'r.', 'Markersize', 20);
hold off;
xlabel('x1'); ylabel('x2'); zlabel('f(x1, x2)');
legend('f(x1, x2)', 'f(x_0)', 'f(x_{opt})');



%%% %%% ZADANIE NR 5

clear all; close all; clc;

T_cel = [23.6, 30, 37.5, 44, 50, 57, 64, 70, 75];
T = T_cel + 273.15; % Zmiana jednostki - na kelwiny

R = [10.5, 8.05, 6.1, 4.78, 3.88, 2.98, 2.34, 1.93, 1.64];
R = R * 1e+3; % Usuwamy przedrostek

% Wartości wynikające z przekształcenia funkcyjnego
x = 1 ./ T; y = log(R);

% Wyznaczamy parametry funkcji liniowej
% i podstawiamy je do oznaczeń z instrukcji
p = polyfit(x, y, 1);
b_1 = p(1); b_0 = p(2);

% Model liniowy
reg_lin = @(x) b_1 * x + b_0;
x_temp = linspace(min(x), max(x), 30);

% Budujemy wykres.
hold on;
plot(x, y, 'r*');
plot(x_temp, reg_lin(x_temp), 'b');
hold off; grid;
xlabel('1 / T [1/K]'); ylabel('ln(R_T) [-]');
legend('Dane z tabeli', 'Wyznaczony model liniowy', ...
	'Location', 'North');
exportgraphics(gcf, 'zd5_img1.pdf', 'ContentType', 'Vector'); close all;

% Wartości wynikające z przekształceń
A = exp(b_0); B = b_1;
R_model = @(t) A * exp(B ./ t);
T_arg = linspace(min(T), max(T), 50);

% Porównanie wyznaczonych wartości modelu z danymi kalibracyjnymi
subplot(2, 1, 1);
hold on;
plot(T-273.15, R*1e-3, 'r*');
plot(T_arg-273.15, R_model(T_arg)*1e-3, 'b');
hold off; grid; ylabel('R_T [k\Omega]');
legend('Dane z tabeli', 'Wyznaczony model', 'Location', 'North');

% Wykres błędu dopasowania
err = abs(R - R_model(T));
subplot(2, 1, 2);
plot(T-273.15, err*1e-3, 'k*');
xlabel('T [^{o}C]'); grid; ylabel('\Delta [k\Omega]');
exportgraphics(gcf, 'zd5_img2.pdf', 'ContentType', 'Vector'); close all;



%%% %%% Minimalizacja wskaźnika

% Wskaźnik, który chcemy minimalizować
J = @(coef) sum((R - coef(1) .* exp(coef(2) ./ T)).^2)
coef_0 = [0, 60]; % Punkt początkowy
[coef, f_min] = fminsearch(J, coef_0);

% Wyznaczone wartości
A = coef(1); B = coef(2);

J([A, B])