//
// Created by numen on 24/03/23.
//

#include "main.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include "lib/rapidxml.hpp"
#include "lib/rapidxml_print.hpp"

class Credentials{
public:
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

    ~Credentials()= default;

    // securely decrypt password
    std::string* decrypt(){
        return new std::string("ENC - " + this->password);
    };

    // securely encrypt password
    std::string* encrypt(){
        return new std::string("ENC - " + this->password);
    };

    // return xml representation of this class to be added as node to db
    std::string* to_xml(){
        auto* res = new std::string(
                "<block><site name=\""
                + this->site + "\"></site><username name=\""
                + this->username + "\"></username><password name=\""
                + this->password + "\"></password></block>"
                );
        return res;
    };

    // prints a debug string
    void debug(){
        std::cout <<
            "Site: " << this->site <<
            "\nUsername: " << this->username <<
            "\nPassword: " << this->password << "\n\n";
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

char* string_to_char_array(std::string *input){

    size_t length = input->size()+1; // get length of string, +1 for terminator /0
    char *char_array = new char[length]; // instantiate char array with appropriate length
    std::strncpy(char_array, input->c_str(), length); // copy string to char array to prevent const

    return char_array;
}

rapidxml::xml_document<>* get_doc(std::string *input){
    auto *doc = new rapidxml::xml_document<>;
    char* data = string_to_char_array(input);
    doc->parse<0>(data);

    return doc;
}

rapidxml::xml_document<>* get_doc(char* input){
    auto *doc = new rapidxml::xml_document<>;
    doc->parse<0>(input);

    return doc;
}

rapidxml::xml_node<>* get_root_node(rapidxml::xml_document<>* doc){

    rapidxml::xml_node<> *root_node;
    root_node = doc->first_node("root");     // get root node

    return root_node;
}

rapidxml::xml_node<>* get_new_node_from_creds(rapidxml::xml_document<>* doc, Credentials *creds){

    rapidxml::xml_node<>* new_node = doc->allocate_node(rapidxml::node_type(1));

    new_node->append_attribute(doc->allocate_attribute("site", string_to_char_array(&creds->site)));
    new_node->append_attribute(doc->allocate_attribute("username", string_to_char_array(&creds->username)));
    new_node->append_attribute(doc->allocate_attribute("password", string_to_char_array(creds->encrypt())));

    return new_node;
}

void print_tree(rapidxml::xml_node<>* root_node){
    for(rapidxml::xml_node<> *block_node = root_node->first_node("block"); block_node; block_node = block_node->next_sibling()){
        std::cout << "Block content: \n\t|_ " <<
             block_node->first_attribute("site")->value() << "\n\t|_ " <<
             block_node->first_attribute("username")->value() << "\n\t|_ " <<
             block_node->first_attribute("password")->value() << "\n" <<
             std::endl;
    }

}

int main(){
    using namespace std;
    using namespace rapidxml;


    char *char_data = file_to_char_array("temp.xml"); // get file into a string to parse
    xml_document<>* doc = get_doc(char_data); // get xml tree from string
    xml_node<>* root_node = get_root_node(doc); //

    Credentials new_creds = Credentials("https://vs.co", "mario_rossi", "MyVscoPassword");
    xml_node<>* new_node = get_new_node_from_creds(doc, &new_creds);

    root_node->append_node(new_node);

    print_tree(root_node);

    cout << "Finished" << endl;
    delete[] char_data; // free char array before exiting program
    return 0;
}