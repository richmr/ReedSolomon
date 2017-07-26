//
//  ReedSolomon16.h
//  RSExp
//
//  Created by MICHAEL RICH on 10/11/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#ifndef __RSExp__ReedSolomon16__
#define __RSExp__ReedSolomon16__

#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

#define gf_mul(x,y) fixint(galois_logtable_multiply(fixint(x),fixint(y),w))
#define gf_div(x,y) fixint(galois_logtable_divide(fixint(x),fixint(y),w))

// These only work on standard containers, specifically designed to use on dataArr_t below
#define len_ptr(x) x->size()
#define len(x) x.size()

typedef uint16_t data_t;
typedef vector<data_t> dataArr_t;

class ReedSolomon16 {
    int n;
    int k;
    int s;
    int w;

public:
    ReedSolomon16(int n_init, int k_init);  //s = 16
    ~ReedSolomon16();
    
    int encode(string* data, string* encoded_data);
    
    // Decode the data
    int decode(string* encodedData, string* decodedData, int padlen = 0);
    
    int gf_exp(int i);
    

    
private:
    int align(dataArr_t* p, dataArr_t* q);
    uint16_t convertBytesToU16(const char* bytes);
    void convertU16ToBytes(uint16_t in, char* bytes);
    int convertStringToU16Vector(string* datain, vector<uint16_t>* dataout);
    void convertU16VectorToString(vector<uint16_t>* datain, string* dataout, int padlength = 0);
    
    void gf_poly_mul(dataArr_t* p, dataArr_t* q, dataArr_t* result);
    unsigned int gf_poly_eval(dataArr_t* p, int x);
    //void gf_poly_div(dataArr_t* dividend, dataArr_t* divisor, dataArr_t* quotient, dataArr_t* remainder);
    int gf_log(int i);
    void gf_poly_scale(dataArr_t* p, int x, dataArr_t* result);
    void gf_poly_add(dataArr_t* p, dataArr_t* q, dataArr_t* result);
    int reduceByteArr(dataArr_t* data);
    int fixint(int i);
    
    void rs_generator_poly(int nsym, dataArr_t* genpoly);
    void rs_encode_msg(dataArr_t* msg_in, int nsym, dataArr_t* msg_out);
    void rs_calc_syndromes(dataArr_t* msg, int nsym, dataArr_t* syndromes);
    void rs_find_errata_locator(dataArr_t* err_pos, dataArr_t* e_loc);
    void rs_find_error_evaluator(dataArr_t* synd, dataArr_t* err_loc, int nsym, dataArr_t* remainder);
    void rs_gen_error_poly(dataArr_t* syndromes, dataArr_t* err_loc);
    void rs_find_errors(dataArr_t* err_loc, int nmess, dataArr_t* err_pos);
    void rs_correct_errata(dataArr_t* msg, dataArr_t* syndromes, dataArr_t* err_pos);
    
};

#endif /* defined(__RSExp__ReedSolomon16__) */
