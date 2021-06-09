/*Utilizzando le system call POSIX per la gestione di semafori (sem_init, sem_wait e sem_post) e
le funzioni per la gestione dei processi, risolvere il seguente problema:
Una tribù di N selvaggi mangia in comune da una pentola che può contenere fino ad M porzioni di
stufato, si assume che inizialmente la pentola sia piena. Quando un selvaggio ha fame controlla la
pentola:
i) se non ci sono porzioni, sveglia il cuoco ed attende che questo abbia completamente riempito di
ii) se la pentola contiene almeno una porzione, se ne appropria.
Il cuoco controlla che ci siano delle porzioni e, se ci sono, si addormenta, altrimenti cuoce M porzioni
e le mette nella pentola. Ciascun selvaggio deve mangiare NGIRI volte prima di terminare.
Il numero di selvaggi N, di porzioni M e NGIRI dovranno essere richiesti come argomenti da linea di
comando per facilitare esperimenti al variare di tali parametri. I selvaggi ed il cuoco devono essere
implementati come processi separati che lavorano su variabili e semafori condivisi. Definire anche
un parametro che permetta di contare complessivamente quante volte il cuoco riempie la pentola prima
del termine del programma. Il programma termina quando tutti i selvaggi hanno completato il loro
ciclo. Durante l'esecuzione stampare opportuni messaggi al fine di determinare il comportamento dei
vari processi.*/

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <errno.h>
#include <pthread.h>


int N; //numero selvaggi
int NGIRI; //numero di giri
int M; //capienza pentola
int shmid, shmid2, shmid3, shmid4, shmid5; //variabili per allocazione memoria condivisa
int *porzioni_attuali; //porzoni presenti al momento x nella pentola
int *lavoro; //quante volte ha lavorato il cuoco
//puntatori alle variabili semafori per mutua esclusione e wakeup dei processi, in memoria condivisa
sem_t *sem_pentola_vuota; //semaforo pentola vuota
sem_t *sem_pentola_piena; //semaforo pentola piena
sem_t *sem_mut_pentola; //semaforo (mutex) per la mutua esclusione della pentola

//processo selvaggio
void selvaggio(int i){
	//il selvaggio accede alla sezione critica e la blocca con il mutex
	//sem_wait(sem_mut_pentola);
	printf("\nSono il selvaggio <%d> (PID = %d): HO FAME\n", i, getpid());
	for (int j = 1; j <= NGIRI; ++j)
	{
		sem_wait(sem_mut_pentola);
		printf("Il selvaggio %d occupa la pentola al turno %d\n", i, j);
		printf("Controllo le porzioni nella pentola\n");
		//verifico se le porzioni presenti nella pentola sono = 0
		if((*porzioni_attuali)==0){
			printf("PENTOLA VUOTA, sveglio il cuoco!\n");
			//sveglio il cuoco
			sem_post(sem_pentola_vuota);
			//aspetta il semaforo della pentola piena
			sem_wait(sem_pentola_piena);
		}
		printf("Porzioni presenti nella pentola: %d\n", *porzioni_attuali);
		printf("Il selvaggio %d mangia una porzione\n", i);
		//il selvaggio consuma una porzione
		(*porzioni_attuali)--;
		printf("Nuovo totale di porzioni nella pentola: %d\n", *porzioni_attuali);
		printf("Il selvaggio %d lascia la pentola\n", i);

		if(j == NGIRI){ /*controllo se il selvaggio ha raggiunto il numero di giri per mangiare*/
			printf("Selvaggio %d ha terminato i giri per mangiare\n", i);
		}
		sem_post(sem_mut_pentola);
	}
	//il selvaggio esce dalla sezione critica, sblocca il mutex
	//sem_post(sem_mut_pentola);
}

//processo cuoco che continua a girare finchè non lo si termina con la kill
void cuoco(){
	printf("Il cuoco dorme...ZZzzZ...\n");
	while(1){
		if(*porzioni_attuali == 0){
			//il cuoco rimane in attesa di riceve il segnale del semaforo pentola_vuota
			sem_wait(sem_pentola_vuota);
			printf("\nSono il cuoco: chi mi cerca?!\n");
			printf("ATTENZIONE: PENTOLA VUOTA:\n");
			puts("Riempio la pentola");
			//riempie la pentola
			(*porzioni_attuali) = M;
			printf("Pentola riempita");
			//incremento il counter per sapere quante volte ha lavorato il cuoco
			(*lavoro)++;
			printf("\nPorzioni nella pentola: %d\n", *porzioni_attuali);
			printf("Torno a dormire...ZzzZZz...\n\n");
			//segnalo al selvaggio che la pentola è piena
			sem_post(sem_pentola_piena);
		}
	}
}


int main(int argc, char *argv[]){

	pid_t pid, pid2;

	//verifica del numero parametri passati
	if(argc!=4){
		perror("Inserire quattro argomenti: ./sorgente, num_selvaggi, num_giri, num_porzioni");
		exit(1);
	}


	//memoria condivisa
	shmid= shmget(IPC_PRIVATE, (sizeof(int)), 0600);//istanzio la memoria condivisa
	if(shmid == -1)
		perror("Errore creazione memoria condivisa int");
	porzioni_attuali = (int*) shmat(shmid, NULL, 0);//"attacco" la variabile porzioni_attuali alla memoria creata precedentemente
	if(porzioni_attuali == (void*)-1)
		perror("Errore attach memoria condivisa");
	N = atoi(argv[1]);//primo valore passato per argomento
	NGIRI = atoi(argv[2]);//secondo valore passato per argomento
	M = atoi(argv[3]);//terzo valore passato come argomento
	*porzioni_attuali = atoi(argv[3]);//pentola inizialmente piena + variabile condivisa

	//variabile condivisa lavoro
	shmid2 = shmget(IPC_PRIVATE, sizeof(int), 0600);//istanzio la memoria condivisa
	if(shmid2 == -1)
		perror("Errore creazione memoria condivisa int");
	lavoro = (int*) shmat(shmid2, NULL, 0);//"attacco" la variabile lavoro alla memoria creata precedentemente
	if(lavoro == (void*)-1)
		perror("Errore attach memoria condivisa");
	*lavoro = 0;

	//semafori condivisi
	//sem_pentola_piena
	shmid3 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);//istanzio la memoria condivisa
	if(shmid3 == -1)
		perror("Errore creazione memoria condivisa int");
	sem_pentola_piena = (sem_t *) shmat(shmid3, NULL, 0);//"attacco" il semaforo pentola_piena alla memoria creata precedentemente
	if(sem_pentola_piena == (void*) -1)
		perror("Errore attach memoria condivisa");
	sem_init(sem_pentola_piena, 1, 0);//inizializzazione semaforo

	//sem_pentola_vuota
	shmid4 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);//istanzio la memoria condivisa
	if(shmid4 == -1)
		perror("Errore creazione memoria condivisa int");
	sem_pentola_vuota = (sem_t *) shmat(shmid4, NULL, 0);//"attacco" il semaforo pentola_vuota alla memoria creata precedentemente
	if(sem_pentola_vuota == (void*) -1)
		perror("Errore attach memoria condivisa");
	sem_init(sem_pentola_vuota, 1, 0);//inizializzazione semaforo

	//sem_mut_pentola
	shmid5 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);//istanzio la memoria condivisa
	if(shmid5 == -1)
		perror("Errore creazione memoria condivisa int");
	sem_mut_pentola = (sem_t *) shmat(shmid5, NULL, 0);//"attacco" il semaforo/mutex alla memoria creata precedentemente
	if(sem_mut_pentola == (void*) -1)
		perror("Errore attach memoria condivisa");
	sem_init(sem_mut_pentola, 1, 1);//inizializzazione semaforo


	if (N < 1 || NGIRI < 1 || M < 1) /*controllo che i valori passati siano corretti (no 0 o negativi)*/
	{
		perror("I valori devono essere maggiori 0");
		exit(1);
	}


	//crea il processo cuoco
	pid2 = fork();
	if (pid2 == -1)
	{
		perror("Fork cuoco fallita");
	}
	else if(pid2 == 0){
				//processo figlio cuoco
		cuoco();
		exit(0);
	}
	else{
		/*processo padre*/
	}


	//crea i processi selvaggi
	for(int i=1;i<=N;i++){
		pid = fork();
		if (pid == -1)
		{
			perror("Fork selvaggio fallita");
		}
		else if(pid==0){

			/*per l'ordine corretto dei selvaggi. La politca di scheduling sarebbe LIFO
			facendo aspettare ogni processo tramite la sleep faccio in modo che i
			processi vengano eseguiti in ordine di entrata*/
			if(i <= N){
				sleep(i);
			}
			//processo figlio selvaggio
			selvaggio(i);

			exit(0);
		}
		else{
			/*processo padre*/
		}
	}



	//prima che un processo termini aspetta che sia terminato quello precedente
	for (int i=1;i<=N;i++)
	{
		wait(0);
		//printf("\nSelvaggio <%d> con pid: <%d> terminato\n", i, getpid());
	}
	printf("\nPorzioni rimaste nella pentola: %d\n", *porzioni_attuali);
	printf("Il cuoco ha riempito la pentola %d volta/e...ORA MUORE, ADDIO\n", *lavoro);
	//termino il cuoco
	kill(pid2, SIGKILL);
  	return 0;
}
