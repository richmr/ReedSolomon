//
//  ReedSolomon16.cpp
//  RSExp
//
//  Created by MICHAEL RICH on 10/11/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#include "ReedSolomon16.h"
#include "galois_cpp.h"

ReedSolomon16::ReedSolomon16(int n_init, int k_init) {
    //s = 16
    n = n_init;
    k = k_init;
    s = 16;
    w = s;
    
    // Note this is 2**16 data symbols of 2 bytes each.  This can actually encode 131kB of data.
    if (n > 65535) {
        throw runtime_error("Codeword too big, limited to 65535");
    }
    
    if ((k == 0) or (k > n)) {
        throw runtime_error("Data size is not set right.  Must be > 0 and < 2^n");
    }
    
    galois_create_log_tables(w);
}

ReedSolomon16::~ReedSolomon16() {
    
}

int ReedSolomon16::encode(string* data, string* encoded_data) {
    int nsym = n - k;
    int padlength = 0;
    encoded_data->clear();
    
    // Is string len odd?  If yes, need to add a byte
    if (data->size() & 1) {
        data->push_back(0x00);
        padlength = 1;
    }
    
    dataArr_t msg_in, msg_out;
    string::iterator data_index = data->begin();
    string::iterator data_end = data->end();
    string data_part;
    
    while (data_index != data_end) {
        // Set data grab size
        int data_grab_length = k * 2;   // times 2 because each data symbol is 2 bytes long
        if ((data_end - data_index) < data_grab_length) {
            data_grab_length = (data_end - data_index);
        }
        // set up the portion to convert
        data_part.assign(data_index, (data_index+data_grab_length));
        
        // increment index
        data_index += data_grab_length;
       
        // convert data to dataArr
        convertStringToU16Vector(&data_part, &msg_in);
        
        // encode it
        rs_encode_msg(&msg_in, nsym, &msg_out);
        
        // insert into outgoing encoded_data
        // convert dataArr to string
        string msg_out_s;
        convertU16VectorToString(&msg_out, &msg_out_s);
        encoded_data->insert(encoded_data->end(), msg_out_s.begin(), msg_out_s.end());
    }

    
    return padlength;
}


// Decode the data
int ReedSolomon16::decode(string* encodedData, string* decodedData, int padlen) {
    
    try {
        decodedData->clear();
        int nsym = n-k;
        dataArr_t syndromes;
        dataArr_t err_loc;
        dataArr_t err_pos;
        dataArr_t msg;
        
        string::iterator data_index = encodedData->begin();
        string::iterator data_end = encodedData->end();
        
        while (data_index != data_end) {// Set data grab size
            int data_grab_length = n * 2;
            if ((data_end - data_index) < data_grab_length) {
                data_grab_length = (data_end - data_index);
            }
            // set up the msg part
            string msg_part;
            msg_part.assign(data_index, (data_index+data_grab_length));
            
            // increment index
            data_index += data_grab_length;
            
            // copy to a dataArr
            convertStringToU16Vector(&msg_part, &msg);
            
            // First calc syndromes
            rs_calc_syndromes(&msg, nsym, &syndromes);
            int syndromeSum = reduceByteArr(&syndromes);
            
            if (syndromeSum != 0) {
                // Don't need to do this if syndromes are zero..  No errors
                // Calc error polynomial
                rs_gen_error_poly(&syndromes, &err_loc);
                
                // Calc error positions
                rs_find_errors(&err_loc, len(msg), &err_pos);
                
                // Fix the message
                rs_correct_errata(&msg, &syndromes, &err_pos);
            }
            
            // msg is data + parity bytes, so need to just pull off the data
            dataArr_t decodedDataArr;
            decodedDataArr.assign(msg.begin(), msg.begin() + (len(msg) - nsym));
            
            // Copy to a string
            string decodedDataArr_s;
            convertU16VectorToString(&decodedDataArr, &decodedDataArr_s);
            
            // Append to decoded data
            decodedData->insert(decodedData->end(), decodedDataArr_s.begin(), decodedDataArr_s.end());
        }
        
        // remove padding
        if (padlen) {
            decodedData->pop_back();
        }
        
    } catch (runtime_error& e) {
        decodedData->assign(e.what());
        return -1;
    }
    
    return 0;

}

uint16_t ReedSolomon16::convertBytesToU16(const char* bytes) {
    uint16_t num = (static_cast<unsigned char>(bytes[1]) << 8) | static_cast<unsigned char>(bytes[0]);
    return num;
}

void ReedSolomon16::convertU16ToBytes(uint16_t in, char* bytes) {
    bytes[1] = (in ^ 0x00ff) >> 8;
    bytes[0] = (in) ^ 0xff00;
    
}

int ReedSolomon16::convertStringToU16Vector(string* datain, dataArr_t* dataout) {
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
        dataout->push_back(convertBytesToU16(bytes_loc + i));
    }
    
    return padlength;
    // Init data out to proper size
//    dataout->clear();
//    dataout->insert(dataout->begin(), (len_ptr(datain)/2), 0x00);
//    
//    unsigned char *datain_ptr = (unsigned char *)datain->data();
//    unsigned char *dataout_ptr = (unsigned char *)dataout->data();
//    
//    for (int i = 0; i < len_ptr(datain); i++) {
//        dataout_ptr[i] = datain_ptr[i];
//    }
//    
//    return 0;
}


void ReedSolomon16::convertU16VectorToString(dataArr_t* datain, string* dataout, int padlength) {
    dataout->clear();
    
    int datasize = datain->size();
    char bytes[2];
    for (int i = 0; i < datasize; i++) {
        convertU16ToBytes((*datain)[i], bytes);
        dataout->push_back(bytes[0]);
        dataout->push_back(bytes[1]);
    }
    
    if (padlength) {
        dataout->pop_back();
    }
    
//    dataout->clear();
//    dataout->insert(dataout->begin(), (len_ptr(datain) * 2), 0x00);
//    
//    unsigned char *datain_ptr = (unsigned char *)datain->data();
//    unsigned char *dataout_ptr = (unsigned char *)dataout->data();
//    
//    for (int i = 0; i < len_ptr(datain) * 2; i++) {
//        dataout_ptr[i] = datain_ptr[i];
//    }


}


int ReedSolomon16::align(dataArr_t* p, dataArr_t* q) {
    int len_p = len_ptr(p);
    int len_q = len_ptr(q);
    
    if (len_p == len_q) return len_p;  // Nothing to be done
    if (len_p < len_q) {
        // Shift p values to the right to align polynomial coefficients
        p->insert(p->begin(), len_q-len_p, 0);
    } else {
        // shift q values to the right to align polynomials
        q->insert(q->begin(), len_p - len_q, 0);
    }
    
    return max(len_p, len_q);

}

void ReedSolomon16::gf_poly_mul(dataArr_t* p, dataArr_t* q, dataArr_t* result) {
    // Clear the answer
    result->clear();
    // fill with zeros
    result->insert(result->begin(), p->size() + q->size() - 1, 0x0000);
    
    // Begin iteration
    for (int j = 0; j < q->size(); j++) {
        for (int i=0; i < p->size(); i++) {
            int x = p->at(i);
            int y = q->at(j);
            result->at(i+j) = result->at(i+j) ^ gf_mul(x, y);
        }
    }
    
}

unsigned int ReedSolomon16::gf_poly_eval(dataArr_t* p, int x) {
    
    unsigned int y = p->at(0);
    for (int i = 1; i < p->size(); i++) {
        unsigned int pati = (*p)[i];
        y = gf_mul(y, x) ^ pati;
    }
    return y;

}

//void ReedSolomon16::gf_poly_div(dataArr_t* dividend, dataArr_t* divisor, dataArr_t* quotient, dataArr_t* remainder) {}

int ReedSolomon16::gf_exp(int i) {
    return fixint(galois_get_ilog_table(w)[fixint(i)]);
}

int ReedSolomon16::gf_log(int i) {
    return fixint(galois_get_log_table(w)[fixint(i)]);
}

void ReedSolomon16::gf_poly_scale(dataArr_t* p, int x, dataArr_t* result) {
//    char* p_char = (char *)p->data();
//    int numbytes = len_ptr(p) *2 ;
//    // Is endianess a problem here?  I think so..
//    // But, binary inspection of the string array and the binary uint16 array show bits in right order
//    // So, in theory it should work.
//    
//    // init result and allocate appropriate space
//    result->clear();
//    result->insert(result->begin(), len_ptr(p), 0);
//    char* result_char = (char *)result->data();
//    
//    galois_w16_region_multiply(p_char, x, numbytes, result_char, 0);
    
    // init result to proper length
    result->clear();
    result->assign(len_ptr(p), 0x0000);
    
    for (int i = 0; i < len_ptr(p); i++) {
        (*result)[i] = gf_mul((*p)[i], x);
    }
}

void ReedSolomon16::gf_poly_add(dataArr_t* p, dataArr_t* q, dataArr_t* result) {
    result->clear();
    result->insert(result->begin(), max(len_ptr(p), len_ptr(q)), 0x0000);
    
    for (int i = 0; i < len_ptr(p); i++) {
        (*result)[i+len_ptr(result)-len_ptr(p)] = (*p)[i];
    }
    
    for (int i = 0; i < len_ptr(q); i++) {
        (*result)[i+len_ptr(result)-len_ptr(q)] ^= (*q)[i];
    }
 
//    // First align the polys
//    int numbytes = align(p, q);
//    
//    // Init the result
//    result->clear();
//    result->insert(result->begin(), numbytes, 0);
//    
//    galois_region_xor((char *)p->data(), (char *)q->data(), (char *)result->data(), numbytes*2);
}

int ReedSolomon16::reduceByteArr(dataArr_t* data) {
    int length = len_ptr(data);
    int sum = 0;
    for (int i = 0; i < len_ptr(data); i++) {
        sum += (*data)[i];
    }
    return sum;

}

int ReedSolomon16::fixint(int i) {
    return (i); // & 0xffff);
}


void ReedSolomon16::rs_generator_poly(int nsym, dataArr_t* genpoly) {
    // Clear the answer
    genpoly->clear();
    // initialize
    genpoly->push_back(1);
    
    for (int i = 0; i < nsym; i++) {
        dataArr_t q, result;
        q.push_back(1);
        q.push_back(gf_exp(i));
        gf_poly_mul(genpoly, &q, &result);
        *genpoly = result;
    }
}

void ReedSolomon16::rs_encode_msg(dataArr_t* msg_in, int nsym, dataArr_t* msg_out) {
    // nsym = number of error correction symbols (need 2*t remember)
    // msg_in needs to be < 2^s length!  Not checking!
    
    dataArr_t gen;
    rs_generator_poly(nsym, &gen);
    // Init msg_out with msg_in and then pad to length of gen - 1 data words (which is actually just nsym)
    *msg_out = *msg_in;
    // Pad with zeros
    msg_out->insert(msg_out->begin()+msg_in->size(), nsym, 0x0000);
    
    for (int i = 0; i < msg_in->size(); i++) {
        data_t coef = msg_out->at(i);
        
        if (coef != 0) {
            for (int j = 1; j < gen.size(); j++) {
                msg_out->at(i+j) ^= gf_mul(gen[j], coef);
            }
        }
    }
    
    // Recopy the original message back into msg_out
    for (int i = 0; i < msg_in->size(); i++) {
        msg_out->at(i) = msg_in->at(i);
    }
    

}

void ReedSolomon16::rs_calc_syndromes(dataArr_t* msg, int nsym, dataArr_t* syndromes) {
    // init syndromes
    syndromes->clear();
    
    for (int i = 0; i < nsym; i++){
        syndromes->push_back(gf_poly_eval(msg, gf_exp(i)));
    }

}

void ReedSolomon16::rs_find_errata_locator(dataArr_t* err_pos, dataArr_t* e_loc) {
    e_loc->clear();
    e_loc->push_back(1);
    
    for (int i = 0; i < len_ptr(err_pos); i++) {
        dataArr_t temp_e_loc, temp_poly1, temp_poly2, result;
        temp_poly1.push_back(1);
        temp_poly2.push_back(gf_exp(err_pos->at(i)));
        temp_poly2.push_back(0);
        gf_poly_add(&temp_poly1, &temp_poly2, &result);
        gf_poly_mul(e_loc, &result, &temp_e_loc);
        *e_loc = temp_e_loc;
    }
}

void ReedSolomon16::rs_find_error_evaluator(dataArr_t* synd, dataArr_t* err_loc, int nsym, dataArr_t* remainder) {
    dataArr_t temp_remainder;
    gf_poly_mul(synd, err_loc, &temp_remainder);
    remainder->assign((temp_remainder.begin()+len(temp_remainder)-(nsym+1)), temp_remainder.end());
}

void ReedSolomon16::rs_gen_error_poly(dataArr_t* syndromes, dataArr_t* err_loc) {
    // init err_loc
    err_loc->clear();
    dataArr_t old_loc;
    err_loc->push_back(1);
    old_loc.push_back(1);
    
    for (int i = 0; i < syndromes->size(); i++) {
        old_loc.push_back(0);
        data_t delta = syndromes->at(i);
        
        for (int j = 1; j < len_ptr(err_loc); j++) {
            delta ^= gf_mul(err_loc->at(len_ptr(err_loc) - 1 - j), syndromes->at(i-j));
        }
        
        if (delta) {
            if (len(old_loc) > len_ptr(err_loc)) {
                // poly scale is galois_wXX_region_multiply(..)
                dataArr_t new_loc;
                gf_poly_scale(&old_loc, delta, &new_loc);
                gf_poly_scale(err_loc, gf_div(1, delta), &old_loc);
                *err_loc = new_loc;
                
            }
            dataArr_t temp_err_loc;
            dataArr_t temp_old_loc;
            gf_poly_scale(&old_loc, delta, &temp_old_loc);
            gf_poly_add(err_loc, &temp_old_loc, &temp_err_loc);
            *err_loc = temp_err_loc;
            int dummy = 1+1;
            // place hold
        }
        
    }
    // Need to drop the leading zeroes from err_loc
    dataArr_t temp_err_loc;
    bool foundFirstNonZero = false;
    for (int i = 0; i < len_ptr(err_loc); i++) {
        char value = err_loc->at(i);
        if (foundFirstNonZero) {
            temp_err_loc.push_back(value);
        } else {
            if (value != 0) {
                foundFirstNonZero = true;
                temp_err_loc.push_back(value);
            }
        }
    }
    
    *err_loc = temp_err_loc;
    int dummy = 1+1;
    // place hold
}

void ReedSolomon16::rs_find_errors(dataArr_t* err_loc, int nmess, dataArr_t* err_pos) {
    int errs = len_ptr(err_loc) - 1;
    err_pos->clear();
    
    for (int i = 0; i < nmess; i++) {
        // This is only set up for w = 16 currently..
        data_t gfexp = gf_exp(65535-i);
        data_t result = gf_poly_eval(err_loc, gfexp);
        if (result == 0) err_pos->push_back(nmess - 1 - i);
    }
    if (len_ptr(err_pos) == 0){
        // Couldn't find any errors
        throw runtime_error("Too many errors to correct");
    }

}

void ReedSolomon16::rs_correct_errata(dataArr_t* msg, dataArr_t* syndromes, dataArr_t* err_pos) {
    // This function will correct the mangled msg in msg, storing the result in msg
    // The final msg will be the data + parity bytes, which need to be stripped off as nsym
    // Could do here or in the public decode function, for now following the tutorial
    // Only valid for w =16
    dataArr_t coef_pos;
    for (int i = 0; i < len_ptr(err_pos); i++) {
        coef_pos.push_back(len_ptr(msg) - 1 - (*err_pos)[i]);
    }
    
    dataArr_t loc;
    rs_find_errata_locator(&coef_pos, &loc);
    
    // Slice and reverse order the syndrome
    dataArr_t synd;
    for (int i = len_ptr(err_pos); i >= 0; i--) {
        synd.push_back((*syndromes)[i]);
    }
    
    dataArr_t eval;
    rs_find_error_evaluator(&synd, &loc, (len_ptr(err_pos)-1), &eval);
    
    dataArr_t locprime;
    // Calculate locprime by eliminating even coefficients from loc
    for (int i = (len(loc) & 1); i < len(loc); i = i + 2) {
        locprime.push_back(loc[i]);
    }
    
    // During debug, locprime gets mangled some how (first byte changes)
    // This stopped it..  But i don't know why it was happening in the first place
    dataArr_t locprime_math = locprime;
    
    for (int i = 0; i < len_ptr(err_pos); i++) {
        //dataArr_t locprime_math = locprime;
        data_t x_index = (*err_pos)[i] + 65536 - len_ptr(msg);
        data_t x = gf_exp(x_index);
        data_t y = gf_poly_eval(&eval, x);
        data_t z = gf_poly_eval(&locprime_math, gf_mul(x, x));
        data_t magnitude = gf_div(y, gf_mul(x, z));
        
        // The signed/unsigned problems here require the line below.
        data_t errpos_i = fixint((*err_pos)[i]);
        (*msg)[errpos_i] ^= magnitude;
    }
    // done?
}

