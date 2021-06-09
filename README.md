# Progetto-SO1-UPO
Progetto di sistemi operativi 1-UPO anno 2018/2019

Modifica di un semplice interprete di comandi "smallsh" presentato nelle esercitazioni in modo da:
- ammettere la possibilità di lanciare comandi in background con la notazione:
"comando &"
- per i comandi lanciati in background, stampare informazioni sul fatto che il comando è terminato; per i comandi (in bg o fg) terminati da un segnale, informare analogamente l'utente.
A tal scopo può essere utile l'opzione WNOHANG della wait (vedere man);
- ammettere la possibilità di interrompere un comando con il segnale di interruzione, senza però interrompere anche l'interprete.
 L’interprete deve ignorare il segnale di interruzione solo quando è in corso un comando in foreground, mentre deve poter essere interrotto negli altri casi;
- stampare il prompt della shell nel formato
%<nome_utente>:<home_utente>:
Ad esempio:
%username:/home/username:
le informazioni su nome e home dell’utente devono essere ricavate a tempo di esecuzione della shell leggendo le corrispondenti variabili di ambiente iniziali;
- tenere traccia tramite una variabile d’ambiente BPID dell’elenco dei processi attualmente in background. Tale variabile deve essere aggiornata quando si crea un nuovo processo in background e quando se ne cattura la sua terminazione (vedi punto 2).
Il formato della variabile d’ambiente è:
BPID=pid1:pid2:...pidn
Ad esempio:
BPID=12034:12045:13089
La shell deve prevedere anche un comando bp che stampi il contenuto della variabile BPID. Il comando bp è interno, ossia quando l’utente digita la stringa bp, la shell la riconosce e stampa il contenuto di BPID senza creare un nuovo processo che esegua tale comando.

Utilizzando le system call POSIX per la gestione di semafori (sem_init, sem_wait e sem_post) e le funzioni per la gestione dei processi, risolvere il seguente problema:

Una tribù di N selvaggi mangia in comune da una pentola che può contenere fino ad M porzioni di stufato, si assume che inizialmente la pentola sia piena. Quando un selvaggio ha fame controlla la pentola:
- se non ci sono porzioni, sveglia il cuoco ed attende che questo abbia completamente riempito di  nuovo la pentola prima di servirsi;
- se la pentola contiene almeno una porzione, se ne appropria.
Il cuoco controlla che ci siano delle porzioni e, se ci sono, si addormenta, altrimenti cuoce M porzioni e le mette nella pentola. Ciascun selvaggio deve mangiare NGIRI volte prima di terminare.
Il numero di selvaggi N, di porzioni M e NGIRI dovranno essere richiesti come argomenti da linea di comando per facilitare esperimenti al variare di tali parametri. I selvaggi ed il cuoco devono essere implementati come processi separati che lavorano su variabili e semafori condivisi. Definire anche un parametro che permetta di contare complessivamente quante volte il cuoco riempie la pentola prima del termine del programma. Il programma termina quando tutti i selvaggi hanno completato il loro ciclo. Durante l'esecuzione stampare opportuni messaggi al fine di determinare il comportamento dei vari processi.
