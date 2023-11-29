#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <bitset>

typedef char byte;
typedef long long ll;

using std::cout;
using std::cin;
using std::endl;
using std::string;


/*
    Структура доступа для класса тома
*/
struct VolumeInfo
{
    uint8_t ZeroVector[16];         // Смещение: 0x00
    uint8_t FileSystemGuid[16];     // Смещение: 0x10
    uint64_t FvLength;              // Смещение: 0x20
    uint32_t Signature;             // Смещение: 0x28
    uint32_t Attributes;            // Смещение: 0x2C
    uint16_t HeaderLength;          // Смещение: 0x30
    uint16_t Checksum;              // Смещение: 0x32
    uint16_t ExtHeaderOffset;       // Смещение: 0x34
    uint8_t Reserved;               // Смещение: 0x36
    uint8_t Revision;               // Смещение: 0x37
};

/*
    Класс, хранящиий информацию о томе.
*/
class Volume
{   
private:

    uint8_t ZeroVector[16];         // Смещение: 0x00
    uint8_t FileSystemGuid[16];     // Смещение: 0x10
    uint64_t FvLength;              // Смещение: 0x20
    uint32_t Signature;             // Смещение: 0x28
    uint32_t Attributes;            // Смещение: 0x2C
    uint16_t HeaderLength;          // Смещение: 0x30
    uint16_t Checksum;              // Смещение: 0x32
    uint16_t ExtHeaderOffset;       // Смещение: 0x34
    uint8_t Reserved;               // Смещение: 0x36
    uint8_t Revision;               // Смещение: 0x37

    static unsigned new_id;
    unsigned id;


public:

    Volume(string read_file, ll offset_begin_volume) : id(this->new_id)
    {
        this->new_id++; //Смещаем id для следующего тома

        for(int i = 0; i < 16; i++)
        {
            this->ZeroVector[i] = read_file[offset_begin_volume+i];
        }

        for(int i = 0; i < 16; i++)
        {
            this->FileSystemGuid[i] = read_file[offset_begin_volume+i+0x10];
        }

        //-------FvLength-------
        this->FvLength = 0;
        for(int i = 0; i < 8; i++)
        {
            this->FvLength = (this->FvLength << 8);
            this->FvLength = this->FvLength | (unsigned char)read_file[offset_begin_volume+i+0x20];  
        }
        //----------------------

        //-------Signature-------
        this->Signature = 0;
        for(int i = 0; i < 4; i++)
        {
            this->Signature = (this->Signature << 8);
            this->Signature = this->Signature| (unsigned char)read_file[offset_begin_volume+i+0x28];
        }
        //-----------------------

        //-------Attributes-------
        this->Attributes = 0;
        for(int i = 0; i < 4; i++)
        {
            this->Attributes = (this->Attributes << 8);
            this->Attributes = this->Attributes | (unsigned char)read_file[offset_begin_volume+i+0x2C];
        }
        //------------------------

        //-------HeaderLength-------
        this->HeaderLength = 0;
        for(int i = 0; i < 2; i++)
        {
            this->HeaderLength = (this->HeaderLength << 8);
            this->HeaderLength = this->HeaderLength | (unsigned char)read_file[offset_begin_volume+i+0x30];
        }
        //--------------------------

        //-------CheckSum-------
        this->Checksum = 0;
        for(int i = 0; i < 2; i++)
        {
            this->Checksum = (this->Checksum << 8);
            this->Checksum = this->Checksum | (unsigned char)read_file[offset_begin_volume+i+0x32];
        }
        //--------------------------

        //-------ExtHeaderOffset-------
        this->ExtHeaderOffset = 0;
        for(int i = 0; i < 2; i++)
        {
            this->ExtHeaderOffset = (this->ExtHeaderOffset << 8);
            this->ExtHeaderOffset = this->ExtHeaderOffset | (unsigned char)read_file[offset_begin_volume+i+0x34];
        }
        //-----------------------------

        this->Reserved = (unsigned char)read_file[offset_begin_volume+0x36];
        this->Revision = (unsigned char)read_file[offset_begin_volume+0x37];
    }
    

    VolumeInfo getData()
    {
        VolumeInfo info;
        info.Attributes = this->Attributes;
        info.Checksum = this->Checksum;
        info.ExtHeaderOffset = this->ExtHeaderOffset;
        info.FvLength = this->FvLength;
        info.HeaderLength = this->HeaderLength;
        info.Reserved = this->Reserved;
        info.Revision = this->Revision;
        info.Signature = this->Signature;
        
        for(int i = 0; i < 16; i++)
        {
            info.ZeroVector[i] = this->ZeroVector[i];
            info.FileSystemGuid[i] = this->FileSystemGuid[i];
        }

        return info;
    }

    friend std::ostream& operator<<(std::ostream& st, const Volume& vol)
    {
        st << "==========Volume:" << vol.id << "==========" << endl;
        st << "ZeroVector: ";
        for(int i = 0; i < 16; i++)
        {
            st << std::hex << (int)vol.ZeroVector[i] << " ";
        }
        st << endl;

        st << "FileSystemGuid: ";
        for(int i = 0; i < 16; i++)
        {
            st << std::hex << (int)vol.FileSystemGuid[i] << " ";
        }
        st << endl;

        st << std::hex << "FvLength: " << (ll)vol.FvLength << endl;
        st << std::hex << "Signature: " << (int)vol.Signature << endl;
        st << std::hex << "Attributes: " << (int)vol.Attributes << endl;
        st << std::hex << "HeaderLength: " << (int)vol.HeaderLength << endl;
        st << std::hex << "CheckSum: " << (int)vol.Checksum << endl;
        st << std::hex << "ExtHeaderOffset: " << (int)vol.ExtHeaderOffset << endl;
        st << std::hex << "Reserved: " << (int)vol.Reserved << endl;
        st << std::hex << "Revision: " << (int)vol.Revision << endl;
        st << "============================"<< endl;

        return st;
    }
};

unsigned Volume::new_id = 0;

/* 
    Singleton фабрика класса тома
*/
class FabricVolume
{
private:
    FabricVolume()
    {

    }

    //Запись байтов в Little-Endian и подсчёт суммы. Если последние два байта суммы равны нулю, метод вернёт true
    bool check_sum(string read_file, ll offset_begin_volume)
    {
        ll check = 0; unsigned short temp = 0;
        temp = temp & 0;
        for(int i = 0; i < 0x39; i+=2) //Запись байтов в Little-Endian и подсчёт суммы
        {
            temp = temp | read_file[offset_begin_volume+i+1];
            temp = (temp << 8);
            temp = temp | read_file[offset_begin_volume+i];
            check += temp; temp = temp & 0;
        }

        check = check & 0xff;          //Оставляем нужные байты для проверки контролькой суммы

        if (check == 0)
            return true;
        else
            return false;
    }
public:
    static FabricVolume& getInstance()
    {
        static FabricVolume fv;
        return fv;
    }

    //Получить вектор томов.
    std::vector<Volume> get_array_volumes(string read_file)
    {
        int position = 0;
        std::vector<Volume> solution;

        while(position < read_file.length())
        {
            position = read_file.find("_FVH", position+1);
            if(this->check_sum(read_file, position - 0x28))
                solution.push_back(Volume(read_file, position - 0x28));
        }

        return solution;
    }

};

int main(void)
{
    string bin; string file_name;

    cout << "Enter name of file: ";
    cin >> file_name;

    std::fstream fin(file_name, std::ios_base::in | std::ios_base::binary);
    if(!fin)
    {
        cout << "File not avaliable!" << endl;
        return -1;
    }
    
    byte temp = 0;

    while(fin.read(&temp, sizeof(byte)))                //Читаем файл
        bin.push_back(temp);

    std::vector<Volume> volumes = FabricVolume::getInstance().get_array_volumes(bin);

    for(int i = 0; i < volumes.size(); i++)
    {
        cout << volumes[i] << endl;
    }

    return 0;
}