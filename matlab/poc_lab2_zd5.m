clear all; close all; clc;

% Odczyt obrazu i zapis
img = im2double(rgb2gray(imread('img/moje_512/img_2.jpg')));
imwrite(img, 'o1.png');

% Wygenerowanie zakłócenia sinusoidalnego.
a = 1; b = 5;
j = 0;
a1 = zeros(512);
for u = 1:512
    for v = 1:512
        a1(u, v) = uint8(127 + 128 * sin(u/a(1) + v/b(1)));
    end
end
% Zmniejszam wpływ zak. sin.
a1 = double(a1) / 255;
a1 = a1 / 5;
a1 = a1 - max(a1(:)) / 2;
% Zakłócam obraz.
img = img + a1;
imwrite(img, 'zak.png');

% Za pomocą funkcji Gaussa tworzę 'obręcz', która
% pełni rolę filtru środkowozaporowego
% Zmieniając wartości dwóch ostatnich argumentów tej funkcji
% możemy dostosować filtr do naszych potrzeb, tzn. do zakłócenia
% na naszym obrazie.
for i = 1:512
    for j = 1:512
        % Najlepiej byłoby gdyby parametry były wybierane automatycznie, w kodzie,
        % ale nie umiałem tego zrobić.
        gauss(i, j) = normpdf(norm([i - 256, j - 256]), 80, 20);
    end
end
% Normalizacja.
gauss = gauss - min(gauss(:));
gauss = gauss / max(gauss(:));

% Odwracam maskę, by tylko 'pierścień' oznaczał tłumione składowe.
mask = imcomplement(gauss);
imwrite(mask, 'maska.png');

% Widmo mocy.
L1 = fftshift(fft2(img));
figure; imshow(log(1 + abs(L1)), []);
saveas(gcf, 'fft_before.png');

% Filtracja.
L1 = L1 .* mask;
figure; imshow(log(1 + abs(L1)), []);
saveas(gcf, 'fft_after.png');

% Transformacja odwrotna.
img = real(ifft2(ifftshift(L1)));
imwrite(img, 'obraz_after.png');
