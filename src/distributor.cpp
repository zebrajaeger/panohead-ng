#include "distributor.h"

Distributor& Distributor::i = Distributor::getInstance();

Distributor& Distributor::getInstance(){
    static Distributor instance_;
    return instance_;
}