/*
*     Autorzy: Maksymilian Skibinski, Jakub Debosz
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512 // Rozmiar stosu kazdego z zadan.
#define  N_TASKS                        5  // Liczba identycznych zadan.
#define N_TASKS_QUE						13 // Rozmiar kolejki dla kolejki zadan obciazajacych. Zadan jest 5, a dla rozmiaru kolejki przyjmuje wzor Rozmiar = 2.5 * tyle ile potrzeba.

#define MSG_QUEUE_SIZE 35 // Rozmiar kolejki dla wyswietlania. Znow rozmiar okreslony analogicznie co ten u gory.
#define BUF_SIZE 11 // Rozmiar wprowadzanego lancucha znakowego. Chcemy wpisywac liczby typu INT32U zatem potrzebujemy 10 znakow + znak '\0'.

#define OBCIAZENIE_INIT 2000 // Jakies poczatkowe obciazenie.

INT8U numbers[5] = {
	0, 1, 2, 3, 4
}; // Potrzebne do poprawnej inicjalizacji struktur z danymi dla zadan obciazajacych.

enum keys {
	KEY_ESCAPE = 0x1B, KEY_BACKSPACE = 0x8, KEY_ENTER = 0xD, KEY_DELETE = 0x53
}; // Wartosci w kodzie szesnastkowym "specjalnych" wprowadzanych znakow. Duzo lepiej stosowac cos takiego niz na etapie rozrozniana znakow w zadaniu edycyjnym poslugiwac sie surowa wartoscia liczbowa.
   // KEY_DELETE to nie jest faktyczna wartosc klawisza delete, a druga wprowadzana wartosc po wcisnieciu go.

enum types {
	S_Q_M = 0, TEXT = 1, ONE_SECOND = 2
}; // Pierwsze pole wysylanych struktur to typ zadan. Potrzebne, przede wszystkim, zeby wiedziec jak interpretowac unie w strukturze.

// Typ struktur dla zadan z obciazeniem
typedef struct {
	char TaskNazwa[4]; // Nazwa zadania.
	void *Os_Event_Ptr; // Potrzebne dla wiadomosci. W moim przypadku 5 wiadomosci nie sa czescia tablicy, tylko sa deklarowane oddzielnie. Mozna by to zrobic prosciej za pomoca wspomnianej tablicy, ale wczesniej wpadlem na ten pomysl i przy nim zostalem.
    INT8U   TaskNumer; // Numer zadania.
	INT8U   TaskPr; // Priorytet zadania.
    INT32U  TaskLicznik; // Aktualny licznik zadania.
    INT32U  TaskObciazenie; // Aktualne obciazenie zadania.
} STRUCT_OBC_DATA;

// Typ struktury dla zadania obslugujacego klawiature.
typedef struct {
    char Text[BUF_SIZE]; // Tekst
	BOOLEAN bl_output; // Czy byl wcisniety Enter ?
} STRUCT_TEXT_DATA;

// Faktycznie wysylana struktura. Jej pole zostaly wytlumaczone wczesniej. Unia jest uzyta z powodu checi oszczedzania na pamieci
typedef struct {
	INT8U type;
	union {
		STRUCT_OBC_DATA ObcData;
		STRUCT_TEXT_DATA TextData;
	} Data;
} TASK_DATA;

// Struktura wysylana do wiadomosci i kolejek za pomoca dynamicznej alokacji pamieci.
typedef struct{
	INT16U NrObc; // Ktory raz wysylamy obciazenie. Potrzebne by w przypadku kolejek poprawnie okreslic kolejnosc obciazen.
	INT8U TaskPr; // Priorytet zadania. Uzywamy by zadanie "kolejkowe" wiedzialo czy obciazenie nalezy do niego czy do innego zadania.
	INT32U TaskObciazenie; // Wprowadzone obciazenie
} MEM_STRUCT;

// Stosy.
OS_STK        TaskSemStk[N_TASKS][TASK_STK_SIZE];
OS_STK        TaskMsgStk[N_TASKS][TASK_STK_SIZE];
OS_STK			TaskQueueStk[N_TASKS][TASK_STK_SIZE];
OS_STK        TaskStartStk[TASK_STK_SIZE];
OS_STK        TaskKlawiaturaStk[TASK_STK_SIZE];
OS_STK        TaskEdycjaStk[TASK_STK_SIZE];
OS_STK        TaskWyswietlanieStk[TASK_STK_SIZE];
OS_STK		  TaskWysylanieStk[TASK_STK_SIZE];

// Wiadomosci, kolejki, semafor, pamiec alokowana dynamicznie.
OS_EVENT	 *CharMsgMBox;
OS_EVENT	 *Queue;
OS_EVENT	 *MsgWysylanie;

OS_EVENT     *ObciazenieSem;
OS_EVENT	 *QueueTask;
OS_EVENT	 *MsgTask_1;
OS_EVENT	 *MsgTask_2;
OS_EVENT	 *MsgTask_3;
OS_EVENT	 *MsgTask_4;
OS_EVENT	 *MsgTask_5;

OS_MEM *MemPtr;

#define N_BLOCKS_2_5 25 // Ile blokow chcemy do dynamicznej alokacji pamieci. Rozmiar = 2.5 * tyle ile nam sie wydaje ze potrzebujemy
#define BLOCK_SIZE_IN_BYTES sizeof(MEM_STRUCT) // Rozmiar bloku pamieci w bajtach

MEM_STRUCT MemStructTable[N_BLOCKS_2_5];

void		*QueueTaskTable[N_TASKS_QUE]; // Tablica dla kolejki zadan obciazajacych
void         *QueueTable[MSG_QUEUE_SIZE]; // Tablica dla kolejki zadania wyswietlajacego

INT32U Obciazenie = OBCIAZENIE_INIT; // Obciazenie obslugiwane przez semafor

// Prototypy funkcji i zadan, czyli de facto funkcji
        void  TaskSem(void *data);
		void  TaskMsg(void *data);
		void  TaskQueue(void *data);
        void  TaskStart(void *data);
		void  TaskKlawiatura(void *data);
		void  TaskEdycja(void *data);
		void  TaskWyswietlanie(void *data);
		void  TaskWysylanie(void *data);
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);

void  main (void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    OSTaskCreate(TaskStart, (void *) 0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
}

void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
	INT8U err;
	TASK_DATA data; // Struktura wykorzystywana dla liczenia delty. Wykorzystuje jedynie pole type. Poza tym jest "pusta" tzn. nie wykorzystuje innych pol.
	data.type = ONE_SECOND;
    pdata = pdata;                                         /* Prevent compiler warning                 */
	
    TaskStartDispInit();                                   /* Initialize the display                   */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

	// Tworzenie kolejek, wiadomosci, semafora i pamieci alokowanej dynamicznej.
	CharMsgMBox = OSMboxCreate((void *) 0);
	Queue = OSQCreate(&QueueTable[0], MSG_QUEUE_SIZE);
	
	ObciazenieSem = OSSemCreate(1);
	QueueTask = OSQCreate(&QueueTaskTable[0], N_TASKS_QUE);
	MsgTask_1 = OSMboxCreate((void *) 0);
	MsgTask_2 = OSMboxCreate((void *) 0);
	MsgTask_3 = OSMboxCreate((void *) 0);
	MsgTask_4 = OSMboxCreate((void *) 0);
	MsgTask_5 = OSMboxCreate((void *) 0);
	
	MsgWysylanie = OSMboxCreate((void *) 0);
	
	MemPtr = OSMemCreate(MemStructTable, N_BLOCKS_2_5, BLOCK_SIZE_IN_BYTES, &err);
	
    TaskStartCreateTasks();                                /* Create all the application tasks         */
    for (;;) {
        TaskStartDisp();                                  /* Update the display                       */
		
        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
		
		OSQPost(Queue, &data); // Wysylam strukture, ktora informuje wyswietlanie ze minela sekunda i czas na liczenie delty.
    }
}


static  void  TaskStartDispInit (void)
{
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr( 0,  0, "                         uC/OS-II, The Real-Time Kernel                         ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr( 0,  1, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  2, "Nazwa Pr. Obciazenie Licznik    DeltaZmian  Stan                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  3, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  4, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  5, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  6, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  7, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  8, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  9, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 10, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 11, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 13, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 14, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 16, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 17, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 19, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 20, "OUT:                                                                            ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 21, " IN:                                                                            ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 22, "#Tasks          :        CPU Usage:     %                                       ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 23, "#Task switch/sec:                                                               ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY + DISP_BLINK);
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           UPDATE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDisp (void)
{
    char   s[80];


    sprintf(s, "%5d", OSTaskCtr);                                  /* Display #tasks running               */
    PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

#if OS_TASK_STAT_EN > 0
    sprintf(s, "%3d", OSCPUUsage);                                 /* Display CPU usage in %               */
    PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
#endif

    sprintf(s, "%5d", OSCtxSwCtr);                                 /* Display #context switches per second */
    PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100); /* Display uC/OS-II's version number    */
    PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    switch (_8087) {                                               /* Display whether FPU present          */
        case 0:
             PC_DispStr(71, 22, " NO  FPU ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 1:
             PC_DispStr(71, 22, " 8087 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 2:
             PC_DispStr(71, 22, "80287 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 3:
             PC_DispStr(71, 22, "80387 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;
    }
}	

static  void  TaskStartCreateTasks (void)
{
    INT8U  i;
	
	OSTaskCreate(TaskKlawiatura, (void *) 0, &TaskKlawiaturaStk[TASK_STK_SIZE - 1], 1);
	OSTaskCreate(TaskEdycja, (void *) 0, &TaskEdycjaStk[TASK_STK_SIZE - 1], 2);
	OSTaskCreate(TaskWyswietlanie, (void *) 0, &TaskWyswietlanieStk[TASK_STK_SIZE - 1], 3);
	OSTaskCreate(TaskWysylanie, (void *) 0, &TaskWysylanieStk[TASK_STK_SIZE - 1], 4);
	
	for (i = 0; i < N_TASKS; i++) {
		OSTaskCreate(TaskQueue, &numbers[i], &TaskQueueStk[i][TASK_STK_SIZE - 1], i + 5);
    }
	
	for (i = 0; i < N_TASKS; i++) {
		OSTaskCreate(TaskMsg, &numbers[i], &TaskMsgStk[i][TASK_STK_SIZE - 1], i + 5 + 5);
    }
	
    for (i = 0; i < N_TASKS; i++) {
		OSTaskCreate(TaskSem, &numbers[i], &TaskSemStk[i][TASK_STK_SIZE - 1], i + 5 + 10);
    }
}

void  TaskSem (void *pdata) {
    INT32U  obc;
	INT32U  i;
	INT32U  licznik;
	INT32U temp;
	INT8U *nr;
	TASK_DATA data;
	
	// Inicjalizacja struktury, ktora trzyma dane zadania
	obc = OBCIAZENIE_INIT;
	nr = (INT8U *) pdata; // Przekazywany na etapie tworzenia zadan numer.
	data.type = S_Q_M;
	data.Data.ObcData.TaskNumer = *nr + 10;
	data.Data.ObcData.TaskPr = *nr + 5 + 10;
	data.Data.ObcData.TaskNazwa[0] = 'S';
	data.Data.ObcData.TaskNazwa[1] = '_';
	data.Data.ObcData.TaskNazwa[2] = '0' + *nr;
	data.Data.ObcData.TaskNazwa[3] = '\0';
	
	licznik = 0;
	i = 0;
	temp = 0;
    for (;;) {
		if(OSSemAccept(ObciazenieSem)) { // Jesli semafor jest dostepny to wykorzystaj go. Jesli nie to idz dalej z poprzednim obciazeniem
			obc = Obciazenie;
			OSSemPost(ObciazenieSem);
		}
		
		data.Data.ObcData.TaskObciazenie = obc;
		data.Data.ObcData.TaskLicznik = licznik;
		
		OSQPost(Queue, &data);
		
		for(i = 0; i < obc; i++)
			temp++;
		
		licznik++;
        OSTimeDly(1);
    }
}

void TaskMsg(void *pdata) {
	INT32U obc;
	INT32U *obc_ptr;
	INT8U *nr;
	TASK_DATA data;
	INT32U temp;
	INT32U i;
	INT32U licznik;
	MEM_STRUCT *mem_struct_ptr; // Wskaznik na strukture, ktora jest alokowana dynamicznie. Za jej pomoca dostajemy obciazenie.
	
	nr = (INT8U *) pdata;
	data.type = S_Q_M;
	data.Data.ObcData.TaskNumer = *nr + 5;
	data.Data.ObcData.TaskPr = *nr + 5 + 5;
	data.Data.ObcData.TaskNazwa[0] = 'M';
	data.Data.ObcData.TaskNazwa[1] = '_';
	data.Data.ObcData.TaskNazwa[2] = '0' + *nr;
	data.Data.ObcData.TaskNazwa[3] = '\0';
	
	// Kolejna konsekwencja mojego nie najlepszego sposobu obchodzenia sie z wiadomosciami. Ale juz przy tym zostalem, bo dziala bez zarzutow.
	switch(*nr) {
		case 0:
			data.Data.ObcData.Os_Event_Ptr = MsgTask_1;
			break;
		case 1:
			data.Data.ObcData.Os_Event_Ptr = MsgTask_2;
			break;
		case 2:
			data.Data.ObcData.Os_Event_Ptr = MsgTask_3;
			break;
		case 3:
			data.Data.ObcData.Os_Event_Ptr = MsgTask_4;
			break;
		case 4:
			data.Data.ObcData.Os_Event_Ptr = MsgTask_5;
			break;
		default:
			break;
	}
	
	licznik = 0;
	temp = 0;
	i = 0;
	obc = OBCIAZENIE_INIT;
	for(;;){
		mem_struct_ptr = (MEM_STRUCT *) OSMboxAccept((OS_EVENT *) (data.Data.ObcData.Os_Event_Ptr));
		if(mem_struct_ptr != (MEM_STRUCT *) NULL) { // Jesli wskaznik nie jest pusty to udalo sie otrzymac adres struktury z obciazeniem. Wtedy ja wykorzystujemy.
			obc = mem_struct_ptr->TaskObciazenie;
			OSMemPut(MemPtr, mem_struct_ptr); // Zwracamy blok pamieci.
		}
		
		data.Data.ObcData.TaskObciazenie = obc;
		data.Data.ObcData.TaskLicznik = licznik;
		
		OSQPost(Queue, &data);
		
		for(i = 0; i < obc; i++)
			temp++;
		
		licznik++;
        OSTimeDly(1);
    }
}

void TaskQueue(void *pdata) {
	INT32U obc;
	OS_Q_DATA QueueData;
	TASK_DATA data;
	INT8U *nr;
	INT32U temp;
	INT32U i;
	INT32U licznik;
	INT16U nr_obc;
	BOOLEAN obc_got;
	MEM_STRUCT *mem_struct_ptr, *mem_struct_ptr_tmp; // Dwa wskazniki. Jeden to kursor, a drugi to adres poprzednio wykorzystanej struktury.
	
	nr = (INT8U *) pdata;
	data.type = S_Q_M;
	data.Data.ObcData.TaskNumer = *nr;
	data.Data.ObcData.TaskPr = *nr + 5;
	data.Data.ObcData.TaskNazwa[0] = 'Q';
	data.Data.ObcData.TaskNazwa[1] = '_';
	data.Data.ObcData.TaskNazwa[2] = '0' + *nr;
	data.Data.ObcData.TaskNazwa[3] = '\0';
	
	licznik = 0;
	temp = 0;
	obc = OBCIAZENIE_INIT;
	for(;;){
		obc_got = FALSE;
		nr_obc = 0;
		mem_struct_ptr_tmp = (MEM_STRUCT *) NULL;
		OSQQuery(QueueTask, &QueueData);
		
		/*
			Poprawne obchodzenie sie z obciazeniami wysylanymi przez kolejke jest najtrudniejsze w porownaniu do innych sposobow tj. semaforow, wiadomosci.
			Najpierw OSQQuery sprawdza ile jest wiadomosci w kolejce. Nastepnie tyle razy sprawdzamy kolejke.
			Jesli priorytety zadania i tego ze struktury z obciazeniem sie nie zgadzaja to wiadomosc jest umieszczana ponownie do kolejki poniewaz nie moze byc wykorzystana przez nie wlasciwe zadanie.
			Jesli priorytety sa zgodne to wykorzystujemy obciazenie. Musimy jednak pamietac jeszcze o kolejnosci obciazen. Do tego wykorzystujemy drugi wskaznik ktory pamieta adres aktualnie, bedac caly czas w petli sprawdzajacej
			wykorzystanego obciazenie. Jesli znajdziemy kolejne obciazenie nalezace do tego zadania to trzeba porownac ich numery, a nastepnie z pomoca tej informacji albo go nie wykorzystac bo jest nastepne
			, albo wykorzystac ale wtedy trzeba zmienic wartosci wskaznikow.
			Na koncu, jesli cos zdobylismy i wykorzystalismy, musimy zwolnic poprawny blok pamieci.
		*/
		for(i = 0; i < QueueData.OSNMsgs; i++) {
			mem_struct_ptr = (MEM_STRUCT *) OSQAccept(QueueTask);
			
			if(mem_struct_ptr->TaskPr == data.Data.ObcData.TaskPr && (!obc_got || mem_struct_ptr->NrObc < nr_obc)) {
				if(obc_got)
					OSQPost(QueueTask, mem_struct_ptr_tmp);
				
				obc = mem_struct_ptr->TaskObciazenie;
				
				nr_obc = mem_struct_ptr->NrObc;
				mem_struct_ptr_tmp = mem_struct_ptr;
				obc_got = TRUE;
			}
			else
				OSQPost(QueueTask, mem_struct_ptr);
			
		}
		if(obc_got)
			OSMemPut(MemPtr, mem_struct_ptr_tmp);
		
		data.Data.ObcData.TaskObciazenie = obc;
		data.Data.ObcData.TaskLicznik = licznik;
		
		OSQPost(Queue, &data);
		
		for(i = 0; i < obc; i++)
			temp++;
		
		licznik++;
		
        OSTimeDly(1);
    }
}

void TaskKlawiatura(void *data) {
	INT16S key;
	data = data;
	for(;;) {
		if(PC_GetKey(&key) == TRUE)
			OSMboxPost(CharMsgMBox, &key);
		
		OSTimeDly(10); // Poprawne usypianie zadania czytajacego wprowadzane znaki. Policzone za pomoca wiedzy o tym ile komputer jest faktycznie w stanie maksymalnie obsluzyc znakow wprowadzonych przez klawiature i jakia jest czestotliwosc systemu.
	}
}

void TaskEdycja(void *pdata) {
	TASK_DATA data;
	char *otrz_znak;
	INT8U err;
	INT16S key;
	INT8U i;
	BOOLEAN null_char = FALSE;
	INT32U obc;
	
	obc = 0;
	pdata = pdata;
	data.type = TEXT;
	i = 0;
	for(i = 0; i < BUF_SIZE; i++)
		data.Data.TextData.Text[i] = '\0';
	data.Data.TextData.bl_output = FALSE;
	i = 0;
	for(;;) {
		otrz_znak = (char *) OSMboxPend(CharMsgMBox, 0, &err);
		
		if(data.Data.TextData.bl_output) { // Jesli byl wcisniety enter to nastepnie wyczysc wiadomosc. Miejsce to celowo umieszczone jest tutaj a nie wczesniej czy pozniej ze wzgledu na to by nie czyscic tego przed wyswietleniem.
			for(i = 0; i < BUF_SIZE; i++)
				data.Data.TextData.Text[i] = '\0';
			i = 0;
			data.Data.TextData.bl_output = FALSE;
		}
		
		// Enter to zakoncz tekst znakiem '\0', zamien tekst na liczbe i wyslij do zadania zajmujacego sie propagacja obciazenia.
		if(*otrz_znak == KEY_ENTER) {
			data.Data.TextData.Text[i] = '\0';
			i = 0;
			obc = strtoul(data.Data.TextData.Text, NULL, 10);
			
			OSMboxPost(MsgWysylanie, &obc);
			
			i = 0;
			data.Data.TextData.bl_output = TRUE;
		}
		else if(*otrz_znak == KEY_BACKSPACE) { // Obsluga Backspace'a.
			if(i > 0)
				data.Data.TextData.Text[--i] = '\0';
		}
		else if(*otrz_znak == '\0') // W przypadku Delete'a najpierw nalezy odczytac znak '\0'
			null_char = TRUE;
		else if(null_char) {
			null_char = FALSE;
			if(*otrz_znak == KEY_DELETE) {
				for(i = 0; i < BUF_SIZE; i++)
					data.Data.TextData.Text[i] = '\0';
				i = 0;
			}
		}
		else if(*otrz_znak == KEY_ESCAPE)
			PC_DOSReturn();
		else if(i == (BUF_SIZE - 1)) // Nie chcemy przekroczyc rozmiaru bufora.
			;
		else
			data.Data.TextData.Text[i++] = *otrz_znak;
		
		OSQPost(Queue, &data);
	}
}

void TaskWyswietlanie(void *data) {
	#define SPACJE 33
	#define INT32U_DIGITS 10
	char  spacje[BUF_SIZE];
	char  spacje_row[SPACJE];
	char spacje_INT32U[INT32U_DIGITS + 1];
	TASK_DATA* ptr_task_data;
	char str_licznik[12];
	char str_obc[12];
	char str_pr[4];
	char str_delta[12];
	char str_stan[15];
	INT8U err;
	INT8U i;
	INT8U *task_id;
	INT32U *ptr_delta;
	INT32U prev_licznik[15] = { // Tablica pamietajaca poprzedni licznik.
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0
	};
	INT32U licznik[15] = { // Licznik aktualny zadania aktualizowany za kazdym razem gdy zadanie sie komunikuje z wyswietlaniem.
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0
	};
	
	// Rozne tablice ze spacjami do czyszczenia wiersza danych.
	for(i = 0; i < BUF_SIZE - 1; i++)
		spacje[i] = ' ';
	spacje[BUF_SIZE - 1] = '\0';
	
	for(i = 0; i < SPACJE - 1; i++)
		spacje_row[i] = ' ';
	spacje_row[SPACJE - 1] = '\0';
	
	for(i = 0; i < INT32U_DIGITS; i++)
		spacje_INT32U[i] = ' ';
	spacje_INT32U[INT32U_DIGITS] = '\0';
	
	data = data;
	for(;;) {
		ptr_task_data = (TASK_DATA *) OSQPend(Queue, 0, &err); 
		
		// Po otrzymaniu danych analizujemy pierwsze pole, typ zadania, by wiedziec jak interpretowac dalsze dane.
		if(ptr_task_data->type == TEXT) {
			if(ptr_task_data->Data.TextData.bl_output == FALSE) { // Dla tekstu potrzebujemy wiedziec czy byl wcisniety Enter
				PC_DispStr(4, 15 + 6, spacje, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
				PC_DispStr(4, 15 + 6, ptr_task_data->Data.TextData.Text, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
			else {
				PC_DispStr(4, 15 + 6, spacje, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
				PC_DispStr(4, 15 + 5, spacje, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
				PC_DispStr(4, 15 + 5, ptr_task_data->Data.TextData.Text, DISP_FGND_YELLOW + DISP_BGND_RED);
			}
		}
		else if(ptr_task_data->type == S_Q_M) { // Dla zadan obciazajacych konwertujemy rozne pola na lancuchy znakowe, a nastepnie je wyswietlamy w odpowiednim wierszu korzystajaca z numeru zadania
			itoa(ptr_task_data->Data.ObcData.TaskPr, str_pr, 10);
			ultoa(ptr_task_data->Data.ObcData.TaskLicznik, str_licznik, 10);
			ultoa(ptr_task_data->Data.ObcData.TaskObciazenie, str_obc, 10);
			licznik[ptr_task_data->Data.ObcData.TaskPr - 5] = ptr_task_data->Data.ObcData.TaskLicznik;
		
			PC_DispStr(0, 3 + ptr_task_data->Data.ObcData.TaskNumer, spacje_row, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			
			PC_DispStr(0, 3 + ptr_task_data->Data.ObcData.TaskNumer, ptr_task_data->Data.ObcData.TaskNazwa, DISP_FGND_YELLOW + DISP_BGND_BLUE);
			PC_DispStr(5, 3 + ptr_task_data->Data.ObcData.TaskNumer, str_pr, DISP_FGND_YELLOW + DISP_BGND_BLUE);
			PC_DispStr(10, 3 + ptr_task_data->Data.ObcData.TaskNumer, str_obc, DISP_FGND_YELLOW + DISP_BGND_BLUE);
			PC_DispStr(21, 3 + ptr_task_data->Data.ObcData.TaskNumer, str_licznik, DISP_FGND_YELLOW + DISP_BGND_BLUE);
		
		}
		else if(ptr_task_data->type == ONE_SECOND) { // Liczenie delty. Konwerowanie roznicy na lancuch znakow i aktualizacja licznikow z poprzedniej sekundy
			for(i = 0; i < 15; i++) {
				ultoa(licznik[i] - prev_licznik[i], str_delta, 10);
				PC_DispStr(32, 3 + i, spacje_INT32U, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
				PC_DispStr(32, 3 + i, str_delta, DISP_FGND_YELLOW + DISP_BGND_BLUE);
				prev_licznik[i] = licznik[i];
			}
		}
	}
}

void TaskWysylanie(void *data) {
	INT32U obc;
	INT16U nr_obc;
	INT8U err;
	INT8U i;
	OS_Q_DATA queue_data;
	OS_MBOX_DATA mbox_data;
	MEM_STRUCT* mem_struct_ptr;
	void *msg_ptr_table[5];
	
	msg_ptr_table[0] = MsgTask_1;
	msg_ptr_table[1] = MsgTask_2;
	msg_ptr_table[2] = MsgTask_3;
	msg_ptr_table[3] = MsgTask_4;
	msg_ptr_table[4] = MsgTask_5;
	
	nr_obc = 0;
	data = data;
	for(;;) {
		obc = *(INT32U *) OSMboxPend(MsgWysylanie, 0, &err);
		
		// Aktualizacja obciazenia odczytywanego przez zadania powiazane z semaforem
		OSSemPend(ObciazenieSem, 0, &err);
		Obciazenie = obc;
		OSSemPost(ObciazenieSem);
		
		// Zbierz informacje o kolejkach, tak by nie stracic blokow pamieci na wysylke obciazen do zajetej w calosci kolejki
		OSQQuery(QueueTask, &queue_data);
		
		for(i = 0; i < 5; i++) {
			if(queue_data.OSQSize - queue_data.OSNMsgs <= i) { // Pomin gdy brak miejsca
				continue;
			}
			
			mem_struct_ptr = OSMemGet(MemPtr, &err); // Odbierz blok pamieci
			mem_struct_ptr->TaskPr = i + 5; // Aktualizuj dane
			mem_struct_ptr->TaskObciazenie = obc;
			mem_struct_ptr->NrObc = nr_obc;
			
			OSQPost(QueueTask, mem_struct_ptr); // Wyslij adres bloku
		}
		
		// Analogicznie do kolejek, w przypadku wiadomosci tez wpierw sprawdzamy czy jest miejsce
		for(i = 0; i < 5; i++) {
			OSMboxQuery((OS_EVENT *) msg_ptr_table[i], &mbox_data);
			
			if(mbox_data.OSMsg != (void *) NULL) {
				continue;
			}
			
			mem_struct_ptr = OSMemGet(MemPtr, &err);
			mem_struct_ptr->TaskPr = i + 10;
			mem_struct_ptr->TaskObciazenie = obc;
			mem_struct_ptr->NrObc = nr_obc;
			
			OSMboxPost((OS_EVENT *) msg_ptr_table[i], mem_struct_ptr);
		}
		
		nr_obc++; // Zwieksz numer wyslanego obciazenia.
	}
}