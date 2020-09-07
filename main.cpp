#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>


#include "muduo/inc/mutex.h"
#include "muduo/inc/signal_slot.h"

using namespace std;
using namespace muduo;

void test(int){
    cout << "test" << endl;
}

int main(){
    shared_ptr<SingalUsed<int>> singalUsed(SingalUsed<int>::construct_signalUsed());
    SingalUsed<int>::SlotRet ret =  singalUsed->connect(test);
    singalUsed->call(1);
    return 0;
}
