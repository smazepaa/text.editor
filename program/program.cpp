#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "program.h"
#include <fstream>
#include <stack>

using namespace std;

class Node
{
public:
    char line[80];
    Node* next;
    int length = 0;
};

class Command
{
public:
    int command, length, line, symbol;
    char* text;
};

class LinkedList
{
public:
    Node* head;

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

        //current->length += strlen(text);

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

    void load(LinkedList stor) 
    {
        FILE* file;
        char line[80];

        file = fopen(filename, "r");

        if (file == nullptr) {
            printf("> Error opening file\n");
        }
        else {
            while (fgets(line, sizeof(line), file) != nullptr) {
                size_t length = strlen(line);

                stor.addLine(line);
            }

            size_t lastLineLength = strlen(line);
            if (lastLineLength == sizeof(line) - 1 && line[lastLineLength] != '\0') {
                line[lastLineLength] = '\0';
                stor.addLine(line);
            }

            fclose(file);
            printf("> Text has been successfully loaded from %s\n", filename);
        }
    }

};

class TextEditor 
{
public:
    LinkedList stor{};
    char buffer[80];
    int appended;
    stack<Command> commandStack;

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

    }

    void clear() {
        system("cls");
    }

    void insert() {
        int lineIndex, symbolIndex;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter line and symbol indexes (like '1 4'): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d %d", &lineIndex, &symbolIndex);
        char insertText[80];

        printf("> enter text to insert: ");
        fgets(insertText, sizeof(insertText), stdin);

        size_t length = strlen(insertText);
        if (length > 0 && insertText[length - 1] == '\n') {
            insertText[length - 1] = '\0';
        }

        Command com{};
        com.command = 5;
        com.length = length - 1;
        com.symbol = symbolIndex;
        com.line = lineIndex;
        com.text = insertText;

        commandStack.push(com);

        stor.insertText(lineIndex, symbolIndex, insertText);
        printf("> text inserted at line %d, symbol %d\n", lineIndex, symbolIndex);
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

        appended = length - 1;

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

    void load() {
        stor.clear();

        char filename[80];
        printf("> Enter the file name for loading: ");
        scanf("%s", filename);

        FileStruct the_file{};
        strcpy(the_file.filename, filename);
        the_file.load(stor);
    }

    void newLine() {
        stor.startNewLine();
        printf("> new line is started\n");

        Command com{};
        com.command = 2;

        commandStack.push(com);
    }

    void replace() {

        int lineIndex, symbolIndex;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter line and symbol indexes (like '1 4'): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d %d", &lineIndex, &symbolIndex);

        char inputText[80];

        printf("> enter text to insert: ");
        fgets(inputText, sizeof(inputText), stdin);

        size_t length = strlen(inputText);
        if (length > 0 && inputText[length - 1] == '\n') {
            inputText[length - 1] = '\0';
        }

        stor.replace(lineIndex, symbolIndex, inputText);

        Command com{};
        com.command = 9;
        com.length = length - 1;
        com.symbol = symbolIndex;
        com.line = lineIndex;
        com.text = inputText;

        commandStack.push(com);
    }

    void deleteText() {
        int lineIndex, symbolIndex, numSymbols;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter line and symbol indexes and number of symbols (like '1 4 9'): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d %d %d", &lineIndex, &symbolIndex, &numSymbols);

        stor.deleteText(lineIndex, symbolIndex, numSymbols);
        printf("> text deleted at line %d, symbol %d\n", lineIndex, symbolIndex);

        Command com{};
        com.command = 10;
        com.length = numSymbols;
        com.symbol = symbolIndex;
        com.line = lineIndex;

        commandStack.push(com);
    }

    void copy() {

        int lineIndex, symbolIndex, numSymbols;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter line and symbol indexes and number of symbols (like '1 4 9'): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d %d %d", &lineIndex, &symbolIndex, &numSymbols);
        strcpy(buffer, stor.copy(lineIndex, symbolIndex, numSymbols));

        printf("> text copied\n");
    }

    void cut() {
        int lineIndex, symbolIndex, numSymbols;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter line and symbol indexes and number of symbols (like '1 4 9'): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d %d %d", &lineIndex, &symbolIndex, &numSymbols);
        strcpy(buffer, stor.cut(lineIndex, symbolIndex, numSymbols));

        printf("> text cut at line %d, symbol %d\n", lineIndex, symbolIndex);

        Command com{};
        com.command = 12;
        com.length = numSymbols;
        com.symbol = symbolIndex;
        com.line = lineIndex;

        commandStack.push(com);
    }

    void paste() {
        int lineIndex, symbolIndex;
        char inputBuffer[80];

        while (getchar() != '\n');
        printf("> enter line and symbol indexes (like '1 4'): ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);

        sscanf(inputBuffer, "%d %d", &lineIndex, &symbolIndex);
        stor.insertText(lineIndex, symbolIndex, buffer);

        printf("> text pasted at line %d, symbol %d\n", lineIndex, symbolIndex);

        Command com{};
        com.command = 13;
        com.symbol = symbolIndex;
        com.line = lineIndex;
        com.text = buffer;

        commandStack.push(com);
    }

    void undoAppend() {
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


};

void main()
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
            text_editor.newLine();
            break;
            
        case 3:
            text_editor.save();
            break;

        case 4:
            text_editor.load();
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
            text_editor.undoAppend();
            break;

        default:
            printf("invalid command");;
            break;

        }
    }
}