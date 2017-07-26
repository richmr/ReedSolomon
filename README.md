# ReedSolomon
This is a c++ reed solomon encoder/decoder.  Currently frozen at 8-bit.  Underlying math library is by Plank (http://web.eecs.utk.edu/~plank/plank/papers/CS-07-593/).  I believe there are memory problems in the galois math library, but it does work.

Use:<br>
  <code>ReedSolomon RS(RS_n,RS_k); // n = max length (256 for 8 bit), k = data size (n-k = parity space)</code><br>
  <code>string originalData = data.str();</code><br>
  <code>string encodedData;</code><br>
  <code>RS.encode(&originalData,&encodedData);</code><br>
  
