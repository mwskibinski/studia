// Line follower -- system wbudowany
// Sekcja nr 1:
//    Maksymilian Skibiński,
//    Paweł Kaźmieruk.
// AiR S2-I/Rob, 2021 r.

// Obsługa mostku H.
#define H_LEFT_PWM 5
#define H_LEFT_DIR 4
#define H_RIGHT_PWM 6
#define H_RIGHT_DIR 9

// Oznaczenie kierunku ruchu na bazie znaku prędkości.
#define FORWARD 0
#define BACKWARDS 1

// Oznaczenie położenia silnika.
#define LEFT 0
#define RIGHT 1

// Maksymalne dozwolone wypełnienie sygnału PWM.
#define PWM_MAX 165

// Piny sensorów analogowych.
#define SENSOR_ANALOG_LEFT A1
#define SENSOR_ANALOG_RIGHT A0

// Piny sensorów cyfrowych.
#define SENSOR_DIGITAL_LEFT 12
#define SENSOR_DIGITAL_RIGHT 8

void setup() {
    // Deklaracja pinów mostka H.
    pinMode(H_LEFT_DIR, OUTPUT);
    pinMode(H_RIGHT_DIR, OUTPUT);
    pinMode(H_LEFT_PWM, OUTPUT);
    pinMode(H_RIGHT_PWM, OUTPUT);

    // W przypadku, gdy chcemy komunikacji robota z komputerem,
    // by odczytać sensory na ekranie, aktywujemy taką komunikację
    // linijką poniżej.
    
    //Serial.begin(9600);
}


// Prędkości względem których algorytm sterowania
// oblicza używane prędkości.
#define BASE_VEL 70
#define PODLOGA_VEL 20
#define TURN_VEL 40

// Kluczowe wskazania sensorów analogowych. Zależą one,
// głównie od warunków ,,podłogowych'':
//    - DOLNA_GRANICA to troche ponad dolna granica wskazania na lini,
//    - TURN_MEAN to troche ponad wskazanie podlogowe,
//    - PODLOGA_MEAN to srednia ze wskazan na podlodze.
#define DOLNA_GRANICA 620
#define TURN_MEAN 600
#define PODLOGA_MEAN 550

// Zmienne:
//    - digSensor -- zapamiętuje, który sensor cyfrowy wskazał jako ostatni,
//    - wskazania czujników analogowych i ich średnia,
//    - zmienne zapamiętujące wyznaczane prędkości,
//    - zmienne algorytmu sterowania.
byte digSensor = 0;
int leftSensor, rightSensor, meanSensor;
int baseVel, leftVel, rightVel, turnVel;
int control, error, errorPrev = 0;

// Nastawy regulatora PD.
float P = 0.9;
float D = 0.3;

void loop() {
    // Odczyt sensorów analogowych.
    leftSensor = analogRead(SENSOR_ANALOG_LEFT);
    rightSensor = analogRead(SENSOR_ANALOG_RIGHT);

    // Zmienna digSensor zapamiętuje, który sensor cyfrowy wykrył linię jako ostatni.
    digSensor = digitalRead(SENSOR_DIGITAL_LEFT) ? SENSOR_DIGITAL_LEFT : digSensor;
    digSensor = digitalRead(SENSOR_DIGITAL_RIGHT) ? SENSOR_DIGITAL_RIGHT : digSensor;

    // Średnia z pomiarów czujników analogowych.
    meanSensor = (rightSensor + leftSensor) / 2.0;
        
    // W zależności od wskazania średniego sensorów analogowych robot:
    //    - realizuje sterowanie według regulatora PD,
    //    - realizuje nawracanie.
    if (meanSensor > TURN_MEAN) {
        // ,,Prędkość bazowa'' -- jeśli średnia z sensorów jest za niska to pr. bazowa 
        // zostaje zmniejszona, bo oznacza to, że pojazd nie jest ponad linią.
        baseVel = meanSensor <= DOLNA_GRANICA ?
            map(meanSensor, PODLOGA_MEAN, DOLNA_GRANICA, PODLOGA_VEL, BASE_VEL)
            : BASE_VEL;
    
        // Uchyb z sensorów analogowych, który jest od razu liniowo przekształcany
        // na zmiany prędkości (równie dobrze nastawy regulatora
        // mogłyby się tym zająć).
        error = map(rightSensor - leftSensor, -150, 150, -baseVel, baseVel);
    
        // Sygnał sterujący regulatora PD.
        control = P * error + D * (error - errorPrev);
    
        // Ograniczenie górne -- jeśli sygnał sterujący jest
        // zbyt duży to stosujemy gorną granicę.
        if (abs(control) > baseVel)
            control = control > 0 ? baseVel : -baseVel;
    
        // Sygnał sterujący trafia na oba koła z innym znakiem.
        leftVel = baseVel + control;
        rightVel = baseVel - control;
    
        // Chcemy jedynie zmniejszyć jedną z prędkości, zatem jeśli, któraś
        // wzrosła to wraca ona na poziom prędkości ,,bazowej''.
        leftVel = leftVel > baseVel ? baseVel : leftVel;
        rightVel = rightVel > baseVel ? baseVel : rightVel;
    } else {
        // W zależności od tego, który sensor cyfrowy wykrył linię jako ostatni
        // wyznaczany jest odpowiedni kierunek zwrotu.
        turnVel = digSensor == SENSOR_DIGITAL_LEFT ? -TURN_VEL : TURN_VEL;

        // Robot nie wykonuje obrotu w miejscu, bo znajduje się trochę za linią
        // dlatego podawane prędkości są odpowiednio przemnażane, by
        // obrócił się oraz wrócił do tyłu.
        leftVel = turnVel < 0 ? turnVel * 1.5 : turnVel * 0.75;
        rightVel = -turnVel < 0 ? -turnVel * 1.5 : -turnVel * 0.75;        
    }

    // Ostatecznie, prędkości trafiają do silników.
    motor(leftVel, LEFT);
    motor(rightVel, RIGHT);    

    // Jeśli chcemy odczytać sensory na komputerze, to
    // wykorzystujemy zakomentowaną funkcję.
    
    //sensorPrint();

    // Uchyb poprzedni, dla następnej iteracji.
    errorPrev = error;
}

// Funkcja do przekazywania sygnału sterowania na silniki.
// Przyjmuje dwa argumenty:
//    - vel -- prędkość zadana w procentach, uwzględniany jest znak,
//    - leftRight -- który silnik.
void motor(int vel, byte leftRight) {
    // Uwzględnienie kierunku.
    byte dir = vel > 0 ? FORWARD : BACKWARDS;

    // Przekształcenie prędkości w procentach,
    // na użytkowany zakres sygnału PWM.
    vel = map(abs(vel), 0, 100, 0, PWM_MAX);

    // Ostatecznie, przypisanie prędkości wraz z obsługą mostka H.
    digitalWrite(leftRight == LEFT ? H_LEFT_DIR : H_RIGHT_DIR, dir);
    analogWrite(leftRight == LEFT ? H_LEFT_PWM : H_RIGHT_PWM, vel);
}

// Funkcja zajmująca się wypisywaniem poprzez monitor portu szeregowego
// wskazań sensora, i ich średniej.
// Funkcja jest istotna na etapie ,,strojenia'' potencjometrów czujników analogowych.
void sensorPrint(void) {
    Serial.print("l: ");
    Serial.print(leftSensor);
    Serial.print(",  r: ");
    Serial.print(rightSensor);
    Serial.print(" ==> mean: ");
    Serial.println(meanSensor);
    delay(250);
}
