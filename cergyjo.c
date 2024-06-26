#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>

#define MAX_TRAINING 100
#define MAX_NAME_LENGHT 100

//structure date
struct Date {
    int day;
    int month;
    int year;
};

//énumération de tous les typezs d'épreuves
enum Tests { CENT_METRES, QUATRE_CENTS_METRES, CINQ_MILLE_METRES, MARATHON, RELAIS_QUATRE_CENTS, AUTRE_PERSONNALISEE };

//structure pour les participants
struct Participant {
    char name[MAX_NAME_LENGHT];
    int position;
};

//structures pour les entrainenemnt
struct Training {
    struct Date date;
    float time;
    enum Tests type;
    struct Participant participants[4];
};

//fonction qui vérifie si une date entrée en paramètre est conforme
bool isValidDate(struct Date date) {
    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (date.year < 1900 || date.month < 1 || date.month > 12 || date.day < 1)
        return false;
    if (date.year % 4 == 0 && (date.year % 100 != 0 || date.year % 400 == 0))
        daysInMonth[1] = 29;
    if (date.day > daysInMonth[date.month - 1])
        return false;
    return true;
}

//fonction qui permet d'enregistrer un entrainement
void enterTraining(struct Training *training, char *nameAthlete, struct Training *trainings , int *nbTraining) {
    struct Date date;
    bool dateValid = false;
    while (!dateValid) {
        printf("Date de l'entrainement (jj/mm/aaaa): ");
        if (scanf("%d/%d/%d", &date.day, &date.month, &date.year) != 3) {
            printf("Format de date invalide. Veuillez réessayer.\n");
            while (getchar() != '\n');
            continue;
        }
        dateValid = isValidDate(date);
        if (!dateValid) {
            printf("Date invalide. Veuillez réessayer.\n");
        }
    }
    training->date = date;

    int choice;
    printf("Choisissez le type d'épreuve :\n");
    printf("1. 100 mètres\n");
    printf("2. 400 mètres\n");
    printf("3. 5000 mètres\n");
    printf("4. Marathon\n");
    printf("5. Relais 4x400 mètres\n");
    printf("6. Autre (personnalisée)\n");
    printf("Votre choix : ");
    if (scanf("%d", &choice) != 1) {
        printf("Choix invalide. Veuillez réessayer.\n");
        while (getchar() != '\n');
        return;
    }

    if (choice == 6) {
        training->type = AUTRE_PERSONNALISEE;
        printf("Saisissez le nom de l'épreuve : ");
        while (getchar() != '\n'); // Clear the input buffer
        fgets(training->participants[0].name, MAX_NAME_LENGHT, stdin); // Using the first participant's name to store custom event name
        training->participants[0].name[strcspn(training->participants[0].name, "\n")] = '\0';
    } else if (choice >= 1 && choice <= 5) {
        training->type = choice - 1;
    } else {
        printf("Choix invalide. Veuillez réessayer.\n");
        return;
    }

    if (training->type == RELAIS_QUATRE_CENTS) {
        printf("Position dans le relais (1-4) : ");
        if (scanf("%d", &training->participants[0].position) != 1 || training->participants[0].position < 1 || training->participants[0].position > 4) {
            printf("Position invalide. Veuillez saisir une position entre 1 et 4.\n");
            return;
        }
    } else {
        training->participants[0].position = 1;
    }

    strncpy(training->participants[0].name, nameAthlete, MAX_NAME_LENGHT);
    training->participants[0].name[MAX_NAME_LENGHT - 1] = '\0'; // Ensure null termination

    printf("Temps de l'entrainement (en secondes): ");
    if (scanf("%f", &training->time) != 1) {
        printf("Temps invalide. Veuillez réessayer.\n");
        while (getchar() != '\n');
        return;
    }

    trainings[(*nbTraining)++] = *training;
    printf("Entrainement enregistré avec succès.\n");
}


//fonction qui rajoute un entrainenemnt dans un fichier pour un athlete dont le nom est entré en parametres; cette fonction crée le fichier pour l'athlète s'il n'rest pas déjà crée
void saveTraining(char *nameFile, struct Training *trainings, int nbTrainings) {
    FILE *file = fopen(nameFile, "w");
    if (file == NULL) {
        printf("Erreur lors de la création du fichier %s.\n", nameFile);
        return;
    }

    for (int i = 0; i < nbTrainings; i++) {
        fprintf(file, "%02d/%02d/%04d %.2f %d", trainings[i].date.day, trainings[i].date.month, trainings[i].date.year, trainings[i].time, trainings[i].type);
        for (int j = 0; j < 4; j++) {
            fprintf(file, " %s %d", trainings[i].participants[j].name, trainings[i].participants[j].position);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Entraînements sauvegardés avec succès dans le fichier %s.\n", nameFile);
}

//fonction qui compare deux dates entrées en parametres
int comparedDates(struct Date date1, struct Date date2) {
    if (date1.year != date2.year) {
        return date1.year - date2.year;
    } else if (date1.month!= date2.month) {
        return date1.month - date2.month;
    } else {
        return date1.day - date2.day;
    }
}

// Fonction de comparaison pour le tri par date
int comparedTrainingsByDate(const void* a, const void* b) {
    struct Training* training1 = (struct Training*)a;
    struct Training* training2 = (struct Training*)b;
    return comparedDates(training1->date, training2->date);
}

// Fonction de tri pour trier les entraînements par date
void sortTrainingsByDate(struct Training* trainings, int nbTrainings) {
    qsort(trainings, nbTrainings, sizeof(struct Training), comparedTrainingsByDate);
}
void consultTrainings(struct Training *trainings, int nbTrainings, int choiceSort) {
    char nameAthlete[MAX_NAME_LENGHT];
    printf("Nom de l'athlète : ");
    fgets(nameAthlete, MAX_NAME_LENGHT, stdin);
    nameAthlete[strcspn(nameAthlete, "\n")] = '\0'; // Supprimer le retour à la ligne

    bool athleteFind = false;

    // Tri des entraînements si nécessaire
    switch (choiceSort) {
        case 1:
            sortTrainingsByDate(trainings, nbTrainings);
            break;
        // Ajoutez d'autres cas pour d'autres critères de tri si nécessaire
        default:
            printf("Option de tri invalide.\n");
            return;
    }

    printf("Historique des entraînements pour l'athlète %s :\n", nameAthlete);
    for (int i = 0; i < nbTrainings; i++) {
        for (int j = 0; j < 4; j++) {
            if (strcmp(trainings[i].participants[j].name, nameAthlete) == 0) {
                athleteFind = true;
                printf("Date : %02d/%02d/%04d, Temps : %.2f secondes, Type : ", trainings[i].date.day, trainings[i].date.month, trainings[i].date.year, trainings[i].time);
                switch (trainings[i].type) {
                    case CENT_METRES:
                        printf("100m\n");
                        break;
                    case QUATRE_CENTS_METRES:
                        printf("400m\n");
                        break;
                    case CINQ_MILLE_METRES:
                        printf("5000m\n");
                        break;
                    case MARATHON:
                        printf("Marathon\n");
                        break;
                    case RELAIS_QUATRE_CENTS:
                        printf("Relais\n");
                        for (int k = 0; k < 4; k++) {
                            printf("Participant %d : %s, Position : %d\n", k + 1, trainings[i].participants[k].name, trainings[i].participants[k].position);
                        }
                        break;
                }
                break;
            }
        }
    }

    if (!athleteFind) {
        printf("Aucun entraînement trouvé pour cet athlète.\n");
    }
}

//fonction qui permet d'afficher en caracteres les noms qui correspondent aux propositions dans l'énumération
const char* type_test_to_string(enum Tests type){
  switch(type){
    case CENT_METRES: return "100m";
    case QUATRE_CENTS_METRES: return "400m";
    case CINQ_MILLE_METRES: return "5000m";
    case MARATHON: return "Marathon";
    case RELAIS_QUATRE_CENTS: return "Relais";
    default: return "UNKNOWN";
  }
}

//fonction qui renvoie le type de l'épreuve en fonction du numéro entré 
enum Tests type_test(int type_test){ //type_test: 1 pour 100m, 2 pour 400m, 3 pour 5000m, 4 pour marathon et 5 pour relais
    enum Tests type;
    switch (type_test){
      case 1:type=CENT_METRES;
        break;
      case 2:type=QUATRE_CENTS_METRES;
        break;
      case 3:type=CINQ_MILLE_METRES;
        break;
      case 4:type=MARATHON;
        break;
      case 5:type=RELAIS_QUATRE_CENTS;
        break;
    }
    return type;
}

//fonction qui permet d'afficher l'historique des entrainements pour un athlete
void consultTrainingsByAthlete(char *nameAthlete, struct Training *trainings, int nbTrainings) {
    bool found = false;
    printf("Historique des entraînements pour l'athlète %s:\n", nameAthlete);
    for (int i = 0; i < nbTrainings; i++) {
        if (strcmp(trainings[i].participants[0].name, nameAthlete) == 0) {
            found = true;
            if(trainings[i].type==RELAIS_QUATRE_CENTS){
                printf("Date : %02d/%02d/%04d, Type: %s, Position: %d, Temps : %.2f secondes\n",
                   trainings[i].date.day, trainings[i].date.month, trainings[i].date.year, type_test_to_string(trainings[i].type), trainings[i].participants->position,trainings[i].time);
            }
            else{
                printf("Date : %02d/%02d/%04d, Type: %s, Temps : %.2f secondes\n",
                   trainings[i].date.day, trainings[i].date.month, trainings[i].date.year, type_test_to_string(trainings[i].type), trainings[i].time);
            }
        }
    }
    if (!found) {
        printf("Aucun entraînement trouvé pour cet athlète.\n");
    }
}

//fonction qui permet d'afficher l'historique des entrainements pour une date
void consultTrainingsByDate(struct Date date, struct Training *trainings, int nbTrainings) {
    bool found = false;
    printf("Historique des entraînements pour la date %02d/%02d/%04d:\n", date.day, date.month, date.year);
    for (int i = 0; i < nbTrainings; i++) {
        if (trainings[i].date.day == date.day &&
            trainings[i].date.month == date.month &&
            trainings[i].date.year == date.year) {
            found = true;
            if(trainings[i].type==RELAIS_QUATRE_CENTS){
                printf("Athlete: %s, Type : %s, Position: %d, Temps : %.2f secondes\n",
                   trainings[i].participants[0].name,type_test_to_string(trainings[i].type), trainings[i].participants->position,trainings[i].time);
            }
            else{
                printf("Athlete: %s, Type : %s, Temps : %.2f secondes\n",
                   trainings[i].participants[0].name,type_test_to_string(trainings[i].type), trainings[i].time);
            }
        }
    }
    if (!found) {
        printf("Aucun entraînement trouvé pour cette date.\n");
    }
}

//fonction qui permet d'afficher l'historique des entrainements pour une épreuve
void consultTrainingsByTest(enum Tests type, struct Training *trainings, int nbTrainings) {
    bool found = false;
    printf("Historique des entraînements pour l'épreuve %s:\n", type_test_to_string(type));
    const char* recover_type=type_test_to_string(type);
    for (int i = 0; i < nbTrainings; i++) {
        const char* compared_type=type_test_to_string(trainings[i].type);
        if (strcmp(compared_type, recover_type) == 0) {
            found = true;
           if(trainings[i].type==RELAIS_QUATRE_CENTS){
                printf("Athlete: %s, Date : %02d/%02d/%04d, Position: %d, Temps : %.2f secondes\n",
                   trainings[i].participants[0].name,trainings[i].date.day, trainings[i].date.month, trainings[i].date.year, trainings[i].participants->position,trainings[i].time);
            }
            else{
                printf("Athlete: %s, Date : %02d/%02d/%04d, Temps : %.2f secondes\n",
                   trainings[i].participants[0].name,trainings[i].date.day, trainings[i].date.month, trainings[i].date.year, trainings[i].time);
            }
        }
    }
    if (!found) {
        printf("Aucun entraînement trouvé pour cette épreuve.\n");
    }
}

//fonction qui permet d'afficher l'historique des entrainements en fonction de certains parametres
void consultHistoric(struct Training* trainings, int nbTrainings){
    int choice,type;
    char* nameAthlete=malloc(sizeof(char*));
    if(nameAthlete==NULL){
        perror("erreur d'allocation de la mémoire\n");
        return;
    }
    struct Date date;
    enum Tests decision;
    printf("1. Consulter entrainement par athlete\n2. Consulter entrainemenr par date\n3. Consulter entrainement par epreuve\n");
    scanf("%d",&choice);
    switch(choice){
        case 1:
            printf("Nom de l'athlète: ");
            scanf("%s",nameAthlete);
            consultTrainingsByAthlete(nameAthlete, trainings, nbTrainings);
        break;
        case 2:
            printf("Date (jj/mm/aaaa): ");
            if (scanf("%d/%d/%d", &date.day, &date.month, &date.year) == 3) {
                consultTrainingsByDate(date, trainings, nbTrainings);
            }
            else {
                printf("Format de date invalide.\n");
                while (getchar() != '\n');
            }
        break;
        case 3:
            printf("Choisissez une epreuve:\n1. 100m\n2. 400m\n3. 5000M\n4. Marathon\n5. Relais\n");
            scanf("%d",&type);
            decision=type_test(type);
            consultTrainingsByTest(decision, trainings, nbTrainings);
        break;
    }   

}

//focntion qui permet de consulter les statistiques des performance d'un athlete
void consultStatistics(char *nameAthlete, struct Training *trainings, int nbTrainings, enum Tests test) {
    float total = 0;
    int count = 0;

    for (int i = 0; i < nbTrainings; i++) {
        if (trainings[i].type == test && strcmp(trainings[i].participants[0].name, nameAthlete) == 0) {
            total += trainings[i].time;
            count++;
        }
    }

    if (count > 0) {
        printf("Temps moyen pour l'épreuve : %.2f secondes\n", total / count);
    } else {
        printf("Aucun entrainement trouvé pour cette épreuve.\n");
    }
}

//?
void summaryPerformancesAthlete(char *nameAthlete, struct Training *trainings, int nbTrainings, enum Tests test) {
    int total = 0;

    for (int i = 0; i < nbTrainings; i++) {
        if (trainings[i].type == test && strcmp(trainings[i].participants[0].name, nameAthlete) == 0) {
            total++;
        }
    }

    printf("Nombre total d'entrainements pour cette épreuve : %d\n", total);
}

//?
void progressAthlete(char *nameAthlete, struct Training *trainings, int nbTrainings, enum Tests test) {
    float timeMin = FLT_MAX;
    float timeMax = FLT_MIN;

    for (int i = 0; i < nbTrainings; i++) {
        if (trainings[i].type == test && strcmp(trainings[i].participants[0].name, nameAthlete) == 0) {
            if (trainings[i].time < timeMin) {
                timeMin = trainings[i].time;
            }
            if (trainings[i].time > timeMax) {
                timeMax = trainings[i].time;
            }
        }
    }

    if (timeMin == FLT_MAX || timeMax == FLT_MIN) {
        printf("Aucun entrainement trouvé pour cette épreuve.\n");
    } else {
        printf("Amélioration maximale : %.2f secondes\n", timeMax - timeMin);
    }
}

enum Tests choiceTypeTest() {
    int choice;
    printf("Choisissez le type d'épreuve :\n");
    printf("1. 100 mètres\n");
    printf("2. 400 mètres\n");
    printf("3. 5000 mètres\n");
    printf("4. Marathon\n");
    printf("5. Relais 4x400 mètres\n");
    printf("6.Épreuve personnalisée \n");
    printf("Votre choix : ");
    if (scanf("%d", &choice) != 1) {
        printf("Choix invalide. Veuillez réessayer.\n");
        while (getchar() != '\n');
        return -1;
    }

    switch (choice) {
        case 1:
            return CENT_METRES;
        case 2:
            return QUATRE_CENTS_METRES;
        case 3:
            return CINQ_MILLE_METRES;
        case 4:
            return MARATHON;
        case 5:
            return RELAIS_QUATRE_CENTS;
        case 6 :
        return AUTRE_PERSONNALISEE;
        default:
            printf("Choix invalide. Veuillez réessayer.\n");
            return -1;
    }
}

//fonction qui recherche le meilleur athlete pour une epreuve
void findBestAthletes(struct Training *trainings, int nbTrainings, enum Tests test) {
    float bestTime = FLT_MAX;
    char bestAthlete[MAX_NAME_LENGHT] = "";

    for (int i = 0; i < nbTrainings; i++) {
        if (trainings[i].type == test) {
            if (trainings[i].time < bestTime) {
                bestTime = trainings[i].time;
                strcpy(bestAthlete, trainings[i].participants[0].name);
            }
        }
    }

    if (bestTime == FLT_MAX) {
        printf("Aucun entrainement trouvé pour cette épreuve.\n");
    } else {
        printf("Meilleur temps : %.2f secondes par %s\n", bestTime, bestAthlete);
    }
}

//?
float differenceTime(struct Training *trainings, int nbTrainings, char *nameAthlete, enum Tests test, struct Date dateBeginning, struct Date dateEnd) {
    float timeBeginning = FLT_MAX;
    float timeEnd = FLT_MAX;

    for (int i = 0; i < nbTrainings; i++) {
        if (trainings[i].type == test && strcmp(trainings[i].participants[0].name, nameAthlete) == 0) {
            if (trainings[i].date.day == dateBeginning.day && trainings[i].date.month == dateBeginning.month && trainings[i].date.year == dateBeginning.year) {
                timeBeginning = trainings[i].time;
            }
            if (trainings[i].date.day == dateEnd.day && trainings[i].date.month == dateEnd.month && trainings[i].date.year == dateEnd.year) {
                timeEnd = trainings[i].time;
            }
        }
    }

    if (timeBeginning == FLT_MAX || timeEnd == FLT_MAX) {
        printf("Aucun entrainement trouvé pour les dates spécifiées.\n");
        return -1;
    }

    return timeEnd - timeBeginning;
}

//?
void regressLinear(char *nameAthlete, struct Training *trainings, int nbTrainings, enum Tests test) {
    int n = 0;
    float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

    for (int i = 0; i < nbTrainings; i++) {
        if (trainings[i].type == test && strcmp(trainings[i].participants[0].name, nameAthlete) == 0) {
            n++;
            sumX += i;
            sumY += trainings[i].time;
            sumXY += i * trainings[i].time;
            sumX2 += i * i;
        }
    }

    if (n == 0) {
        printf("Aucun entraînement trouvé pour cet athlète et cette épreuve.\n");
        return;
    }

    float a = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    float b = (sumY - a * sumX) / n;

    printf("Régression linéaire : temps = %.2f * session + %.2f\n", a, b);
}

int main() {
    struct Training trainings[MAX_TRAINING];
    int nbTraining = 0;
    char nameFile [MAX_NAME_LENGHT];
    char nameAthlete[MAX_NAME_LENGHT];
    enum Tests test;
    struct Date date;

    bool quit = false;

    printf("Bienvenue au CER-J-O\n");
    printf("Veuillez choisir parmi nos épreuves proposées et consulter les statistiques\n");

    while (!quit) {
        int choice;
        printf("\nMenu:\n");
        printf("1. Saisir un nouvel entraînement\n");
        printf("2. Consulter l'historique d'entrainements\n");
        printf("3. Consulter les statistiques pour un athlète\n");
        printf("4. Résumer les performances d'un athlète\n");
        printf("5. Progression d'un athlète\n");
        printf("6. Trouver le meilleur athlète pour une épreuve\n");
        printf("7. Différence de temps pour un athlète entre deux dates\n");
        printf("8. Calculer la régression linéaire des performances d'un athlète\n");
        printf("9. Quitter\n");
        printf("Votre choix : ");
        if (scanf("%d", &choice) != 1) {
            printf("Choix invalide. Veuillez réessayer.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n'); // Clear the input buffer

        switch (choice) {
            case 1:
                {
                    struct Training newTraining;
                    printf("Nom de l'athlète : ");
                    fgets(nameAthlete, MAX_NAME_LENGHT, stdin);
                    nameAthlete[strcspn(nameAthlete, "\n")] = '\0'; // Remove trailing newline
                    enterTraining(&newTraining, nameAthlete, trainings, &nbTraining);
                }
                break;
            case 2:
                consultHistoric(trainings,nbTraining);
                break;
            case 3:
                printf("Nom de l'athlète: ");
                fgets(nameAthlete, MAX_NAME_LENGHT, stdin);
                nameAthlete[strcspn(nameAthlete, "\n")] = '\0'; // Remove trailing newline
                consultStatistics(nameAthlete, trainings, nbTraining, test);
                break;
            case 4:
                printf("Nom de l'athlète: ");
                fgets(nameAthlete, MAX_NAME_LENGHT, stdin);
                nameAthlete[strcspn(nameAthlete, "\n")] = '\0'; // Remove trailing newline
                summaryPerformancesAthlete(nameAthlete, trainings, nbTraining, test);
                break;
            case 5:
                printf("Nom de l'athlète: ");
                fgets(nameAthlete, MAX_NAME_LENGHT, stdin);
                nameAthlete[strcspn(nameAthlete, "\n")] = '\0'; // Remove trailing newline
                progressAthlete(nameAthlete, trainings, nbTraining, test);
                break;
            case 6:
                printf("Choisissez le type d'épreuve :\n");
                printf("0. 100 mètres\n");
                printf("1. 400 mètres\n");
                printf("2. 5000 mètres\n");
                printf("3. Marathon\n");
                printf("4. Relais 4x400 mètres\n");
                printf("Votre choix : ");
                if (scanf("%d", (int *)&test) != 1) {
                    printf("Choix invalide.\n");
                    while (getchar() != '\n');
                    continue;
                }
                findBestAthletes(trainings, nbTraining, test);
                break;
            case 7:
                {
                    struct Date date2;
                    printf("Nom de l'athlète: ");
                    fgets(nameAthlete, MAX_NAME_LENGHT, stdin);
                    nameAthlete[strcspn(nameAthlete, "\n")] = '\0'; // Remove trailing newline
                    printf("Date 1 (jj/mm/aaaa): ");
                    if (scanf("%d/%d/%d", &date.day, &date.month, &date.year) == 3) {
                        printf("Date 2 (jj/mm/aaaa): ");
                        if (scanf("%d/%d/%d", &date2.day, &date2.month, &date2.year) == 3) {
                            differenceTime(trainings, nbTraining, nameAthlete, test, date, date2);
                        } else {
                            printf("Format de date invalide.\n");
                            while (getchar() != '\n');
                        }
                    } else {
                        printf("Format de date invalide.\n");
                        while (getchar() != '\n');
                    }
                }
                break;
            case 8:
                printf("Nom de l'athlète: ");
                fgets(nameAthlete, MAX_NAME_LENGHT, stdin);
                nameAthlete[strcspn(nameAthlete, "\n")] = '\0'; // Remove trailing newline
                regressLinear(nameAthlete, trainings, nbTraining, test);
                break;
            case 9:
                quit = true;
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    }
    return 0;
}
   
    
    


 


    

      

  
 
          
               
               
               
