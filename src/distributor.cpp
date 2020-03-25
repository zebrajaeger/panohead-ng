#include "distributor.h"

Distributor& Distributor::i = *(new Distributor());

// Distributor& Distributor::getInstance(){
//     static Distributor instance_;
//     return instance_;
// }