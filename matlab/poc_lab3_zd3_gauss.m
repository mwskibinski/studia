clear all; close all; clc;

% Odczyt obrazu.
img = rgb2gray(im2double(imread('./my_img/img_1.jpg')));
% Jeśli obraz tego wymaga to odwracam poziomy szarości.
img = imcomplement(img);
imwrite(img, 'o_2_g.png');

% Zakłócenie szumem gaussowskim.
img_n = imnoise(img, 'gaussian', 0.05);
imwrite(img_n, 'n_2_g.png');

% Filtracja wg. kodu z ostatnich zajęć poprzedniego semestru
% Wytłumaczenie na dole.
img_filt = filt_gauss_noise(img_n, [5, 5]);
imwrite(img_filt, 'f_2_g.png');

% Histogramy
figure; imhist(img);
figure; imhist(img_n);
figure; imhist(img_filt);

% Wybór progu na podstawie histogramu.
prog = 0.4;
prog_n = 0.45;
prog_filt = 0.45;

% Binaryzacja przy pomocy "macierzowości" MATLABa, tzn.
% nie wykonuję tego w pętli.
bin_img = img > prog;
bin_img_n = img_n > prog_n;
bin_img_filt = img_filt > prog_filt;

imwrite(bin_img, 'bin_img_2_g.png');
imwrite(bin_img_n, 'bin_img_n_2_g.png');
imwrite(bin_img_filt, 'bin_img_filt_2_g.png');

% Etykietowanie.
[L, n] = bwlabel(bin_img);                bw_img = label2rgb(L);
[L_n, n_n] = bwlabel(bin_img_n);          bw_img_n = label2rgb(L_n);
[L_filt, n_filt] = bwlabel(bin_img_filt); bw_img_filt = label2rgb(L_filt);

% Zapis
imwrite(bw_img, 'bw_img_2_g.png');
imwrite(bw_img_n, 'bw_img_n_2_g.png');
imwrite(bw_img_filt, 'bw_img_filt_2_g.png');

% Kod, który został stworzony na potrzeby ostatnich zajęć w zeszłym
% semstrze "Filtracja liniowa"
% Najpierw obraz przechodzi przez filtr dolnoprzepustowy, u mnie jest
% średnia z "okna", a następnie przez f. górnoprzepustowy,
% u mnie jest to laplasjan.
% Poza tym usuwane są krawędzie.
% Ciało funkcji pozostawiam już bez komentarza.
function img_filt = filt_gauss_noise(img_n, lowpass)
    filter_i = lowpass(1); filter_j = lowpass(2);

    mask = ones(filter_i, filter_j);
    mask = mask ./ sum(sum(mask));
    
    img_filt = my_filter(img_n, mask);
    img_filt = img_filt( ...
        1 + floor(filter_i / 2) : end - floor(filter_i / 2), ...
        1 + floor(filter_j / 2) : end - floor(filter_j / 2) ...
        );

    mask = [0, -1, 0; -1, 5, -1; 0, -1, 0];
    filter_i = size(mask, 1) ; filter_j = size(mask, 2);
    img_filt = my_filter(img_filt, mask);

    img_filt = img_filt( ...
        1 + floor(filter_i / 2) : end - floor(filter_i / 2), ...
        1 + floor(filter_j / 2) : end - floor(filter_j / 2) ...
        );

    function output = my_filter(img, mask)
        output = zeros(size(img, 1), size(img, 2));
        filter_i = size(mask, 1); filter_j = size(mask, 2);

        for i = 1 + floor(filter_i / 2):size(img, 1) - floor(filter_i / 2)
            for j = 1 + floor(filter_j / 2):size(img, 2) - floor(filter_j / 2)
                tmp = img( ...
                    i - floor(filter_i / 2) : i + floor(filter_i / 2), ...
                    j - floor(filter_j / 2) : j + floor(filter_j / 2) ...
                );
                output(i, j) = sum(sum(tmp .* mask));
            end
        end
    end
end