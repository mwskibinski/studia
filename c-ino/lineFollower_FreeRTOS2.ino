// Line follower -- system FreeRTOS
// Sekcja nr 1:
//    Maksymilian Skibiński,
//    Paweł Kaźmieruk.
// AiR S2-I/Rob, 2021 r.

// Ładujemy FreeRTOSa oraz obsługę kolejek.
#include <Arduino_FreeRTOS.h>
#include <queue.h>

// W kodzie tworzymy 3 zadania:
//    - taskSensors -- obsługa sensorów,
//    - taskControl -- generowanie sterowania na podstawie danych z sensorów,
//    - taskMotors -- podawanie sygnałów sterujących na silniki.
void taskSensors(void *pvParameters);
void taskControl(void *pvParameters);
void taskMotors(void *pvParameters);

// Pomiędzy zadaniami są 2 kolejki.
QueueHandle_t sensorQueue;
QueueHandle_t controlQueue;

// Paczka danych wysyłana przez taskSensors do taskControl.
struct sensorStruct {
    int leftSensor, rightSensor;
    byte digSensor;
};

// Paczka danych wysyłana przez taskControl do taskMotors.
struct controlStruct {
    int leftVel, rightVel;
};

void setup() {
    // Tworzymy dwie kolejki do wymiany danych o odpowiednich rozmiarach (w bajtach)
    // i założyliśmy kolejki o długości 10 wiadomości.
    sensorQueue = xQueueCreate(10, sizeof(struct sensorStruct));
    controlQueue = xQueueCreate(10, sizeof(struct controlStruct));

    // Jeśli kolejki zostały utworzone, tworzymy zadania.
    // Zadania różnią się priorytetem, gdzie 3 to najwyższy.
    if (sensorQueue != NULL && controlQueue != NULL) {
        xTaskCreate(
            taskSensors, "Obsluga czujnikow",
            128, NULL, 1, NULL
        );
        xTaskCreate(
            taskControl, "Regulacja",
            128, NULL, 2, NULL
        );
        xTaskCreate(
            taskMotors, "Obsluga silnikow",
            128, NULL, 3, NULL
        );        
    }
}

// Funkcja ,,loop'' jest teraz oczywiście pusta.
void loop() { }

// Piny sensorów analogowych.
#define SENSOR_ANALOG_LEFT A1
#define SENSOR_ANALOG_RIGHT A0

// Piny sensorów cyfrowych.
#define SENSOR_DIGITAL_LEFT 12
#define SENSOR_DIGITAL_RIGHT 8

// Zadanie taskSensors zajmuje się odczytem wskazań sensorów
// oraz przekazaniem ich poprzez kolejkę do następnego zadania.
void taskSensors(void *pvParameters) {

    for (;;) {
        struct sensorStruct sensorData;

        // Odczyt sensorów analogowych.
        sensorData.leftSensor = analogRead(SENSOR_ANALOG_LEFT);
        sensorData.rightSensor = analogRead(SENSOR_ANALOG_RIGHT);

        // Zmienna digSensor zapamiętuje, który
        // sensor cyfrowy wykrył linię jako ostatni.
        sensorData.digSensor =
            digitalRead(SENSOR_DIGITAL_LEFT) ? SENSOR_DIGITAL_LEFT
            : sensorData.digSensor;
        sensorData.digSensor =
            digitalRead(SENSOR_DIGITAL_RIGHT) ? SENSOR_DIGITAL_RIGHT
            : sensorData.digSensor;

        // Dane zostają przekazane dalej.
        xQueueSend(sensorQueue, &sensorData, portMAX_DELAY);
    }
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

// Zadanie taskControl zajmuje się wygenerowaniem odpowiednich sygnałów
// sterujących, czyli prędkości dla obu silników.
// Następnie, te prędkości zostają przekazane do zadania taskMotors.
void taskControl(void *pvParameters) {
    // Zmienne:
    //    - meanSensor -- średnia z dwóch sensorów analogowych,
    //    - baseVel, turnVel -- prędkość bazowa dla ruchu wzdłuż linii, i obrotu,
    //    - zmienne algorytmu sterowania.
    int meanSensor;
    int baseVel, turnVel;
    int control, error, error_prev;
    float P, D;
    struct sensorStruct sensorData;

    // Inicjalizacja uchyba poprzedniego oraz nastaw regulatora.
    error_prev = 0;
    P = 0.9;
    D = 0.3;

    for (;;) {
        // Jeśli zadania otrzymało paczkę danych od sensora to kod jest realizowany.
        // W przeciwnym razie zadanie czeka.
        if (xQueueReceive(sensorQueue, &sensorData, portMAX_DELAY) == pdPASS) {
            struct controlStruct controlData;

            // Średnia z pomiarów czujników analogowych.
            meanSensor = (sensorData.rightSensor + sensorData.leftSensor) / 2.0;

            // W zależności od wskazania średniego sensorów analogowych robot:
            //    - realizuje sterowanie według regulatora PD,
            //    - realizuje nawracanie.
            if (meanSensor > TURN_MEAN) {
                // ,,Prędkość bazowa'' -- jeśli średnia z sensorów jest za niska
                // to pr. bazowa zostaje zmniejszona, bo oznacza to,
                // że pojazd nie jest ponad linią.
                baseVel = meanSensor <= DOLNA_GRANICA ?
                    map(meanSensor, PODLOGA_MEAN, DOLNA_GRANICA, PODLOGA_VEL, BASE_VEL)
                    : BASE_VEL;

                // Uchyb z sensorów analogowych, który jest od razu
                // liniowo przekształcany na zmiany prędkości
                // (równie dobrze nastawy regulatora mogłyby się tym zająć).
                error = map(sensorData.rightSensor - sensorData.leftSensor,
                    -150, 150, -baseVel, baseVel);

                // Sygnał sterujący regulatora PD.
                control = P * error + D * (error - error_prev);

                // Ograniczenie górne -- jeśli sygnał sterujący
                // jest zbyt duży to stosujemy gorną granicę.
                if (abs(control) > baseVel)
                    control = control > 0 ? baseVel : -baseVel;

                // Sygnał sterujący trafia na oba koła z innym znakiem.
                controlData.leftVel = baseVel + control;
                controlData.rightVel = baseVel - control;

                // Chcemy jedynie zmniejszyć jedną z prędkości, zatem jeśli, któraś
                // wzrosła to wraca ona na poziom prędkości ,,bazowej''.    
                controlData.leftVel =
                    controlData.leftVel > baseVel ?
                    baseVel : controlData.leftVel;
                controlData.rightVel =
                    controlData.rightVel > baseVel ?
                    baseVel : controlData.rightVel;
            } else {
                // W zależności od tego, który sensor cyfrowy wykrył linię jako ostatni
                // wyznaczany jest odpowiedni kierunek zwrotu.                
                turnVel =
                    sensorData.digSensor == SENSOR_DIGITAL_LEFT ?
                    -TURN_VEL : TURN_VEL;

                // Robot nie wykonuje obrotu w miejscu, bo znajduje się trochę za linią
                // dlatego podawane prędkości są odpowiednio przemnażane, by
                // obrócił się oraz wrócił do tyłu.
                controlData.leftVel = turnVel < 0 ? turnVel * 1.5 : turnVel * 0.75;
                controlData.rightVel = -turnVel < 0 ? -turnVel * 1.5 : -turnVel * 0.75;
            }

            // Dane zostają przekazane do następnego zadania.
            xQueueSend(controlQueue, &controlData, portMAX_DELAY);
            
            // Uchyb poprzedni, dla następnej iteracji.
            error = error_prev;
        }
    }
}


// Obsługa mostku H.
#define H_LEFT_PWM 5
#define H_LEFT_DIR 4
#define H_RIGHT_PWM 6
#define H_RIGHT_DIR 9

// Maksymalne dozwolone wypełnienie sygnału PWM.
#define PWM_MAX 165

// Oznaczenie kierunku ruchu na bazie znaku prędkości.
#define FORWARD 0
#define BACKWARDS 1

void taskMotors(void *pvParameters) {
    // Deklaracja pinów mostka H.
    pinMode(H_LEFT_DIR, OUTPUT);
    pinMode(H_RIGHT_DIR, OUTPUT);
    pinMode(H_LEFT_PWM, OUTPUT);
    pinMode(H_RIGHT_PWM, OUTPUT);

    struct controlStruct controlData;
    byte dir;

    for (;;) {
        // Jeśli zadanie otrzymało paczkę danych od taskControl, to kod
        // jest realizowany. W przeciwnym wypadku zadanie oczekuje.
        if (xQueueReceive(controlQueue, &controlData, portMAX_DELAY) == pdPASS) {
            // Najpierw silnik lewy.
            // Uwzględnienie kierunku.
            dir = controlData.leftVel > 0 ? FORWARD : BACKWARDS;

            // Przekształcenie prędkości w procentach,
            // na użytkowany zakres sygnału PWM.            
            controlData.leftVel = map(abs(controlData.leftVel), 0, 100, 0, PWM_MAX);
            digitalWrite(H_LEFT_DIR, dir);
            analogWrite(H_LEFT_PWM, controlData.leftVel);

            // Teraz to samo dla silnika prawego.
            dir = controlData.rightVel > 0 ? FORWARD : BACKWARDS;
            
            controlData.rightVel = map(abs(controlData.rightVel), 0, 100, 0, PWM_MAX);
            digitalWrite(H_RIGHT_DIR, dir);
            analogWrite(H_RIGHT_PWM, controlData.rightVel);
        }
    }    
}
