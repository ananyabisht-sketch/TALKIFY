#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_USERS 400
#define MAX_SESSIONS 400
#define MAX_EVENTS 200
#define MAX_MATCHES 400
#define MAX_HOBBIES 20
#define TIME_LIMIT 120

typedef struct {
    char username[50];
    char password[50];
    char fullName[100];
    char gender[30];
    char email[100];
    char branch[120];
    char subject[120];
    char hobbies[300];   /* semicolon-separated */
    int semester;
    char bio[200];
    int matchPreference; /* 0: none, 1: same-branch, 2: all */
} User;

typedef struct {
    char host[50];
    char coHost[50];     /* Optional second host */
    char topic[200];
    char location[80];
    char time[40];
    char duration[20];
    int isCollaborative; /* 1 if created via matchmaking */
} Session;

typedef struct {
    char name[150];
    char field[80];
    char date[40];
} Event;

typedef struct {
    int userId1;         /* Index in users array */
    int userId2;
    float matchScore;    /* 0-100 */
    char commonSubjects[200];
    char commonHobbies[300];
    int commonSemester;
    char matchDate[40];
    int viewed;          /* 0: not viewed, 1: viewed */
} Match;

typedef struct {
    int senderId;
    int receiverId;
    int status;          /* 0: pending, 1: accepted, 2: rejected */
    char requestDate[40];
} ConnectionRequest;

/* Globals */
User users[MAX_USERS];
Session sessions[MAX_SESSIONS];
Event events[MAX_EVENTS];
Match matches[MAX_MATCHES];
ConnectionRequest requests[MAX_MATCHES];
int userCount = 0, sessionCount = 0, eventCount = 0, matchCount = 0, requestCount = 0;

char currentUser[50] = "";
int currentUserIndex = -1;
char currentBranch[120] = "";
char currentSubject[120] = "";

/* Branches and subjects (full lists) */
const char *branches[] = {
    "Computer Science Engineering (CSE)",
    "Information Technology (IT)",
    "Electronics & Communication (ECE)",
    "Electrical Engineering (EE)",
    "Mechanical Engineering (ME)",
    "Civil Engineering (CE)",
    "Artificial Intelligence & ML (AIML)",
    "Data Science Engineering (DSE)",
    "MBBS",
    "Nursing",
    "Pharmacy",
    "Physiotherapy",
    "Health Science",
    "BBA",
    "MBA",
    "Commerce (B.Com)",
    "BA-LLB",
    "BBA-LLB",
    "LLB",
    "Fashion Design",
    "Interior Design",
    "Graphic Design",
    "Fine Arts"
};
int branchCount = sizeof(branches) / sizeof(branches[0]);

const char *subjects_CS[] = {
    "C Programming", "Python", "Java", "Linux",
    "Data Structures", "Algorithms", "DBMS", "OOPS", "Operating Systems", "AI & ML"
};
const char *subjects_IT[] = {
    "C Programming", "Python", "Web Development", "Networking",
    "DBMS", "Operating Systems", "Software Engineering", "Security"
};
const char *subjects_ECE[] = {
    "Digital Electronics", "Signals & Systems", "Microprocessors",
    "Embedded Systems", "VLSI", "Control Systems", "Communication Systems"
};
const char *subjects_EE[] = {
    "Circuit Theory", "Power Systems", "Control Systems",
    "Electrical Machines", "Power Electronics", "Measurements"
};
const char *subjects_ME[] = {
    "Thermodynamics", "Manufacturing Technology", "Strength of Materials",
    "Fluid Mechanics", "Automobile Engineering", "Mechatronics"
};
const char *subjects_CE[] = {
    "Structural Analysis", "Building Materials", "Surveying",
    "Transportation Engineering", "Environmental Engineering", "Concrete Technology"
};
const char *subjects_AIML[] = {
    "Python", "Machine Learning", "Deep Learning", "Data Mining",
    "Neural Networks", "NLP", "Computer Vision"
};
const char *subjects_DSE[] = {
    "Statistics", "Python", "Data Mining", "Data Visualization",
    "Big Data", "Machine Learning"
};
const char *subjects_MBBS[] = {
    "Anatomy", "Physiology", "Biochemistry", "Microbiology", "Pathology"
};
const char *subjects_NURSING[] = {
    "Nursing Fundamentals", "Clinical Practice", "Pharmacology", "Community Health"
};
const char *subjects_PHARM[] = {
    "Pharmacology", "Pharmaceutics", "Medicinal Chemistry", "Pharmacognosy"
};
const char *subjects_PHYSIO[] = {
    "Anatomy", "Physiotherapy Practice", "Biomechanics", "Rehabilitation"
};
const char *subjects_HS[] = {
    "Public Health", "Health Informatics", "Nutrition", "Epidemiology"
};
const char *subjects_BBA[] = {
    "Accounting", "Finance", "Marketing", "Human Resource Management", "Economics"
};
const char *subjects_MBA[] = {
    "Business Strategy", "Finance", "Marketing", "Operations", "HR", "Entrepreneurship"
};
const char *subjects_COM[] = {
    "Accounting", "Business Law", "Economics", "Taxation", "Finance"
};
const char *subjects_LAW[] = {
    "Constitutional Law", "Criminal Law", "Contract Law", "Family Law", "Corporate Law"
};
const char *subjects_DESIGN[] = {
    "Graphic Design", "Fashion Illustration", "Photography",
    "UI/UX Design", "Drawing & Sketching", "Interior Space Planning"
};
const char *subjects_FA[] = {
    "Painting", "Sculpture", "Printmaking", "History of Art", "Drawing"
};

const char **subjectsMap[] = {
    subjects_CS, subjects_IT, subjects_ECE, subjects_EE, subjects_ME, subjects_CE,
    subjects_AIML, subjects_DSE, subjects_MBBS, subjects_NURSING, subjects_PHARM,
    subjects_PHYSIO, subjects_HS, subjects_BBA, subjects_MBA, subjects_COM,
    subjects_LAW, subjects_LAW, subjects_LAW, subjects_DESIGN, subjects_DESIGN,
    subjects_DESIGN, subjects_FA
};

int subjectsCountMap[] = {
    (int)(sizeof(subjects_CS)/sizeof(subjects_CS[0])),
    (int)(sizeof(subjects_IT)/sizeof(subjects_IT[0])),
    (int)(sizeof(subjects_ECE)/sizeof(subjects_ECE[0])),
    (int)(sizeof(subjects_EE)/sizeof(subjects_EE[0])),
    (int)(sizeof(subjects_ME)/sizeof(subjects_ME[0])),
    (int)(sizeof(subjects_CE)/sizeof(subjects_CE[0])),
    (int)(sizeof(subjects_AIML)/sizeof(subjects_AIML[0])),
    (int)(sizeof(subjects_DSE)/sizeof(subjects_DSE[0])),
    (int)(sizeof(subjects_MBBS)/sizeof(subjects_MBBS[0])),
    (int)(sizeof(subjects_NURSING)/sizeof(subjects_NURSING[0])),
    (int)(sizeof(subjects_PHARM)/sizeof(subjects_PHARM[0])),
    (int)(sizeof(subjects_PHYSIO)/sizeof(subjects_PHYSIO[0])),
    (int)(sizeof(subjects_HS)/sizeof(subjects_HS[0])),
    (int)(sizeof(subjects_BBA)/sizeof(subjects_BBA[0])),
    (int)(sizeof(subjects_MBA)/sizeof(subjects_MBA[0])),
    (int)(sizeof(subjects_COM)/sizeof(subjects_COM[0])),
    (int)(sizeof(subjects_LAW)/sizeof(subjects_LAW[0])),
    (int)(sizeof(subjects_LAW)/sizeof(subjects_LAW[0])),
    (int)(sizeof(subjects_LAW)/sizeof(subjects_LAW[0])),
    (int)(sizeof(subjects_DESIGN)/sizeof(subjects_DESIGN[0])),
    (int)(sizeof(subjects_DESIGN)/sizeof(subjects_DESIGN[0])),
    (int)(sizeof(subjects_DESIGN)/sizeof(subjects_DESIGN[0])),
    (int)(sizeof(subjects_FA)/sizeof(subjects_FA[0]))
};

const char *hobbiesMaster[] = {
    "Football","Basketball","Cricket","Badminton","Athletics",
    "Swimming","Yoga","Gym","Dancing","Singing",
    "Guitar","Piano","Painting","Sketching","Photography",
    "Coding","Robotics","Debate","Reading","Chess"
};
int hobbyMasterCount = sizeof(hobbiesMaster)/sizeof(hobbiesMaster[0]);

/* Helper prototypes */
void loadFiles();
void saveUsers();
void saveSessions();
void saveEvents();
void saveMatches();
void saveRequests();
void toLowerCase(const char *str, char *lower);
void remove_pipes(char *s);
void clearInputBuffer();
int branchSemesterMax(int branchIndex);
int findUserByUsername(const char *username);
float calculateMatchScore(int user1Idx, int user2Idx, char *commonSubjects, char *commonHobbies, int *commonSemester);
void generateMatches();
void displayMatches();
void viewMatchProfile(int matchUserIdx);
void sendConnectionRequest(int receiverIdx);
void viewConnectionRequests();
void createCollaborativeSession(int coHostIdx);

/* Feature prototypes */
void registerUser();
int loginUser();
void createSession();
void viewSessions();
void filterSessions();
void viewEvents();
void addEvent();
void deleteExpiredSessions();
void matchmakingMenu();
void menu();

/* ===== HELPER FUNCTIONS ===== */

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void toLowerCase(const char *str, char *lower) {
    int i;
    for (i = 0; str[i]; i++) {
        lower[i] = tolower((unsigned char)str[i]);
    }
    lower[i] = '\0';
}

void remove_pipes(char *s) {
    char *src = s, *dst = s;
    while (*src) {
        if (*src != '|') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

int branchSemesterMax(int branchIndex) {
    if (branchIndex < 0 || branchIndex >= branchCount) return 8;
    const char *b = branches[branchIndex];
    if (strstr(b, "MBBS") || strstr(b, "Nursing") || strstr(b, "Pharmacy") ||
        strstr(b, "Physiotherapy") || strstr(b, "Health Science") || strstr(b, "LLB")) {
        return 10;
    }
    if (strstr(b, "BBA") || strstr(b, "MBA") || strstr(b, "Commerce")) {
        return 6;
    }
    return 8;
}

int findUserByUsername(const char *username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

/* ===== MATCHMAKING ENGINE ===== */

int countCommonHobbies(const char *hobbies1, const char *hobbies2) {
    if (strlen(hobbies1) == 0 || strlen(hobbies2) == 0) return 0;
    
    int count = 0;
    char h1[300], h2[300];
    strcpy(h1, hobbies1);
    strcpy(h2, hobbies2);
    
    char *token1 = strtok(h1, ";");
    while (token1) {
        char *token2 = strtok(h2, ";");
        while (token2) {
            if (strcmp(token1, token2) == 0) count++;
            token2 = strtok(NULL, ";");
        }
        strcpy(h2, hobbies2);
        token1 = strtok(NULL, ";");
    }
    return count;
}

void getCommonHobbies(const char *hobbies1, const char *hobbies2, char *result) {
    strcpy(result, "");
    char h1[300], h2[300];
    strcpy(h1, hobbies1);
    strcpy(h2, hobbies2);
    
    char *token1 = strtok(h1, ";");
    while (token1) {
        char temp[300];
        strcpy(temp, hobbies2);
        char *token2 = strtok(temp, ";");
        while (token2) {
            if (strcmp(token1, token2) == 0) {
                if (strlen(result) > 0) strcat(result, "; ");
                strcat(result, token1);
            }
            token2 = strtok(NULL, ";");
        }
        token1 = strtok(NULL, ";");
    }
}

float calculateMatchScore(int user1Idx, int user2Idx, char *commonSubjects, char *commonHobbies, int *commonSemester) {
    float score = 0.0f;
    
    strcpy(commonSubjects, "");
    strcpy(commonHobbies, "");
    *commonSemester = 0;
    
    if (user1Idx < 0 || user2Idx < 0 || user1Idx >= userCount || user2Idx >= userCount) {
        return 0.0f;
    }
    
    User *u1 = &users[user1Idx];
    User *u2 = &users[user2Idx];
    
    /* Same branch: 40 points */
    if (strcmp(u1->branch, u2->branch) == 0) {
        score += 40.0f;
    }
    
    /* Same subject: 30 points */
    if (strcmp(u1->subject, u2->subject) == 0) {
        score += 30.0f;
        strcpy(commonSubjects, u1->subject);
    }
    
    /* Adjacent or same semester: 15 points */
    if (abs(u1->semester - u2->semester) <= 1) {
        score += 15.0f;
        *commonSemester = 1;
    }
    
    /* Common hobbies: 15 points (+ 1 per common hobby) */
    int hobbyCommonCount = countCommonHobbies(u1->hobbies, u2->hobbies);
    if (hobbyCommonCount > 0) {
        score += 10.0f + (hobbyCommonCount * 1.0f);
        getCommonHobbies(u1->hobbies, u2->hobbies, commonHobbies);
    }
    
    return fminf(score, 100.0f); /* Cap at 100 */
}

void generateMatches() {
    if (currentUserIndex < 0) {
        printf("You must be logged in to generate matches.\n");
        return;
    }
    
    matchCount = 0;
    
    for (int i = 0; i < userCount; i++) {
        if (i == currentUserIndex) continue;
        if (matchCount >= MAX_MATCHES) break;
        
        char commonSubjects[200], commonHobbies[300];
        int commonSemester;
        float score = calculateMatchScore(currentUserIndex, i, commonSubjects, commonHobbies, &commonSemester);
        
        /* Only include matches with score > 30 */
        if (score > 30.0f) {
            matches[matchCount].userId1 = currentUserIndex;
            matches[matchCount].userId2 = i;
            matches[matchCount].matchScore = score;
            strcpy(matches[matchCount].commonSubjects, commonSubjects);
            strcpy(matches[matchCount].commonHobbies, commonHobbies);
            matches[matchCount].commonSemester = commonSemester;
            matches[matchCount].viewed = 0;
            strcpy(matches[matchCount].matchDate, "2025-11-17");
            matchCount++;
        }
    }
    
    /* Sort matches by score (descending) */
    for (int i = 0; i < matchCount - 1; i++) {
        for (int j = i + 1; j < matchCount; j++) {
            if (matches[i].matchScore < matches[j].matchScore) {
                Match temp = matches[i];
                matches[i] = matches[j];
                matches[j] = temp;
            }
        }
    }
    
    printf("\n✅ Generated %d potential matches for you!\n", matchCount);
}

void displayMatches() {
    if (currentUserIndex < 0) {
        printf("You must be logged in to view matches.\n");
        return;
    }
    
    if (matchCount == 0) {
        printf("\n❌ No matches found. Try adjusting your profile or check back later.\n");
        return;
    }
    
    printf("\n========== YOUR MATCHES ==========\n");
    printf("Found %d potential study partners!\n\n", matchCount);
    
    for (int i = 0; i < matchCount; i++) {
        int matchUserIdx = matches[i].userId2;
        User *matchUser = &users[matchUserIdx];
        
        printf("[%d] %s - Match Score: %.1f%%\n", i+1, matchUser->fullName, matches[i].matchScore);
        printf("    Username: %s\n", matchUser->username);
        printf("    Branch: %s | Subject: %s | Semester: %d\n", 
            matchUser->branch, matchUser->subject, matchUser->semester);
        
        if (strlen(matches[i].commonSubjects) > 0) {
            printf("    Common Subject: %s\n", matches[i].commonSubjects);
        }
        
        if (strlen(matches[i].commonHobbies) > 0) {
            printf("    Common Hobbies: %s\n", matches[i].commonHobbies);
        }
        
        printf("    Bio: %s\n", matchUser->bio);
        printf("\n");
    }
    
    int choice;
    printf("Enter match number to view profile (0 to cancel): ");
    if (scanf("%d", &choice) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if (choice > 0 && choice <= matchCount) {
        viewMatchProfile(matches[choice-1].userId2);
    }
}

void viewMatchProfile(int matchUserIdx) {
    User *matchUser = &users[matchUserIdx];
    
    printf("\n========== DETAILED PROFILE ==========\n");
    printf("Name: %s\n", matchUser->fullName);
    printf("Email: %s\n", matchUser->email);
    printf("Gender: %s\n", matchUser->gender);
    printf("Branch: %s\n", matchUser->branch);
    printf("Subject: %s\n", matchUser->subject);
    printf("Semester: %d\n", matchUser->semester);
    printf("Hobbies: %s\n", matchUser->hobbies);
    printf("Bio: %s\n\n", matchUser->bio);
    
    printf("Options:\n");
    printf("1. Send Connection Request\n");
    printf("2. Create Collaborative Session\n");
    printf("3. Back\n");
    printf("Enter choice: ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    if (choice == 1) {
        sendConnectionRequest(matchUserIdx);
    } else if (choice == 2) {
        createCollaborativeSession(matchUserIdx);
    }
}

void sendConnectionRequest(int receiverIdx) {
    if (requestCount >= MAX_MATCHES) {
        printf("Error: Maximum requests reached.\n");
        return;
    }
    
    /* Check if request already exists */
    for (int i = 0; i < requestCount; i++) {
        if ((requests[i].senderId == currentUserIndex && requests[i].receiverId == receiverIdx &&
             requests[i].status == 0) || 
            (requests[i].senderId == receiverIdx && requests[i].receiverId == currentUserIndex &&
             requests[i].status == 0)) {
            printf("❌ Request already pending!\n");
            return;
        }
    }
    
    requests[requestCount].senderId = currentUserIndex;
    requests[requestCount].receiverId = receiverIdx;
    requests[requestCount].status = 0; /* pending */
    strcpy(requests[requestCount].requestDate, "2025-11-17");
    requestCount++;
    saveRequests();
    
    printf("✅ Connection request sent to %s!\n", users[receiverIdx].fullName);
}

void viewConnectionRequests() {
    if (currentUserIndex < 0) {
        printf("You must be logged in.\n");
        return;
    }
    
    int found = 0;
    printf("\n========== CONNECTION REQUESTS ==========\n");
    
    for (int i = 0; i < requestCount; i++) {
        if (requests[i].receiverId == currentUserIndex && requests[i].status == 0) {
            found++;
            printf("[%d] From: %s (%s)\n", found, users[requests[i].senderId].fullName,
                users[requests[i].senderId].username);
            printf("    Branch: %s | Subject: %s\n", 
                users[requests[i].senderId].branch, users[requests[i].senderId].subject);
            printf("    Date: %s\n\n", requests[i].requestDate);
        }
    }
    
    if (found == 0) {
        printf("No pending requests.\n");
    }
}

void createCollaborativeSession(int coHostIdx) {
    if (sessionCount >= MAX_SESSIONS) {
        printf("Error: Maximum session limit reached.\n");
        return;
    }
    
    Session s;
    memset(&s, 0, sizeof(Session));
    strcpy(s.host, currentUser);
    strcpy(s.coHost, users[coHostIdx].username);
    s.isCollaborative = 1;
    
    clearInputBuffer();
    
    printf("\n========== CREATE COLLABORATIVE SESSION ==========\n");
    printf("Co-host: %s\n\n", users[coHostIdx].fullName);
    
    printf("Enter topic/subject for this session: ");
    if (fgets(s.topic, sizeof(s.topic), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    s.topic[strcspn(s.topic, "\n")] = '\0';
    remove_pipes(s.topic);
    
    printf("Enter location: ");
    if (fgets(s.location, sizeof(s.location), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    s.location[strcspn(s.location, "\n")] = '\0';
    remove_pipes(s.location);
    
    printf("Enter time (e.g., 4 PM or 2025-11-10 16:00): ");
    if (fgets(s.time, sizeof(s.time), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    s.time[strcspn(s.time, "\n")] = '\0';
    remove_pipes(s.time);
    
    printf("Enter duration (in minutes): ");
    if (scanf("%19s", s.duration) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }
    clearInputBuffer();
    
    sessions[sessionCount++] = s;
    saveSessions();
    
    printf("\n✅ Collaborative session created!\n");
    printf("Session: %s\n", s.topic);
    printf("Hosts: %s & %s\n", currentUser, s.coHost);
}

/* ===== FILE I/O ===== */

void loadFiles() {
    FILE *fu = fopen("users.txt", "r");
    if (fu) {
        char line[1024];
        while (userCount < MAX_USERS && fgets(line, sizeof(line), fu)) {
            char *tokens[11];
            int tokenCount = 0;
            char lineCopy[1024];
            strncpy(lineCopy, line, sizeof(lineCopy)-1);
            lineCopy[sizeof(lineCopy)-1] = '\0';
            
            char *token = strtok(lineCopy, "|");
            while (token != NULL && tokenCount < 11) {
                tokens[tokenCount++] = token;
                token = strtok(NULL, (tokenCount == 10) ? "\n" : "|");
            }
            
            if (tokenCount >= 10) {
                strncpy(users[userCount].username, tokens[0], sizeof(users[userCount].username)-1);
                strncpy(users[userCount].password, tokens[1], sizeof(users[userCount].password)-1);
                strncpy(users[userCount].fullName, tokens[2], sizeof(users[userCount].fullName)-1);
                strncpy(users[userCount].gender, tokens[3], sizeof(users[userCount].gender)-1);
                strncpy(users[userCount].email, tokens[4], sizeof(users[userCount].email)-1);
                strncpy(users[userCount].branch, tokens[5], sizeof(users[userCount].branch)-1);
                strncpy(users[userCount].subject, tokens[6], sizeof(users[userCount].subject)-1);
                strncpy(users[userCount].hobbies, tokens[7], sizeof(users[userCount].hobbies)-1);
                users[userCount].semester = atoi(tokens[8]);
                strncpy(users[userCount].bio, tokens[9], sizeof(users[userCount].bio)-1);
                users[userCount].bio[strcspn(users[userCount].bio, "\n")] = '\0';
                users[userCount].matchPreference = (tokenCount > 10) ? atoi(tokens[10]) : 1;
                userCount++;
            }
        }
        fclose(fu);
    }

    FILE *fs = fopen("sessions.txt", "r");
    if (fs) {
        char line[512];
        while (sessionCount < MAX_SESSIONS && fgets(line, sizeof(line), fs)) {
            char *tokens[7];
            int tokenCount = 0;
            char lineCopy[512];
            strncpy(lineCopy, line, sizeof(lineCopy)-1);
            lineCopy[sizeof(lineCopy)-1] = '\0';
            
            char *token = strtok(lineCopy, "|");
            while (token != NULL && tokenCount < 7) {
                tokens[tokenCount++] = token;
                token = strtok(NULL, (tokenCount == 6) ? "\n" : "|");
            }
            
            if (tokenCount >= 5) {
                strncpy(sessions[sessionCount].host, tokens[0], sizeof(sessions[sessionCount].host)-1);
                strncpy(sessions[sessionCount].coHost, (tokenCount > 5) ? tokens[1] : "", sizeof(sessions[sessionCount].coHost)-1);
                strncpy(sessions[sessionCount].topic, tokens[tokenCount > 5 ? 2 : 1], sizeof(sessions[sessionCount].topic)-1);
                strncpy(sessions[sessionCount].location, tokens[tokenCount > 5 ? 3 : 2], sizeof(sessions[sessionCount].location)-1);
                strncpy(sessions[sessionCount].time, tokens[tokenCount > 5 ? 4 : 3], sizeof(sessions[sessionCount].time)-1);
                strncpy(sessions[sessionCount].duration, tokens[tokenCount > 5 ? 5 : 4], sizeof(sessions[sessionCount].duration)-1);
                sessions[sessionCount].duration[strcspn(sessions[sessionCount].duration, "\n")] = '\0';
                sessions[sessionCount].isCollaborative = (tokenCount > 5) ? 1 : 0;
                sessionCount++;
            }
        }
        fclose(fs);
    }

    FILE *fe = fopen("events.txt", "r");
    if (fe) {
        char line[512];
        while (eventCount < MAX_EVENTS && fgets(line, sizeof(line), fe)) {
            char *tokens[3];
            int tokenCount = 0;
            char lineCopy[512];
            strncpy(lineCopy, line, sizeof(lineCopy)-1);
            lineCopy[sizeof(lineCopy)-1] = '\0';
            
            char *token = strtok(lineCopy, "|");
            while (token != NULL && tokenCount < 3) {
                tokens[tokenCount++] = token;
                token = strtok(NULL, (tokenCount == 2) ? "\n" : "|");
            }
            
            if (tokenCount == 3) {
                strncpy(events[eventCount].name, tokens[0], sizeof(events[eventCount].name)-1);
                strncpy(events[eventCount].field, tokens[1], sizeof(events[eventCount].field)-1);
                strncpy(events[eventCount].date, tokens[2], sizeof(events[eventCount].date)-1);
                events[eventCount].date[strcspn(events[eventCount].date, "\n")] = '\0';
                eventCount++;
            }
        }
        fclose(fe);
    }
}

void saveUsers() {
    FILE *fp = fopen("users.txt", "w");
    if (!fp) {
        printf("Error: Could not save users file.\n");
        return;
    }
    for (int i = 0; i < userCount; i++) {
        fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%d|%s|%d\n",
            users[i].username, users[i].password, users[i].fullName, users[i].gender,
            users[i].email, users[i].branch, users[i].subject, users[i].hobbies,
            users[i].semester, users[i].bio, users[i].matchPreference);
    }
    fclose(fp);
}

void saveSessions() {
    FILE *fp = fopen("sessions.txt", "w");
    if (!fp) {
        printf("Error: Could not save sessions file.\n");
        return;
    }
    for (int i = 0; i < sessionCount; i++) {
        if (sessions[i].isCollaborative) {
            fprintf(fp, "%s|%s|%s|%s|%s|%s|%d\n",
                sessions[i].host, sessions[i].coHost, sessions[i].topic,
                sessions[i].location, sessions[i].time, sessions[i].duration,
                sessions[i].isCollaborative);
        } else {
            fprintf(fp, "%s||%s|%s|%s|%s|0\n",
                sessions[i].host, sessions[i].topic, sessions[i].location,
                sessions[i].time, sessions[i].duration);
        }
    }
    fclose(fp);
}

void saveEvents() {
    FILE *fp = fopen("events.txt", "w");
    if (!fp) {
        printf("Error: Could not save events file.\n");
        return;
    }
    for (int i = 0; i < eventCount; i++) {
        fprintf(fp, "%s|%s|%s\n", events[i].name, events[i].field, events[i].date);
    }
    fclose(fp);
}

void saveMatches() {
    /* Matches are generated dynamically, no need to save */
}

void saveRequests() {
    FILE *fp = fopen("requests.txt", "w");
    if (!fp) return;
    for (int i = 0; i < requestCount; i++) {
        fprintf(fp, "%d|%d|%d|%s\n",
            requests[i].senderId, requests[i].receiverId,
            requests[i].status, requests[i].requestDate);
    }
    fclose(fp);
}

/* ===== REGISTRATION & LOGIN ===== */

void registerUser() {
    if (userCount >= MAX_USERS) {
        printf("\nError: Maximum user limit reached.\n");
        return;
    }

    User u;
    memset(&u, 0, sizeof(User));
    u.matchPreference = 1;
    int bchoice = -1;

    printf("\n========== REGISTER NEW USER ==========\n");
    printf("Enter username (no spaces): ");
    if (scanf("%49s", u.username) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }
    
    for (int i = 0; i < userCount; i++) {
        if (strcmp(u.username, users[i].username) == 0) {
            printf("Error: Username already exists.\n");
            clearInputBuffer();
            return;
        }
    }
    
    printf("Enter password (no spaces): ");
    if (scanf("%49s", u.password) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }
    clearInputBuffer();

    printf("Enter your full name: ");
    if (fgets(u.fullName, sizeof(u.fullName), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    u.fullName[strcspn(u.fullName, "\n")] = '\0';
    remove_pipes(u.fullName);

    printf("\nChoose gender:\n1. Male\n2. Female\n3. Other\nEnter choice: ");
    int gchoice;
    if (scanf("%d", &gchoice) != 1) {
        clearInputBuffer();
        gchoice = 3;
    }
    clearInputBuffer();
    
    if (gchoice == 1) strcpy(u.gender, "Male");
    else if (gchoice == 2) strcpy(u.gender, "Female");
    else strcpy(u.gender, "Other");

    printf("Enter email id: ");
    if (scanf("%99s", u.email) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }
    clearInputBuffer();
    remove_pipes(u.email);

    printf("\nChoose your branch/field:\n");
    for (int i = 0; i < branchCount; i++) {
        printf("%2d. %s\n", i+1, branches[i]);
    }
    printf("Enter choice (number): ");
    if (scanf("%d", &bchoice) != 1 || bchoice < 1 || bchoice > branchCount) {
        clearInputBuffer();
        printf("Invalid branch choice. Registration cancelled.\n");
        return;
    }
    clearInputBuffer();
    strncpy(u.branch, branches[bchoice - 1], sizeof(u.branch)-1);
    u.branch[sizeof(u.branch)-1] = '\0';

    int scount = subjectsCountMap[bchoice - 1];
    const char **slist = subjectsMap[bchoice - 1];
    printf("\nChoose subject from %s:\n", branches[bchoice - 1]);
    for (int s = 0; s < scount; s++) {
        printf("%2d. %s\n", s+1, slist[s]);
    }
    printf("Enter choice (number): ");
    int schoice;
    if (scanf("%d", &schoice) != 1 || schoice < 1 || schoice > scount) {
        clearInputBuffer();
        printf("Invalid subject choice. Registration cancelled.\n");
        return;
    }
    clearInputBuffer();
    strncpy(u.subject, slist[schoice - 1], sizeof(u.subject)-1);
    u.subject[sizeof(u.subject)-1] = '\0';
    remove_pipes(u.subject);

    printf("\nChoose your hobbies/activities (enter count then indices).\nAvailable hobbies:\n");
    for (int i = 0; i < hobbyMasterCount; i++) {
        printf("%2d. %s\n", i+1, hobbiesMaster[i]);
    }
    printf("Enter how many hobbies to choose (max 10): ");
    int hcount;
    if (scanf("%d", &hcount) != 1) {
        clearInputBuffer();
        hcount = 0;
    }
    if (hcount < 0) hcount = 0;
    if (hcount > 10) hcount = 10;
    
    char hobbies_buf[300] = "";
    if (hcount > 0) {
        printf("Now enter %d numbers (space or newline separated):\n", hcount);
        for (int i = 0; i < hcount; i++) {
            int idx;
            if (scanf("%d", &idx) != 1) {
                clearInputBuffer();
                continue;
            }
            if (idx >= 1 && idx <= hobbyMasterCount) {
                size_t current_len = strlen(hobbies_buf);
                size_t hobby_len = strlen(hobbiesMaster[idx-1]);
                if (current_len + hobby_len + 2 < sizeof(hobbies_buf)) {
                    if (current_len > 0) strcat(hobbies_buf, ";");
                    strcat(hobbies_buf, hobbiesMaster[idx-1]);
                }
            }
        }
    }
    clearInputBuffer();
    
    strncpy(u.hobbies, hobbies_buf, sizeof(u.hobbies)-1);
    u.hobbies[sizeof(u.hobbies)-1] = '\0';
    remove_pipes(u.hobbies);

    int maxSem = branchSemesterMax(bchoice - 1);
    printf("\nEnter current semester (1 - %d): ", maxSem);
    int sem;
    if (scanf("%d", &sem) != 1) {
        clearInputBuffer();
        sem = 1;
    } else {
        clearInputBuffer();
    }
    
    if (sem < 1) sem = 1;
    if (sem > maxSem) sem = maxSem;
    u.semester = sem;

    printf("\nEnter a short bio (one line) - avoid pipes '|': ");
    if (fgets(u.bio, sizeof(u.bio), stdin) == NULL) {
        strcpy(u.bio, "");
    }
    u.bio[strcspn(u.bio, "\n")] = '\0';
    remove_pipes(u.bio);

    users[userCount++] = u;
    saveUsers();
    printf("\n✅ Registration successful! Welcome %s\n", u.username);
}

int loginUser() {
    char uname[50], pass[50];
    printf("\n========== LOGIN ==========\n");
    printf("Username: ");
    if (scanf("%49s", uname) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return 0;
    }
    printf("Password: ");
    if (scanf("%49s", pass) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return 0;
    }
    clearInputBuffer();
    
    for (int i = 0; i < userCount; i++) {
        if (strcmp(uname, users[i].username) == 0 &&
            strcmp(pass, users[i].password) == 0) {
            strcpy(currentUser, users[i].username);
            currentUserIndex = i;
            strcpy(currentBranch, users[i].branch);
            strcpy(currentSubject, users[i].subject);
            printf("\n✅ Login successful. Welcome %s!\n", users[i].username);
            printf("Branch: %s | Subject: %s | Semester: %d\n",
                users[i].branch, users[i].subject, users[i].semester);
            return 1;
        }
    }
    printf("\n❌ Invalid credentials.\n");
    return 0;
}

/* ===== SESSIONS ===== */

void createSession() {
    if (strlen(currentUser) == 0) {
        printf("You must be logged in to create sessions.\n");
        return;
    }
    if (sessionCount >= MAX_SESSIONS) {
        printf("Error: Maximum session limit reached.\n");
        return;
    }

    Session s;
    memset(&s, 0, sizeof(Session));
    strcpy(s.host, currentUser);
    strcpy(s.coHost, "");
    s.isCollaborative = 0;
    
    printf("\nEnter topic/subject for this session: ");
    if (fgets(s.topic, sizeof(s.topic), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    s.topic[strcspn(s.topic, "\n")] = '\0';
    remove_pipes(s.topic);

    printf("Enter location: ");
    if (fgets(s.location, sizeof(s.location), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    s.location[strcspn(s.location, "\n")] = '\0';
    remove_pipes(s.location);

    printf("Enter time (e.g., 4 PM or 2025-11-10 16:00): ");
    if (fgets(s.time, sizeof(s.time), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    s.time[strcspn(s.time, "\n")] = '\0';
    remove_pipes(s.time);

    printf("Enter duration (in minutes): ");
    if (scanf("%19s", s.duration) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }
    clearInputBuffer();

    sessions[sessionCount++] = s;
    saveSessions();
    printf("\n✅ Session created.\n");
}

void viewSessions() {
    if (sessionCount == 0) {
        printf("\nNo sessions available.\n");
        return;
    }
    printf("\n========== ALL SESSIONS ==========\n");
    for (int i = 0; i < sessionCount; i++) {
        int dur = atoi(sessions[i].duration);
        const char *status = (dur > TIME_LIMIT) ? "EXPIRED" : "ACTIVE";
        printf("\n[%d] %s (%s)\n Host: %s", i+1, sessions[i].topic, status, sessions[i].host);
        if (strlen(sessions[i].coHost) > 0) {
            printf(" & %s", sessions[i].coHost);
        }
        printf("\n Location: %s | Time: %s | Duration: %s min\n",
            sessions[i].location, sessions[i].time, sessions[i].duration);
    }
}

void filterSessions() {
    if (sessionCount == 0) {
        printf("\nNo sessions available to filter.\n");
        return;
    }
    printf("\n========== FILTER SESSIONS ==========\n");
    printf("1. Filter by Branch\n2. Filter by Hobby\n3. Collaborative Sessions Only\nEnter choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) {
        clearInputBuffer();
        printf("Invalid input.\n");
        return;
    }

    if (choice == 3) {
        printf("\n--- Collaborative Sessions ---\n");
        int found = 0;
        for (int i = 0; i < sessionCount; i++) {
            if (sessions[i].isCollaborative) {
                printf("\nSession: %s\nHosts: %s & %s\nLocation: %s | Time: %s\n",
                    sessions[i].topic, sessions[i].host, sessions[i].coHost,
                    sessions[i].location, sessions[i].time);
                found = 1;
            }
        }
        if (!found) printf("No collaborative sessions found.\n");
    }
}

void viewEvents() {
    if (eventCount == 0) {
        printf("\nNo campus events available.\n");
        return;
    }
    printf("\n========== CAMPUS EVENTS ==========\n");
    for (int i = 0; i < eventCount; i++) {
        printf("\n[%d] %s\n Field: %s | Date: %s\n", i+1, events[i].name, events[i].field, events[i].date);
    }
}

void addEvent() {
    if (eventCount >= MAX_EVENTS) {
        printf("Error: Maximum event limit reached.\n");
        return;
    }

    Event e;
    memset(&e, 0, sizeof(Event));
    
    printf("\nEnter event name: ");
    if (fgets(e.name, sizeof(e.name), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    e.name[strcspn(e.name, "\n")] = '\0';
    remove_pipes(e.name);
    
    printf("Enter event field (e.g., Sports, Tech, Arts): ");
    if (fgets(e.field, sizeof(e.field), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    e.field[strcspn(e.field, "\n")] = '\0';
    remove_pipes(e.field);
    
    printf("Enter event date (e.g., 2025-11-10): ");
    if (fgets(e.date, sizeof(e.date), stdin) == NULL) {
        printf("Invalid input.\n");
        return;
    }
    e.date[strcspn(e.date, "\n")] = '\0';
    remove_pipes(e.date);
    
    events[eventCount++] = e;
    saveEvents();
    printf("\n✅ Event added: %s (%s) on %s\n", e.name, e.field, e.date);
}

void deleteExpiredSessions() {
    if (sessionCount == 0) {
        printf("\nNo sessions to clean.\n");
        return;
    }

    int newCount = 0;
    for (int i = 0; i < sessionCount; i++) {
        int dur = atoi(sessions[i].duration);
        if (dur <= TIME_LIMIT) {
            sessions[newCount++] = sessions[i];
        }
    }

    int removed = sessionCount - newCount;
    sessionCount = newCount;
    saveSessions();

    if (removed == 0)
        printf("\n✅ No expired sessions found.\n");
    else
        printf("\n✅ %d expired session(s) deleted.\n", removed);
}

/* ===== MATCHMAKING MENU ===== */

void matchmakingMenu() {
    while (1) {
        printf("\n========== MATCHMAKING MENU ==========\n");
        printf("1. Find Study Partners (Generate Matches)\n");
        printf("2. View My Matches\n");
        printf("3. View Connection Requests\n");
        printf("4. Back to Main Menu\n");
        printf("Enter choice: ");

        int c;
        if (scanf("%d", &c) != 1) {
            clearInputBuffer();
            printf("Invalid input.\n");
            continue;
        }
        clearInputBuffer();

        if (c == 1) generateMatches();
        else if (c == 2) displayMatches();
        else if (c == 3) viewConnectionRequests();
        else if (c == 4) return;
        else printf("Invalid choice.\n");
    }
}

/* ===== MAIN MENU ===== */

void menu() {
    while (1) {
        printf("\n========== TALKIFY MENU ==========\n");
        printf("1. Find Study Partners (Matchmaking)\n");
        printf("2. Create Study Session\n");
        printf("3. View All Sessions\n");
        printf("4. Filter Sessions\n");
        printf("5. View Campus Events\n");
        printf("6. Add Campus Event\n");
        printf("7. Logout\n");
        printf("Enter choice: ");

        int c;
        if (scanf("%d", &c) != 1) {
            clearInputBuffer();
            printf("Invalid input.\n");
            continue;
        }
        clearInputBuffer();

        if (c == 1) matchmakingMenu();
        else if (c == 2) createSession();
        else if (c == 3) viewSessions();
        else if (c == 4) filterSessions();
        else if (c == 5) viewEvents();
        else if (c == 6) addEvent();
        else if (c == 7) {
            printf("Logged out: %s\n", currentUser);
            currentUser[0] = '\0';
            currentUserIndex = -1;
            currentBranch[0] = '\0';
            currentSubject[0] = '\0';
            return;
        }
        else printf("Invalid choice.\n");
    }
}

/* ===== MAIN ===== */

int main() {
    loadFiles();
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║  TALKIFY - StudyMate & Activity Finder with Matchmaking  ║\n");
    printf("║           Find Study Partners, Build Networks             ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    
    while (1) {
        printf("\n========== MAIN MENU ==========\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Invalid input.\n");
            continue;
        }
        clearInputBuffer();
        
        if (choice == 1) {
            registerUser();
        }
        else if (choice == 2) {
            if (loginUser()) {
                menu();
            }
        }
        else if (choice == 3) {
            printf("\n👋 Thank you for using Talkify. Goodbye!\n\n");
            break;
        }
        else {
            printf("Invalid choice.\n");
        }
    }
    
    return 0;
}
