#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <cstdint> // Para tipos como int64_t

using Byte = uint8_t;
using Short = uint16_t;
using Int = uint32_t;
using Long = int64_t; // Garante que Long seja um alias para int64_t

Byte ReadByte(std::ifstream &file);
Short ReadShort(std::ifstream &file);
Int ReadInt(std::ifstream &file);
Long ReadLong(std::ifstream &file);

void WriteByte(std::ofstream &file, Byte value);
void WriteShort(std::ofstream &file, Short value);
void WriteInt(std::ofstream &file, Int value);
void WriteLong(std::ofstream &file, Long value);

std::string SimplifyString(const std::string &string);
std::string StringToLower(const std::string &string);
