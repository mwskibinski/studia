clear all; close all; clc;

% Odczyt obrazu.
img = rgb2gray(im2double(imread('./my_img/img_9.jpg')));
% Jeśli obraz tego wymaga to odwracam poziomy szarości.
% img = imcomplement(img);
imwrite(img, 'o_2.png');

% Szum impulsowy o dużej gęstości. Wybieram 0.1
img_n = imnoise(img, 'salt & pepper', 0.1);
imwrite(img_n, 'n_2.png');

% Filtrowanie medianowe. Funkcja z pierwszych zajęć lab.
okno = [3, 3];
img_filt = my_median(img_n, okno);
imwrite(img_filt, 'f_2.png');

% Histogramy
figure; imhist(img);
figure; imhist(img_n);
figure; imhist(img_filt);

% Wybór progu na podstawie histogramu.
% Ze wzgledu na charakter 'salt & pepper' dla obrazu oryginalnego
% i zaszumionego progi wybiorę te same.
% Dla przefiltrowanego wybiorę inny lub ten sam.
prog = 0.55;
prog_n = prog;
prog_filt = 0.4;

% Binaryzacja przy pomocy "macierzowości" MATLABa, tzn. nie wykonuję tego w
% pętli.
bin_img = img > prog;
bin_img_n = img_n > prog_n;
bin_img_filt = img_filt > prog_filt;

imwrite(bin_img, 'bin_img_2.png');
imwrite(bin_img_n, 'bin_img_n_2.png');
imwrite(bin_img_filt, 'bin_img_filt_2.png');

% Etykietowanie.
[L, n] = bwlabel(bin_img);           bw_img = label2rgb(L);
[L_n, n_n] = bwlabel(bin_img_n);       bw_img_n = label2rgb(L_n);
[L_filt, n_filt] = bwlabel(bin_img_filt); bw_img_filt = label2rgb(L_filt);

% Zapis
imwrite(bw_img, 'bw_img_2.png');
imwrite(bw_img_n, 'bw_img_n_2.png');
imwrite(bw_img_filt, 'bw_img_filt_2.png');

% Filtracja za pomocą mediany, z zajęć nr 1. Kod pozostawiam bez komentarza
% po "przeszedł" przez tamte zajęcia.
function output = my_median(img, okno)
    output = zeros(size(img));
    
    for i = 1 + floor(okno(1) / 2) : size(img, 1) - floor(okno(1) / 2)
       for j = 1 + floor(okno(2) / 2) : size(img, 2) - floor(okno(2) / 2)
            otoczenie = img( ...
                i - floor(okno(1) / 2) : i + floor(okno(1) / 2), ...
                j - floor(okno(2) / 2) : j + floor(okno(2) / 2) ...
            );
            otoczenie = sort(otoczenie(:));
            mediana = otoczenie(ceil(length(otoczenie) / 2));
            output(i, j) = mediana;
        end
    end
    
    output = output( ...
                1 + floor(okno(1) / 2) : end - floor(okno(1) / 2), ...
                1 + floor(okno(2) / 2) : end - floor(okno(2) / 2) ...
    );    
end
