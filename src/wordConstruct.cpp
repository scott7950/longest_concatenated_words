#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

using std::ifstream;
using std::cout;
using std::endl;
using std::queue;
using std::string;
using std::ios;
using std::vector;

#define subNodeNum 26

//x is not necessary as from the position of the children,
//we can get the char of the x
struct treeNode {
    char x;
    bool isWordEnd;
    struct treeNode* children[subNodeNum];
};

//////////////////////////////////////////////////////////////////////
//insertWord
//////////////////////////////////////////////////////////////////////
void insertWord(struct treeNode* root, const char* word, int size) {
    struct treeNode* node = root;

    //add the first node under the root
    int index = *word - 0x61;
    if(node->children[index] != NULL) {
        node = node->children[index];
    } else {
        node->children[index] = new treeNode;

        node = node->children[index];
        node->x = *word;
        for(int i=0; i<subNodeNum; ++i) {
            node->children[i] = NULL;
        }
        node->isWordEnd = false;
    }
    //if the size of the word is 1
    if(size == 1) {
        node->isWordEnd = true;
        return;
    }

    //add the nodes for the rest of the word
    for(int i=1; i<size; ++i) {
        index = *(word+i) - 0x61;
        if(node->children[index] != NULL) {
            node = node->children[index];
        } else {
            node->children[index] = new treeNode;

            node = node->children[index];
            node->x = *(word+i);
            for(int i=0; i<subNodeNum; ++i) {
                node->children[i] = NULL;
            }
            node->isWordEnd = false;
        }
    }
    node->isWordEnd = true;
}

//////////////////////////////////////////////////////////////////////
//containString
//////////////////////////////////////////////////////////////////////
bool containString(struct treeNode* root, const char* cstr, int start, int end, vector<vector<char> >& resultMatrix) {
    struct treeNode* node = root;

    //if only one char
    if(start == end) {
        int index = *(cstr + start) - 0x61;
        if(node->children[index] != NULL && node->children[index]->isWordEnd == true) {
            resultMatrix[start][end] = 1;
            return true;
        } else {
            resultMatrix[start][end] = -1;
            return false;
        }
    }

    //check substrings of the substring
    for(int i=start; i<end; ++i) {
        if(resultMatrix[start][i] == 0) {
            containString(root, cstr, start, i, resultMatrix);
        }
        if(resultMatrix[i+1][end] == 0) {
            containString(root, cstr, i+1, end, resultMatrix);
        }

        if(resultMatrix[start][i] == 1 && resultMatrix[i+1][end] == 1) {
            resultMatrix[start][end] = 1;
            return true;
        }
    }

    //check the whole substring
    int pos = start;
    int index = *(cstr + pos) - 0x61;
    while(node->children[index] != NULL) {
        node = node->children[index];

        pos++;
        if(pos > end) {
            break;
        }
        index = *(cstr + pos) - 0x61;
    }

    if(node->isWordEnd == true && pos > end) {
        resultMatrix[start][end] = 1;
        return true;
    } else {
        resultMatrix[start][end] = -1;
        return false;
    }

}

//////////////////////////////////////////////////////////////////////
//checkIfCompound
//////////////////////////////////////////////////////////////////////
bool checkIfCompound(struct treeNode* root, const char* cstr, int size) {
    //if the size of the string is 1
    if(size == 1) {
        return false;
    }

    bool result = false;

    //use a matrix to store the result of each combination
    //to avoid to be calculated more than once in function containString
    vector<vector<char> > resultMatrix;
    resultMatrix.resize(size);
    for(int i=0; i<size; ++i) {
        resultMatrix[i].resize(size);
        for(int j=0; j<size; ++j) {
            resultMatrix[i][j] = 0;
        }
    }

    //set the first break point for the substrings and call the function to check the substring
    for(int i=0; i<size-1; ++i) {
        if(containString(root, cstr, 0, i, resultMatrix) && containString(root, cstr, i+1, size-1, resultMatrix)) {
            result = true;
            break;
        }

    }

    return result;
}

//////////////////////////////////////////////////////////////////////
//main
//////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
    //parse argument
    string fileName = "wordsforproblem.txt";
    if(argc == 2) {
        fileName = argv[1];
    }

    //open file
    ifstream inFile;
    inFile.open(fileName.c_str());
    //C++11
    //inFile.open(fileName);

    if(!inFile) {
        cout << "Error: failed to open file " << fileName << endl;
        return -1;
    } 

    //create root node
    treeNode* root = new treeNode;
    for(int i=0; i<subNodeNum; ++i) {
        root->children[i] = NULL;
    }

    //parse file to generate trie
    string str;
    while (inFile >> str) {
        if(str.length() == 0) {
            cout << "Error: length of the string is 0" << endl;
            return -1;
        }
        const char *cstr = str.c_str();
        insertWord(root, cstr, str.length());
    }

    //reset the file pointer to the header of the file
    inFile.clear();
    inFile.seekg(0, ios::beg);

    //parse file to get the longest string, second longest string and 
    //number of words that can be constructed
    string longestStr = "";
    string secondLongestStr = "";
    int numOfWords = 0;
    while (inFile >> str) {
        const char *cstr = str.c_str();
        int strLen = str.length();
        if(checkIfCompound(root, cstr, strLen)) {
            if(strLen >= longestStr.length()) {
                secondLongestStr = longestStr;
                longestStr = str;
            } else if(strLen > secondLongestStr.length()) {
                secondLongestStr = str;
            }
            numOfWords++;
        }
    }

    //close file
    inFile.close();

    //destroy root
    queue<struct treeNode*> nodeQueue;
    nodeQueue.push(root);

    while(nodeQueue.size()) {
        struct treeNode* node = nodeQueue.front();

        for(int i=0; i<subNodeNum; ++i) {
            if(node->children[i] != NULL) {
                nodeQueue.push(node->children[i]);
            }
        }

        nodeQueue.pop();
        delete node;
    }

    //print result
    cout << "Longest String                             : " << longestStr << endl;
    cout << "Second Longest String                      : " << secondLongestStr << endl;
    cout << "The count of words that can be constructed : " << numOfWords << endl;

    return 0;
}

