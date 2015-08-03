#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <memory>

using std::ifstream;
using std::cout;
using std::endl;
using std::queue;
using std::string;
using std::ios;
using std::vector;
using std::shared_ptr;

#define subNodeNum 26

//x is not necessary as from the position of the children,
//we can get the char of the x
struct treeNode {
    char x;
    bool isWordEnd;
    shared_ptr<struct treeNode> children[subNodeNum];

};

class wordTree {
public:
    wordTree(): root(new treeNode) {}

    void insertWord(const char*, int);
    bool containString(const char*, int, int, vector<vector<char> >&);
    bool checkIfCompound(const char*, int);

private:
    shared_ptr<struct treeNode> root;
    
};

//////////////////////////////////////////////////////////////////////
//insertWord
//////////////////////////////////////////////////////////////////////
void wordTree::insertWord(const char* word, int size) {
    shared_ptr<struct treeNode> node = root;

    //add the first node under the root
    int index = *word - 0x61;
    if(node->children[index] != nullptr) {
        node = node->children[index];
    } else {
        //node->children[index] = std::make_shared<treeNode>();
        node->children[index].reset(new treeNode);

        node = node->children[index];
        node->x = *word;
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
        if(node->children[index] != nullptr) {
            node = node->children[index];
        } else {
            node->children[index].reset(new treeNode);

            node = node->children[index];
            node->x = *(word+i);
            node->isWordEnd = false;
        }
    }
    node->isWordEnd = true;
}

//////////////////////////////////////////////////////////////////////
//containString
//////////////////////////////////////////////////////////////////////
bool wordTree::containString(const char* cstr, int start, int end, vector<vector<char> >& resultMatrix) {
    shared_ptr<struct treeNode> node = root;

    //if only one char
    if(start == end) {
        int index = *(cstr + start) - 0x61;
        if(node->children[index] && node->children[index]->isWordEnd == true) {
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
            containString(cstr, start, i, resultMatrix);
        }
        if(resultMatrix[i+1][end] == 0) {
            containString(cstr, i+1, end, resultMatrix);
        }

        if(resultMatrix[start][i] == 1 && resultMatrix[i+1][end] == 1) {
            resultMatrix[start][end] = 1;
            return true;
        }
    }

    //check the whole substring
    int pos = start;
    int index = *(cstr + pos) - 0x61;
    while(node->children[index]) {
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
bool wordTree::checkIfCompound(const char* cstr, int size) {
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
        if(containString(cstr, 0, i, resultMatrix) && containString(cstr, i+1, size-1, resultMatrix)) {
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

    //create instance of wordTree
    wordTree wt;

    //parse file to generate trie
    string str;
    while (inFile >> str) {
        if(str.length() == 0) {
            cout << "Error: length of the string is 0" << endl;
            return -1;
        }
        const char *cstr = str.c_str();
        wt.insertWord(cstr, str.length());
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
        if(wt.checkIfCompound(cstr, strLen)) {
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

    //print result
    cout << "Longest String                             : " << longestStr << endl;
    cout << "Second Longest String                      : " << secondLongestStr << endl;
    cout << "The count of words that can be constructed : " << numOfWords << endl;

    return 0;
}

