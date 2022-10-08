clear all;
close all;
clc;

% Ścieżki używanych plików.
path_gray = '../obrazy/TestImages1/gray/';
path_color = '../obrazy/TestImages1/color/';
name_gray_1 = '7_1_03_512x512.bmp';
name_gray_2 = 'boat_512x512.bmp';
name_color_1 = 'baboon_512x512.bmp';
name_color_2 = 'fruits_512x512.bmp';

% Odczyt obrazu oryginalnego i jego zapis.
img = [path_gray, name_gray_2];
img = imread(img);
imwrite(img, 'zd2a_o2.png');

% Tworzę i zapisuję obraz zaszumiony.
img_n_1 = imnoise(img, 'salt & pepper', 0.01);
imwrite(img_n_1, 'zd2a_o2_s1.png');

% Tworzę używane przeze mnie okno.
okno = [7, 7];

% Używam mojej funkcji.
img_filt = moja_mediana(img_n_1, okno);

% Usuwam brzegi z obrazu oryginalnego i odfiltrowanego.
img_filt = img_filt( ...
            1 + floor(okno(1) / 2) : end - floor(okno(1) / 2), ...
            1 + floor(okno(2) / 2) : end - floor(okno(2) / 2) ...
);
img = img( ...
            1 + floor(okno(1) / 2) : end - floor(okno(1) / 2), ...
            1 + floor(okno(2) / 2) : end - floor(okno(2) / 2) ...
);

% Zapisuję obraz przefiltrowany, oraz wyświetlam PSNR.
imwrite(img_filt, 'zd2a_o2_m7.png');
psnr(img_filt, img)

% Moja funkcja filtrująca.
function output = moja_mediana(img, okno)
    % Tworzę pustą macierz "wynikową".
    output = uint8(zeros(size(img)));
    
    % W pętli filtruję każdy piksel z pominięciem brzegów.
    for i = 1 + floor(okno(1) / 2) : size(img, 1) - floor(okno(1) / 2)
       for j = 1 + floor(okno(2) / 2) : size(img, 2) - floor(okno(2) / 2)
            % Tworzę otoczenie badanego piksela
            otoczenie = img( ...
                i - floor(okno(1) / 2) : i + floor(okno(1) / 2), ...
                j - floor(okno(2) / 2) : j + floor(okno(2) / 2) ...
            );
            % Tworzę wektor z macierzy, następnie sortuję wektor.
            otoczenie = reshape(otoczenie, [], 1);
            otoczenie = sort(otoczenie);
            % Medianą jest wartość po środku.
            mediana = otoczenie(ceil(length(otoczenie) / 2));
            output(i, j) = mediana;
        end
    end
end
