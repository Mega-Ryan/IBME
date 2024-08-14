#ifndef UTILS_HPP
#define UTILS_HPP

#include <bitset>

#include "IB-ME.hpp"

class Utils {
 public:
  static string valueToBitString(BigInt value, BigInt q) {
    // Determine the number of bits needed
    int numBits = ceil(log2(q));
    // cout << "numBits: " << numBits << endl;

    // Create a bitset with the determined number of bits
    bitset<64> bitSet(value);  // Use a sufficiently large bitset
    // cout << "bitSet: " << bitSet << endl;

    // Convert the bitset to a string and take only the required number of bits
    string bitString = bitSet.to_string();
    bitString = bitString.substr(64 - numBits, numBits);
    // cout << "bitString: " << bitString << endl;

    return bitString;
  }

  static BigInt bitStringToValue(string bitString, BigInt q) {
    // Determine the number of bits needed
    int numBits = ceil(log2(q));

    // abort if the bit string is not of the required length
    if (bitString.length() != numBits) {
      cout << "Error: bit string length is not equal to the required number of "
              "bits"
           << endl;
      exit(1);
    }

    // Convert the bit string to a bitset
    bitset<64> bitSet(bitString);
    // cout << "bitSet: " << bitSet << endl;

    // Convert the bitset to a value
    BigInt value = bitSet.to_ullong();
    // cout << "value: " << value << endl;

    return value;
  }
};

#endif  // UTILS_HPP
