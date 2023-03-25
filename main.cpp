//
// Created by numen on 24/03/23.
//

#include "main.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <regex>
#include "lib/rapidxml.hpp"
#include "lib/encrypt.h"

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

    rapidxml::xml_node<>* new_node = doc->allocate_node(rapidxml::node_type(1), "block");

    new_node->append_attribute(doc->allocate_attribute("site", string_to_char_array(&creds->site)));
    new_node->append_attribute(doc->allocate_attribute("username", string_to_char_array(&creds->username)));
    new_node->append_attribute(doc->allocate_attribute("password", string_to_char_array(&creds->password)));

    return new_node;
}

void print_tree(rapidxml::xml_node<>* root_node){
    for(rapidxml::xml_node<> *block_node = root_node->first_node("block"); block_node; block_node = block_node->next_sibling()){

        Credentials creds = Credentials(
                block_node->first_attribute("site")->value(),
                block_node->first_attribute("username")->value(),
                block_node->first_attribute("password")->value()
                );

        creds.debug();
    }

}

std::vector<Credentials*>* find_credentials(std::string *filter, std::string *key, rapidxml::xml_node<>* root_node){
    auto* res = new std::vector<Credentials*>;
    char* char_key = string_to_char_array(key);
    char* char_filter = string_to_char_array(filter);
    const std::regex txt_regex("(.*)(" + *key + ")(.*)");

    for(rapidxml::xml_node<>* block_node = root_node->first_node("block"); block_node; block_node = block_node->next_sibling()){

        char* value = block_node->first_attribute(char_filter)->value();
        std::cout << std::regex_match(value, txt_regex) << std::endl;
        if(std::regex_match(value, txt_regex)){
            std::cout << "found one!" << endl;
            auto* found = new Credentials(
                    block_node->first_attribute("site")->value(),
                block_node->first_attribute("username")->value(),
                block_node->first_attribute("password")->value()
                    );
            res->push_back(found);
        }else{ std::cout << "no match" << endl; }
    }

    return res;
}

int main(){
    using namespace std;
    using namespace rapidxml;


    char *char_data = file_to_char_array("temp.xml"); // get file into a string to parse
    xml_document<>* doc = get_doc(char_data); // get xml tree from string
    xml_node<>* root_node = get_root_node(doc); //

    Credentials new_creds = Credentials("https://vs.co", "mario_rossi", "MyVscoPassword");
    xml_node<>* new_node = get_new_node_from_creds(doc, &new_creds);

    Credentials new_creds1 = Credentials("https://instagram.com", "mario_rossi", "MyInstagramPassword");
    xml_node<>* new_node1 = get_new_node_from_creds(doc, &new_creds1);

    Credentials new_creds2 = Credentials("https://twitter.com", "mario_rossi", "MyTwitterPassword");
    xml_node<>* new_node2 = get_new_node_from_creds(doc, &new_creds2);

    Credentials new_creds3 = Credentials("https://twich.com", "mario_rossi", "MyTwichPassword");
    xml_node<>* new_node3 = get_new_node_from_creds(doc, &new_creds3);

    Credentials new_creds4 = Credentials("https://youtube.com", "mario_rossi", "MyYoutubePassword");
    xml_node<>* new_node4 = get_new_node_from_creds(doc, &new_creds4);

    root_node->append_node(new_node);
    root_node->append_node(new_node1);
    root_node->append_node(new_node2);
    root_node->append_node(new_node3);
    root_node->append_node(new_node4);



    string site = "mario";
    string filter = "username";
    vector<Credentials*>* results = find_credentials(&filter, &site, root_node);

    for(int i = 0; i < results->size(); i++){
        results->at(i)->debug();
    }

    cout << "Finished" << endl;
    delete[] char_data; // free char array before exiting program
    return 0;
}