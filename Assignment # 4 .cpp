#include <cstring>
#include <iostream>
#include<fstream>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#pragma warning(disable : 4996)
using namespace std;
//////////////// PLAYERANSWER CLASS ////////////////
class PlayerAnswer 
{
    char playerAnswer[40];
public:
    PlayerAnswer() 
    {

        playerAnswer[0] = '\0';

    }
    void setPlayerAnswer(const char pa[])
    {
        strcpy(playerAnswer, pa);
    }
    char* getPlayerAnswer() 
    {
        return playerAnswer;
    }

};
//////////////// BASE CLASS QUESTION  ////////////////// 
class Question 
{
protected:
    int id;
    static int nextID;
    char queType[10]; // "MCQ", "TF"
    char questionText[100];
    PlayerAnswer pA;

public:
    Question() : id(nextID++) {}
    virtual ~Question() {}
    virtual void inputQuestion() = 0;
    int getID() const { return id; }
    void setQuestionText(const char* text) { strcpy(questionText, text); }
    virtual void display(bool isAdmin) = 0;
    virtual bool checkAnswer(char pa[]) = 0;
    virtual void readQuestionsFromFile(ifstream& file) = 0;
    virtual void saveToFile(ofstream& file) = 0;      // Save to file
    PlayerAnswer& getPlayerAnswerObject()
    {
        return pA; // Return the object itself
    }
    char* getType() { return queType; }
    virtual char* getCorrectAnswer() = 0;
    // Operator overloading to check duplicate questions
    bool operator==(const Question& other)
    {
        char modifiedOther[100];
        strcpy(modifiedOther, other.questionText);
        for (int i = 0; modifiedOther[i] != '\0'; i++)
        {
            modifiedOther[i] = tolower(modifiedOther[i]);  // Convert each character to lowercase
        }

        char modifiedthis[100];
        strcpy(modifiedthis, questionText);
        // Also convert the current questionText to lowercase
        for (int i = 0; modifiedthis[i] != '\0'; i++)
        {
            modifiedthis[i] = tolower(modifiedthis[i]);  // Convert each character to lowercase
        }
        // cout<<"\n\n"<<modifiedthis<<"\n"<<modifiedOther<<"\n\n";
        // cout<<"\n\n"<<queType<<"\n"<<other.queType<<"\n\n";
        if (strcmp(modifiedthis, modifiedOther) == 0 && strcmp(queType, other.queType) == 0)
        {
            return true;
        }
        return false;
    }
    void setType(char type[]) 
    {
        strcpy(queType, type);
    }
};
int Question::nextID = 1;

//////////////// DERIVED CLASS MCQUESTION  ////////////////// 
class MCQuestion : public Question {
private:
    char choices[4][40]; // 4 choices
    int correctChoice;   // Correct choice index (0-3)


public:
    // MCQQuestion(int id, const char *qText, const char choicesArray[4][50],
    //             int correctIndex)
    //     : Question(id, "MCQ", qText), correctOption(correctIndex) {
    //   for (int i = 0; i < 4; i++) {
    //     strcpy(options[i], choicesArray[i]);
    //   }
    // }

    MCQuestion() : correctChoice(0), choices{ '\0' } {}

    // void setChoices(const char* c[4], int correct) {
    //     for (int i = 0; i < 4; i++) {
    //         strcpy(choices[i], c[i]); 
    //     }
    //     correctChoice = correct;
    // }

    void inputQuestion() override {
        cout << "Enter question statement: ";
        cin.getline(questionText, 100);

        cout << "\nEnter 4 choices, make sure the first choice is the correct one";
        for (int i = 0; i < 4; i++) {
            cout << "\nChoice " << i + 1 << ": ";
            cin.getline(choices[i], 40);
        }

    }

    void display(bool isAdmin) override {
        if (isAdmin) {
            cout << "(ID: " << id << ") " << questionText << endl;
            for (int i = 0; i < 4; i++) {
                cout << char('a' + i) << ") ";
                if (i == 0) {
                    cout << "* " << choices[i] << endl; // Mark the correct choice
                }
                else {
                    cout << choices[i] << endl;
                }
            }
        }
        else {
            // Player mode hides question ID and correct answer
            cout << questionText << endl;
            for (int i = 0; i < 4; i++) {
                cout << char('a' + i) << ") " << choices[i] << endl;
            }

        }
    }
    void randomizeChoices() {
        int j;
        for (int i = 3; i > 0; i--) {
            j = rand() % (i + 1); // Generate random index within the remaining choices

            // Swap choices[i] and choices[j]
            char temp[20];
            strcpy(temp, choices[i]);
            strcpy(choices[i], choices[j]);
            strcpy(choices[j], temp);

            // Adjust correct answer index if necessary
            if (correctChoice == i) {
                correctChoice = j;
            }
            else if (correctChoice == j) {
                correctChoice = i;
            }
        }
    }

    void readQuestionsFromFile(ifstream& file) override {
        strcpy(queType, "MCQ");
        file.getline(questionText, sizeof(questionText));
        for (int i = 0; i < 4; i++) {
            file.getline(choices[i], sizeof(choices[i]));
        }

        // file.ignore(); // Ignore the newline character after reading correctChoice
    }
    char* getCorrectAnswer() override {
        return choices[correctChoice]; // correct choice has an index
    }
    bool checkAnswer(char pa[]) override {
        //case sensitive so converting both player answer and correct answer to lower case
        for (int i = 0; pa[i]; i++) {
            pa[i] = tolower(pa[i]);
        }
        //correcAnswer shudnt be changed so storing it inanother variable then changing that
        char newCorrectAns[40];
        strcpy(newCorrectAns, choices[correctChoice]);

        for (int i = 0; newCorrectAns[i]; i++) {
            newCorrectAns[i] = tolower(newCorrectAns[i]);
        }
        // Compare player's answer with the correct choice
        if (strcmp(pa, newCorrectAns) == 0) {
            return true;
        }
        return false;
    }
    ~MCQuestion() {}
    void saveToFile(ofstream& file) override {
        file << "MCQ\n" << questionText << "\n";
        for (int i = 0; i < 4; i++) {
            file << choices[i] << "\n";
        }
    }
};

//////////////// DERIVED CLASS TFQUESTION  ////////////////// 
class TFQuestion : public Question {
private:
    char correctAnswer[6]; // "True" or "False"

public:
    TFQuestion() :correctAnswer{ '\0' } {}
    ~TFQuestion() {}
    void display(bool isAdmin) override {
        if (isAdmin) {
            cout << "(ID: " << id << ") " << questionText << " (Answer: " << correctAnswer << ")\n";
        }
        else {
            // Player mode
            cout << questionText << endl;
            cout << "a) True\nb) False\n";
        }
    }
    void inputQuestion() override {
        cout << "Enter question statement: ";
        cin.getline(questionText, 100);

        do {
            cout << "Enter the correct answer (True/False): ";
            cin.getline(correctAnswer, 6);

            if (strcmp(correctAnswer, "true") == 0 || strcmp(correctAnswer, "false") == 0)
                break;

            cout << "Invalid input! Please enter 'true' or 'false'.\n";
        } while (true);
    }
    void readQuestionsFromFile(ifstream& file) override {
        strcpy(queType, "TF");
        file.getline(questionText, sizeof(questionText));
        file.getline(correctAnswer, sizeof(correctAnswer));
    }
    void saveToFile(ofstream& file) override {
        file << "TF\n" << questionText << "\n";
        file << correctAnswer << "\n";
    }
    char* getCorrectAnswer()override {
        return correctAnswer;
    }
    bool checkAnswer(char pa[]) override {
        //case sensitive so converting both player answer and correct answer to lower case
        for (int i = 0; pa[i]; i++) {
            pa[i] = tolower(pa[i]);
        }
        //correcAnswer shudnt be changed so storing it inanother variable then changing that
        char newCorrectAns[40];

        strcpy(newCorrectAns, correctAnswer);
        for (int i = 0; newCorrectAns[i]; i++) {
            newCorrectAns[i] = tolower(newCorrectAns[i]);
        }
        // Compare player's answer with the correct choice
        if (strcmp(pa, newCorrectAns) == 0) {
            return true;
        }
        return false;
    }
};
//////////////// QUIZ CLASS  //////////////////
class Quiz {
protected:
    int score;
    Question* selectedQuestions[3];
    int selectedCount;
    const int totalMcqs;
    const int totalTf;
    const int marksPerMcq;
    const int marksPerTf;
    int correctCount;
    int wrongCount;


public:
    Quiz();
    void randomizeQuestions(Question* questionPool[], int& questionCount);
    void playQuiz();
    void quizReport();
    void displayDetails();
    int getScore();
    int getMcqCount();
    int getTfCount();
    int getTfScore();
    int getMcqScore();
    ~Quiz();
};
Quiz::Quiz() : score(0), selectedCount(0), selectedQuestions{ nullptr }, marksPerMcq(2),
marksPerTf(1), correctCount(0), wrongCount(0), totalMcqs(2), totalTf(1) {}
void Quiz::randomizeQuestions(Question* questionPool[], int& questionCount) {
    //3 mcqs  1 tf
    // randomly select questions from question pool and store them in selectedQuestions 
    int mcqCount = 0;
    int tfCount = 0;
    bool usedIndices[50] = { false };

    while (mcqCount < 2 || tfCount < 1) {
        int randomIndex = rand() % questionCount;

        if (usedIndices[randomIndex]) {
            continue;
        }

        // Mark the index as used
        usedIndices[randomIndex] = true;

        if ((strcmp(questionPool[randomIndex]->getType(), "MCQ") == 0) && mcqCount < 2) {
            selectedQuestions[selectedCount++] = questionPool[randomIndex];
            mcqCount++;
            //randomize choices of mcqs as well

            MCQuestion* mcq = dynamic_cast<MCQuestion*>(questionPool[randomIndex]);
            if (mcq) {
                mcq->randomizeChoices();
            }


        }
        else if ((strcmp(questionPool[randomIndex]->getType(), "TF") == 0) && tfCount < 1) {
            selectedQuestions[selectedCount++] = questionPool[randomIndex];
            tfCount++;
        }
    }


}
void Quiz::playQuiz() {
    //display a que get ans increment score; keep count of wrong and right answers
    char playerAnswer[40]; // to get choice from user
    Question* q; // for easy readability duhh
    cout << "\n\n****  Answer the following questions ****\n\n";
    cin.ignore();
    for (int i = 0; i < selectedCount; i++) {
        q = selectedQuestions[i];
        q->display(false); // Player mode

        cout << "Enter your answer: ";

        cin.getline(playerAnswer, 40);

        //link with player answer check how
        //player answer composition question
        //pA object stored in question class its functions can only be accessed like pA.get etc 
        // so to get that pA object here we return it from the question class 
        q->getPlayerAnswerObject().setPlayerAnswer(playerAnswer);

        if (q->checkAnswer(playerAnswer)) {
            cout << "Correct Answer!\n\n";
            correctCount++;
            //in my quiz im setting 2 points for mcq & 1 for tf so to assign those:
            if (strcmp(q->getType(), "MCQ") == 0) {
                score += marksPerMcq;
            }
            else {
                score += marksPerTf;
            }

        }
        else {
            //made getCorrectAnswer() virtual even tho no attribute of correct in base; easy use
            cout << "Incorrect Answer! The correct answer was " << q->getCorrectAnswer() << endl << endl;
            wrongCount++;
        }

    }//quiz finished now display report
    cout << "Quiz Report:";
    quizReport();
    //now control goes to player menu
}
void Quiz::quizReport() {
    cout << "\nNumber of Correct Answers: " << correctCount
        << "\nNumber of Incorrect Answers: " << wrongCount
        << "\nYour Score: " << score << "/5\n";
}
void Quiz::displayDetails() {
    Question* q;
    for (int i = 0; i < selectedCount; i++) {
        q = selectedQuestions[i];
        cout << "\n\nQuestion " << (i + 1) << ":\n";
        q->display(false); // Display the question
        char playerAnswer[40];
        //pa.
        strcpy(playerAnswer, q->getPlayerAnswerObject().getPlayerAnswer());

        cout << "\nYour Answer: " << playerAnswer << endl;
        cout << "Your answer was ";
        if (q->checkAnswer(playerAnswer)) {
            cout << "correct!\n";
        }
        else {
            cout << "incorrect!\n";
        }

        cout << "Correct Answer: " << q->getCorrectAnswer() << endl;
        cout << "Score for this Question: ";
        if (strcmp(q->getType(), "MCQ") == 0) {
            cout << marksPerMcq;
        }
        else {
            cout << marksPerTf;
        }
    }
    cout << "\nTotal Score for this Quiz: " << score << "\n";
}
int Quiz::getScore() { return score; }
int Quiz::getMcqCount() { return totalMcqs; }
int Quiz::getTfCount() { return totalTf; }
int Quiz::getTfScore() { return marksPerTf; }
int Quiz::getMcqScore() { return marksPerMcq; }
Quiz::~Quiz() {
    for (int i = 0; i < selectedCount; ++i) {
        selectedQuestions[i] = nullptr;  // nullify the pointers
    }
}
////////////////// BASE CLASS USER //////////////////
class User {
protected:
    char username[15];
    char password[15];
    char firstName[15];
    char lastName[15];
    char role[10];

public:
    User(const char uN[], const char pw[], const char fN[], const char lN[],
        const char r[]);

    bool checkValidity(const char uN[], const char pw[]);
    char* getRole();
    virtual void displayMenu() = 0;
    void updateName();
    char* getFName();
    char* getLName();
    char* getUName();
    char* getUsername();
    virtual ~User() {}
};
////////////////// USER FUNCTIONS //////////////////
char* User::getFName() { return firstName; }
char* User::getLName() { return lastName; }
char* User::getUName() { return username; }

void User::updateName()
{
    char fName[15];
    char lName[15];
    cin.ignore();
    cout << "\nEnter Your First Name: ";
    cin.getline(fName, 15);


    cout << "Enter Your Last Name: ";
    cin.getline(lName, 15);


    strcpy(firstName, fName);
    strcpy(lastName, lName);

    cout << "Name Updated Successfully\n";
}
User::User(const char uN[], const char pw[], const char fN[], const char lN[],
    const char r[]) {
    strcpy(username, uN);
    strcpy(password, pw);
    strcpy(firstName, fN);
    strcpy(lastName, lN);
    strcpy(role, r);
}

// to check for a valid username & password (input)
bool User::checkValidity(const char uN[], const char pw[]) {

    if (strcmp(username, uN) == 0) {
        // Names match now checking pw

        if (strcmp(password, pw) == 0) {
            // passwords match
            return true;
        }
        else {
            // passwords don't match
            return false;
        }

    }
    else {
        // Names dont match
        return false;
    }
}

char* User::getRole() { return role; }
char* User::getUsername() { return username; }
////////////////// DERIVED CLASS PLAYER //////////////////

class Player : public User {
protected:
    Quiz* quizzesObjs[20]; // array of ptrs
    int totalQuizzes;

public:
    Player(const char uN[], const char pw[], const char fN[], const char lN[]);
    void displayMenu() override;
    void startANewQuiz(Quiz* q, Question* questionPool[], int& questionCount);
    int highestScore();
    int lowestScore();
    float avgScore();
    int totalMCQuestions();
    int totalTFQuestions();
    float avgMcqGrade();
    float avgTFGrade();

    void displayStatistics();
    void displayAllScores();
    void displayLast2QuizDetails();
    ~Player();
};

////////////////// PLAYER FUNCTIONS //////////////////

Player::Player(const char uN[], const char pw[], const char fN[],
    const char lN[])
    : User(uN, pw, fN, lN, "player"), totalQuizzes(0), quizzesObjs{ nullptr } {}

void Player::displayMenu() {
    cout << "\nWelcome " << firstName << " " << lastName
        << "(PLAYER), please choose from the following options: "

        << "\n[1] Switch accounts"
        << "\n[2] Update your name"
        << "\n[3] Start a new quiz"
        << "\n[4] Display your scores statistics"
        << "\n[5] Display all your scores"
        << "\n[6] Display details of your last 2 quizzes"
        << "\n[7] Exit the program"
        << "\nMy choice: ";
}
void Player::startANewQuiz(Quiz* q, Question* questionPool[], int& questionCount) {
    cin.ignore();
    cout << "Quiz started";

    quizzesObjs[totalQuizzes++] = q;
    q->randomizeQuestions(questionPool, questionCount);//quiz object will be a shared resource so here when im adding questions in the ptr to objects array they will be visible to all
    q->playQuiz(); //when i will be incrementing score it will be available in manageUserChoice quiz[] also in player quiz []

}
int Player::highestScore() {
    int highest = quizzesObjs[0]->getScore();
    for (int i = 1; i < totalQuizzes; i++) {
        if (quizzesObjs[i]->getScore() > highest) {
            highest = quizzesObjs[i]->getScore();
        }
    }
    return highest;
}
int Player::lowestScore() {
    int lowest = quizzesObjs[0]->getScore();
    for (int i = 1; i < totalQuizzes; i++) {
        if (quizzesObjs[i]->getScore() < lowest) {
            lowest = quizzesObjs[i]->getScore();
        }
    }
    return lowest;
}
float Player::avgScore() {
    float totalScore = 0;
    for (int i = 0; i < totalQuizzes; i++) {
        totalScore += quizzesObjs[i]->getScore();
    }

    return totalScore / totalQuizzes;
}
int Player::totalMCQuestions() {
    int count = 0;
    for (int i = 0; i < totalQuizzes; i++) {
        count += quizzesObjs[i]->getMcqCount();
    }
    return count;
}
int Player::totalTFQuestions() {
    int count = 0;
    for (int i = 0; i < totalQuizzes; i++) {
        count += quizzesObjs[i]->getTfCount();
    }
    return count;
}
float Player::avgMcqGrade() {
    int totalQuestions = totalMCQuestions();
    int totalGrade = 0;
    for (int i = 0; i < totalQuizzes; i++) {
        totalGrade += quizzesObjs[i]->getMcqScore();
    }
    return float(totalGrade / totalQuestions);
}
float Player::avgTFGrade() {
    int totalQuestions = totalTFQuestions();
    float totalGrade = 0;
    for (int i = 0; i < totalQuizzes; i++) {
        totalGrade += quizzesObjs[i]->getTfScore();
    }
    return float(totalGrade / totalQuestions);
}

void Player::displayStatistics() {
    if (totalQuizzes == 0) {
        cout << "No quizzes taken yet. Returning to menu.\n";
        return;
    }

    // Display statistics
    cout << "\nYour score statistics per quiz:\n";
    cout << "- Number of Quizzes taken: " << totalQuizzes << "\n";
    cout << "- Highest quiz score: " << highestScore() << "/5\n";
    cout << "- Lowest quiz score: " << lowestScore() << "/5\n";
    cout << "- Average quiz score: " << avgScore() << "/5\n";

    cout << "\nYour score statistics per question type:\n";
    cout << "- Number of MC questions: " << totalMCQuestions() << "\n";
    cout << "- Number of T/F Questions: " << totalTFQuestions() << "\n";
    cout << "- Average grade for MC questions: " << avgMcqGrade() << "/2\n";
    cout << "- Average grade for T/F questions: " << avgTFGrade() << "/1\n";
}

void Player::displayAllScores() {
    cout << "\nTotal number of quizzes played: " << totalQuizzes;
    Quiz* q;
    for (int i = 0; i < totalQuizzes; i++) {
        cout << "\nQuiz " << i + 1 << " Report: ";
        quizzesObjs[i]->quizReport();
    }
}
void Player::displayLast2QuizDetails() {
    if (totalQuizzes == 0) {
        cout << "No quizzes have been taken yet.\n";
        return;
    }
    else if (totalQuizzes == 1) {
        cout << "\nOnly 1 Quiz has been taken yet";
        quizzesObjs[0]->displayDetails();
    }
    else {
        int start = totalQuizzes - 2;
        for (int i = start; i < totalQuizzes; i++) {
            cout << "\nDetails for Quiz " << (i + 1) << ":\n";
            quizzesObjs[i]->displayDetails(); // Call the display method in Quiz
        }
    }

}
Player::~Player() {
    for (int i = 0; i < totalQuizzes; ++i) {
        quizzesObjs[i] = nullptr;  // Simply nullify the pointers
    }
}
////////////////// DERIVED CLASS ADMIN //////////////////

class Admin : public User {
public:
    Admin(const char uN[], const char pw[], const char fN[], const char lN[]);
    void displayMenu() override;
    void addNewUser(User* objects[], int& objCount);
    void addNewQuestion(Question* questionPool[], int& questionCount);
    void viewAllQuestions(Question* questionPool[], int& questionCount);
    void loadQuestions(Question* questionPool[], int& questionCount);
    void viewAllUsers(User* objects[], int objCount);
    ~Admin() {}
};

////////////////// ADMIN FUNCTIONS //////////////////
void Admin::loadQuestions(Question* questionPool[], int& questionCount)
{
    ifstream in;
    in.open("exam_questions.txt");
    if (in)
    {
        char type[10];
        Question* q = nullptr;
        while (in.getline(type, 10))
        {

            if (strcmp(type, "MCQ") == 0) {
                q = new MCQuestion();
            }
            else if (strcmp(type, "TF") == 0) {
                q = new TFQuestion();
            }
            else {
                cout << "Invalid question type: " << type << "\nStopping the loading process.\n";
                break;
            }



            q->readQuestionsFromFile(in); // Calls the overridden function
            //error handling in tf case
            if (strcmp(type, "TF") == 0) {

                if (strcmp(q->getCorrectAnswer(), "true") != 0 && strcmp(q->getCorrectAnswer(), "false") != 0) {
                    cout << "Error: Invalid answer encountered.\nStopping the loading process.\n";
                    delete q; // Free the allocated memory for the duplicate
                    q = nullptr;
                    break;
                }

            }
            if (q)
            {
                // Check for duplicates
                bool notDuplicate = true;
                for (int i = 0; i < questionCount; ++i) {
                    if (*q == *questionPool[i]) { // Use operator== to check for duplicates
                        notDuplicate = false;
                        cout << "Duplicate question found. Skipping...\n";
                        delete q; // Free the allocated memory for the duplicate
                        q = nullptr;
                        break;
                    }
                }

                // Add to questionPool if not a duplicate
                if (notDuplicate) {
                    questionPool[questionCount++] = q;
                    // cout<<"\nquestion loaded";
                }
            }

        }
        cout << "\nquestions loaded successfully";
        in.close();
    }
    else {
        cout << "File not found. Returning to Menu.\n";
    }
}
void Admin::viewAllQuestions(Question* questionPool[], int& questionCount) {
    cout << "Number of questions available: " << questionCount
        << "\n---------------------------------------------------";
    //to count total mcq and tf ques 
    int totalmcqs = 0;
    int totaltf = 0;
    for (int i = 0; i < questionCount; i++) {
        if (strcmp(questionPool[i]->getType(), "MCQ") == 0) {
            totalmcqs++;
        }
        else {
            totaltf++;
        }

    }

    cout << "\nMC Questions list (Total: " << totalmcqs << " Questions)"
        << "\n---------------------------------------------------\n";
    int n = 1;
    for (int i = 0; i < questionCount; i++) {
        if (strcmp(questionPool[i]->getType(), "MCQ") == 0) {
            cout << "[" << n++ << "]";
            questionPool[i]->display(true);
        }
        //overrided display cuz question* calling tf mcq display
    }
    cout << "----------------------------------------------------"
        << "\nTF Questions list (Total: " << totaltf << " Questions)"
        << "\n---------------------------------------------------\n";

    n = 1;
    for (int i = 0; i < questionCount; i++) {
        if (strcmp(questionPool[i]->getType(), "TF") == 0) {
            cout << "[" << n++ << "]";
            questionPool[i]->display(true);
        }
        //overrided display cuz question* calling tf mcq display
    }
    //delete a question
    cout << "\n-------------------------------------------------------------------------------------"
        << "\nPress [d] and the question ID if you want to delete a question (Example: d 2) "
        << "\nPress [b] if you want to go back to the main menu: ";
    char choice;
    cin >> choice;
    while (choice != 'd' && choice != 'b') {
        cout << "\nInvalid Input! enter d to delete question b to return to menu: ";
        cin >> choice;
    }
    if (choice == 'b') {
        cout << "\nReturning to Menu.\n";
        return;
    }
    //perform deletion of question
    int deleteID;
    cout << "Enter the Question ID to delete: ";
    cin >> deleteID;

    // Search for the question to delete
    int indexToDelete = -1;
    for (int i = 0; i < questionCount; i++) {
        if (questionPool[i]->getID() == deleteID) {
            indexToDelete = i;
            break;
        }
    }

    if (indexToDelete == -1) {
        cout << "Question ID not found. Returning to menu.\n";
        return;
    }

    // Delete from questionPool
    delete questionPool[indexToDelete]; // Free memory
    questionPool[indexToDelete] = nullptr;
    for (int i = indexToDelete; i < questionCount - 1; i++) {
        questionPool[i] = questionPool[i + 1]; // Shift questions
    }
    questionCount--;

    // Rewrite the file
    ofstream fout("exam_questions.txt");
    // Save each question to the file
    for (int i = 0; i < questionCount; i++) {
        questionPool[i]->saveToFile(fout);
    }
    fout.close();
    cout << "\nQuestion deleted successfully!\n";
}

Admin::Admin(const char uN[], const char pw[], const char fN[], const char lN[])
    : User(uN, pw, fN, lN, "admin") {}
void Admin::viewAllUsers(User* objects[], int objCount) {
    cout << "\nExisting users in the system: \n"
        << "First name\t\tLast name\t\tUsername\t\tRole\n";
    for (int i = 0; i < objCount; i++) {
        cout << objects[i]->getFName() << "\t\t\t"
            << objects[i]->getLName() << "\t\t\t"
            << objects[i]->getUName() << "\t\t\t"
            << objects[i]->getRole() << endl;
    }
}
void Admin::displayMenu() {
    cout << "\nWelcome " << firstName << " " << lastName
        << "(ADMIN), please choose from the following options: "

        << "\n[1] Switch accounts"
        << "\n[2] Update your name"
        << "\n[3] View all users"
        << "\n[4] Add new user"
        << "\n[5] View all questions"
        << "\n[6] Add new question"
        << "\n[7] Load questions from file"
        << "\n[8] Exit the program"
        << "\nMy choice: ";
}
void Admin::addNewUser(User* objects[], int& objCount) {

    char uN[15], pw[15], fN[15], lN[15], role[10];
    cout << "\nEnter username: ";
    cin.ignore();
    cin.getline(uN, 15);
    // Check if username already exists
    for (int i = 0; i < objCount; i++) {
        if (strcmp(objects[i]->getUsername(), uN) == 0) {  // Assuming getUsername() is a member function that returns the username
            cout << "Username already exists. Please choose a different username.\n";
            cout << "Enter username: ";
            cin.getline(uN, 15);
            i = -1;  // Restart the loop to check against all users again
        }
    }
    cout << "Enter password: ";
    cin.getline(pw, 15);
    cout << "Enter first name: ";
    cin.getline(fN, 15);
    cout << "Enter last name: ";
    cin.getline(lN, 15);
    //check for role must be admin or player
    do {
        cout << "Enter role: ";
        cin.getline(role, 15);
        for (int i = 0; role[i] != '\0'; i++) {
            role[i] = tolower(role[i]);
        }
        if (strcmp(role, "admin") == 0 || strcmp(role, "player") == 0) {
            break;
        }
        cout << "Invalid role, choose either admin or player\n";
    } while (true);

    if (strcmp(role, "admin") == 0) {
        objects[objCount++] = new Admin(uN, pw, fN, lN);
    }
    else {
        objects[objCount++] = new Player(uN, pw, fN, lN);

    }
}
void Admin::addNewQuestion(Question* questionPool[], int& questionCount) {
    char type[10]{};
    // Ask for question type
    cin.ignore();
    do {
        cout << "Enter question type (MCQ/TF): ";

        cin.getline(type, 10);
        for (int i = 0; type[i] != '\0'; i++) {
            type[i] = toupper(type[i]);
        }

        if (strcmp(type, "MCQ") == 0 || strcmp(type, "TF") == 0) {
            break;
        }

        cout << "Invalid type! Please enter either 'MCQ' or 'TF'.\n";
    } while (true);

    Question* q = nullptr;

    if (strcmp(type, "MCQ") == 0) {
        q = new MCQuestion();

    }
    else {
        q = new TFQuestion();
    }

    // Use polymorphism to handle specific input logic
    if (q) {
        q->setType(type);
        q->inputQuestion();
        bool notDuplicate = true;
        for (int i = 0; i < questionCount; ++i) {
            if (*q == *questionPool[i]) { // Use operator== to check for duplicates
                notDuplicate = false;
                cout << "It is a duplicate question. Returning to Menu\n";
                delete q; // Free the allocated memory for the duplicate
                q = nullptr;
                return;
            }
        }

        // Add to questionPool if not a duplicate
        if (notDuplicate) {
            questionPool[questionCount++] = q;

            //save to file
            ofstream out("exam_questions.txt", ios::app);

            q->saveToFile(out);
            out.close();
            cout << "\nQuestion loaded successfully!\n";
        }
    }
}

// global
User* logInPage(User* objects[], int objCount) {
    char username[15], password[15];
    cout
        << "*"
        " Welcome to the Quiz game program"
        "! *******************************************************************";

    while (true) {
        // Infinite loop, but exits upon valid login
        cout << "\nPlease enter your username: ";

        cin.getline(username, 15);

        cout << "Please enter your password: ";
        cin.getline(password, 15);

        // Check validity of username and password
        for (int i = 0; i < objCount; i++) {
            if (objects[i]->checkValidity(username, password)) {
                return objects[i]; // Return the user object if credentials are valid
            }
        }

        // If no match is found, display error message and prompt again
        cout << "\nWrong Username or Password, Try Again\n";
    }
}
//global 
void autoloadquestions(Question* questionPool[], int& questionCount) {
    ifstream in;
    in.open("exam_questions.txt");
    if (in) {
        char type[10];
        Question* q = nullptr;
        while (in.getline(type, 10)) {

            if (strcmp(type, "MCQ") == 0) {
                q = new MCQuestion();

            }
            else if (strcmp(type, "TF") == 0) {
                q = new TFQuestion();
            }
            else {
                cout << "Invalid question type.\n";
                break;
            }

            if (q) {
                q->readQuestionsFromFile(in); // Calls the overridden function
                // Check for duplicates
                bool notDuplicate = true;
                for (int i = 0; i < questionCount; ++i) {
                    if (*q == *questionPool[i]) { // Use operator== to check for duplicates
                        notDuplicate = false;
                        cout << "Duplicate question.\n";
                        delete q; // Free the allocated memory for the duplicate
                        q = nullptr;
                        break;
                    }
                }

                // Add to questionPool if not a duplicate
                if (notDuplicate) {
                    questionPool[questionCount++] = q;
                    // cout<<"\nqloaded";
                }

            }

        }
        in.close();
    }
    else {
        cout << "\nQuestions file not found, cannot generate quiz. Returning to Menu.\n";
    }
}

// global
void manageUserChoice(User* objects[], int objCount, User* currentUser,
    bool& switchAcc) {
    bool dontExit;
    Question* questionPool[100];  // Shared 
    int questionCount = 0;

    Quiz* quizzes[20];//consider it like how we make object in main for aggregation n pass reference
    int quizCount = 0;
    // keep looping through display and choice handling until exit
    do {
        dontExit = true;
        currentUser->displayMenu();
        int choice;
        cin >> choice;
        while (choice < 1 || choice > 8) {
            cout << "Invalid choice. Try Again: ";
            cin >> choice;
        }
        // choice handling for admin
        if (strcmp(currentUser->getRole(), "admin") == 0) {
            // typcasting necessary of user* cuz we want to access player functions
            // from user*
            Admin* admin = dynamic_cast<Admin*>(currentUser);
            switch (choice) {
            case 1:
                // switch acc
                switchAcc = true;
                dontExit = false;
                cin.ignore();
                break;
            case 2:
                // Update your name
                admin->updateName();
                break;
            case 3:
                // View all users
                admin->viewAllUsers(objects, objCount);
                break;
            case 4:
                // Add new user
                admin->addNewUser(objects, objCount);
                break;
            case 5:
                // View all questions
                if (questionCount == 0) {
                    cout << "\nLoad questions from file first to view";
                }
                else {
                    admin->viewAllQuestions(questionPool, questionCount);
                }
                break;
            case 6:
                admin->addNewQuestion(questionPool, questionCount);
                break;
            case 7:
                // Load questions from file this function exists in question class
                admin->loadQuestions(questionPool, questionCount);
                break;
            case 8:
                dontExit = false;
                switchAcc = false;
                cout << "\nProgram ended.";
                break;
            }
        }
        // choice handling for player
        else {
            Player* player = dynamic_cast<Player*>(currentUser);
            switch (choice) {
            case 1:
                // switch acc
                switchAcc = true;
                dontExit = false;
                cin.ignore();
                break;
            case 2:
                // Update your name
                player->updateName();
                break;
            case 3:
                // Start a new quiz
                //player is abt to play quiz for the first time so loadquestions, after that no need to load
                if (questionCount == 0) {

                    autoloadquestions(questionPool, questionCount);

                }
                //after loading questions ensure questions are enough to play quiz and start quiz
                if (questionCount >= 3) {

                    quizzes[quizCount] = new Quiz;

                    player->startANewQuiz(quizzes[quizCount], questionPool, questionCount);
                    quizCount++;
                }
                if (questionCount < 3) {
                    cout << "Not enough questions to make quiz. Returning to Menu";
                    break;
                }
                //note: had to use multiple ifs and not if else cuz when im using if if else it throws error
                break;
            case 4:
                // Display your scores statistics
                player->displayStatistics();
                break;
            case 5:
                // Display all your scores
                player->displayAllScores();
                break;
            case 6:
                // Display details of your last quiz
                player->displayLast2QuizDetails();
                break;
            case 7:
                // Exit the program
                dontExit = false;
                switchAcc = false;
                cout << "\nProgram ended.";
                break;
            }
        }
        if (dontExit == false)
        {

            if (dontExit == false || switchAcc == true) {

                for (int i = questionCount - 1; i >= 0; i--) {
                    delete questionPool[i];
                    questionPool[i] = nullptr;
                }
                for (int i = quizCount - 1; i >= 0; i--) {
                    delete quizzes[i];
                    quizzes[i] = nullptr;
                }
            }
        }

    } while (dontExit);
}
int main() {
    srand(time(0)); // Seed random number generator used in quiz class 
    // array of pointers, of User datatype, to keep track of all the objects of
    // admin and player
    User* objects[15];
    // 2 default users "admin" & "player"
    int objCount = 0;
    objects[objCount++] = new Admin("admin", "12345", "user", "user");
    objects[objCount++] = new Player("player", "12345", "user", "user");

    // taking login info from console and checking validity
    // making a function of this cuz in switch acc function we will also require
    // the same procedure, made loginpage a friend cuz no obj to call it with if
    // member
    User* currentUser = nullptr;
    bool switchAcc = false;
    do {
        currentUser = logInPage(objects, objCount);
        // display menu handle the choice until user exits or switches acc
        manageUserChoice(objects, objCount, currentUser, switchAcc);
    } while (switchAcc);

    for (int i = objCount - 1; i >= 0; i--) {
        delete objects[i];
        objects[i] = nullptr;
    }
}