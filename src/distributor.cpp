#include "distributor.h"

Distributor& Distributor::getInstance(){
    static Distributor instance_;
    return instance_;
}