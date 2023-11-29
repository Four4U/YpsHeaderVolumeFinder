<p align="center"><img src="https://i.ibb.co/YW0zQ4Y/VHF.png"></p>

<p align="center">
   <img alt="Static Badge" src="https://img.shields.io/badge/%20-Linux-orange?style=flat&logo=Linux">
   <img alt="Static Badge" src="https://img.shields.io/badge/%20-Windows-orange?style=flat&logo=Windows&color=%2319a9d1">
   <img src="https://img.shields.io/badge/%20-C%2B%2B-blue?style=flat&logo=C%2B%2B">
</p>

# About
This is a small program to find `EFI_FIRMWARE_VOLUME_HEADER`. It reads the binary file and found Header. The program checks correctness of volume header through a checksum and print data about it.<br>
For use: enter the path to file(or name then file located in same directory)

# EFI_FIRMWARE_VOLUME_HEADER

The prototype of structure:

```cpp
typedef struct
{
    uint8_t ZeroVector[16];         // offset: 0x00
    uint8_t FileSystemGuid[16];     // offset: 0x10
    uint64_t FvLength;              // offset: 0x20
    uint32_t Signature;             // offset: 0x28
    uint32_t Attributes;            // offset: 0x2C
    uint16_t HeaderLength;          // offset: 0x30
    uint16_t Checksum;              // offset: 0x32
    uint16_t ExtHeaderOffset;       // offset: 0x34
    uint8_t Reserved;               // offset: 0x36
    uint8_t Revision;               // offset: 0x37
}
```

# class Volume
The main class, that contain information about founded VolumeHeader.

## Methods
+ getData(). Return object of struct with all readed fields.
+ operator<<. Operator `<<` is redefined to print all info in console.

# class FactoryVolume
Singleton Factory for class Volume. It takes a `string line` with a read program then searching signature `_FVH`. If little two bytes of checksum is zero, Factory create new object and append a vector.
