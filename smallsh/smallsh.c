#include "smallsh.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//char *prompt = "Scrivere un comando>";

/*void printSignal(int sig){
  printf("\nSegnale %d ricevuto\n", sig);
}*/


void procline(void) 	/* tratta una riga di input */
{

  char *arg[MAXARG+1];	/* array di puntatori per runcommand */
  int toktype;  	/* tipo del simbolo nel comando */
  int narg;		/* numero di argomenti considerati finora */
	int type = 0;  /* R-Background-variabile che viene mandata come secondo argomento della runcommand*/
  narg=0;
  int exitstat2; /*exit_status per la waitpid*/
  pid_t out; /*pid*/

  do {

    /* mette un simbolo in arg[narg] 
       ed esegue un'azione a seconda del tipo di simbolo */
	
    switch (toktype = gettok(&arg[narg])) {
	
      case ARG:   

        /* se argomento: passa al prossimo simbolo */
		
        if (narg < MAXARG) narg++;
	break;
       
      /* se fine riga o ';' o '&' esegue il comando ora contenuto in arg,
   mettendo NULL per segnalare la fine degli argomenti: serve a execvp */
      case EOL:
      case AMPERSAND:      
      case SEMICOLON:
      
      /* ad ogni ciclo di procline nel main controllo se ci sono pid 
        da rimuovere nella tabella dei processi e si ci sono li elimino (per non avere più di 1 proc in background)
        avvisando l'utente della terminazione dei rispettivi processi in background
        inoltre distinguo se i processi sono terminati correttamente o da un segnale */
      while((out = waitpid(-1, &exitstat2, WNOHANG))>0){ 
        printf("\nIl processo in background: %d\n", out); 
        if (WIFEXITED(exitstat2))
          printf("E' terminato normalmente\n");
        
        if (WIFSIGNALED(exitstat2))
          printf("E' stato terminato da un segnale\n");
    }

        /*R-se il tipo del simbolo nel comando è & assegno alla variabile type il valore della costante BACKGROUND*/
       type = (toktype == AMPERSAND) ? BACKGROUND : FOREGROUND;

        /*if(toktype == AMPERSAND){
          type = BACKGROUND;
        }
        else
          type = FOREGROUND;*/


        if (narg != 0) {
	  arg[narg] = NULL;
	  runcommand(arg, type); /*R-passo type alla runcommand in modo che possa valutare se è presente &*/
        }
      
	/* se non fine riga (descrizione comando finisce con ';')
           bisogna ricominciare a riempire arg dall'indice 0 */

        if (toktype != EOL)  narg = 0; 

        break;


     }

  }while (toktype != EOL);  /* fine riga, procline finita */

}

void runcommand(char **cline, int type)	/* esegue un comando--R- type serve per il valore Di type*/
{
  pid_t pid;
  int exitstat,ret;
  struct sigaction signal; /*struct sigaction per i segnali*/

  pid = fork();
  if (pid == (pid_t) -1) {
     perror("smallsh: fork fallita");
     return;
  }

  if (pid == (pid_t) 0) { 	/* processo figlio */

    
    if (type == BACKGROUND)
    {
      /*se il figlio viene lanciato in background, deve poter essere interrotto
      dai segnali di interruzione insieme alla shell*/
      signal.sa_handler = SIG_DFL;  /*esegue la funzione di default per quel segnale*/
      sigemptyset(&signal.sa_mask);
      signal.sa_flags = 0;
      sigaction(SIGINT, &signal, NULL);      
    }
    else
      printf("il PID del porcesso figlio in foreground è: %d\n\n", getpid());

  /* esegue il comando il cui nome e' il primo elemento di cline,
       passando cline come vettore di argomenti */

    execvp(*cline,cline);
    perror(*cline);
    exit(1);
  }

  /* non serve "else"... ma bisogna aver capito perche' :-)  */

  /*R-valuto in modo che solo con & vengano eseguiti i processi in backround
  Quindi solo quelli in FOREGROUND venrranno eseguiti*/
  if(type == FOREGROUND){

    /*il processo padre (la shell) non deve essere terminato dai segnali di
    interruzione se c'è un processo in foreground attivo*/
    signal.sa_handler = SIG_IGN;   /*printSignal;*/
    sigemptyset(&signal.sa_mask);
    signal.sa_flags = 0;
    sigaction(SIGINT, &signal, NULL);

    ret = waitpid(pid, &exitstat, 0);

    if (ret == -1){
      perror("wait");
  }

  /*R-la waitpid sopra aspetta che un processo figlio dello stesso process group ID termini, salvando in exitstat il modo 
  in cui è terminato il processo. Questo verrà analizzato dalle WIFEXITED (terminazione corretta) 
  e WIFSIGNALED (terminazione per un segnale) per vedere il modo in cui è terminato il processo*/
  
    printf("\nIl processo in foreground: %d\n", pid);
    if (WIFEXITED(exitstat))
      printf("E' terminato normalmente\n");
    if (WIFSIGNALED(exitstat))
      printf("E' stato terminato da un segnale\n");
  }

  else //type = BACKGROUND
      printf("\nIl processo in background: %d\n", pid);
    
  
  //ripristino il comportamento di default alla fine di ogni ciclo
  signal.sa_handler = SIG_DFL;
  sigemptyset(&signal.sa_mask);
  signal.sa_flags = 0;
  sigaction(SIGINT, &signal, NULL);
}

int main()
{
  /*vado a lavorare sulle variabili d'ambiente*, mi servono due puntatori a char*/
  char* name;
  char* home;
  /*getenv consente di ottenere il valore di una variabile d'ambiente dato il suo nome*/
  name = getenv("USER");
  home = getenv("HOME");
  while(userin(name, home) != EOF)
    procline();
  return 0;
}