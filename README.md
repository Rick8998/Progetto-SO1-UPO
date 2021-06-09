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
