% Tu na górze mam kod odczytujący obrazy oraz zapisujący je
% I wykonujący jakieś inne czynności wstępne.

% Tworzę używane przeze mnie okno, oraz parametr k.
okno = [3, 3];
k = 4;

% Używam mojej funkcji.
img_filt = moj_LUM(img_n_1, okno, k);

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
imwrite(img_filt, 'zd2b_o1_m3_4.png');
psnr(img_filt, img)

% Tworzę funkcję filtrującą LUM.
function output = moj_LUM(img, okno, k)
    % Tworzę pustą macierz wynikową.
    output = uint8(zeros(size(img)));
    
    % Przetwarzam wszystkie piksele poza tymi na brzegach.
    for i = 1 + floor(okno(1) / 2) : size(img, 1) - floor(okno(1) / 2)
       for j = 1 + floor(okno(2) / 2) : size(img, 2) - floor(okno(2) / 2)
            % Tworzę macierz zawierającą otoczenie badanego piksela.
            otoczenie = img( ...
                i - floor(okno(1) / 2) : i + floor(okno(1) / 2), ...
                j - floor(okno(2) / 2) : j + floor(okno(2) / 2) ...
            );
            % Zamieniam macierz
            otoczenie = sort(reshape(otoczenie, [], 1));
            srodek = ceil(length(otoczenie) / 2);
            x_L = otoczenie(srodek - k);
            x_U = otoczenie(srodek + k);
            x_0 = img(i, j);
            mediana = sort([x_L, x_0, x_U]);
            output(i, j) = mediana(2);
        end
    end
end
