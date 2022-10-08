clear all;
close all;
clc;

% ROTATE
% Odczyt obrazu. Zabieram jeden wymiar, odczytywany jest jako RGB,
% ale jest binarny, więc potrzebuję jednego wymiaru.
img = imread('img/trojkat_0.png');
img = img(:, :, 1);
j = 0;
figure;
for i = 1:25:91
    j = j + 1;
    img_r = imrotate(img, i - 1, 'crop');
    L1 = fftshift(fft2(img_r));
    subplot(3, 4, j); imshow(img_r, []);
    title(['kat = ', num2str(i - 1)]);
    subplot(3, 4, j + 4); imshow(log(1 + abs(L1)), []);
    subplot(3, 4, j + 8); imshow(angle(L1), []);
end
subplot(3, 4, 1); ylabel('Obrazy');
subplot(3, 4, 5); ylabel('Widma mocy');
subplot(3, 4, 9); ylabel('Widma fazowe');

% TRANSLATE
img = imread('img/trojkat_0.png');
img = img(:, :, 1);
trans = [0, 0; 125, 0; 80, -110; -50, 60];
figure;
for i = 1:4
    img_r = imtranslate(img, trans(i, :));
    L1 = fftshift(fft2(img_r));
    subplot(3, 4, i); imshow(img_r, []);
    title(['trans = [', num2str(trans(i, 1)), ', ', num2str(trans(i, 2)), ']']);
    subplot(3, 4, i + 4); imshow(log(1 + abs(L1)), []);
    subplot(3, 4, i + 8); imshow(angle(L1), []);
end
subplot(3, 4, 1); ylabel('Obrazy');
subplot(3, 4, 5); ylabel('Widma mocy');
subplot(3, 4, 9); ylabel('Widma fazowe');

% RESIZE
img = imread('img/kolo_00.png');
img = img(:, :, 1);
res = [1, 1.4, 2.1, 2.75];
figure;
for i = 1:4
    img_r = imresize(img, res(i));
    x = ceil((size(img_r, 1) - 512) / 2) + 1;
    y = ceil((size(img_r, 2) - 512) / 2) + 1;
    img_r = img_r(x : x + 511, y : y + 511);
    L1 = fftshift(fft2(img_r));
    subplot(3, 4, i); imshow(img_r, []);
    title(['resize = ', num2str(res(i))]);
    subplot(3, 4, i + 4); imshow(log(1 + abs(L1)), []);
    subplot(3, 4, i + 8); imshow(angle(L1), []);
end
subplot(3, 4, 1); ylabel('Obrazy');
subplot(3, 4, 5); ylabel('Widma mocy');
subplot(3, 4, 9); ylabel('Widma fazowe');

% RURKI
img = uint8(zeros(512));
img(180:181, 150:350) = 255;
img(210:211, 150:350) = 255;
img(240:241, 150:350) = 255;
img(270:271, 150:350) = 255;

j = 0;
figure;
for i = 1:25:91
    j = j + 1;
    img_r = imrotate(img, i - 1, 'crop');
    L1 = fftshift(fft2(img_r));
    subplot(3, 4, j); imshow(img_r, []);
    title(['kat = ', num2str(i - 1)]);
    subplot(3, 4, j + 4); imshow(log(1 + abs(L1)), []);
    subplot(3, 4, j + 8); imshow(angle(L1), []);
end
subplot(3, 4, 1); ylabel('Obrazy');
subplot(3, 4, 5); ylabel('Widma mocy');
subplot(3, 4, 9); ylabel('Widma fazowe');

% SIN
a = 10 * [1, -1, 2, -4000];
b = 5 * [1, 1, 4, 2];
j = 0;
a1 = zeros(512);
figure;
for i = 1:4
    for u = 1:512
        for v = 1:512
            a1(u, v) = uint8(127 + 128 * sin(u/a(i) + v/b(i)));
        end
    end
    j = j + 1;
    L1 = fftshift(fft2(a1));
    
    subplot(3, 4, j); imshow(a1, []);
    title(['a, b = ', num2str(a(i)), ',', num2str(b(i))]);
    subplot(3, 4, j + 4); imshow(log(1 + abs(L1)), []);
    subplot(3, 4, j + 8); imshow(angle(L1), []);
end
subplot(3, 4, 1); ylabel('Obrazy');
subplot(3, 4, 5); ylabel('Widma mocy');
subplot(3, 4, 9); ylabel('Widma fazowe');