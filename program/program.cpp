#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <cstring>
#include <windows.h>
#include <stack>
#include <random>

using namespace std;

class Node
{
public:
    char line[80];
    Node* next;
    int length = 0;
};

class Cursor
{
public:
    int symbol;
    int line;
};

class Command
{
public:
    int command, length, line, symbol;
    char* text;
};

class Encryptor {
    typedef char* (*encrypt_ptr)(char*, int, int);
    typedef char* (*decrypt_ptr)(char*, int, int);

public:
    encrypt_ptr encrypt;
    decrypt_ptr decrypt;

    Encryptor() {
        HINSTANCE handle = LoadLibrary(TEXT("encryption.dll"));
        DWORD err = GetLastError();
        if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
        {
            std::cout << "Lib not found" << std::endl;
            exit(-1);
        }

        encrypt = (encrypt_ptr)GetProcAddress(handle, "encrypt");
        decrypt = (decrypt_ptr)GetProcAddress(handle, "decrypt");

    }
};

class LinkedList
{
public:
    Node* head;
    Encryptor encryptor{};

    LinkedList() : head(nullptr) {}

    // Copy constructor
    LinkedList(const LinkedList& other) : head(nullptr) {
        if (other.head) {
            head = new Node();
            strcpy(head->line, other.head->line);
            Node* current = head;
            Node* otherCurrent = other.head->next;
            while (otherCurrent) {
                current->next = new Node();
                strcpy(current->next->line, otherCurrent->line);
                current = current->next;
                otherCurrent = otherCurrent->next;
            }
        }
    }

    // Assignment operator
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) { // protect against invalid self-assignment
            // 1: deallocate old memory
            Node* current = head;
            while (current) {
                Node* next = current->next;
                delete current;
                current = next;
            }

            // 2: allocate new memory and copy the elements
            head = nullptr;
            if (other.head) {
                head = new Node();
                strcpy(head->line, other.head->line);
                current = head;
                Node* otherCurrent = other.head->next;
                while (otherCurrent) {
                    current->next = new Node();
                    strcpy(current->next->line, otherCurrent->line);
                    current = current->next;
                    otherCurrent = otherCurrent->next;
                }
            }
        }
        return *this; // by convention, always return *this
    }

    void addLine(const char* l)
    {
        // create a new node for the input line
        Node* newNode = new Node;
        newNode->next = nullptr;
        newNode->length = 0;

        // check if the previous line is shorter than 79 characters
        Node* current = head;
        Node* previous = nullptr;
        int currentLineLength = 0;

        // traverse the linked list to find the last non-full line
        while (current != nullptr) {
            currentLineLength = current->length;
            if (currentLineLength < 79) {
                break;
            }
            else {
                previous = current;
                current = current->next;
            }
        }

        while (*l != '\0') {
            if (currentLineLength < 79) {
                if (current != nullptr)
                {
                    current->line[current->length] = *l;
                    current->length++;
                    current->line[79] = '\0';

                }
                else {
                    newNode->line[newNode->length] = *l;
                    newNode->length++;
                    newNode->line[79] = '\0';
                }

                currentLineLength++;
                l++;
            }
            else {
                // if the current line is full
                if (current == nullptr) {
                    // if there's no next node (end of the list), create a new one
                    newNode->line[newNode->length] = '\0';
                    if (previous == nullptr) {

                        head = newNode;
                    }
                    else {
                        newNode = new Node;
                        newNode->next = nullptr;
                        newNode->length = 0;
                        currentLineLength = 0;
                    }
                }
                else {
                    // if there is a next node, move to it
                    previous = current;
                    current = current->next;
                }
            }
        }

        // add the final node if the input line ends or is full
        if (current == nullptr) {
            newNode->line[newNode->length] = '\0';
            if (previous == nullptr) {
                head = newNode;
                current = head;
            }
            else {
                previous->next = newNode;
                current = newNode;
            }
        }

    }

    void startNewLine()
    {
        Node* newNode = new Node;
        newNode->next = nullptr;
        newNode->length = 0;

        Node* current = head;
        Node* previous = nullptr;
        int currentLineLength = 0;

        //go till the last non-empty line
        while (current->next != nullptr) {
            previous = current;
            current = current->next;
        }

        // make sure we go to the next node when adding new text
        current->line[current->length] = '\0';
        current->length = 79;

        previous = current;
        current = newNode;
    }

    void searchText(const char* search)
    {
        Node* current = head;
        int lineCount = 1;

        while (current != nullptr) {
            const char* line = current->line;
            const char* found = strstr(line, search);

            int position = 0;

            while (found != nullptr) {
                position = found - line;
                printf("> your text is present on line %d, position %d\n", lineCount, position);

                // move to the next occurrence within the same line
                found = strstr(found + 1, search);
            }

            //go to the next line
            current = current->next;
            lineCount++;
        }

        if (lineCount == 1) {
            printf("text not found\n");
        }
    }

    char* stringPrint() {
        Node* current = head;
        int totalLength = 0;

        // Calculate the total length of the resulting string
        while (current != nullptr) {
            totalLength += strlen(current->line) + 1; // +1 for '\0'
            current = current->next;
        }

        // allocate memory for the string (+1 for \0)
        char* result = (char*)malloc(totalLength + 1);
        int currentIndex = 0;

        // copy lines into the result string with proper line breaks
        current = head;
        while (current != nullptr) {
            size_t lineLength = strlen(current->line);

            for (size_t i = 0; i < lineLength + 1; i++) {
                if (current->line[i] == '\0') {
                    result[currentIndex++] = '\n';
                }
                else {
                    result[currentIndex++] = current->line[i];
                }
            }

            current = current->next;
        }

        result[currentIndex] = '\0';

        return result;
    }

    void insertText(int lineIndex, int symbolIndex, const char* text)
    {
        if (lineIndex < 1 || symbolIndex < 0 || symbolIndex > 79)
        {
            printf("> invalid line or index\n");
            return;
        }

        Node* current = head;
        int currentLine = 1;

        while (current != nullptr && currentLine < lineIndex)
        {
            current = current->next;
            currentLine++;
        }

        if (current == nullptr)
        {
            printf("> line not found\n");
            return;
        }

        int length = current->length;

        if (symbolIndex > length)
        {
            printf("> index out of range\n");
            return;
        }

        char newValue[80];
        int newIndex = 0;
        int oldIndex = 0;

        while (oldIndex < symbolIndex)
        {
            newValue[newIndex] = current->line[newIndex];
            newIndex++;
            oldIndex++;
        }

        int textIndex = 0;
        while (text[textIndex] != '\0')
        {
            newValue[newIndex] = text[textIndex];
            newIndex++;
            textIndex++;
        }

        current->length += strlen(text);

        while (oldIndex < length)
        {
            newValue[newIndex] = current->line[oldIndex];
            newIndex++;
            oldIndex++;
        }

        newValue[newIndex] = '\0';

        strcpy(current->line, newValue);

    }

    void clear()
    {
        Node* current = head;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
    }

    void replace(int lineIndex, int symbolIndex, const char* text) {

        if (lineIndex < 1 || symbolIndex < 0 || symbolIndex > 79)
        {
            printf("> invalid line or index\n");
            return;
        }

        Node* current = head;
        int currentLine = 1;

        while (current != nullptr && currentLine < lineIndex)
        {
            current = current->next;
            currentLine++;
        }

        if (current == nullptr)
        {
            printf("> line not found\n");
            return;
        }

        int length = current->length;

        if (symbolIndex > length)
        {
            printf("> index out of range\n");
            return;
        }

        char newValue[80];
        int newIndex = 0;
        int oldIndex = 0;

        while (oldIndex < symbolIndex)
        {
            newValue[newIndex] = current->line[newIndex];
            newIndex++;
            oldIndex++;
        }

        int textIndex = 0;
        while (text[textIndex] != '\0')
        {
            newValue[newIndex] = text[textIndex];
            newIndex++;
            textIndex++;
        }

        oldIndex += strlen(text);

        while (oldIndex < length)
        {
            newValue[newIndex] = current->line[oldIndex];
            newIndex++;
            oldIndex++;
        }

        newValue[newIndex] = '\0';

        strcpy(current->line, newValue);


        printf("> text replaced at line %d, symbol %d\n", lineIndex, symbolIndex);

    }

    void deleteText(int lineIndex, int symbolIndex, int numSymbols) {

        if (lineIndex < 1 || symbolIndex < 0 || symbolIndex > 79)
        {
            printf("> invalid line or index\n");
            return;
        }

        Node* current = head;
        int currentLine = 1;

        while (current != nullptr && currentLine < lineIndex)
        {
            current = current->next;
            currentLine++;
        }

        if (current == nullptr)
        {
            printf("> line not found\n");
            return;
        }

        int length = current->length;

        if (symbolIndex > length)
        {
            printf("> index out of range\n");
            return;
        }

        char newValue[80];
        int newIndex = 0;
        int oldIndex = 0;

        while (oldIndex < symbolIndex)
        {
            newValue[newIndex] = current->line[newIndex];
            newIndex++;
            oldIndex++;
        }

        oldIndex += numSymbols;

        while (oldIndex < length)
        {
            newValue[newIndex] = current->line[oldIndex];
            newIndex++;
            oldIndex++;
        }

        newValue[newIndex] = '\0';

        current->length -= numSymbols;

        if (current->length < 0) {
            current->length = 0;
        }
        strcpy(current->line, newValue);

    }

    char* copy(int lineIndex, int symbolIndex, int numSymbols) {
        char buf[80];
        if (lineIndex < 1 || symbolIndex < 0 || symbolIndex > 79)
        {
            printf("> invalid line or index\n");
            return NULL;
        }

        Node* current = head;
        int currentLine = 1;

        while (current != nullptr && currentLine < lineIndex)
        {
            current = current->next;
            currentLine++;
        }

        if (current == nullptr)
        {
            printf("> line not found\n");
            return NULL;
        }

        int length = current->length;

        if (symbolIndex > length)
        {
            printf("> index out of range\n");
            return NULL;
        }

        int i = 0;
        for (i = 0; i < numSymbols && (symbolIndex + i) < current->length; i++) {
            buf[i] = current->line[symbolIndex + i];
        }

        // Null-terminate the string in the buffer
        buf[i] = '\0';

        return buf;
    }

    char* cut(int lineIndex, int symbolIndex, int numSymbols) {

        char buffer[80];
        strcpy(buffer, copy(lineIndex, symbolIndex, numSymbols));
        deleteText(lineIndex, symbolIndex, numSymbols);

        return buffer;
    }

    void undoStartNewLine() {
        Node* current = head;
        Node* previous = nullptr;

        // Go till the last node
        while (current->next != nullptr) {
            previous = current;
            current = current->next;
        }

        // Delete the last node and update the previous node's next pointer
        if (previous != nullptr) {
            previous->next = nullptr;
            delete current;
        }
    }

    void encrypt(int key) {
        Node* current = head;
        while (current != nullptr) {
            encryptor.encrypt(current->line, key, current->length);
            current = current->next;
        }
    }

    void decrypt(int key) {
        Node* current = head;
        while (current != nullptr) {
            encryptor.decrypt(current->line, key, current->length);
            current = current->next;
        }
    }
};

class FileStruct
{
public:
    char filename[80];

    void save(LinkedList stor)
    {
        char* text = stor.stringPrint();

        FILE* file;
        file = fopen(filename, "w");

        if (file != nullptr) {
            fputs(text, file);
            fclose(file);
            printf("> text has been successfully saved to %s\n", filename);
        }
        else {
            printf("> failed to open the file\n");
        }
        free(text);
    }

};

class Random {
public:
    int GetRandomNumber() {
        // Seed the random number generator with a value (you can use a different seed or even a time-based seed)
        std::random_device rd;
        std::mt19937 gen(rd());

        // Define the range of random numbers
        int min = 1;
        int max = 100;

        // Create a distribution
        std::uniform_int_distribution<int> distribution(min, max);

        // Generate a random number within the specified range
        int random_number = distribution(gen);

        return random_number;
    }
};

class IReader
{
public:
    virtual ~IReader() {}
    virtual void Load(LinkedList& stor, string filename) = 0;
};

class Reader : public IReader
{
public:
    virtual void Load(LinkedList& stor, string filename) {
        FILE* file;
        char line[80];
        char* filen = new char[filename.length() + 1];
        std::strcpy(filen, filename.c_str());

        file = fopen(filen, "r");

        if (file == nullptr) {
            printf("> Error opening file\n");
        }
        else {
            while (fgets(line, sizeof(line), file) != nullptr) {
                size_t length = strlen(line);

                // If the last character is not a newline, keep reading
                while (length > 0 && line[length - 1] != '\n' && fgets(line + length, sizeof(line) - length, file) != nullptr) {
                    length = strlen(line);
                }

                stor.addLine(line);
            }

            fclose(file);
            std::cout << "> Text has been successfully loaded from " << filename << std::endl;
        }
    }
};

class TextEditor
{
public:
    LinkedList stor;
    LinkedList originalList;
    char buffer[80];
    Cursor cursor{};
    stack<Command> commandStack;
    Encryptor encryptor{};

    void commands() {
        printf("\nList of commands\n");
        printf("1 - append text symbols to the end\n");
        printf("2 - start the new line\n");
        printf("3 - saving the information to file\n");
        printf("4 - loading the information from file\n");
        printf("5 - insert the text by line and symbol index\n");
        printf("6 - print the current text to console\n");
        printf("7 - search\n");
        printf("8 - clearing the console\n");
        printf("9 - insert with replacement\n");
        printf("10 - delete\n");
        printf("11 - copy\n");
        printf("12 - cut\n");
        printf("13 - paste\n");
        printf("14 - undo\n");
        printf("15 - redo\n");
        printf("16 - set cursor\n");
        printf("17 - print cursor position\n");
        printf("18 - encrypt/decrypt\n");
    }

    void clear() {
        system("cls");
    }

    void insert() {
        
        char insertText[80];
        while (getchar() != '\n');
        printf("> enter text to insert: ");
        fgets(insertText, sizeof(insertText), stdin);

        size_t length = strlen(insertText);
        if (length > 0 && insertText[length - 1] == '\n') {
            insertText[length - 1] = '\0';
        }

        Command com{};
        com.command = 5;
        com.length = length - 1;
        com.symbol = cursor.symbol;
        com.line = cursor.line;
        com.text = insertText;

        commandStack.push(com);

        stor.insertText(cursor.line, cursor.symbol, insertText);
        printf("> text inserted at line %d, symbol %d\n", cursor.line, cursor.symbol);
        cursor.symbol += length - 1;
        
    }

    void print() {
        char* text = stor.stringPrint();
        printf("\n%s\n", text);
        free(text);
    }

    void search() {
        char search[80];
        printf("> enter text to search: ");
        while (getchar() != '\n');

        fgets(search, sizeof(search), stdin);
        search[strcspn(search, "\n")] = '\0';

        stor.searchText(search);
    }

    void append() {
        char str[80];
        while (getchar() != '\n');
        printf("> enter text to append (maximum 79 symbols): ");
        fgets(str, sizeof(str), stdin);

        size_t length = strlen(str); // getting the length of the string
        if (length > 0 && str[length - 1] == '\n') {
            // remove the newline character
            str[length - 1] = '\0';
        }

        Command com{};
        com.command = 1;
        com.length = length - 1;

        commandStack.push(com);

        stor.addLine(str);
    }

    void save() {
        char filename[80];
        printf("> enter the file name for saving:");
        scanf("%s", filename);

        FileStruct the_file{};
        strcpy(the_file.filename, filename);
        the_file.save(stor);
    }

    void load(string path) {
        //stor.clearList();

        //IReader reader;
        //reader.Load(stor, path);
    }

    //void newLine()

    void replace() {
        
        char inputText[80];
        char originalText[80];

        while (getchar() != '\n');
        printf("> enter text to replace with: ");
        fgets(inputText, sizeof(inputText), stdin);

        size_t length = strlen(inputText);
        if (length > 0 && inputText[length - 1] == '\n') {
            inputText[length - 1] = '\0';
        }

        strcpy(originalText, stor.copy(cursor.line, cursor.symbol, length - 1));
        stor.replace(cursor.line, cursor.symbol, inputText);

        Command com{};
        com.command = 9;
        com.length = length - 1;
        com.symbol = cursor.symbol;
        com.line = cursor.line;
        com.text = originalText;

        commandStack.push(com);

        cursor.symbol += length - 1;
        
    }

    void deleteText() {
        
        int numSymbols = 0;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter number of symbols to delete: ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d", &numSymbols);

        char* deleted = new char[80];
        strcpy(deleted, stor.copy(cursor.line, cursor.symbol, numSymbols));
        stor.deleteText(cursor.line, cursor.symbol, numSymbols);
        printf("> text deleted at line %d, symbol %d\n", cursor.line, cursor.symbol);

        Command com{};
        com.command = 10;
        com.length = numSymbols;
        com.symbol = cursor.symbol;
        com.line = cursor.line;
        com.text = deleted;

        commandStack.push(com);
        
    }

    void copy() {

        int numSymbols = 0;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter number of symbols to copy: ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d", &numSymbols);

        strcpy(buffer, stor.copy(cursor.line, cursor.symbol, numSymbols));

        printf("> text copied\n");
    }

    void cut() {
        
        int numSymbols = 0;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter number of symbols to cut: ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d", &numSymbols);

        strcpy(buffer, stor.cut(cursor.line, cursor.symbol, numSymbols));

        printf("> text cut at line %d, symbol %d\n", cursor.line, cursor.symbol);
     
        Command com{};
        com.command = 12;
        com.length = numSymbols;
        com.symbol = cursor.symbol;
        com.line = cursor.line;
        com.text = buffer;

        commandStack.push(com);
    }

    void paste() {

        stor.insertText(cursor.line, cursor.symbol, buffer);
        printf("> text pasted at line %d, symbol %d\n", cursor.line, cursor.symbol);

        Command com{};
        com.command = 13;
        com.symbol = cursor.symbol;
        com.line = cursor.line;
        com.text = buffer;
        com.length = strlen(buffer);

        commandStack.push(com);

        cursor.symbol += strlen(buffer);
    }

    void setCursor() {

        int lineIndex, symbolIndex;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter line and symbol indexes to set a cursor (like '1 4'): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d %d", &lineIndex, &symbolIndex);
        
        cursor.line = lineIndex;
        cursor.symbol = symbolIndex;

        printf("> cursor was set at line %d, symbol %d\n", cursor.line, cursor.symbol);
    }

    void getCursor() {
        printf("> cursor at line %d, symbol %d\n", cursor.line, cursor.symbol);
    }

    void undoAppend(int appended) {
        Node* current = stor.head;

        while (current->next != NULL) {
            current = current->next;
        }

        int newLength = current->length - appended;
        if (newLength < 0) {
            newLength = 0;
        }

        current->length = newLength;
        current->line[newLength] = '\0';
    }

    void encryptDecrypt() {

        int operation;
        std::string modeEncrypt;
        std::string inputFile;
        std::string outputFile;

        std::cout << "> Choose an operation (1 for encrypt, 2 for decrypt): ";
        std::cin >> operation;

        std::cout << "> Choose a mode (n for normal, s for secret): ";
        std::cin >> modeEncrypt;

        std::cout << "> enter path to the input file: ";
        std::cin >> inputFile;

        std::cout << "> enter path to the output file: ";
        std::cin >> outputFile;

        int key;
        Random random{};

        if (modeEncrypt == "n") {
            std::cout << "> enter a key: ";
            std::cin >> key;
        }
        else if (modeEncrypt == "s") {
            key = random.GetRandomNumber();
        }
        else {
            std::cout << "> invalid mode" << std::endl;
        }

        stor.clear();

        IReader* reader = new Reader();
        reader->Load(stor, inputFile);
    }

    void undo() {

        Command comToUndo{};
        originalList = stor;

        comToUndo = commandStack.top();
        commandStack.pop();

        switch (comToUndo.command)
        {
        case 1:
            undoAppend(comToUndo.length);
            break;

        case 2:
            stor.undoStartNewLine();
            break;

        case 5:
            stor.deleteText(comToUndo.line, comToUndo.symbol, comToUndo.length);
            break;

        case 9:
            stor.replace(comToUndo.line, comToUndo.symbol, comToUndo.text);
            break;

        case 10:
            stor.insertText(comToUndo.line, comToUndo.symbol, comToUndo.text);
            break;

        case 12:
            stor.insertText(comToUndo.line, comToUndo.symbol, comToUndo.text);
            break;

        case 13:
            stor.deleteText(comToUndo.line, comToUndo.symbol, comToUndo.length);
            break;

        default:
            break;
        }

    }

    void redo() {
        stor = originalList;
        printf("> redo completed");
    }
};

int main()
{

    int command;
    int counter = 0;
    TextEditor text_editor{};

    while (true) {

        printf("> enter a number: ");
        printf("\n*if you need help with the commands - enter 0*\n");
        printf("> ");
        scanf_s("%d", &command);

        switch (command) {
        case 0:
            text_editor.commands();
            break;

        case 1:
            text_editor.append();
            break;

        case 2:
            //text_editor.newLine();
            break;

        case 3:
            text_editor.save();
            break;

        case 4:
            //text_editor.load();
            text_editor.commandStack.empty();
            break;

        case 5:
            text_editor.insert();
            break;

        case 6:
            text_editor.print();
            break;

        case 7:
            text_editor.search();
            break;

        case 8:
            text_editor.clear();
            break;

        case 9:
            text_editor.replace();
            break;

        case 10:
            text_editor.deleteText();
            break;

        case 11:
            text_editor.copy();
            break;

        case 12:
            text_editor.cut();
            break;

        case 13:
            text_editor.paste();
            break;

        case 14:
            text_editor.undo();
            break;

        case 15:
            text_editor.redo();
            break;

        case 16:
            text_editor.setCursor();
            break;

        case 17:
            text_editor.getCursor();
            break;

        case 18:
            text_editor.encryptDecrypt();
            break;

        default:
            printf("invalid command");;
            break;

        }
    }
    return 0;
}