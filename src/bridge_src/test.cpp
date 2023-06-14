#include <iostream>
#include <string>


std::string h(std::string MACAddress, std::string characteristicNumber){
    std::string MAP[16];

    //break down mac address
    char seperator = ':';

    for(int i = 0,j = 0; i < MACAddress.size(); i++){
        if(MACAddress[i] != seperator){
            MAP[j] = MACAddress[i];
            j++;
        }
    }


    //Scheme
    //  MACAddress 0-7 - MACAddress 8-11 - MACAddress 12-15 - characteristicNumber(as four digit number) - 000000000000

    std::string UUID = MAP[0] + MAP[1] + MAP[2] + MAP[3] + MAP[4] + MAP[5] + MAP[6] + MAP[7]
        + "-" + MAP[8] + MAP[9] + MAP[10] + MAP[11]
        + "-" + MAP[12] + MAP[13] + MAP[14] + MAP[15]
        + "-" + characteristicNumber[0] + characteristicNumber[1] + characteristicNumber[2] + characteristicNumber[3]
        + "-00000000";
    
    return UUID;

}

int main(){
    std::string g = h("12:12:23:45:3f:a2:3e:5b", "0010");
    std::cout << g << std::endl;
    return 0;
}