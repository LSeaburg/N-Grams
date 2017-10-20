/*Logan Seaburg
 * CS06B N-Grams
 *
 * I used the Stanford C++ libraries, and the references, including
 * "random.h" and functions I found in "hashmap.h" and "vector.h"
 * I also used lecture notes.
 *
 * --- Extras ---
 * By typing 'complete sentences' when it prompts for the file name,
 * the program will make all outputs in sentences that start with a
 * capital shirt and ends with punctuation
 */

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "hashmap.h"
#include "random.h"
#include "simpio.h"
#include "vector.h"

using namespace std;

void intro();
void mapFile(HashMap<Vector<string>, Vector<string> >& wordMap, int& n, bool& completeSentences);
void readFile(HashMap<Vector<string>, Vector<string> >& wordMap, string fileName, int n);
void outputText(HashMap<Vector<string>, Vector<string> >& wordMap, int n, bool completeSentences);
Vector<string> pickStart(HashMap<Vector<string>, Vector<string> >& wordMap, bool completeSentences);
void pickWord(Vector<string>& window, HashMap<Vector<string>, Vector<string> >& wordMap);
void addWord(string word, Vector<string>& window, HashMap<Vector<string>, Vector<string> >& wordMap);

int main() {
    HashMap<Vector<string>, Vector<string> > wordMap;
    bool completeSentences = false;
    int n;
    intro();
    mapFile(wordMap, n, completeSentences);
    outputText(wordMap, n, completeSentences);
    cout << "Exiting." << endl;
    return 0;
}

//displays an introductory message for the user
void intro() {
    cout << "Welcome to CS 106B Random Writer ('N-Grams')." << endl
         << "This program makes random text based on a document." << endl
         << "Give me an input file and an 'N' value for groups" << endl
         << "of words, and I'll create random text for you." << endl
         << "Additionally, type 'complete sentences' when it prompts" << endl
         << "for the file name to make all outputs be complete sentences." << endl
         << endl;
    return;
}

/* Takes in the map that will be populated, and passes that, along with the n value,
 * back to the main program so the output function can use it. It collects the name
 * of the file to be analyzed and the n-value.
 */
void mapFile(HashMap<Vector<string>, Vector<string> >& wordMap, int& n, bool& completeSentences) {
    string fileName;
    fileName = getLine ("Input file name? ");
    while (! fileExists(fileName)) {
        if (toLowerCase(fileName) == "complete sentences") {
            completeSentences = !completeSentences;
            if (completeSentences) {
                cout << "Complete sentences are enabled." << endl;
            } else {
                cout << "Complete sentences are disabled." << endl;
            }
        } else {
            cout << "Unable to open that file.  Try again." << endl;
        }
        fileName = getLine ("Input file name? ");
    }
    do {
        n = getInteger("Value of N? ");
        if (n < 2) {
            cout << "N must be 2 or greater." << endl;
        }
    } while (n < 2);
    readFile(wordMap, fileName, n);
    return;
}

/* Takes in the word map to be populated, the filename, and the n value,
 * and generates a map between a 'window' and the following word
 */
void readFile(HashMap<Vector<string>, Vector<string> >& wordMap, string fileName, int n) {
    ifstream input;
    string word;
    Vector <string> window, saveBeginning;
    openFile(input, fileName);
    for(int i = 0; i < n - 1; i++) { //stores the initial key for the first values
        input >> word;
        trim(word);
        window.add(word);
    }
    saveBeginning = window;
    while(input >> word) { //takes care of the bulk of the file
        trim(word);
        addWord(word, window, wordMap);
    }
    while(! saveBeginning.isEmpty()) { //handels the wrapping
        word = saveBeginning[0];
        addWord(word, window, wordMap);
        saveBeginning.remove(0);
    }
    input.close();
    return;
}

/*Takes in a word, a window, and a map, and adds the window as a key and the word as a value
 * to that map. It also changes the window to remove the word at the beginning and add the word at
 * the end so as to set it up for the next word. Makes mapFile shorter and less redundant.
 */
void addWord(string word, Vector<string>& window, HashMap<Vector<string>, Vector<string> >& wordMap) {
    Vector <string> temp;
    temp = wordMap[window];
    temp.add(word);
    wordMap.put(window, temp);
    window.remove(0);
    window.add(word);
}

/* Generates the output that the program gives when you enter the
 * number of words. Takes in the generated map so it can chose a starting point
 * and look up proper values to follow it.
 */
void outputText(HashMap<Vector<string>, Vector<string> >& wordMap, int n, bool completeSentences) {
    Vector<string> window;
    int words;
    cout << endl;
    words = getInteger("# of random words to generate (0 to quit)? ");
    while(words != 0) {
        if (words < n) {
            cout << "Must be at least " << n << " words." << endl;
        } else {
            if (!completeSentences) {
                cout << "... ";
            }
            window = pickStart(wordMap, completeSentences);
            for(int i = 0; i < words - n + 1; i++) {
                pickWord(window, wordMap);
            }
            if(completeSentences) {
                string curWord = window[window.size() - 1];
                while(curWord[curWord.length() -1] != '.' && curWord[curWord.length() -1] != '?'
                      && curWord[curWord.length() -1] != '!' ) {
                    pickWord(window, wordMap);
                    curWord = window[window.size() - 1];
                }
            }
            if (!completeSentences) {
                cout << "... ";
            }
            cout << endl << endl;
        }
        words = getInteger("# of random words to generate (0 to quit)? ");
    }
    return;
}

/* Takes in the map of windows mapped to follow-up words, and selects a random key as
 * a 'seed' to start the dialogue. Then it outputs the 'seed' that it selected.
 */
Vector<string> pickStart(HashMap<Vector<string>, Vector<string> >& wordMap, bool completeSentences) {
    Vector<string>key;
    string word;
    char firstLetter;
    do {
        key = randomKey(wordMap);
        word = key[0];
        firstLetter = (char)word[0];
    } while (completeSentences && ((int)firstLetter >= 91));
    for (int i = 0; i < key.size(); i++) {
        cout << key[i] << " ";
    }
    return key;
}

/* Takes in a window and the map of windows to follow-up words, then
 * selects an appropriate value for the window that is currently set. It then adjusts the
 * window to remove the first word and add the last one to get it ready for the next word generation.
 */
void pickWord(Vector<string>& window, HashMap<Vector<string>, Vector<string> >& wordMap) {
    string word;
    word = randomElement(wordMap[window]);
    cout << word << " ";
    window.remove(0);
    window.add(word);
    return;
}
