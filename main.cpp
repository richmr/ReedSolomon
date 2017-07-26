//
//  main.cpp
//  RSExp
//
//  Created by MICHAEL RICH on 9/25/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <cstdlib>

#include <string>
#include <sstream>
#include <vector>

#include "galois_cpp.h"
#include "ReedSolomon.h"
#include "ReedSolomon16.h"

#define ARSIZE 5

using namespace std;

void convertToIntArray(char* input, int* output, int numbytes) {
    for (int i = 0; i < numbytes; i++) {
        output[i] = input[i];
    }
}

string arrayString(int* input, int numbytes) {
    stringstream result;
    result << "[";
    for (int i = 0; i < numbytes; i++) {
        result << input[i] << ", ";
    }
    result << "]";
    return result.str();
}

string arrayString(char* input, int numbytes) {
    stringstream result;
    result << "[";
    for (int i = 0; i < numbytes; i++) {
        result << hex << setw(2) << setfill('0') << (((short int)input[i]) & 0xff) << ", ";
    }
    
    result << "]";
    return result.str();
}

string stringHex(string input) {
    stringstream result;
    result << "[";
    for (int i = 0; i < input.length(); i++) {
        result << hex << setw(2) << setfill('0') << (int)input[i] << ", ";
    }
    result << "]";
    return result.str();

}

//void stringReserveTest() {
//    byteArr_t test;
//    test.clear();
//    test.insert(test.begin(),10,0);
//   
//    cout << "Data 1: " << arrayString(test.data(), test.size()) << endl;
//
//    for (char i; i < 5; i++) {
//        for (char j; j < 5; j++) {
//            test[i+j] = i + j;
//        }
//    }
//    cout << "Data: " << arrayString(test.data(), test.size()) << endl;
//}
//
//int sizeOfPointer(char *p) {
//    return sizeof(*p);
//}
//
//void polyMulTest() {
//    char p[5];
//    char q[5];
//    char r[sizeof(p) + (sizeof(q)-1)];
//    for (int i; i < sizeof(q); i++) {
//        for (int j; j < sizeof(p); j++) {
//            r[i+j] ^= i + j;
//        }
//    }
//    cout << "Data: " << arrayString(r, sizeof(r)) << endl;
//    //cout << "Size of p: " << sizeOfPointer(p) << endl;
//    cout << "Size of p2: " << sizeof(p) << endl;
//    
//    
//}
//
//void copyArrayToVector(byteArr_t* target, char* source, int numbytes) {
//    for (int i = 0; i < numbytes; i++) {
//        target->push_back(source[i]);
//    }
//}

void weirdPrintTest() {
    for (unsigned char i = 0; i < 255; i++) {
        cout << (unsigned int)i << endl;
    }
}

unsigned short convertBytesToU16(unsigned char* bytes) {
    unsigned short int* int16 = (unsigned short int*) bytes;
    return *int16;
}

void convertU16ToBytes(unsigned short in, unsigned char* bytes) {
    bytes = (unsigned char*) &in;
}

uint16_t convertBytesToU16_2(const char* bytes) {
    uint16_t num = (static_cast<unsigned char>(bytes[1]) << 8) | static_cast<unsigned char>(bytes[0]);
    return num;
}

void convertU16ToBytes_2(uint16_t in, char* bytes) {
    bytes[1] = (in ^ 0x00ff) >> 8;
    bytes[0] = (in) ^ 0xff00;
}

int convertStringToU16Vector(string* datain, vector<uint16_t>* dataout) {
    // returns the padlength (either 0 or 1)
    dataout->clear();
    int padlength = 0;
    
    // Is string len odd?  If yes, need to add a byte
    if (datain->size() & 1) {
        datain->push_back(0x00);
        padlength = 1;
    }
    
    int data_size = datain->size();
    const char* bytes_loc = datain->data();
    for (int i = 0; i < data_size; i += 2) {
        dataout->push_back(convertBytesToU16_2(bytes_loc + i));
    }
    
    return padlength;
}

void convertU16VectorToString(vector<uint16_t>* datain, string* dataout, int padlength = 0) {
    dataout->clear();
    
    int datasize = datain->size();
    char bytes[2];
    for (int i = 0; i < datasize; i++) {
        convertU16ToBytes_2((*datain)[i], bytes);
        dataout->push_back(bytes[0]);
        dataout->push_back(bytes[1]);
    }
    
    if (padlength) {
        dataout->pop_back();
    }
}

void mangleData(string* datain, int numMangles) {
    int length = datain->size();
    for (int i = 0; i < numMangles; i++) {
        int index = rand() % length;
        cout << "Spot to mangle: " << index << "\n";
        if (index == 0) {
            index = 21; // Not 1, because that's still an error at the first location!
            cout << "Index 0, corrected to: " << index << "\n";
            
        }
        int newbyte = rand() % 256;
        (*datain)[index] = newbyte;
    }
}

int main(int argc, const char * argv[]) {
    //char p[ARSIZE] = {0x01, 0x0f, 0x36, 0x78, 0x40 };
    //char q[ARSIZE] = {0x40, 0x78, 0x36, 0x0f, 0x01};
    //byteArr_t p_arr, q_arr;
    //copyArrayToVector(&p_arr, p, sizeof(p));
    //copyArrayToVector(&q_arr, q, sizeof(q));
    //p_arr.push_back(0x23);
    //p_arr.push_back(0xa4);
    //p_arr.push_back(0x9b);
    //q_arr.push_back(0x67);
    //q_arr.push_back(0xde);
    //q_arr.push_back(0xef);
    //q_arr.push_back(0x12);
    //byteArr_t result;
    //RS.gf_poly_mul(&p_arr, &q_arr, &result);
    
    //byteArr_t genpoly;
    //RS.rs_generator_poly(10, &genpoly);
    //std::cout << "Galois result: " << arrayString(genpoly.data(), genpoly.size()) << "\n";
    //int i_result[ARSIZE];
    //int x = 3;
    //cout << RS.gf_exp(10) << endl;
    //cout << RS.gf_log(10) << endl;
    
    //galois_w08_region_multiply(p, x, ARSIZE, result, 0);
    //galois_region_xor(p, q, result, 5);
    //convertToIntArray(p, i_result, ARSIZE);
    //std::cout << "Galois result: " << arrayString(result.data(), result.size()) << "\n";
    
    //std::cout << "Mult test: " << RS.gf_test_mul(35, (unsigned char)-34) << endl;
    //cout << "Div test: " << RS.gf_test_div(1, 200) << endl;
    
    //std::cout << "Galois result: " << (unsigned int)RS.gf_poly_eval(&p_arr, 3) << endl;
    //stringReserveTest();
    //polyMulTest();
    //weirdPrintTest();
    int nsym = 10;
    
//    byteArr_t msg_in, msg_out;
//    msg_in.push_back('H');
//    msg_in.push_back('o');
//    msg_in.push_back('w');
//    msg_in.push_back('d');
//    msg_in.push_back('y');
//    msg_in.push_back(' ');
//    msg_in.push_back('D');
//    msg_in.push_back('o');
//    msg_in.push_back('o');
//    msg_in.push_back('d');
//    msg_in.push_back('y');
////    msg_in.push_back(0xc6);
////    msg_in.push_back(0xc6);
////    msg_in.push_back(0x96);
////    msg_in.push_back(0x70);
////    msg_in.push_back(0xec);
//    RS.rs_encode_msg(&msg_in, nsym, &msg_out);
//    
//    std::cout << "Encoded result: " << printByteArr(&msg_out) << "\n";
//    
//    byteArr_t syndromes;
//    byteArr_t mangleMsg = msg_out;
//    // Test syndromes first
//    RS.rs_calc_syndromes(&msg_out, nsym, &syndromes);
//    cout << "Syndromes no mangle: " << printByteArr(&syndromes) << "\n";
//    
//    mangleMsg[1] = 'p';
//    mangleMsg[2] = 'p';
//    RS.rs_calc_syndromes(&mangleMsg, nsym, &syndromes);
//    cout << "Syndromes mangle: " << printByteArr(&syndromes) << "\n";
//    
//    
//    byteArr_t err_loc;
//    RS.rs_gen_error_poly(&syndromes, &err_loc);
//    cout << "Error loc poly: " << printByteArr(&err_loc) << "\n";
//    
//    byteArr_t err_pos;
//    RS.rs_find_errors(&err_loc, len(mangleMsg), &err_pos);
//    cout << "Error positions (sb 10 & 4): " << printByteArr(&err_pos) << "\n";
//    
//    byteArr_t e_loc;
//    RS.rs_find_errata_locator(&err_pos, &e_loc);
//    cout << "Error locator poly: " << printByteArr(&e_loc) << "\n";
//    
//    byteArr_t remainder;
//    RS.rs_find_error_evaluator(&syndromes, &e_loc, 10, &remainder);
//    cout << "Error evaluator: " << printByteArr(&remainder) << "\n";
//    
//    cout << "Mangled message: " << printByteArr(&mangleMsg) << "\n";
//    
//    RS.rs_correct_errata(&mangleMsg, &syndromes, &err_pos);
//    cout << "Fixed message: " << printByteArr(&mangleMsg) << "\n";
    
    
    ReedSolomon RS(255,235);
    
    string li300 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent fermentum ultricies arcu a vestibulum. Aliquam vitae lacinia nisl, eget fermentum magna. Nulla nec venenatis dui, egestas finibus risus. Nullam tempor nisi non urna rutrum, sed consectetur nibh luctus. Vestibulum pellentesque posuere.";
    
    string li245 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur dictum convallis cursus. Cras accumsan elit erat, et commodo risus gravida et. Aliquam imperdiet tellus lectus, nec dapibus mi tincidunt vitae. Praesent eu tellus metus. Nam sed.";
    
    string li244 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur dictum convallis cursus. Cras accumsan elit erat, et commodo risus gravida et. Aliquam imperdiet tellus lectus, nec dapibus mi tincidunt vitae. Praesent eu tellus metus. Nam sed";
    
    string li120 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce lobortis augue a leo consequat porttitor. Nam massa nunc.";
    
    string li128 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus congue nunc pharetra eleifend pellentesque. Fusce massa nunc.";
    
    string li129 = "1Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus congue nunc pharetra eleifend pellentesque. Fusce massa nunc.";
    
    string li127 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus congue nunc pharetra eleifend pellentesque. Fusce massa nunc";
    
    string hd = "Howdy Doody!";
    
    string message = li300;
    string encodedMessage;
    
    cout << "Orig: " << message << "\n";
    
    RS.encode(&message, &encodedMessage);
    
    //cout << "Encoded: " << stringHex(encodedMessage) << "\n";
    
    // Mangle a bit
    encodedMessage[10] = 0x70;
    encodedMessage[20] = 0x70;
    encodedMessage[30] = 0x70;
    encodedMessage[128] = 0x70;
    encodedMessage[275] = 'p';
    encodedMessage[60] = 'p';
    encodedMessage[70] = 'p';
    encodedMessage[270] = 'p';
    encodedMessage[280] = 'p';
    encodedMessage[290] = 'p';

    //cout << "Mangled: " << encodedMessage << "\n";
    
    string decodedMessage;
    int result = RS.decode(&encodedMessage, &decodedMessage);
    
    if (result < 0) {
        cout << "Fail: " << decodedMessage << "\n";
    } else {
        cout << "Decoded: " << decodedMessage << "\n";
        if (decodedMessage == message) {
            cout << "Success!\n";
        } else {
            cout << "Fail!\n";
        }
    }
    
//    ReedSolomon16 RS(65535, 65525);
//    //ReedSolomon RS(255, 245);
//    int rangestart = 2;
//    int rangeend = 20;
//    int step = 2;
//    int numMangles = 1;
//    
//    for (int trial = rangestart; trial <= rangeend; trial += step) {
//        cout << "Trial #" << trial << "\n";
//        string message;
//        message.assign(trial, 'A');
//        
//        string encodedMessage;
//        int padlength = RS.encode(&message, &encodedMessage);
//        
//        //cout << "Len msg: " << len(encodedMessage) << "\n";
//        
//        mangleData(&encodedMessage, numMangles);
//        //encodedMessage[0] = 0x00;
//        
//        //cout << "Len mangle msg: " << len(encodedMessage) << "\n";
//        
//        string decodedMessage;
//        int result = RS.decode(&encodedMessage, &decodedMessage, padlength);
//        
//        if (result < 0) {
//            cout << "Fail: " << decodedMessage << " at trial = " << trial << "\n";
//            return -1;
//        } else {
//            //cout << "Decoded: " << decodedMessage << "\n";
//            if (decodedMessage == message) {
//                cout << "Success at trial = " << trial << "\n";
//            } else {
//                cout << "Fail at trial = " << trial << "\n";
//                return -1;
//            }
//        }
//
//        
//    }
//    cout << "All trials successful!\n";

    
    //ReedSolomon16 RS(65535, 65528);
    // Compare/rectify data conversion methods.
    //string temp = {static_cast<char>(0xab), static_cast<char>(0xba), static_cast<char>(0xab), static_cast<char>(0xba), static_cast<char>(0xab), static_cast<char>(0xba), static_cast<char>(0xab), static_cast<char>(0xba), static_cast<char>(0xab), static_cast<char>(0xba), static_cast<char>(0xab), static_cast<char>(0xba), static_cast<char>(0xab), static_cast<char>(0xba), static_cast<char>(0xab), static_cast<char>(0xba)};
//    string temp = "mommie";
//    dataArr_t ptr_method;
//    dataArr_t shift_method;
//    
//    RS.convertStringToU16Vector(&temp, &ptr_method);
//    convertStringToU16Vector(&temp, &shift_method);
//    
//    string rebuilt_ptr_method, rebuilt_shift_method;
//    RS.convertU16VectorToString(&ptr_method, &rebuilt_ptr_method);
//    convertU16VectorToString(&shift_method, &rebuilt_shift_method);
//    
//    cout << "done\n";
    
//    ReedSolomon16 RS(65535, 58982);
//    //ReedSolomon16 RS(100, 93);
//    //for (int i = 0; i < 20; i++) {
//    //    cout << RS.gf_exp(65535-i) << "\n";
//    //}
//    
//    //ReedSolomon RS(255, 235);
//    int rangestart = 2;
//    int rangeend = 20;
//    int step = 2;
//    int numMangles = 1;
//    
//    string message;
//    message.assign(117964, 'A');
//    string encodedMessage;
//    int padlength = RS.encode(&message, &encodedMessage);
//    
//    //cout << stringHex(encodedMessage) << "\n";
//    int totalfails = 0;
//    int successes = 0;
//    int scrambles = 0;
//    
//    for (int trial = 0; trial < len(encodedMessage); trial += 1) {
//        string tempmessage = encodedMessage;
//        
//        cout << "Posn #" << trial << "\n";
//        tempmessage[trial] = 0x00;
//        
//        //cout << "Len mangle msg: " << len(encodedMessage) << "\n";
//        
//        string decodedMessage;
//        int result = RS.decode(&tempmessage, &decodedMessage, padlength);
//        if (result < 0) {
//            //cout << "Fail: " << decodedMessage << ", msg len, posn: " << len(encodedMessage)<< ", " << trial << "\n";
//            totalfails++;
//        } else {
//            //cout << "Decoded: " << decodedMessage << "\n";
//            if (decodedMessage == message) {
//                cout << "Success at posn = " << trial << ": posn - len = "<< (len(encodedMessage) - trial) << "\n";
//                successes++;
//            } else {
//                scrambles++;
//                //cout << "Scamble at trial = " << trial << "\n";
//                //return -1;
//            }
//        }
//        
//        
//    }
//    cout << "fails: " << totalfails << " scrambles: " << scrambles << " successes: " << successes << "\n";

//    unsigned char numin[2];
//    numin[0] = 'F';
//    numin[1] = 'C';
//    
//    cout << numin[0] << numin[1] << "\n";
//    
//    unsigned short int numint = convertBytesToU16_2((char *)numin);
//    
//    cout << "numint: " << numint << "\n";
//   
//    char bytes[2];
//    
//    convertU16ToBytes_2(numint, bytes);
//    
//    cout << bytes[0] << bytes[1] << "\n";
//    
//    string teststring = "FC";
//    vector<unsigned short> testarr;
//    
//    string test = "pauls your uncle";
//    cout << "test len: " << test.size() << "\n";
//    vector<uint16_t> dataarr;
//    int padlength = convertStringToU16Vector(&test, &dataarr);
//    cout  << "dataarr size: " << dataarr.size() << "\n";
//    
//    string retest;
//    convertU16VectorToString(&dataarr, &retest, padlength);
//    
//    cout << retest << "\n";
//    
//    dataArr_t buftestarr;
//    buftestarr.insert(buftestarr.begin(), (len(test)/2), 0x00);
//    char *testptr = (char *)test.data();
//    char *buftestarr_p = (char *)buftestarr.data();
//    
//    for (int i = 0; i < len(test); i++) {
//        buftestarr_p[i] = testptr[i];
//    }
//    
//    string buftest;
//    buftest.insert(buftest.begin(), len(test), 0x00);
//    char *bufptr = (char *)buftest.data();
//    //char *vecbufptr = (char *)dataarr.data();
//    
//    for (int i = 0; i < len(test); i++) {
//        bufptr[i] = buftestarr_p[i];
//    }
//    
//    cout << "buftest: " << buftest << "\n";
    
    
    return 0;
}
