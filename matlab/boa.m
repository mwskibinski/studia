clear; close all; clc;

% Mapa.
% img = ~logical(rgb2gray(imread('./mapy/mapa_3_extended.png')));
img = ~logical(rgb2gray(imread('./mapy/mapa_1_gruch.png')));
% img = ~logical(rgb2gray(imread('./mapy/AnschlussBOA.png')));
% img = imresize(img, 10);
map = binaryOccupancyMap(img);

% Pojazd.
R = 0.1; L = 0.5;
vehicle = DifferentialDrive(R, L);

% Wizualizacja.
viz = Visualizer2D;
viz.mapName = 'map';
goal = [ % Cel - pozycja losowa.
    rand * size(img, 1);
    rand * size(img, 2)
]';
goal = [186, 554]; % Cel - nielosowany.
viz.hasWaypoints = true;

% Lidar.
lidar = LidarSensor;
lidar.scanAngles = linspace(-pi/2, pi/2, 10);
lidar.maxRange = 75;
attachLidarSensor(viz, lidar);

% Dane początkowe.
pose = [ % Pozycja poczatkowa - losowana.
    rand * size(img, 1);
    rand * size(img, 2);
    (rand - 0.5) * 2*pi
];
pose = [257; 111; pi];     % Pozycja początkowa - nielosowana.
predkosc_liniowa = 1;       % Pożądana prędkość liniowa.
wL = 10; wR = 10;           % Wartości początkowe pr. kół.
dt = 1;                     % Krok czasowy.


% goal
% 186.7, 554

% pose
% 257.3, 111.5, pi

% Pętla symulacyjna.
for t = 0:dt:90000
    % Zadanie proste kinematyki.
    % Znając prędkości kół, dostajemy prędkość liniową i kątową.
    % Znając krok czasowy znamy jak zmieni się pozycja robota.
    [v, w] = forwardKinematics(vehicle, wL, wR);
    angle = w * dt;
    x = v * cos(pose(3)) * dt;
    y = v * sin(pose(3)) * dt;
    
    % Aktualizacja pozycji.
    pose = pose + [x; y; angle];
    
    % Odczyty lidara.
    ranges = lidar(pose);
    
    % Dystans od celu.
    dist = sqrt((pose(1) - goal(1))^2 + (pose(2) - goal(2))^2);
    
    
    %%%
    %%%     ZACHOWANIA I KOORDYNACJA
    %%%
        
    % Jeśli odległość robota od celu jest mała,
    % to zmieniamy kąt skretu - im bliżej tym ciaśniejszy.
    if dist > 50
        radius = 50;
    else
        radius = (1/sqrt(50) * dist).^2;
    end
    
    
    % Prędkości obu kół wynikające z przeszkód.
    omega_obstacle_L = sum( ...
        f_sigmoidalna(0.1, radius, ranges(1:5)) .* linspace(0.01, 0.1, 5)');
    omega_obstacle_R = sum( ...
        f_sigmoidalna(0.1, radius, ranges(6:10)) .* linspace(0.1, 0.01, 5)');
    
    % Koordynacja obu tych prędkości.
    omega_obstacle = omega_obstacle_L - omega_obstacle_R;
    
    
    % Kąt celu.
    angle_goal = atan2(goal(2) - pose(2), goal(1) - pose(1));
    
    % Różnica kątowa wraz z rozwiązaniem problemu nieciągłości
    % funkcji atan2.
    angle_diff = atan2( ...
        sin(angle_goal - pose(3)), ...
        cos(angle_goal - pose(3)) ...
    );

    % Prędkość kątowa wynikająca z różnicy kątowej.
    omega_goal = angle_diff * 0.1;
    
    % Mechanizm antyzatrzymaniowy.
    if predkosc_liniowa < 0.01
        omega_goal = (angle_diff - pi/2) * 0.1;
    end
    
    
    % Zmiany prędkości liniowej w funkcji odległości od przeszkody.
    % Zabezpiecza robota przed kolizją.
    if min(ranges) < 20
        predkosc_liniowa = (1/20 * min(ranges))^2;
    else
        predkosc_liniowa = 1;
    end
    
    
    % Współczynnik alfa, przyjmuje wartości z zakresu 0 do 1.
    % Odpowiada za fuzję prędkości kątowych wyzacznonych od
    % celu oraz przeszkód.
    alfa = f_sigmoidalna(0.1, radius, min(ranges));
    
    % Prędkość kątowa ostateczna.
    omega = alfa * omega_obstacle + (1 - alfa) * omega_goal;
    
    
    % Kinematyka odwrotna - otrzymujemy prędkości
    % obu kół wykorzystywane w następnej chwili czasowej.
    [wL, wR] = inverseKinematics(vehicle, predkosc_liniowa, omega);
    
    
    %%%
    %%%     RESZTA
    %%%
    
    % Aktualizacja wizualizacji, co pewien krok czasowy.
    if mod(t, 10) == 0
        viz(pose, goal, ranges);
        % pause(0.01);
    end
    
    % Jeśli robot jest bardzo blisko celu to wyłącz symulację.
    if dist < 5
        break;
    end
end



function ret = f_sigmoidalna(alpha, D_L0, D_L)
    % Jeśli D_L = NaN, to znaczy że lidar nic nie wykrył.
    % Możemy zatem przyjąć, że przeszkoda jest w nieskończoności.
    D_L(isnan(D_L)) = inf;
    
    % Funkcja sigmoidalna nastrojona według argumentów wejściowych.
    ret = 1 ./ (1 + exp(alpha .* (D_L - D_L0)));
end