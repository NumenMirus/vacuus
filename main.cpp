//
// Created by numen on 24/03/23.
//

#include "main.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include "lib/rapidxml.hpp"

class Credentials{
    std::string site;
    std::string username;
    std::string password;

    Credentials(){
        site = "";
        username = "";
        password = "";
    }

    Credentials(std::string site, std::string username, std::string password){
        this->site = site;
        this->username = username;
        this->password = password;
    }

    ~Credentials(){}

    // securely decrypt password
    void decrypt(){};

    // securely encrypt password
    void encrypt(){};

    // return xml representation of this class to be added as node to db
    void to_xml(){};

    // prints a debug string
    void debug(){
        std::cout <<
            "Site: " << this->site <<
            " Username: " << this->username <<
            " Password: " << this->password << std::endl;
    }
};


char* file_to_char_array(std::string filename){
    std::ifstream f(filename); //taking file as inputstream
    std::string file_content;
    if(f) {
        std::ostringstream ss;
        ss << f.rdbuf(); // reading data
        file_content = ss.str();
    }

    size_t length = file_content.size()+1; // get length of string, +1 for terminator /0
    char *char_array = new char[length]; // instantiate char array with appropriate length
    std::strncpy(char_array, file_content.c_str(), length); // copy string to char array to prevent const

    std::cout << "Size+1: " << length << " content: " << char_array << std::endl;

    return char_array;
}

rapidxml::xml_node<>* get_root_node(char *input){

    rapidxml::xml_document<> doc;    // character type defaults to char
    rapidxml::xml_node<> *root_node;
    doc.parse<0>(input);    // parsing file string - 0 means default parse flags
    root_node = doc.first_node("root");     // get root node

    return root_node;
}



int main(){
    using namespace std;
    using namespace rapidxml;

    char *char_data = file_to_char_array("temp.xml"); // get file into a string to parse

    xml_node<>* root_node = get_root_node(char_data); // parse string and get root

    // print file values
    for(xml_node<> *block_node = root_node->first_node("block"); block_node; block_node = block_node->next_sibling()){
        cout << "Block content: \n\t|_ " <<
            block_node->first_node("site")->first_attribute()->value() << "\n\t|_ " <<
            block_node->first_node("username")->first_attribute()->value() << "\n\t|_ " <<
            block_node->first_node("password")->first_attribute()->value() << "\n" <<
            endl;
    }

    delete[] char_data; // free char array before exiting program
    return 0;
}