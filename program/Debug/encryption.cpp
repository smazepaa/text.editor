#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

extern "C"
{
    __declspec(dllexport) char* encrypt(char* text, int number, int length) {
        for (int i = 0; i < length; i++) {
            text[i] = text[i] + number;
        }
        return text;
    }

    __declspec(dllexport) char* decrypt(char* text, int number, int length) {
        for (int i = 0; i < length; i++) {
            text[i] = text[i] - number;
        }
        return text;
    }
}
